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

/************************************************
 *
 *    object
 *
 */
 
typedef struct 
{
  fts_object_t o;
  fts_array_t list;
} list_obj_t;

static void
list_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  fts_array_init(&this->list, 0, 0);
  fts_array_set(&this->list, ac - 1, at + 1);
}

static void
list_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  fts_array_destroy(&this->list);
}

/************************************************
 *
 *  user methods
 *
 */

static void
list_obj_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->list), fts_array_get_atoms(&this->list));
}

static void
list_obj_store_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  fts_array_set(&this->list, ac, at);
}

static void
list_obj_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_obj_t *this = (list_obj_t *)o;

  fts_array_set(&this->list, ac, at);
  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->list), fts_array_get_atoms(&this->list));
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
list_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(list_obj_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, list_obj_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, list_obj_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, list_obj_bang);

  fts_method_define_varargs(cl, 0, fts_s_list, list_obj_list);
  fts_method_define_varargs(cl, 1, fts_s_list, list_obj_store_list);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
list_config(void)
{
  fts_class_install(fts_new_symbol("list"), list_obj_instantiate);
  fts_class_alias(fts_new_symbol("l"), fts_new_symbol("list"));
}
