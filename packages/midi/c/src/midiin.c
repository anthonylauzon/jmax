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

static fts_symbol_t sym_omni = 0;

typedef struct _midiin_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int number;
} midiin_t;

/************************************************************
 *
 *  callbacks
 *
 */

static void
note_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  fts_outlet_int(listener, 2, channel);
  fts_outlet_int(listener, 1, value);
  fts_outlet_int(listener, 0, number);
}

static void
note_channel_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  fts_outlet_int(listener, 1, value);
  fts_outlet_int(listener, 0, number);
}

static void
note_number_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  midiin_t *this = (midiin_t *)listener;

  fts_outlet_int(listener, 0, value);
  fts_outlet_int(listener, 1, channel);
}

static void
note_number_channel_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  midiin_t *this = (midiin_t *)listener;

  fts_outlet_int(listener, 0, value);
}

static void
poly_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  fts_outlet_int(listener, 2, channel);
  fts_outlet_int(listener, 1, number);
  fts_outlet_int(listener, 0, value);
}

static void
poly_channel_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  fts_outlet_int(listener, 1, number);
  fts_outlet_int(listener, 0, value);
}

static void
poly_number_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  midiin_t *this = (midiin_t *)listener;

  fts_outlet_int(listener, 1, channel);
  fts_outlet_int(listener, 0, value);
}

static void
poly_number_channel_callback(fts_object_t *listener, int channel, int number, int value, double time)
{
  midiin_t *this = (midiin_t *)listener;

  fts_outlet_int(listener, 0, value);
}

static void
value_callback(fts_object_t *listener, int channel, int value, double time)
{
  fts_outlet_int(listener, 1, channel);
  fts_outlet_int(listener, 0, value);
}

static void
value_channel_callback(fts_object_t *listener, int channel, int value, double time)
{
  fts_outlet_int(listener, 0, value);
}

static void
bend_callback(fts_object_t *listener, int channel, int value, double time)
{
  fts_outlet_int(listener, 1, channel);
  fts_outlet_int(listener, 0, value >> 7);
}

static void
bend_channel_callback(fts_object_t *listener, int channel, int value, double time)
{
  fts_outlet_int(listener, 0, value >> 7);
}

/************************************************************
 *
 *  class
 *
 */

static int 
midiin_check(int ac, const fts_atom_t *at, fts_midiport_t **port, int *channel, int *number)
{
  *port = 0;
  *channel = MIDIPORT_ALL_CHANNELS;
  *number = MIDIPORT_ALL_NUMBERS;

  if(ac > 0)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);
	  
	  if(fts_midiport_check(obj) && fts_midiport_is_input((fts_midiport_t *)obj))
	    {
	      *port = (fts_midiport_t *)fts_get_object(at);
	  
	      /* skip port argument */
	      ac--;
	      at++;
	    }
	  else
	    return 0;
	}
  
      if(ac == 2)
	{
	  if(fts_is_number(at))
	    {
	      int n = fts_get_number_int(at);;
	      
	      *number = (n < 0)? 0: ((n > 127)? 127: n);      
	      
	      /* skip port argument */
	      ac--;
	      at++;
	    }
	  else if(fts_is_symbol(at) && (fts_get_symbol(at) == sym_omni))
	    *number = MIDIPORT_ALL_NUMBERS;
	  else
	    return 0;
	}	
      else
	*number = MIDIPORT_ALL_NUMBERS;

      if(ac == 1)
	{
      
	  if(fts_is_number(at))
	    {
	      int n = fts_get_number_int(at);
	  
	      *channel = (n < 0)? 0: ((n > 16)? 16: n);
	    }
	  else if(fts_is_symbol(at) && (fts_get_symbol(at) == sym_omni))
	    *channel = MIDIPORT_ALL_CHANNELS;
	  else
	    return 0;
	}
    }

  /* if there is still no port just get default */
  if(!*port)
    *port = fts_midiport_get_default();
  
  if(!*port)
    return 0;
  
  return 1;
}

/******************************************************
 *
 *  init
 *
 */
static void
notein_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  midiin_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
  
  if(this->number == MIDIPORT_ALL_NUMBERS)
    {
      if(this->channel == MIDIPORT_ALL_CHANNELS)
	fts_midiport_add_listener_note(this->port, this->channel, this->number, o, note_callback);
      else
	fts_midiport_add_listener_note(this->port, this->channel, this->number, o, note_channel_callback);
    }
  else
    {
      if(this->channel == MIDIPORT_ALL_CHANNELS)
	fts_midiport_add_listener_note(this->port, this->channel, this->number, o, note_number_callback);
      else
	fts_midiport_add_listener_note(this->port, this->channel, this->number, o, note_number_channel_callback); 
    }
}


static void
polyin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  midiin_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
  
  if(this->number == MIDIPORT_ALL_NUMBERS)
    {
      if(this->channel == MIDIPORT_ALL_CHANNELS)
	fts_midiport_add_listener_poly_pressure(this->port, this->channel, this->number, o, poly_callback);
      else
	fts_midiport_add_listener_poly_pressure(this->port, this->channel, this->number, o, poly_channel_callback);
    }
  else
    {
      if(this->channel == MIDIPORT_ALL_CHANNELS)
	fts_midiport_add_listener_poly_pressure(this->port, this->channel, this->number, o, poly_number_callback);
      else
	fts_midiport_add_listener_poly_pressure(this->port, this->channel, this->number, o, poly_number_channel_callback); 
    }
}

static void
ctlin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  midiin_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
  
  if(this->number == MIDIPORT_ALL_NUMBERS)
    {
      if(this->channel == MIDIPORT_ALL_CHANNELS)
	fts_midiport_add_listener_control_change(this->port, this->channel, this->number, o, poly_callback);
      else
	fts_midiport_add_listener_control_change(this->port, this->channel, this->number, o, poly_channel_callback);
    }
  else
    {
      if(this->channel == MIDIPORT_ALL_CHANNELS)
	fts_midiport_add_listener_control_change(this->port, this->channel, this->number, o, poly_number_callback);
      else
	fts_midiport_add_listener_control_change(this->port, this->channel, this->number, o, poly_number_channel_callback); 
    }
}

static void
pgmin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiin_t *this = (midiin_t *)o;

  midiin_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
  
  if(this->channel == MIDIPORT_ALL_CHANNELS)
    fts_midiport_add_listener_program_change(this->port, this->channel, o, value_callback);
  else
    fts_midiport_add_listener_program_change(this->port, this->channel, o, value_channel_callback);
}

static void
touchin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiin_t *this = (midiin_t *)o;

  midiin_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
  
  if(this->channel == MIDIPORT_ALL_CHANNELS)
    fts_midiport_add_listener_channel_pressure(this->port, this->channel, o, value_callback);
  else
    fts_midiport_add_listener_channel_pressure(this->port, this->channel, o, value_channel_callback);
}

static void
bendin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiin_t *this = (midiin_t *)o;

  midiin_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
  
  if(this->channel == MIDIPORT_ALL_CHANNELS)
    fts_midiport_add_listener_pitch_bend(this->port, this->channel, o, bend_callback);
  else
    fts_midiport_add_listener_pitch_bend(this->port, this->channel, o, bend_channel_callback);
}

/******************************************************
 *
 *  delete
 *
 */
static void 
notein_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener_note(this->port, this->channel, this->number, o);
}

static void 
polyin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener_poly_pressure(this->port, this->channel, this->number, o);
}

static void 
ctlin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener_control_change(this->port, this->channel, this->number, o);
}

static void 
pgmin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener_program_change(this->port, this->channel, o);
}

static void 
touchin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener_channel_pressure(this->port, this->channel, o);
}

static void 
bendin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener_pitch_bend(this->port, this->channel, o);
}

/******************************************************
 *
 *  instantiate
 *
 */
static fts_status_t
notein_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiin_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (number == MIDIPORT_ALL_NUMBERS) + (channel == MIDIPORT_ALL_CHANNELS), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, notein_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, notein_delete);

  return fts_Success;
}

static fts_status_t
polyin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiin_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (number == MIDIPORT_ALL_NUMBERS) + (channel == MIDIPORT_ALL_CHANNELS), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, polyin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, polyin_delete);

  return fts_Success;
}

static fts_status_t
ctlin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiin_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (number == MIDIPORT_ALL_NUMBERS) + (channel == MIDIPORT_ALL_CHANNELS), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ctlin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, ctlin_delete);

  return fts_Success;
}

static fts_status_t
pgmin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiin_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == MIDIPORT_ALL_CHANNELS), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pgmin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, pgmin_delete);

  return fts_Success;
}

static fts_status_t
touchin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiin_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == MIDIPORT_ALL_CHANNELS), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, touchin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, touchin_delete);

  return fts_Success;
}

static fts_status_t
bendin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiin_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == MIDIPORT_ALL_CHANNELS), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bendin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bendin_delete);

  return fts_Success;
}

static int 
midiin_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  fts_midiport_t *port1;
  int channel1;
  int number1;

  if(midiin_check(ac1 - 1, at1 + 1, &port1, &channel1, &number1))
    {
      fts_midiport_t *port0;
      int channel0;
      int number0;

      midiin_check(ac0 - 1, at0 + 1, &port0, &channel0, &number0);

      if((channel0 != MIDIPORT_ALL_CHANNELS) != (channel1 != MIDIPORT_ALL_CHANNELS))
	return 0;
      else if((number0 != MIDIPORT_ALL_NUMBERS) != (number1 != MIDIPORT_ALL_NUMBERS))
	return 0;
      else
	return 1;
    }

  return 0;
}

void
midiin_config(void)
{
  sym_omni = fts_new_symbol("omni");

  fts_metaclass_install(fts_new_symbol("notein"), notein_instantiate, midiin_equiv);
  fts_metaclass_install(fts_new_symbol("polyin"), polyin_instantiate, midiin_equiv);
  fts_metaclass_install(fts_new_symbol("ctlin"), ctlin_instantiate, midiin_equiv);
  fts_metaclass_install(fts_new_symbol("pgmin"), pgmin_instantiate, midiin_equiv);
  fts_metaclass_install(fts_new_symbol("touchin"), touchin_instantiate, midiin_equiv);
  fts_metaclass_install(fts_new_symbol("bendin"), bendin_instantiate, midiin_equiv);
}
