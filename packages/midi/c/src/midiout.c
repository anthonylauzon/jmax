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

#include <fts/fts.h>

typedef struct _midiout_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int number;
} midiout_t;

static fts_symbol_t sym_omni = 0;

static void
midiout_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  int channel = fts_get_number_int(at) - 1;
  
  if(channel < 0)
    this->channel = 0;
  else if(channel > 15)
    this->channel = 15;
  else
    this->channel = channel;
}

static void
midiout_set_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  int number = fts_get_number_int(at);
  
  if(number < 0)
    this->number = 0;
  else if(number > 127)
    this->number = 127;
  else
    this->number = number;
}

static void
midiout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_midiport_output(this->port, event, 0.0);
}

static void
noteout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	midiout_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_number(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  fts_midievent_t *event;
	  int note = fts_get_number_int(at);

	  if(note < 0)
	    note = 0;
	  else if(note > 127)
	    note = 127;
	    
	  event = fts_midievent_note_new(this->channel, note, this->number);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

static void
polyout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	midiout_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_number(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  fts_midievent_t *event;
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  event = fts_midievent_poly_pressure_new(this->channel, this->number, value);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

static void
ctlout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	midiout_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_number(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  fts_midievent_t *event;
	  int value = fts_get_number_int(at);
	  
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	  
	  event = fts_midievent_control_change_new(this->channel, this->number, value);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

static void
pgmout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  fts_midievent_t *event;
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  event = fts_midievent_program_change_new(this->channel, value);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

static void
touchout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  fts_midievent_t *event;
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  event = fts_midievent_channel_pressure_new(this->channel, value);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

static void
bendout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	{
	  fts_midievent_t *event;
	  int value = fts_get_number_int(at);
	    
	  if(value < 0)
	    value = 0;
	  else if(value > 127)
	    value = 127;
	    
	  event = fts_midievent_pitch_bend_new(this->channel, 0, value);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

static void
xbendout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  switch (ac)
    {
    default:
    case 2:
      if(fts_is_number(at + 1))
	midiout_set_channel(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at))
	{
	  fts_midievent_t *event;
	  int value = fts_get_number_int(at);
	  
	  if(value < 0)
	    value = 0;
	  else if(value > 16383)
	    value = 16383;
	  
	  event = fts_midievent_pitch_bend_new(this->channel, value & 127, value >> 7);
	  fts_midiport_output(this->port, event, 0.0);
	}
    case 0:
      break;
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
midiout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;

  ac--;
  at++;

  this->port = 0;
  this->channel = 0;
  this->number = 0;

  if(ac > 0)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);

	  /* skip port argument */
	  ac--;
	  at++;

	  if(fts_object_is_midiport(obj) && fts_midiport_is_output((fts_midiport_t *)obj)) 
	    this->port = (fts_midiport_t *) obj;
	  else
	    {
	      fts_object_set_error(o, "Wrong argument for midiport");
	      return;
	    }
	}
  
      if(ac == 2 && fts_is_number(at))
	{
	  int n = fts_get_number_int(at);;
	  
	  this->number = (n < 0)? 0: ((n > 127)? 127: n);      
	  
	  /* skip number argument */
	  ac--;
	  at++;
	}

      if(ac == 1 && fts_is_number(at))
	{
	  int n = fts_get_number_int(at) - 1;
	  
	  this->channel = (n < 0)? 0: ((n > 15)? 15: n);
	}
    }
  
  /* if there is still no port just get default */
  if(!this->port)
    this->port = fts_midiport_get_default();
  
  if(!this->port)
    fts_object_set_error(o, "Default MIDI port is not defined");
}

static fts_status_t
midiout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_midievent, midiout_send);

  return fts_Success;
}

static fts_status_t
noteout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, noteout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, noteout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, noteout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_number);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_number);
  
  fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);

  return fts_Success;
}

static fts_status_t
polyout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, polyout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, polyout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, polyout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_number);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_number);
  
  fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);

  return fts_Success;
}

static fts_status_t
ctlout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, ctlout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, ctlout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, ctlout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_number);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_number);
  
  fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);

  return fts_Success;
}

static fts_status_t
pgmout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, pgmout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, pgmout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, pgmout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  return fts_Success;
}

static fts_status_t
touchout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, touchout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, touchout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, touchout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  return fts_Success;
}


static fts_status_t
bendout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, bendout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, bendout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, bendout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  return fts_Success;
}

static fts_status_t
xbendout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  fts_method_define_varargs(cl, 0, fts_s_int, xbendout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, xbendout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, xbendout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  return fts_Success;
}

void
midiout_config(void)
{
  sym_omni = fts_new_symbol("omni");

  fts_class_install(fts_new_symbol("midiout"), midiout_instantiate);
  fts_class_install(fts_new_symbol("noteout"), noteout_instantiate);
  fts_class_install(fts_new_symbol("polyout"), polyout_instantiate);
  fts_class_install(fts_new_symbol("ctlout"), ctlout_instantiate);
  fts_class_install(fts_new_symbol("pgmout"), pgmout_instantiate);
  fts_class_install(fts_new_symbol("touchout"), touchout_instantiate);
  fts_class_install(fts_new_symbol("bendout"), bendout_instantiate);
  fts_class_install(fts_new_symbol("xbendout"), xbendout_instantiate);
}
