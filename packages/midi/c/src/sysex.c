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
 */

#include <fts/fts.h>

typedef struct _sysex_
{
  fts_object_t o;
  fts_midiport_t *port;
} sysex_t;

/************************************************************
 *
 *  MIDI system exclusive input
 *
 */

static void
sysexin_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_varargs(o, 0, fts_midievent_system_exclusive_get_size(event), fts_midievent_system_exclusive_get_atoms(event));
}

static void
sysexin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  this->port = NULL;

  if(ac > 0 && fts_is_symbol(at)) 
    {
      fts_symbol_t label = fts_get_symbol(at);

      if(label != fts_s_minus)
	this->port = fts_midiconfig_get_output(label);      
    }
  else
    this->port = fts_midiconfig_get_output(fts_s_default);

  /* add call back to midi port or set error */
  if(this->port != NULL)
    {
      fts_midiport_add_listener(this->port, midi_system_exclusive, midi_channel_any, midi_controller_any, o, sysexin_output);
      fts_midiconfig_add_listener(o);
    }
}

static void 
sysexin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  if(this->port != NULL)
    fts_midiport_remove_listener(this->port, midi_system_exclusive, midi_channel_any, midi_controller_any, o);
}

static void
sysexin_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sysex_t), sysexin_init, sysexin_delete);

  fts_class_message_varargs(cl, fts_s_midievent, sysexin_output);
}

/************************************************************
 *
 *  MIDI system exclusive output
 *
 */

static void
sysexout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysex_t *this = (sysex_t *)o;
  fts_midievent_t *event = fts_midievent_system_exclusive_new(ac, at);

  fts_object_refer((fts_object_t *)event);

  if(this->port)
    fts_midiport_output(this->port, event, 0.0);
  else
    fts_outlet_object((fts_object_t *)this, 0, (fts_object_t *)event);

  fts_object_release((fts_object_t *)event);
}

static void
sysexout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysex_t *this = (sysex_t *)o;

  this->port = NULL;

  if(ac > 0 && fts_is_symbol(at)) 
    {
      fts_symbol_t label = fts_get_symbol(at);

      if(label != fts_s_minus)
	this->port = fts_midiconfig_get_output(label);      
    }
  else
    this->port = fts_midiconfig_get_output(fts_s_default);
  
  if(this->port != NULL)
    fts_midiconfig_add_listener(o);
}

static void 
sysexout_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
}

static void
sysexout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sysex_t), sysexout_init, sysexout_delete);
  
  fts_class_inlet_int(cl, 0, sysexout_send);
  fts_class_inlet_float(cl, 0, sysexout_send);
  fts_class_inlet_varargs(cl, 0, sysexout_send);
}

void
sysex_config(void)
{
  fts_class_install(fts_new_symbol("sysexin"), sysexin_instantiate);
  fts_class_install(fts_new_symbol("sysexout"), sysexout_instantiate);
}
