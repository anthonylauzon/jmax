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
  int type;
  int channel;
  int number;
} midiin_t;

/************************************************************
 *
 *  callbacks
 *
 */

static void
note_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 2, fts_midievent_channel_message_get_channel(event) + 1);
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_second(event));
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_first(event));
}

static void
note_channel_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_second(event));
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_first(event));
}

static void
note_channel_number_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  midiin_t *this = (midiin_t *)listener;

  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_second(event) + 1);
}

static void
poly_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 2, fts_midievent_channel_message_get_channel(event) + 1);
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_first(event));
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_second(event));
}

static void
poly_channel_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_first(event));
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_second(event));
}

static void
poly_channel_number_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  midiin_t *this = (midiin_t *)listener;

  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_second(event));
}

static void
value_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_channel(event) + 1);
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_first(event));
}

static void
value_channel_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_first(event));
}

static void
bend_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_channel(event) + 1);
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_second(event));
}

static void
bend_channel_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_second(event));
}

static void
xbend_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 1, fts_midievent_channel_message_get_channel(event) + 1);
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_first(event) + (fts_midievent_channel_message_get_second(event) << 7));
}

static void
xbend_channel_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_outlet_int(listener, 0, fts_midievent_channel_message_get_first(event) + (fts_midievent_channel_message_get_second(event) << 7));
}

/************************************************************
 *
 *  class
 *
 */

static int 
midiin_check(fts_object_t *o, int ac, const fts_atom_t *at, fts_midiport_t **port, int *channel, int *number)
{
  *port = 0;
  *channel = midi_channel_any;
  *number = midi_controller_any;

  if(ac > 0)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);

	  /* skip port argument */
	  ac--;
	  at++;
	  
	  /* grab midiport argument in init (o != 0) only */
	  if (o != NULL) 
	    {
	      if(fts_object_is_midiport(obj) && fts_midiport_is_input((fts_midiport_t *)obj))
		*port = (fts_midiport_t *)obj;	  
	      else
		return 0;
	    }
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
	    *number = midi_controller_any;
	  else
	    return 0;
	}	
      else
	*number = midi_controller_any;

      if(ac == 1)
	{
      
	  if(fts_is_number(at))
	    {
	      int n = fts_get_number_int(at) - 1;
	  
	      *channel = (n < midi_channel_any)? 0: ((n > 15)? 15: n);
	    }
	  else if(fts_is_symbol(at) && (fts_get_symbol(at) == sym_omni))
	    *channel = midi_channel_any;
	  else
	    return 0;
	}
    }

  if(o)
    {
      /* if there is still no port just get default */
      if(!*port)
	*port = fts_midiport_get_default();
      
      if(!*port)
	{
	  fts_object_set_error(o, "Default MIDI port is not defined");
	  return 0;
	}
    }

  return 1;
}

static void
notein_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  ac--;
  at++;

  this->type = midi_type_note;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_controller_any && this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, note_callback);
      else if(this->number == midi_controller_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, note_channel_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, note_channel_number_callback); 
    }
}


static void
polyin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  
  ac--;
  at++;

  this->type = midi_type_poly_pressure;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_controller_any && this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_callback);
      else if(this->number == midi_controller_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_number_callback); 
    }
}

static void
ctlin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  
  ac--;
  at++;

  this->type = midi_type_control_change;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_controller_any && this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_callback);
      else if(this->number == midi_controller_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_number_callback); 
    }
}

static void
pgmin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  
  ac--;
  at++;

  this->type = midi_type_program_change;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_channel_callback); 
    }
}

static void
touchin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  
  ac--;
  at++;

  this->type = midi_type_channel_pressure;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_channel_callback); 
    }
}

static void
bendin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  
  ac--;
  at++;

  this->type = midi_type_pitch_bend;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, bend_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, bend_channel_callback); 
    }
}

static void
xbendin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  
  ac--;
  at++;

  this->type = midi_type_pitch_bend;

  if(midiin_check(o, ac, at, &this->port, &this->channel, &this->number))
    {
      if(this->number == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, xbend_callback);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, xbend_channel_callback); 
    }
}

static void 
midiin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener(this->port, this->type, this->channel, this->number, o);
}

static fts_status_t
notein_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (number == midi_controller_any) + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, notein_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
polyin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (number == midi_controller_any) + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, polyin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
ctlin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (number == midi_controller_any) + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ctlin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
pgmin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pgmin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
touchin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, touchin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
bendin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bendin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
xbendin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  ac--;
  at++;

  if(!midiin_check(0, ac, at, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiin_t), 0, 1 + (channel == midi_channel_any), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, xbendin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static int 
midiin_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  fts_midiport_t *port1;
  int channel1;
  int number1;

  if(midiin_check(0, ac1 - 1, at1 + 1, &port1, &channel1, &number1))
    {
      fts_midiport_t *port0;
      int channel0;
      int number0;

      midiin_check(0, ac0 - 1, at0 + 1, &port0, &channel0, &number0);

      if((channel0 != midi_channel_any) != (channel1 != midi_channel_any))
	return 0;
      else if((number0 != midi_controller_any) != (number1 != midi_controller_any))
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
  fts_metaclass_install(fts_new_symbol("xbendin"), xbendin_instantiate, midiin_equiv);
}
