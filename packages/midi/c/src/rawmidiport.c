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
#include <fts/fts.h>

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

#define SYSEX_BLOCK_SIZE 512

typedef struct _rawmidiport_
{
  fts_midiparser_t head; /* parser is a MIDI port */
  fts_bytestream_t *stream;

  /* system exclusive output buffer */
  int sysex_head;
} rawmidiport_t;

/************************************************************
 *
 *  bytestream input callback
 *
 */

static void
rawmidiport_input(fts_object_t *o, int n, const unsigned char *c)
{
  fts_midiparser_t *parser = (fts_midiparser_t *)o;
  int i;
  
  for(i=0; i<n; i++)
    fts_midiparser_byte(parser, c[i]);
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */

static void
rawmidiport_send_note(fts_object_t *o, int channel, int number, int value, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, STATUS_BYTE_NOTE | ((channel - 1) & 0x0F));
  fts_bytestream_output_char(this->stream, (unsigned char)(number & 127));
  fts_bytestream_output_char(this->stream, (unsigned char)(value & 127));
  fts_bytestream_flush(this->stream);
}

static void
rawmidiport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, STATUS_BYTE_POLY_PRESSURE | ((channel - 1) & 0x0F));
  fts_bytestream_output_char(this->stream, (unsigned char)(number & 127));
  fts_bytestream_output_char(this->stream, (unsigned char)(value & 127));
  fts_bytestream_flush(this->stream);
}

static void
rawmidiport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, STATUS_BYTE_CONTROL_CHANGE | ((channel - 1) & 0x0F));
  fts_bytestream_output_char(this->stream, (unsigned char)(number & 127));
  fts_bytestream_output_char(this->stream, (unsigned char)(value & 127));
  fts_bytestream_flush(this->stream);
}

static void
rawmidiport_send_program_change(fts_object_t *o, int channel, int value, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, STATUS_BYTE_PROGRAM_CHANGE | ((channel - 1) & 0x0F));
  fts_bytestream_output_char(this->stream, (unsigned char)(value & 127));
  fts_bytestream_flush(this->stream);
}

static void
rawmidiport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, STATUS_BYTE_CHANNEL_PRESSURE | ((channel - 1) & 0x0F));
  fts_bytestream_output_char(this->stream, (unsigned char)(value & 127));
  fts_bytestream_flush(this->stream);
}

static void
rawmidiport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, STATUS_BYTE_PITCH_BEND | ((channel - 1) & 0x0F));
  fts_bytestream_output_char(this->stream, (unsigned char)(value & 127)); /* LSB */
  fts_bytestream_output_char(this->stream, (unsigned char)(value >> 7)); /* MSB */
  fts_bytestream_flush(this->stream);
}

static void
rawmidiport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  if(this->sysex_head == 0)
    {
      this->sysex_head = 1;

      fts_bytestream_output_char(this->stream, (unsigned char)STATUS_BYTE_SYSEX);
      fts_bytestream_output_char(this->stream, (unsigned char)STATUS_BYTE_SYSEX_REALTIME);
    }

  fts_bytestream_output_char(this->stream, (unsigned char)(value & 0x7f));
}

static void
rawmidiport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  fts_bytestream_output_char(this->stream, (unsigned char)STATUS_BYTE_SYSEX_END);
  fts_bytestream_flush(this->stream);

  this->sysex_head = 0;
}

static fts_midiport_output_functions_t rawmidiport_output_functions =
{
  rawmidiport_send_note,
  rawmidiport_send_poly_pressure,
  rawmidiport_send_control_change,
  rawmidiport_send_program_change,
  rawmidiport_send_channel_pressure,
  rawmidiport_send_pitch_bend,
  rawmidiport_send_system_exclusive_byte,
  rawmidiport_send_system_exclusive_flush,
};

/************************************************************
 *
 *  get midiport variable
 *
 */
static void
rawmidiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  rawmidiport_t *this = (rawmidiport_t *)o;

  if(this->stream)
    fts_set_object(value, o);
  else
    fts_set_void(value);
}

/************************************************************
 *
 *  class
 *
 */
static void
rawmidiport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  rawmidiport_t *this = (rawmidiport_t *)o;
  fts_midiport_t *port = (fts_midiport_t *)o;
  fts_midiparser_t *parser = (fts_midiparser_t *)o;

  this->stream = (fts_bytestream_t *)fts_get_object(at + 1);
  
  fts_midiport_init(port);
  fts_midiparser_init(parser);

  if(fts_bytestream_is_input(this->stream))
    {
      fts_midiport_set_input(port);
      fts_bytestream_add_listener(this->stream, o, rawmidiport_input);
    }

  if(fts_bytestream_is_output(this->stream))
    fts_midiport_set_output(port, &rawmidiport_output_functions);

  this->sysex_head = 0;
}

static void 
rawmidiport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  rawmidiport_t *this = (rawmidiport_t *)o;
  fts_midiport_t *port = (fts_midiport_t *)o;

  if(fts_bytestream_is_input(this->stream))
    fts_bytestream_remove_listener(this->stream, o);

  fts_midiport_delete(port);
}

static int 
rawmidiport_check(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1))
    {
      fts_object_t *obj = fts_get_object(at + 1);
      
      if(fts_bytestream_check(obj) && (fts_bytestream_is_output((fts_bytestream_t *)obj) || fts_bytestream_is_input((fts_bytestream_t *)obj)))
	return 1;
    }
  
  return 0;
}

static fts_status_t
rawmidiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(rawmidiport_check(ac, at))
    {
      fts_class_init(cl, sizeof(rawmidiport_t), 1, 0, 0);

      fts_midiport_class_init(cl);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, rawmidiport_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, rawmidiport_delete);

      /* define variable */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, rawmidiport_get_state);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static int 
rawmidiport_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  return rawmidiport_check(ac1, at1);
}

void
rawmidiport_config(void)
{
  fts_metaclass_install(fts_new_symbol("rawmidiport"), rawmidiport_instantiate, rawmidiport_equiv);
}
