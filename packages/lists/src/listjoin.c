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
#include "list.h"

/*********************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  list_t out;
  list_t store;
} listjoin_t;

static void
listjoin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  list_init(&this->out);
  list_init(&this->store);

  if(ac > 1)
    list_set(&this->store, ac - 1, at + 1);
}

static void
listjoin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  list_free(&this->out);
  list_free(&this->store);
}

/*********************************************
 *
 *  methods
 *
 */

static void
listjoin_store_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  list_set(&this->store, ac, at);
}

static void
listjoin_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int store_size = list_get_size(&this->store);
  int total_size = store_size + ac;

  list_raw_resize(&this->out, total_size);
  list_raw_set(&this->out, 0, ac, at);
  list_raw_set(&this->out, ac, store_size, list_get_ptr(&this->store));

  fts_outlet_send(o, 0, fts_s_list, total_size, list_get_ptr(&this->out));
}


static void
listjoin_prepend(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int store_size = list_get_size(&this->store);
  int total_size = store_size + ac;

  list_raw_resize(&this->out, total_size);
  list_raw_set(&this->out, 0, store_size, list_get_ptr(&this->store));
  list_raw_set(&this->out, store_size, ac, at);

  fts_outlet_send(o, 0, fts_s_list, total_size, list_get_ptr(&this->out));
}

/*********************************************
 *
 *  class
 *
 */

static fts_status_t
listjoin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  /* initialization */
  fts_class_init(cl, sizeof(listjoin_t), 2, 1, 0); 

  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listjoin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listjoin_delete);

  /* user methods */
  fts_method_define_varargs(cl, 1, fts_s_list, listjoin_store_list);

  if(fts_get_symbol(at) == fts_new_symbol("listappend"))
    fts_method_define_varargs(cl, 0, fts_s_list, listjoin_append);
  else
    fts_method_define_varargs(cl, 0, fts_s_list, listjoin_prepend);

  /* outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listjoin_config(void)
{
  fts_class_install(fts_new_symbol("listappend"), listjoin_instantiate);
  fts_class_install(fts_new_symbol("listprepend"), listjoin_instantiate);

  fts_class_alias(fts_new_symbol("lapp"), fts_new_symbol("listappend"));
  fts_class_alias(fts_new_symbol("lpre"), fts_new_symbol("listprepend"));
}
