/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <unistd.h>
#include <dmedia/midi.h>
#include <fts/fts.h>

#undef MIDI_SYSEX_DEBUG

static fts_status_description_t sgi_no_midi =
{
  "No Midi port configured"
};


/******************************************************************************/
/*                                                                            */
/*                              MIDI Device                                   */
/*                                                                            */
/******************************************************************************/

/* The MIDI device, called "sgi_midi" support input and output on the same device */

/* Forward declaration of midi device/class functions */

static fts_status_t sgi_midi_open(fts_dev_t *dev, int nargs, const fts_atom_t *args);
static fts_status_t sgi_midi_close(fts_dev_t *dev);

static fts_status_t sgi_midi_get(fts_dev_t *dev, unsigned char *cp);
static fts_status_t sgi_midi_put(fts_dev_t *dev, unsigned char c);

void sgi_midi_init(void)
{
  fts_dev_class_t *sgi_midi_dev_class;

  /* adding device functions: the device support only basic 
   character i/o; no callback functions, no sync functions */

  sgi_midi_dev_class = fts_dev_class_new(fts_char_dev, fts_new_symbol("sgi_midi"));

  /* (fd) HACK : the sgi midi device is no longer used */
#if 0
  if (mdInit() <= 0)
    return;

  fts_dev_class_set_open_fun(sgi_midi_dev_class, sgi_midi_open);
  fts_dev_class_set_close_fun(sgi_midi_dev_class, sgi_midi_close);
  fts_dev_class_char_set_get_fun(sgi_midi_dev_class, sgi_midi_get);
  fts_dev_class_char_set_put_fun(sgi_midi_dev_class, sgi_midi_put);
#endif
}


/* The instantiation function has one keyword argument "port", the
   name of the MIDI port; it is an optional argument; we don't do any
   locking on the MIDI port 
   */

#define MAX_OUT_SYSEX 1024
#define SGI_MAX_MIDI_EV 32

typedef struct
{
  MDport in_port;
  MDport out_port;

  /* input unparsing support */

  enum {input_waiting, reading_message, reading_sysex} input_status;
  unsigned char in_buf[3];	
  unsigned char *in_sysex_buf;

  MDevent in_midi_event[SGI_MAX_MIDI_EV];
  int in_current_char_count;
  int in_current_event_length;
  int in_current_event;
  int in_event_count;

  /* output parsing support */

  MDevent out_midi_event;

  enum {output_waiting, message_need_first, message_need_first_and_second,
	  message_need_second, getting_sysex } output_status;

  unsigned char out_sysex_buf[MAX_OUT_SYSEX];
  unsigned int out_sysex_count;
} sgimidi_dev_data;


static fts_status_t
sgi_midi_open(fts_dev_t *dev, int nargs, const fts_atom_t *args)
{
  sgimidi_dev_data *data;
  char *port_name = 0;
  char *in_port_name = 0;
  char *out_port_name = 0;
  int i;

  /* Look for the port name; if not specified, just use NULL,
     that will open the first MIDI port found (according to the manual)
   */

  for (i = 0; i < nargs - 1; i++)
    if (fts_is_symbol(&args[i]) && fts_is_symbol(&args[i+1]))
      {
	fts_symbol_t s;

	s = fts_get_symbol(&args[i]);

	if (s == fts_new_symbol("port"))
	  port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
	else if (s == fts_new_symbol("in_port"))
	  in_port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
	else if (s == fts_new_symbol("out_port"))
	  out_port_name = (char *) fts_symbol_name(fts_get_symbol(&args[i+1]));
      }



  /* allocate the device data and open the input and output port */

  data = fts_malloc(sizeof(sgimidi_dev_data));
  data->input_status = input_waiting;
  data->output_status = output_waiting;

  data->in_port = mdOpenInPort(in_port_name ? in_port_name : port_name);
  data->out_port = mdOpenOutPort(out_port_name ? out_port_name : port_name);

  if (data->in_port && data->out_port)
    {
      /* no error, end configuring the port and return */
      
      dev->device_data = (void *) data;	
      mdSetStampMode(data->in_port, MD_NOSTAMP);
      mdSetStampMode(data->out_port, MD_NOSTAMP);

      return fts_Success;
    }
  else
    return &fts_dev_open_error;
}

static fts_status_t
sgi_midi_close(fts_dev_t *dev)
{
  sgimidi_dev_data *data = (sgimidi_dev_data *)dev->device_data;

  /* Send a all not off, and close the port */

  mdPanic(data->out_port);
  mdClosePort(data->out_port);
  mdClosePort(data->in_port);

  fts_free(data);

  return fts_Success;
}

/* I/O functions */

static void
sgi_midi_prepare_current_event(sgimidi_dev_data *data)
{
  MDevent *midi_event;

  midi_event = &(data->in_midi_event[data->in_current_event]);

  if ((midi_event->msg[0] == MD_SYSEX) &&  midi_event->sysexmsg && midi_event->msglen > 0)
    {
      /* received sysex */

#ifdef MIDI_SYSEX_DEBUG
      {
	int i;

	fprintf(stderr, "EV[%d]: Received sysex, length %d, buffer %lx:", 
		data->in_current_event, midi_event->msglen, midi_event->sysexmsg);

	for (i = 0; i < midi_event->msglen; i++)
	  fprintf(stderr, "%c", midi_event->sysexmsg[i]);

	fprintf(stderr, "\n");
      }
#endif

      data->input_status = reading_sysex;
      data->in_current_char_count = 0;

      data->in_sysex_buf = (unsigned char *) midi_event->sysexmsg;
      data->in_current_event_length = midi_event->msglen;
    }
  else
    {
      data->input_status = reading_message;
      data->in_current_char_count = 0;
      
      data->in_buf[0] = midi_event->msg[0];

      data->in_current_event_length = midi_event->msglen;

      if (data->in_current_event_length > 1)
	data->in_buf[1] = mdGetByte1(midi_event->msg);

      if (data->in_current_event_length > 2)
	data->in_buf[2] = mdGetByte2(midi_event->msg);
    }
}

static fts_status_t
sgi_midi_get(fts_dev_t *dev, unsigned char *cp)
{
  sgimidi_dev_data *data = (sgimidi_dev_data *)dev->device_data;

  switch(data->input_status)
    {
    case reading_message:
      if (data->in_current_char_count < data->in_current_event_length)
	{
	  /* More bytes in the buffer, just read the next byte */

	  *cp = (char) (data->in_buf[data->in_current_char_count ++]);

	  return fts_Success;
	}
      else if (data->in_current_event < (data->in_event_count - 1))
	{
	  /* No more bytes in the buffer,
	     but more events received, look at the next event */

	  (data->in_current_event)++;
	  sgi_midi_prepare_current_event(data);

	  /* And call recursively this function */

	  return sgi_midi_get(dev, cp);
	}
      else
	{
	  /* No more events, no more chars, do a recursive call to see
	   * if there is more data ready.
	   */

	  data->input_status = input_waiting;
	  return sgi_midi_get(dev, cp);
	}

    case reading_sysex:
      if (data->in_current_char_count < data->in_current_event_length)
	{
	  /* More bytes in the buffer, just read the next byte */

	  *cp = (char) (data->in_sysex_buf[data->in_current_char_count ++]);

	  return fts_Success;
	}
      else if (data->in_current_event < (data->in_event_count - 1))
	{
	  /* No more bytes in the buffer,
	     but more events received, look at the next event */

	  (data->in_current_event)++;
	  sgi_midi_prepare_current_event(data);

	  /* And call recursively this function */

	  return sgi_midi_get(dev, cp);
	}
      else
	{
	  /* No more events, no more chars, do a recursive call to see
	   * if there is more data ready.
	   */

	  data->input_status = input_waiting;
	  mdFree(data->in_sysex_buf);
	  return sgi_midi_get(dev, cp);
	}

    case input_waiting:
      {
	fd_set check;
	struct timeval timeout;
	int fd;
	int ret;

	/* First, test the fd using the select, otherwise MIreceive
	   is blocking !!! */

	fd = mdGetFd(data->in_port);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	FD_ZERO(&check);
	FD_SET(fd, &check);
	ret = select(64, &check, (fd_set *)0, (fd_set *)0, &timeout);

	if (FD_ISSET(fd, &check))
	  {
	    data->in_event_count = mdReceive(data->in_port,  data->in_midi_event,  SGI_MAX_MIDI_EV);

	    if (data->in_event_count <= 0)
	      return &fts_data_not_ready;
	    else
	      {
		data->in_current_event = 0;
		sgi_midi_prepare_current_event(data);
		return sgi_midi_get(dev, cp);
	      }
	  }
	else
	  return &fts_data_not_ready;
      }

    default :
      return &fts_data_not_ready;
    }
}


static fts_status_t
sgi_midi_put(fts_dev_t *dev, unsigned char c)
{
  int ret;
  sgimidi_dev_data *data = (sgimidi_dev_data *)dev->device_data;

  switch (data->output_status)
    {
    case output_waiting:
      switch (c & 0xf0)
	{
	case MD_NOTEOFF:
	case MD_NOTEON:
	case MD_POLYKEYPRESSURE:
	case MD_CONTROLCHANGE:
	case MD_PITCHBENDCHANGE:
	  data->output_status = message_need_first_and_second;
	  data->out_midi_event.msglen = 3;
	  break;
	case MD_PROGRAMCHANGE:
	case MD_CHANNELPRESSURE:
	  data->output_status = message_need_first;
	  data->out_midi_event.msglen = 2;
	  break;
	case MD_SYSEX:
	  data->output_status = getting_sysex;
	  data->out_sysex_count = 0;
	  return fts_Success;
	default:
	  data->output_status = output_waiting;
	  mdSend(data->out_port, &(data->out_midi_event), 1);
	  data->out_midi_event.msglen = 1;
	  break;
	}
      data->out_midi_event.msg[0] = c;
      return fts_Success;

    case message_need_first_and_second:
      data->out_midi_event.msg[1] = c;
      data->output_status = message_need_second;
      break;

    case message_need_first:
      data->out_midi_event.msg[1] = c;
      data->output_status = output_waiting;
      mdSend(data->out_port, &(data->out_midi_event), 1);
      break;

    case message_need_second:
      data->out_midi_event.msg[2] = c;
      data->output_status = output_waiting;
      ret = mdSend(data->out_port, &(data->out_midi_event), 1);
      break;

    case getting_sysex:

      if (data->out_sysex_count >= MAX_OUT_SYSEX)
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count] = c;

	  /* send */

	  data->out_midi_event.sysexmsg = (char *) data->out_sysex_buf;
	  data->out_midi_event.msglen   = data->out_sysex_count;
	  mdSend(data->out_port, &(data->out_midi_event), 1);

	  /* keep status to sysex, setting count to 0 and msg[0] to 0xf0 */

	  data->out_sysex_count = 0;
	  data->out_midi_event.msg[0] = MD_SYSEX; 
	  data->output_status = getting_sysex;
	}
      else if (c == MD_EOX) 
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count++] = c;

	  /* send */

	  data->out_midi_event.sysexmsg = (char *)data->out_sysex_buf;
	  data->out_midi_event.msglen   = data->out_sysex_count;
	  mdSend(data->out_port, &(data->out_midi_event), 1);

	  data->output_status = output_waiting;
	}
      else
	{
	  /* store char */

	  data->out_sysex_buf[data->out_sysex_count++] = c;
	}
    }

  return fts_Success;
}


