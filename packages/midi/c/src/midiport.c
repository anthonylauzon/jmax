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
 * Authors: Riccardo Borghesi, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>

/* listen to input of midiport */
static void
midiport_input( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_midiport_input(this, event, 0.0);
}

/* midiport port output function */
static void
midiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_outlet_object(o, 0, (fts_object_t *)event);
}

static void 
midiport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  ac--;
  at++;

  fts_midiport_init(this);
  fts_midiport_set_input(this);
  fts_midiport_set_output(this, midiport_output);
}

static void
midiport_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midiport_t *this = (fts_midiport_t *)o;

  fts_midiport_reset(this);
}

static void 
midiport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, o);
}

static fts_status_t 
midiport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_midiport_t), 1, 1, 0);

  fts_midiport_class_init(cl);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiport_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiport_delete);

  fts_method_define_varargs(cl, 0, fts_s_midievent, midiport_input);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, midiport_get_state);

  return fts_Success;
}

void 
midiport_config( void)
{
  fts_class_install( fts_new_symbol("midiport"), midiport_instantiate);
}
