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

static fts_symbol_t sysex_get_classname(int ac, const fts_atom_t *at);
static fts_midiport_t * sysex_get_midiport(int ac, const fts_atom_t *at);

static void sysex_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void sysex_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/************************************************************
 *
 *  object
 *
 */

typedef struct _sysex_
{
  fts_object_t o;
  fts_midiport_t *port;
} sysex_t;

static void
sysex_in_callback(fts_object_t *o, int ac, const fts_atom_t *at, double time)
{
  sysex_t *this = (sysex_t *)o;

  fts_outlet_send(o, 0, fts_s_list, ac, at);
}

static void
sysex_in_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;
  fts_midiport_t *port = sysex_get_midiport(ac, at);
  int channel = fts_get_int_arg(ac, at, 2, 0);
  fts_midiport_callback_t callback;

  if(!port)
    port = fts_midiport_get_default();

  if(port)
    {
      this->port = port;
      
      callback.system_exclusive = sysex_in_callback;
      
      fts_midiport_add_listener(port, fts_midi_status_system_exclusive, 0, o, callback);
    }
}

static void 
sysex_in_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;

  if(this->port)
    fts_midiport_remove_listener(this->port, fts_midi_status_system_exclusive, 0, o);
}

static void
sysex_out_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;
  fts_midiport_t *port = sysex_get_midiport(ac, at);

  if(!port)
    port = fts_midiport_get_default();
    
  this->port = port;
}

static void 
sysex_out_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  sysex_t *this = (sysex_t *)o;
}

static void
sysex_out_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sysex_t *this = (sysex_t *)o;

  if(this->port)
    fts_midiport_output_system_exclusive(this->port, ac, at, 0.0);
}

/************************************************************
 *
 *  class
 *
 */

static fts_symbol_t
sysex_get_classname(int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_symbol(at))
    return fts_get_symbol(at);
  else
    return 0;
}

static fts_midiport_t *
sysex_get_midiport(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1) && fts_midiport_has_superclass(fts_get_object(at + 1)))
    return (fts_midiport_t *)fts_get_object(at + 1);
  else
    return 0;
}

int 
sysex_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return 
    fts_is_symbol(at0) && fts_is_symbol(at1) &&
    (fts_get_symbol(at0) == fts_get_symbol(at1)) &&
    fts_is_object(at0 + 1) && fts_is_object(at1 + 1) &&
    (fts_get_object(at0 + 1) == fts_get_object(at1 + 1));
}

static fts_status_t
sysex_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t class = sysex_get_classname(ac, at);

  if(class == fts_new_symbol("sysexin") || class == fts_new_symbol("midiin"))
    {
      fts_class_init(cl, sizeof(sysex_t), 0, 1, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysex_in_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysex_in_delete);
    }
  else if(class == fts_new_symbol("sysexout") || class == fts_new_symbol("midiout"))
    {
      fts_class_init(cl, sizeof(sysex_t), 1, 0, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysex_out_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysex_out_delete);

      fts_method_define_varargs(cl, 0, fts_s_int, sysex_out_send);
      fts_method_define_varargs(cl, 0, fts_s_float, sysex_out_send);
      fts_method_define_varargs(cl, 0, fts_s_list, sysex_out_send);
    }
  else
    return &fts_CannotInstantiate;    

  /* fts_method_define_varargs(cl, 0, fts_s_list, sysex_input); */

  return fts_Success;
}


void
sysex_config(void)
{
  fts_metaclass_install(fts_new_symbol("sysexin"), sysex_instantiate, sysex_equiv);
  fts_metaclass_install(fts_new_symbol("sysexout"), sysex_instantiate, sysex_equiv);
  fts_metaclass_install(fts_new_symbol("midiin"), sysex_instantiate, sysex_equiv);
  fts_metaclass_install(fts_new_symbol("midiout"), sysex_instantiate, sysex_equiv);
}
