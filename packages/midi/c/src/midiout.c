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

#include "fts.h"

static fts_symbol_t sym_noteout = 0;
static fts_symbol_t sym_polyout = 0;
static fts_symbol_t sym_ctlout = 0;
static fts_symbol_t sym_pgmout = 0;
static fts_symbol_t sym_touchout = 0;
static fts_symbol_t sym_bendout = 0;

static void midiout_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void midiout_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/************************************************************
 *
 *  object
 *
 */

typedef struct _midiout_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int value;
  fts_midi_status_t status;
} midiout_t;

static void
midiout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;
  fts_symbol_t classname = fts_get_symbol(at);
  fts_midiport_t *port = 0;

  /* skip class name */
  ac--;
  at++;

  if(ac > 0 && fts_is_object(at) && fts_midiport_has_superclass(fts_get_object(at)))
    {
      port = (fts_midiport_t *)fts_get_object(at);

      /* skip port argument */
      ac--;
      at++;
    }

  /* if there is still no port just get default */
  if(port)
    this->port = port;
  else
    this->port = fts_midiport_get_default();

  if(classname == sym_noteout)
    this->status = fts_midi_status_note;
  else if(classname == sym_polyout)
    this->status = fts_midi_status_poly_pressure;
  else if(classname == sym_ctlout)
    this->status = fts_midi_status_control_change;
  else if(classname == sym_pgmout)
    this->status = fts_midi_status_program_change;
  else if(classname == sym_touchout)
    this->status = fts_midi_status_channel_pressure;
  else if(classname == sym_bendout)
    this->status = fts_midi_status_pitch_bend;
  
  this->channel = 1; /* defaults to 1 */
  this->value = 0;
    
  if(ac == 1)
    midiout_set_channel(o, 0, 0, 1, at);
  else if(ac > 1)
    {
      midiout_set_value(o, 0, 0, 1, at);
      midiout_set_channel(o, 0, 0, 1, at + 1);
    }
}

static void
midiout_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  int channel = fts_get_number_int(at);
  
  if(channel < 1)
    this->channel = 1;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;
}

static void
midiout_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  
  this->value = fts_get_number_int(at);
}

static void
midiout_pgm_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 2:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 1:
	  fts_midiport_output_channel_message(this->port, this->status, this->channel, fts_get_number_int(at), 0, 0.0);
	case 0:
	  break;
	}
    }
}

static void
midiout_note_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 3:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 2:
	  midiout_set_value(o, 0, 0, 1, at + 1);
	case 1:
	  fts_midiport_output_channel_message(this->port, this->status, this->channel, fts_get_number_int(at), this->value, 0.0);
	case 0:
	  break;
	}
    }
}

static void
midiout_poly_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 3:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 2:
	  midiout_set_value(o, 0, 0, 1, at + 1);
	case 1:
	  fts_midiport_output_channel_message(this->port, this->status, this->channel, this->value, fts_get_number_int(at), 0.0);
	case 0:
	  break;
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static fts_status_t
midiout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t classname = fts_get_symbol(at);

  if(classname == sym_noteout)
    {
      fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_int, midiout_note_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_note_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_note_send);

      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_value);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_value);

      fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);
    }
  else if(classname == sym_polyout)
    {
      fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_int, midiout_poly_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_poly_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_poly_send);

      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_value);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_value);

      fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);
    }
  else if(classname == sym_ctlout)
    {
      fts_class_init(cl, sizeof(midiout_t), 3 , 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_int, midiout_poly_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_poly_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_poly_send);

      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_value);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_value);

      fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);
    }
  else if(classname == sym_pgmout)
    {
      fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_int, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_pgm_send);
  
      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);
    }
  else if(classname == sym_touchout)
    {
      fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_int, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_pgm_send);
  
      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);
    }
  else if(classname == sym_bendout)
    {
      fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_int, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_pgm_send);
  
      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);
    }
  else
    return &fts_CannotInstantiate;    

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);
  /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiout_delete);*/

  return fts_Success;
}


void
midiout_config(void)
{
  sym_noteout = fts_new_symbol("noteout");
  sym_polyout = fts_new_symbol("polyout");
  sym_ctlout = fts_new_symbol("ctlout");
  sym_pgmout = fts_new_symbol("pgmout");
  sym_touchout = fts_new_symbol("touchout");
  sym_bendout = fts_new_symbol("bendout");

  fts_class_install(sym_noteout, midiout_instantiate);
  fts_class_install(sym_polyout, midiout_instantiate);
  fts_class_install(sym_ctlout, midiout_instantiate);
  fts_class_install(sym_pgmout, midiout_instantiate);
  fts_class_install(sym_touchout, midiout_instantiate);
  fts_class_install(sym_bendout, midiout_instantiate);
}
