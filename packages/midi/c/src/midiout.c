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
} midiout_t;

typedef struct _midiout_classdata_
{
  fts_midi_status_t status;
} midiout_classdata_t;

static void
midiout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;
  fts_symbol_t classname = fts_get_symbol(at);
  fts_midiport_t *port = 0;

  if(ac > 1 && fts_is_object(at + 1) && fts_object_get_class_name(fts_get_object(at + 1)) == fts_s__midiport)
    port = (fts_midiport_t *)fts_get_object(at + 1);

  /* skip class name */
  ac--;
  at++;

  if(ac > 1 && fts_is_object(at + 1) && fts_object_get_class_name(fts_get_object(at + 1)) == fts_s__midiport)
    {
      port = (fts_midiport_t *)fts_get_object(at + 1);

      /* skip port argument */
      ac--;
      at++;
    }

  /* if there is still no port just get default */
  if(!port)
    port = fts_midiport_get_default();

  this->port = port;
  this->channel = 1; /* defaults to 1 */
  this->value = 0;
    
  if(ac == 3)
    midiout_set_channel(o, 0, 0, 1, at + 2);
  else if(ac > 3)
    {
      midiout_set_value(o, 0, 0, 1, at + 2);
      midiout_set_channel(o, 0, 0, 1, at + 3);
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
  midiout_classdata_t *classdata = (midiout_classdata_t *)fts_object_get_user_data(o);

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 2:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 1:
	  fts_midiport_output_channel_message(this->port, classdata->status, this->channel, fts_get_number_int(at), 0, 0.0);
	case 0:
	  break;
	}
    }
}

static void
midiout_note_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  midiout_classdata_t *classdata = (midiout_classdata_t *)fts_object_get_user_data(o);

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
	  fts_midiport_output_channel_message(this->port, classdata->status, this->channel, fts_get_number_int(at), this->value, 0.0);
	case 0:
	  break;
	}
    }
}

static void
midiout_poly_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  midiout_classdata_t *classdata = (midiout_classdata_t *)fts_object_get_user_data(o);

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
	  fts_midiport_output_channel_message(this->port, classdata->status, this->channel, this->value, fts_get_number_int(at), 0.0);
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
  midiout_classdata_t *classdata = fts_malloc(sizeof(midiout_classdata_t));
  fts_symbol_t type;
  int n_ins = 0;

  /* skip class name */
  ac--;
  at++;

  /* skip port argument */
  if(fts_is_object(at + 1))
    {
      ac--;
      at++;
    }

  type = fts_get_symbol(at);

  if(type == fts_new_symbol("note") || type == fts_new_symbol("nt"))
    {
      classdata->status = fts_midi_status_note;
      n_ins = 3;
    }
  else if(type == fts_new_symbol("poly-pressure") || type == fts_new_symbol("pp"))
    {
      classdata->status = fts_midi_status_poly_pressure;
      n_ins = 3;
    }
  else if(type == fts_new_symbol("control-change") || type == fts_new_symbol("cc"))
    {
      classdata->status = fts_midi_status_control_change;
      n_ins = 3 ;
    }
  else if(type == fts_new_symbol("program-change") || type == fts_new_symbol("pc"))
    {
      classdata->status = fts_midi_status_program_change;
      n_ins = 2;
    }
  else if(type == fts_new_symbol("channel-pressure") || type == fts_new_symbol("cp"))
    {
      classdata->status = fts_midi_status_channel_pressure;
      n_ins = 2;
    }
  else if(type == fts_new_symbol("pitch-bend") || type == fts_new_symbol("pb"))
    {
      classdata->status = fts_midi_status_pitch_bend;
      n_ins = 2;
    }
  else
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), n_ins, 0, (void *)classdata);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);
  /*fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiout_delete);*/

  if(n_ins == 2)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiout_pgm_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiout_pgm_send);
  
      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);
    }
  else if(n_ins == 3)
    {
      if(type == fts_new_symbol("note") || type == fts_new_symbol("nt"))
	{
	  fts_method_define_varargs(cl, 0, fts_s_int, midiout_note_send);
	  fts_method_define_varargs(cl, 0, fts_s_float, midiout_note_send);
	  fts_method_define_varargs(cl, 0, fts_s_list, midiout_note_send);
	}
      else
	{
	  fts_method_define_varargs(cl, 0, fts_s_int, midiout_poly_send);
	  fts_method_define_varargs(cl, 0, fts_s_float, midiout_poly_send);
	  fts_method_define_varargs(cl, 0, fts_s_list, midiout_poly_send);
	}
  
      fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_value);
      fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_value);

      fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
      fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);
    }
  
  return fts_Success;
}


int
midiout_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  /* skip class name */
  ac0--;
  ac1--;
  at0++;
  at1++;

  /* skip port argument */
  if(fts_is_object(at0))
    {
      ac0--;
      at0++;
    }

  if(fts_is_object(at1))
    {
      ac1--;
      at1++;      
    }

  if(ac0 > 0 && ac1 > 0 && fts_is_symbol(at0) && fts_is_symbol(at1))
    return (fts_get_symbol(at0) == fts_get_symbol(at1));
  else
    return 0;
}

void
midiout_config(void)
{
  fts_metaclass_install(fts_new_symbol("midiout"), midiout_instantiate, midiout_equiv);
}
