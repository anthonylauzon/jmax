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

#include <fts/sys.h>
#include <fts/lang.h>
#include "messP.h"

fts_class_t *fts_label_class = 0;

static void
label_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_channel_find_friends(&this->channel, ac, at);
}      

static void
label_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_label_send(this, s, ac, at);
}

static void
label_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object_with_type(value, obj, fts_s_label);
}

static void
label_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_channel_init(&this->channel);
}

static fts_status_t
label_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_label_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, label_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, label_find_friends);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, label_get_state);

  /* sending anything else to lable is like sending to all channel targets */
  fts_method_define_varargs(cl, 0, fts_s_anything, label_send);

  return fts_Success;
}

void
fts_label_config(void)
{
  fts_class_install(fts_s_label, label_instantiate);

  fts_label_class = fts_class_get_by_name(fts_s_label);
}
