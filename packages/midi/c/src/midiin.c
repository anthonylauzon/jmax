/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
midiin_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);
  
  fts_outlet_object(o, 0, (fts_object_t *)event);
}

static void
note_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);
  
  fts_outlet_int(o, 2, fts_midievent_channel_message_get_channel(event));
  fts_outlet_int(o, 1, fts_midievent_channel_message_get_second(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_first(event));
}

static void
note_channel_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);
  
  fts_outlet_int(o, 1, fts_midievent_channel_message_get_second(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_first(event));
}

static void
note_channel_number_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 0, fts_midievent_channel_message_get_second(event));
}

static void
poly_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 2, fts_midievent_channel_message_get_channel(event));
  fts_outlet_int(o, 1, fts_midievent_channel_message_get_first(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_second(event));
}

static void
poly_channel_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 1, fts_midievent_channel_message_get_first(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_second(event));
}

static void
poly_channel_number_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 0, fts_midievent_channel_message_get_second(event));
}

static void
value_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 1, fts_midievent_channel_message_get_channel(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_first(event));
}

static void
value_channel_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 0, fts_midievent_channel_message_get_first(event));
}

static void
bend_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 1, fts_midievent_channel_message_get_channel(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_second(event));
}

static void
bend_channel_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 0, fts_midievent_channel_message_get_second(event));
}

static void
xbend_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 1, fts_midievent_channel_message_get_channel(event));
  fts_outlet_int(o, 0, fts_midievent_channel_message_get_first(event) + (fts_midievent_channel_message_get_second(event) << 7));
}

static void
xbend_channel_output(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_int(o, 0, fts_midievent_channel_message_get_first(event) + (fts_midievent_channel_message_get_second(event) << 7));
}

/************************************************************
 *
 *  class
 *
 */

static fts_midiport_t *
midiin_get_port(fts_object_t *o, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_symbol(at)) 
    {
      fts_symbol_t label = fts_get_symbol(at);
      
      return fts_midiconfig_get_input(label);
    }
  else
    return fts_midiconfig_get_input(fts_s_default);

  return NULL;
}

static void
midiin_get_filters(int ac, const fts_atom_t *at, int *channel, int *number)
{
  *channel = midi_channel_any;
  *number = midi_controller_any;

  if(ac > 0)
    {
      if(fts_is_symbol(at))
	{
	  /* skip port argument */
	  ac--;
	  at++;
	}

      if(ac >= 2 && fts_is_number(at))
	{
	  int n = fts_get_number_int(at);;
	  
	  *number = (n < 0)? 0: ((n > 127)? 127: n);      
	  
	  /* skip argument */
	  ac--;
	  at++;
	}
      else
	*number = midi_controller_any;
      
      if(ac >= 1 && fts_is_number(at))
	{
	  int n = fts_get_number_int(at);
	  
	  *channel = (n < midi_channel_any)? midi_channel_any: ((n > 16)? 16: n);
	}
      else
	*channel = midi_channel_any;
    }
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
midiin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;

  this->port = midiin_get_port(o, ac, at);
  
  if(this->port)
    {
      this->type = midi_type_any;
      this->channel = midi_channel_any;
      this->number = midi_note_any;

      fts_midiport_add_listener(this->port, midi_type_any, midi_channel_any, midi_note_any, o, midiin_output);
      fts_midiconfig_add_listener(o);
    }
}


static void
notein_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;

  this->type = midi_note;
  this->port = midiin_get_port(o, ac, at);

  midiin_get_filters(ac, at, &this->channel, &this->number);

  if(this->port)
    {    
      if(this->number == midi_controller_any && this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, note_output);
      else if(this->number == midi_controller_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, note_channel_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, note_channel_number_output); 
      
      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 3));
}


static void
polyin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;
  
  this->type = midi_poly_pressure;
  this->port = midiin_get_port(o, ac, at);

  midiin_get_filters(ac, at, &this->channel, &this->number);
    
  if(this->port)
    {    
      if(this->number == midi_controller_any && this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_output);
      else if(this->number == midi_controller_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_number_output); 
      
      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 3));
}

static void
ctlin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;
  
  this->type = midi_control_change;
  this->port = midiin_get_port(o, ac, at);

  midiin_get_filters(ac, at, &this->channel, &this->number);
      
  if(this->port)
    {    
      if(this->number == midi_controller_any && this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_output);
      else if(this->number == midi_controller_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, poly_channel_number_output); 
      
      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 3));
}

static void
pgmin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;
  
  this->type = midi_program_change;
  this->port = midiin_get_port(o, ac, at);

  midiin_get_filters(ac, at, &this->channel, &this->number);
    
  if(this->port)
    {    
      if(this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_channel_output); 

      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 2));
}

static void
touchin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;
  
  this->type = midi_channel_pressure;
  this->port = midiin_get_port(o, ac, at);
  
  midiin_get_filters(ac, at, &this->channel, &this->number);

  if(this->port)
    {    
      if(this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, value_channel_output); 

      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 2));
}

static void
bendin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;
  
  this->type = midi_pitch_bend;
  this->port = midiin_get_port(o, ac, at);

  midiin_get_filters(ac, at, &this->channel, &this->number);
    
  if(this->port)
    {    
      if(this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, bend_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, bend_channel_output); 

      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 2));
}

static void
xbendin_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;
  
  this->type = midi_pitch_bend;
  this->port = midiin_get_port(o, ac, at);

  midiin_get_filters(ac, at, &this->channel, &this->number);

  if(this->port)
    {    
      if(this->channel == midi_channel_any)
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, xbend_output);
      else
	fts_midiport_add_listener(this->port, this->type, this->channel, this->number, o, xbend_channel_output); 

      fts_midiconfig_add_listener(o);
    }

  fts_object_set_outlets_number(o, midiin_get_outlets(ac, at, 2));
}

static void 
midiin_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
  {
    fts_midiport_remove_listener(this->port, this->type, this->channel, this->number, o);
    fts_midiconfig_remove_listener(o);
  }
}

static void
midiin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), midiin_init, midiin_delete);

  fts_class_outlet(cl, 0, fts_midievent_type);
}

static void
notein_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), notein_init, midiin_delete);

  fts_class_outlet_int(cl, 0);
}

static void
polyin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), polyin_init, midiin_delete);

  fts_class_outlet_int(cl, 0);
}

static void
ctlin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), ctlin_init, midiin_delete);

  fts_class_outlet_int(cl, 0);
}

static void
pgmin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), pgmin_init, midiin_delete);

  fts_class_outlet_int(cl, 0);
}

static void
touchin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), touchin_init, midiin_delete);

  fts_class_outlet_int(cl, 0);
}

static void
bendin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), bendin_init, midiin_delete);
 
  fts_class_outlet_int(cl, 0);
}

static void
xbendin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midiin_t), xbendin_init, midiin_delete);

  fts_class_outlet_int(cl, 0);
}

void
midiin_config(void)
{
  sym_omni = fts_new_symbol("omni");

  fts_class_install(fts_new_symbol("midiin"), midiin_instantiate);

  fts_class_install(fts_new_symbol("notein"), notein_instantiate);
  fts_class_install(fts_new_symbol("polyin"), polyin_instantiate);
  fts_class_install(fts_new_symbol("ctlin"), ctlin_instantiate);
  fts_class_install(fts_new_symbol("pgmin"), pgmin_instantiate);
  fts_class_install(fts_new_symbol("touchin"), touchin_instantiate);
  fts_class_install(fts_new_symbol("bendin"), bendin_instantiate);
  fts_class_install(fts_new_symbol("xbendin"), xbendin_instantiate);
}
