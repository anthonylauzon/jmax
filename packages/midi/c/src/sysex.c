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
sysexin_callback(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_atoms(o, 0, fts_midievent_system_exclusive_get_size(event), fts_midievent_system_exclusive_get_atoms(event));
}

static void
sysexin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;
  fts_symbol_t name;

  ac--;
  at++;

  this->port = NULL;

  name = fts_get_symbol(at);

  if(ac > 0 && fts_is_symbol(at)) {
    this->port = fts_midiconfig_get_input(name);

    if(this->port == NULL) {
      fts_object_set_error(o, "Cannot find MIDI input %s", name);
      return;
    }
  }

  if(this->port == NULL)
    this->port = fts_midiconfig_get_input(fts_s_default);
  
  /* add call back to midi port or set error */
  if(this->port != NULL)
    {
      fts_midiport_add_listener(this->port, midi_system_exclusive, midi_channel_any, midi_controller_any, o, sysexin_callback);
      fts_midiconfig_add_listener(o);
    }
  else
    fts_object_set_error(o, "Cannot find default MIDI output");
}

static void 
sysexin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  if(this->port)
    fts_midiport_remove_listener(this->port, midi_system_exclusive, midi_channel_any, midi_controller_any, o);
}

static fts_status_t
sysexin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sysex_t), 0, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysexin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysexin_delete);
  
  return fts_Success;
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

  if(this->port)
    {
      fts_midievent_t *event;
      int i;
      
      event = fts_midievent_system_exclusive_new(ac, at);
      fts_midiport_output(this->port, event, 0.0);
    }
}

static void
sysexout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysex_t *this = (sysex_t *)o;
  fts_symbol_t name;

  ac--;
  at++;

  this->port = NULL;

  name = fts_get_symbol(at);

  if(ac > 0 && fts_is_symbol(at)) {
    this->port = fts_midiconfig_get_output(name);

    if(this->port == NULL) {
      fts_object_set_error(o, "Cannot find MIDI output %s", name);
      return;
    }
  }

  if(this->port == NULL)
    this->port = fts_midiconfig_get_output(fts_s_default);

  if(this->port != NULL)
    fts_midiconfig_add_listener(o);
  else
    fts_object_set_error(o, "Cannot find default MIDI output");
}

static void 
sysexout_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;
}

static fts_status_t
sysexout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sysex_t), 1, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysexout_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysexout_delete);
  
  fts_method_define_varargs(cl, 0, fts_s_int, sysexout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, sysexout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, sysexout_send);
  
  return fts_Success;
}

void
sysex_config(void)
{
  fts_class_install(fts_new_symbol("sysexin"), sysexin_instantiate);
  fts_class_install(fts_new_symbol("sysexout"), sysexout_instantiate);
}
