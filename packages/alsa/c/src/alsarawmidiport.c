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
 * Authors: Riccardo Borghesi, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/asoundlib.h>
#include <linux/asound.h>

#include <fts/fts.h>

static fts_symbol_t s_hw_0_0;

/* MIDI status bytes */
#define STATUS_BYTE_NOTE 0x90
#define STATUS_BYTE_POLY_PRESSURE 0xa0
#define STATUS_BYTE_CONTROL_CHANGE 0xb0
#define STATUS_BYTE_PROGRAM_CHANGE 0xc0
#define STATUS_BYTE_CHANNEL_PRESSURE 0xd0
#define STATUS_BYTE_PITCH_BEND 0xe0

#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_SYSEX_REALTIME 0x7f
#define STATUS_BYTE_SYSEX_END 0xf7

#define BUFFER_LENGTH 512

typedef struct _alsarawmidiport_
{
  fts_midiparser_t head; /* parser is a MIDI port */

  snd_rawmidi_t *handle_in;
  snd_rawmidi_t *handle_out;
  int fd;

  unsigned char receive_buffer[BUFFER_LENGTH];
  /* system exclusive output buffer */
  int sysex_head;
} alsarawmidiport_t;

static void alsarawmidiport_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  fts_midiparser_t *parser = (fts_midiparser_t *)o;
  int n, i;

  n = snd_rawmidi_read( this->handle_in, this->receive_buffer, BUFFER_LENGTH);
  if ( n <= 0 && n != -EAGAIN)
    {
      post( "snd_rawmidi_read returns %d !!!\n", n);
      return;
    }

  for ( i = 0; i < n; i++)
    {
      fts_midiparser_byte( parser, this->receive_buffer[i]);
    }
}


static void alsarawmidiport_send_note(fts_object_t *o, int channel, int number, int value, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];
  int n;

  buffer[0] = STATUS_BYTE_NOTE | ((channel - 1) & 0x0F);
  buffer[1] = (unsigned char)(number & 127);
  buffer[2] = (unsigned char)(value & 127);

  n = snd_rawmidi_write( this->handle_out, buffer, 3);
}

static void
alsarawmidiport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];
  int n;

  buffer[0] = STATUS_BYTE_POLY_PRESSURE | ((channel - 1) & 0x0F);
  buffer[1] = (unsigned char)(number & 127);
  buffer[2] = (unsigned char)(value & 127);

  n = snd_rawmidi_write( this->handle_out, buffer, 3);
}

static void
alsarawmidiport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];
  int n;

  buffer[0] = STATUS_BYTE_CONTROL_CHANGE | ((channel - 1) & 0x0F);
  buffer[1] = (unsigned char)(number & 127);
  buffer[2] = (unsigned char)(value & 127);

  n = snd_rawmidi_write( this->handle_out, buffer, 3);
}

static void
alsarawmidiport_send_program_change(fts_object_t *o, int channel, int value, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];
  int n;

  buffer[0] = STATUS_BYTE_PROGRAM_CHANGE | ((channel - 1) & 0x0F);
  buffer[1] = (unsigned char)(value & 127);

  n = snd_rawmidi_write( this->handle_out, buffer, 2);
}

static void
alsarawmidiport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];
  int n;

  buffer[0] = STATUS_BYTE_CHANNEL_PRESSURE | ((channel - 1) & 0x0F);
  buffer[1] = (unsigned char)(value & 127);

  n = snd_rawmidi_write( this->handle_out, buffer, 2);
}

static void
alsarawmidiport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];
  int n;

  buffer[0] = STATUS_BYTE_PITCH_BEND | ((channel - 1) & 0x0F);
  buffer[1] = (unsigned char)(value & 127); /* LSB */
  buffer[2] = (unsigned char)(value >> 7); /* MSB */

  n = snd_rawmidi_write( this->handle_out, buffer, 3);
}

static void
alsarawmidiport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[2];
  int n = 0;

  if(this->sysex_head == 0)
    {
      buffer[0] = (unsigned char)STATUS_BYTE_SYSEX;
      buffer[1] = (unsigned char)(value & 0x7f);
      snd_rawmidi_write( this->handle_out, buffer, 2);

      this->sysex_head = 1;
    }
  else
    {
      buffer[0] = (unsigned char)(value & 0x7f);
      snd_rawmidi_write( this->handle_out, buffer, 1);
    }
}

static void
alsarawmidiport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  unsigned char buffer[3];

  buffer[0] = (unsigned char)STATUS_BYTE_SYSEX_END;
  snd_rawmidi_write( this->handle_out, buffer, 1);

  this->sysex_head = 0;
}

static fts_midiport_output_functions_t alsarawmidiport_output_functions =
{
  alsarawmidiport_send_note,
  alsarawmidiport_send_poly_pressure,
  alsarawmidiport_send_control_change,
  alsarawmidiport_send_program_change,
  alsarawmidiport_send_channel_pressure,
  alsarawmidiport_send_pitch_bend,
  alsarawmidiport_send_system_exclusive_byte,
  alsarawmidiport_send_system_exclusive_flush,
};



static void alsarawmidiport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  fts_midiport_t *port = (fts_midiport_t *)o;
  fts_midiparser_t *parser = (fts_midiparser_t *)o;
  char name[256];
  int err, fd;
  struct pollfd fds;

  ac--;
  at++;

  strcpy( name, fts_symbol_name( fts_get_symbol_arg( ac, at, 0, s_hw_0_0)) );

  if( (err = snd_rawmidi_open( &this->handle_in, &this->handle_out, name, O_RDWR | SND_RAWMIDI_NONBLOCK)) < 0)
    {
      fts_object_set_error(o, "Error opening ALSA raw MIDI port (%s)", snd_strerror( err));
      post("alsarawmidiport: cannot open ALSA raw MIDI port %s (%s)\n", name, snd_strerror( err));
      return;
    }

  if ( snd_rawmidi_poll_descriptors( this->handle_in, &fds, 1) == 0)
    {
      fts_object_set_error(o, "Cannot get file descriptor");
      return;
    }

  this->fd = fds.fd;

  fts_sched_add( (fts_object_t *)this, FTS_SCHED_READ, this->fd);

  fts_midiport_init( port);
  fts_midiparser_init( parser);

  fts_midiport_set_input( port);
  fts_midiport_set_output( port, &alsarawmidiport_output_functions);

  this->sysex_head = 0;
}

static void alsarawmidiport_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;

  fts_sched_remove( (fts_object_t *)this);

  if (this->handle_in)
    {
      snd_rawmidi_drain( this->handle_in); 
      snd_rawmidi_close( this->handle_in);	
    }

  if ( this->handle_out)
    {
      snd_rawmidi_drain( this->handle_out); 
      snd_rawmidi_close( this->handle_out);	
    }
}

static void alsarawmidiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;

  if( this->handle_in && this->handle_out)
    fts_set_object( value, o);
  else
    fts_set_void( value);
}

static fts_status_t alsarawmidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof( alsarawmidiport_t), 0, 0, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, alsarawmidiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, alsarawmidiport_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, alsarawmidiport_select);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, alsarawmidiport_get_state);

  return fts_Success;
}


/* ********************************************************************** */
/* init function                                                          */
/* ********************************************************************** */

void alsarawmidiport_config( void)
{
  fts_class_install( fts_new_symbol("alsarawmidiport"), alsarawmidiport_instantiate);

  s_hw_0_0 = fts_new_symbol( "hw:0,0");
}
