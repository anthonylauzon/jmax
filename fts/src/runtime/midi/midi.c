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

/* 
   This file include the FTS level MIDI implementation.

   It should quickly became something more complex,
   including multiport support, device naming and control.

   But for now, it just reproduce the old mechanism, and support
   only one port.

   The actual parsing will always be in the application, anyway;
   a MAX independent add-on parsing library may be added if needed.


   This release support Midi Time Code: the simplest implementation possible:
   we support a 2 frame temporal resolution, (later can become 1 or 
   either quarter frame, and we always get the time from
   the qf messages (or the last mtc full message); 30 drop-frame
   is not supported; every port provide a long with the time converted
   to milliseconds.

   Also locking will not work very well if there are a lot of movement
   without the MTC full message in between.
   */

#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@ */
fts_symbol_t fts_midi_hack_default_device_name = 0;
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@ */

/******************************************************************************/
/*                                                                            */
/*             error description definition                                   */
/*                                                                            */
/******************************************************************************/

static fts_status_description_t invalid_midi_dev =
{
  "Invalid logical midi device specification"
};


/******************************************************************************/
/*                                                                            */
/*             MIDI module definition                                         */
/*                                                                            */
/******************************************************************************/

static void midi_init(void);
static void midi_shutdown(void);

static fts_status_t fts_set_midi_logical_dev(fts_dev_t *dev, int ac, const fts_atom_t *at);
static fts_dev_t   *fts_get_midi_logical_dev(int ac, const fts_atom_t *at);
static fts_status_t fts_unset_midi_logical_dev(int ac, const fts_atom_t *at);
static fts_status_t fts_reset_midi_logical_dev(void);
static void         fts_midi_close_all(void);

fts_module_t fts_midi_module = {"Midi", "Midi communication", midi_init, midi_shutdown, 0};

extern void fts_midiport_config(void);

static void midi_init(void)
{
  fts_declare_logical_dev(fts_new_symbol("midi"),
			  fts_char_dev,
			  fts_set_midi_logical_dev,
			  fts_get_midi_logical_dev,
			  fts_unset_midi_logical_dev,
			  fts_reset_midi_logical_dev
			  );

  fts_midiport_config();
}


static void midi_shutdown(void)
{
  fts_midi_close_all();
}


/******************************************************************************/
/*                                                                            */
/*             MIDI Port and logic device handling                            */
/*                                                                            */
/******************************************************************************/

struct midi_closure
{
  midi_action_t fun;
  void *user_data;
  struct midi_closure *next;
};
  
struct _fts_midi_port
{
  /* device pointer  */

  fts_dev_t  *dev;
  int       idx;

  /* action support */

  struct midi_closure *action_table[MAX_FTS_MIDI_EV+1];

  /* parsing status  */

  int chan;	
  int arg1;
  int cmd;

  enum {midi_normal, midi_sysex, midi_mtc_qf, midi_mtc_fm, midi_mtc_sysex, midi_sysex_system} status;
  int argc;

  /* accumulated mtc for the MTC full message */

  enum {mtc_invalid, mtc_valid, mtc_coming} mtc_status;
  int mtc_fm_count;
  unsigned char mtc_type;
  unsigned char mtc_hour;
  unsigned char mtc_min;
  unsigned char mtc_sec;
  unsigned char mtc_frame;

  double mtc_time;		/* the time in millisecond corresponding to the MTC received */

  /* next port */

  struct _fts_midi_port *next;
};

static fts_midi_port_t *port_list = 0;

/* Operations to get a midi port */

fts_midi_port_t *
fts_midi_get_port(int idx)
{
  fts_midi_port_t *p;

  p = port_list;

  while (p)
    {
      if (p->idx == idx)
	return p;
	
      p = p->next;
    }

  return (fts_midi_port_t *)0;
}


double *
fts_get_midi_time_code_p(int idx)
{
  fts_midi_port_t *p;

  p = fts_midi_get_port(idx);

  if (p)
    return &p->mtc_time;
  else
    return (double *)0;
}


static fts_status_t
fts_set_midi_logical_dev(fts_dev_t *dev, int ac, const fts_atom_t *at)
{
  if (ac == 1 && fts_is_int(at))
    {
      fts_midi_port_t *p;
      int idx;

      idx = fts_get_int(at);
      p = fts_midi_get_port(idx);

      /* @@@@@@@@@@@@@@@@@@@@@@@@@@@ */
      if(idx == 0)
	fts_midi_hack_default_device_name = fts_get_symbol(at + 4);
      /* @@@@@@@@@@@@@@@@@@@@@@@@@@@ */

      if (p)
	{
	  p->dev = dev;
	}
      else
	{
	  int i;

	  p = (fts_midi_port_t *) fts_malloc(sizeof(fts_midi_port_t));

	  p->dev  = dev;
	  p->idx  = idx;

	  p->next = port_list;
	  port_list = p;

	  /* initialize the midi parsr status  */

	  p->cmd  = 0;
	  p->argc = 0;
	  p->status = midi_normal;
	  p->mtc_status =  mtc_invalid;
	  p->mtc_time = 0.0;
	  p->mtc_type = 0;
	  p->mtc_hour = 0;
	  p->mtc_min  = 0;
	  p->mtc_sec  = 0;
	  p->mtc_frame = 0;

	  /* initialize the action table */
	  
	  for (i = 0; i <= MAX_FTS_MIDI_EV; i++)
	    p->action_table[i] = (struct midi_closure *)0;

	  /* if the index is zero, define the mtc clock */

	  if (idx == 0)
	    fts_clock_define_protected(fts_new_symbol("mtc"), &(p->mtc_time));
	}

      return fts_Success;
    }
  else
    return &invalid_midi_dev;	/* error: invalid MIDI device specification */
}


static fts_dev_t *
fts_get_midi_logical_dev(int ac, const fts_atom_t *at)
{
  if (ac == 1 && fts_is_int(at))
    {
      fts_midi_port_t *p;

      p = fts_midi_get_port(fts_get_int(at));

      if (p)
	return p->dev;
      else
	return (fts_dev_t *)0;
    }
  else
    return (fts_dev_t *)0;
}


static fts_status_t
fts_unset_midi_logical_dev(int ac, const fts_atom_t *at)
{
  if (ac == 1 && fts_is_int(at))
    {
      fts_midi_port_t **p;
      int idx;

      /* indirect precursor iteration */

      idx = fts_get_int(at);

      p = &port_list;

      while (*p)
	{
	  if ((*p)->idx == idx)
	    {
	      fts_midi_port_t *tmp;

	      tmp = (*p);
	      *p = (*p)->next;
	      fts_free(tmp);
	      break;
	    }
	  else
	    p = &((*p)->next);
	}

      return fts_Success;
    }
  else
    return &invalid_midi_dev;	/* error: invalid MIDI device specification */
}


fts_status_t fts_reset_midi_logical_dev(void)
{
  fts_midi_close_all();
  return fts_Success;
}


static void
fts_midi_close_all(void)
{
  fts_atom_t a;

  /* indirect precursor iteration */

  while (port_list)
    {
      fts_set_int(&a, port_list->idx);

      fts_close_logical_device(fts_new_symbol("midi"), 1, &a);
    }
}


/******************************************************************************/
/*                                                                            */
/*             MIDI Polling function                                          */
/*                                                                            */
/******************************************************************************/

/* Installed in the scheduler */

static void fts_run_midi_parser(fts_midi_port_t *port, int nextbyte);

void fts_midi_poll(void)
{
  fts_midi_port_t *p;

  for (p = port_list; p; p = p->next)
    {
      unsigned char midibyte;
      fts_status_t ret;

      ret = fts_char_dev_get(p->dev, &midibyte);

      while (ret == fts_Success)
	{
	  fts_run_midi_parser(p, midibyte); 
	  ret =  fts_char_dev_get(p->dev, &midibyte);
	}
    }
}

/******************************************************************************/
/*                                                                            */
/*             MIDI Input/Output functions                                    */
/*                                                                            */
/******************************************************************************/

/* Input is done indirectly, by installing handler functions for
   MIDI events.
   Output is done directly
*/
/* NOTE: the rule about null port action is that must be taken care
   at the fts kernel level, and not above: this is because future version
   will store the bindings and functions anyway, before the
   actual logical device is installed; 
*/

/* handling of the action table */

void
fts_midi_install_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data)
{
  if (port)
    {
      struct midi_closure *p;

      p = (struct midi_closure *) fts_malloc(sizeof(struct midi_closure));

      p->fun = fun;
      p->user_data = user_data;
      p->next = port->action_table[midi_ev];

      port->action_table[midi_ev] = p;
    }
}

void
fts_midi_deinstall_fun(fts_midi_port_t *port, int midi_ev, midi_action_t fun, void *user_data)
{
  if (port)
    {
      struct midi_closure **p;	/* indirect precursor */

      p = & (port->action_table[midi_ev]);
      
      while (*p)
	{
	  if (((*p)->fun == fun) && ((*p)->user_data == user_data))
	    {
	      struct midi_closure *tmp;
	      
	      tmp = *p;

	      *p = (*p)->next;
	      fts_free(tmp);
	      break;
	    }
	  else
	    p = &((*p)->next);
	}
    }
}

static void
run_midi_action(fts_midi_port_t *port, int midi_ev, int ac, fts_atom_t *at)
{
  struct midi_closure *p;

  if (port)
    for (p = port->action_table[midi_ev]; p; p = p->next)
      (* p->fun)(port, midi_ev, p->user_data, ac, at);
}


void
fts_midi_send(fts_midi_port_t *p, long val)
{
  if (p)
    fts_char_dev_put(p->dev, val);
}


/******************************************************************************/
/*                                                                            */
/*             MIDI Parsing                                                   */
/*                                                                            */
/******************************************************************************/

/* the parser algorithm  is derived from the orginal code by Miller Puckette */

#define NOTEOFF 0
#define NOTEON 1
#define POLY 2
#define CTL 3
#define PGM 4
#define TCH 5
#define BEND 6

#define FR_24   0
#define FR_25   1
#define FR_30DF 2
#define FR_30   3

/* drop frame don't supported !!! */

static void
fts_midi_compute_time_code(fts_midi_port_t *port)
{
  fts_atom_t av[5];
  float total_secs;
  double new_time;

  total_secs = (((port->mtc_hour * 60) + port->mtc_min) * 60) + port->mtc_sec;

  /* add two to the frames to compensate the MTC delay */

  new_time = total_secs * 1000.0 + (((float) port->mtc_frame + 2) *
				    (1000.0 / (float) (port->mtc_type == 0 ? 24 :
						       (port->mtc_type == 1 ? 25 : 30))));

  if (new_time < port->mtc_time)
    fts_clock_reset(fts_new_symbol("mtc"));

  port->mtc_time = new_time;

  /* Raise the midi action */

  fts_set_int(&av[0], port->mtc_type);
  fts_set_int(&av[1], port->mtc_hour);
  fts_set_int(&av[2], port->mtc_min);
  fts_set_int(&av[3], port->mtc_sec);
  fts_set_int(&av[4], port->mtc_frame);
  run_midi_action(port, FTS_MIDI_MTC, 5 , av);
}



static void
fts_run_midi_parser(fts_midi_port_t *port, int nextbyte)	
{
  fts_atom_t av[5];

  if (nextbyte >= 0xf8)
    {
      fts_set_int(av, nextbyte);
      run_midi_action(port, FTS_MIDI_REALTIME, 1 , av);

      return;
    }
  
  fts_set_int(&av[0], nextbyte);
  run_midi_action(port, FTS_MIDI_BYTE, 1 , av);

  if (port->status == midi_mtc_fm)
    {
      fts_set_int(av, nextbyte);
      run_midi_action(port, FTS_MIDI_SYSEX, 1 , av);

      switch (port->mtc_fm_count)
	{
	case 0:
	  port->mtc_type = (nextbyte & 0x60) >> 5;
	  port->mtc_hour = (nextbyte & 0x1f);
	  break;
	case 1:
	  port->mtc_min = nextbyte;
	  break;
	case 2:
	  port->mtc_sec = nextbyte;
	  break;
	case 3:
	  port->mtc_frame = nextbyte;
	  break;
	default:
	  break;		/* don'thandle here the end of the message */
	}

      port->mtc_fm_count++;

      if (nextbyte == 0xf7)
	{
	  port->mtc_status =  mtc_valid;
	  fts_midi_compute_time_code(port);
	}
    }
  else if (port->status == midi_mtc_sysex)
    {
      fts_set_int(av, nextbyte);
      run_midi_action(port, FTS_MIDI_SYSEX, 1 , av);

      if (nextbyte == 0x01)
	{
	  port->status = midi_mtc_fm;
	  port->mtc_fm_count  = 0;
	}
    }
  else if (port->status == midi_sysex_system)
    {
      fts_set_int(av, nextbyte);
      run_midi_action(port, FTS_MIDI_SYSEX, 1 , av);

      if (nextbyte == 0x01)
	port->status = midi_mtc_sysex;
    }
  else if (port->status == midi_sysex)
    {
      fts_set_int(av, nextbyte);
      run_midi_action(port, FTS_MIDI_SYSEX, 1 , av);

      if (nextbyte == 0xf7)
	port->status = midi_normal;
      else if (nextbyte == 0x7f)
	port->status = midi_sysex_system;
    }
  else if (port->status == midi_mtc_qf)
    {

      if ((nextbyte & 0xf0) == 0x00)
	port->mtc_status = mtc_coming;

      switch ((nextbyte & 0xf0) >> 4)
	{
	case 0:
	  if (port->mtc_status == mtc_invalid)
	    port->mtc_status = mtc_coming;
	  else if (port->mtc_status == mtc_coming)
	    port->mtc_status = mtc_valid;

	  if (port->mtc_status == mtc_valid)
	    {
	      /* compute the time here, before considering
		 the frame LSB, because this is the time code 
		 of the previous previous frame; compute mtc
		 will add two frames in time */

	      fts_midi_compute_time_code(port);
	    }

	  port->mtc_frame = (port->mtc_frame & 0xf0) | (nextbyte & 0x0f);
	  break;
	case 1:
	  port->mtc_frame = (port->mtc_frame & 0x0f) | (nextbyte & 0x0f) << 4;
	  break;
	case 2:
	  port->mtc_sec = (port->mtc_sec & 0xf0) | (nextbyte & 0x0f);
	  break;
	case 3:
	  port->mtc_sec = (port->mtc_sec & 0x0f) | (nextbyte & 0x0f) << 4;
	  break;
	case 4:
	  port->mtc_min = (port->mtc_min & 0xf0) | (nextbyte & 0x0f);
	  break;
	case 5:
	  port->mtc_min = (port->mtc_min & 0x0f) | (nextbyte & 0x0f) << 4;
	  break;
	case 6:
	  port->mtc_hour = (port->mtc_hour & 0xf0) | (nextbyte & 0x0f);
	  break;
	case 7:
	  port->mtc_hour = (port->mtc_hour & 0x0f) | (nextbyte & 0x01) << 4;
	  port->mtc_type = (nextbyte & 0x60) >> 5;
	  break;
	}

      port->status = midi_normal;
    }
  else if (port->status == midi_normal)
    {
      if (nextbyte == 0xf1)
	port->status = midi_mtc_qf;
      else if (nextbyte == 0xf0)
	{
	  port->status = midi_sysex;
	  fts_set_int(&av[0], nextbyte);
	  run_midi_action(port, FTS_MIDI_SYSEX, 1 , av);
	}
      else if (nextbyte & 0x80)
	{
	  port->cmd = (nextbyte >> 4) & 7;
	  port->chan = (nextbyte & 0xf) + 1; /* chan go from 1 to 16 */
	  port->argc = 0;
	}
      else switch (port->cmd)
	{
	case NOTEON:
	  if (port->argc == 0)
	    {
	      port->arg1 = nextbyte;
	      port->argc = 1;
	    }
	  else if (port->argc == 1)
	    {
	      fts_set_int(av + 0, port->chan);
	      fts_set_int(av + 1, port->arg1);
	      fts_set_int(av + 2, nextbyte);

	      run_midi_action(port, FTS_MIDI_NOTE, 3 , av);
	      run_midi_action(port, FTS_MIDI_NOTE_CH(port->chan), 3, av);

	      port->argc = 0;
	    }
	  break;

	case POLY:
	  /* polyphonic aftertouch */
	  if (port->argc == 0)
	    {
	      port->arg1 = nextbyte;
	      port->argc = 1;
	    }
	  else if (port->argc == 1)
	    {
	      fts_set_int(av + 0, port->chan);
	      fts_set_int(av + 1, port->arg1);
	      fts_set_int(av + 2, nextbyte);

	      run_midi_action(port, FTS_MIDI_POLY_AFTERTOUCH, 3 , av);
	      run_midi_action(port, FTS_MIDI_POLY_AFTERTOUCH_CH(port->chan), 3 , av);

	      port->argc = 0;
	    }
	  break;

	case CTL:
	  /* controller change */
	  if (port->argc == 0)
	    {
	      port->arg1 = nextbyte;
	      port->argc = 1;
	    }
	  else if (port->argc == 1)
	    {
	      fts_set_int(av + 0, port->chan);
	      fts_set_int(av + 1, port->arg1);
	      fts_set_int(av + 2, nextbyte);

	      run_midi_action(port, FTS_MIDI_CONTROLLER, 3 , av);
	      run_midi_action(port, FTS_MIDI_CONTROLLER_CH(port->chan), 3 , av);

	      port->argc = 0;
	    }
	  break;

	case BEND:	/* pitchbend */
	  if (port->argc == 0)
	    {
	      port->arg1 = nextbyte;
	      port->argc = 1;
	    }
	  else if (port->argc == 1)
	    {
	      fts_set_int(av + 0, port->chan);
	      fts_set_int(av + 1, port->arg1);
	      fts_set_int(av + 2, nextbyte);

	      run_midi_action(port, FTS_MIDI_PITCH_BEND, 3 , av);
	      run_midi_action(port, FTS_MIDI_PITCH_BEND_CH(port->chan), 3 , av);

	      port->argc = 0;
	    }
	  break;

	case TCH:		
	  {
	    /* channel aftertouch */

	    fts_set_int(av + 0, port->chan);
	    fts_set_int(av + 1, nextbyte);

	    run_midi_action(port, FTS_MIDI_CHANNEL_AFTERTOUCH, 2 , av);
	    run_midi_action(port, FTS_MIDI_CHANNEL_AFTERTOUCH_CH(port->chan), 2 , av);

	    port->argc = 0;
	    break;
	  }

	case PGM:
	  {
	    /* program change */

	    fts_set_int(av + 0, port->chan);
	    fts_set_int(av + 1, nextbyte);

	    run_midi_action(port, FTS_MIDI_PROGRAM_CHANGE, 2 , av);
	    run_midi_action(port, FTS_MIDI_PROGRAM_CHANGE_CH(port->chan), 2 , av);

	    port->argc = 0;
	    break;
	  }

	case NOTEOFF:
	  if (port->argc == 0)
	    {
	      port->arg1 = nextbyte;
	      port->argc = 1;
	    }
	  else if (port->argc == 1)
	    {
	      fts_set_int(av + 0, port->chan);
	      fts_set_int(av + 1, port->arg1);
	      fts_set_int(av + 2, 0);

	      run_midi_action(port, FTS_MIDI_NOTE, 3 , av);
	      run_midi_action(port, FTS_MIDI_NOTE_CH(port->chan), 3 , av);

	      port->argc = 0;
	    }
	  break;
	}
    }
}
