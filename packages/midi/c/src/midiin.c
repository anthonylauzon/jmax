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
midiin_callback(fts_object_t *listener, fts_midievent_t *event, double time)
{
  fts_atom_t a;

  fts_set_midievent(&a, event);
  fts_outlet_send(listener, 0, 0, 1, &a);
}

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

int
midiin_get_port(fts_object_t *o, int ac, const fts_atom_t *at, fts_midiport_t **port)
{
  *port = 0;

  if(ac > 0 && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);
      
      if(fts_object_is_midiport(obj) && fts_midiport_is_input((fts_midiport_t *)obj))
	*port = (fts_midiport_t *)obj;	  
      else if(o)
	{
	  fts_object_set_error(o, "Wrong argument for midiport");
	  return 0;
	}
    }
  
  if(!*port)
    *port = fts_midiport_get_default();  

  if(!*port)
    {
      fts_object_set_error(o, "Default MIDI port is not defined");
      return 0;
    }

  return 1;
}

static int 
midiin_get_filters(fts_object_t *o, int ac, const fts_atom_t *at, int *channel, int *number)
{
  *channel = midi_channel_any;
  *number = midi_controller_any;

  if(ac > 0)
    {
      if(fts_is_object(at))
	{
	  /* skip port argument */
	  ac--;
	  at++;
	}

      if(ac == 2)
	{
	  if(fts_is_number(at))
	    {
	      int n = fts_get_number_int(at);;
	      
	      *number = (n < 0)? 0: ((n > 127)? 127: n);      
	      
	      /* skip argument */
	      ac--;
	      at++;
	    }
	  else if(fts_is_symbol(at) && (fts_get_symbol(at) == sym_omni))
	    *number = midi_controller_any;
	  else if(o)
	    {
	      fts_object_set_error(o, "Wrong argument for number filter");
	      return 0;
	    }
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
	  else if(o)
	    {
	      fts_object_set_error(o, "Wrong argument for channel filter");
	      return 0;
	    }
	}
    }

  return 1;
}

static int 
midiin_get_outlets(int ac, const fts_atom_t *at, int max)
{
  /* skip port argument */
  if(ac > 0 && fts_is_object(at))
    {
      ac--;
      at++;
    }

  if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
    return 1;
  else if(ac == 1 && fts_is_number(at))
    return max - 1;
  else
    return max;
}

static void
midiin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  ac--;
  at++;

  if(midiin_get_port(o, ac, at, &this->port))
    {
      this->type = midi_type_any;
      this->channel = midi_channel_any;
      this->number = midi_note_any;

      fts_midiport_add_listener(this->port, midi_type_any, midi_channel_any, midi_note_any, o, midiin_callback);
    }
}


static void
notein_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  ac--;
  at++;

  this->type = midi_type_note;

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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

  if(midiin_get_port(o, ac, at, &this->port) && midiin_get_filters(o, ac, at, &this->channel, &this->number))
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
midiin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
notein_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 3), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, notein_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
polyin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 3), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, polyin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
ctlin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 3), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, ctlin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
pgmin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 2), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pgmin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
touchin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 2), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, touchin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
bendin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 2), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bendin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

static fts_status_t
xbendin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  fts_class_init(cl, sizeof(midiin_t), 0, midiin_get_outlets(ac, at, 2), 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, xbendin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

void
midiin_config(void)
{
  sym_omni = fts_new_symbol("omni");

  fts_class_install(fts_new_symbol("midiin"), midiin_instantiate);
  fts_metaclass_install(fts_new_symbol("notein"), notein_instantiate, fts_arg_equiv);
  fts_metaclass_install(fts_new_symbol("polyin"), polyin_instantiate, fts_arg_equiv);
  fts_metaclass_install(fts_new_symbol("ctlin"), ctlin_instantiate, fts_arg_equiv);
  fts_metaclass_install(fts_new_symbol("pgmin"), pgmin_instantiate, fts_arg_equiv);
  fts_metaclass_install(fts_new_symbol("touchin"), touchin_instantiate, fts_arg_equiv);
  fts_metaclass_install(fts_new_symbol("bendin"), bendin_instantiate, fts_arg_equiv);
  fts_metaclass_install(fts_new_symbol("xbendin"), xbendin_instantiate, fts_arg_equiv);
}
