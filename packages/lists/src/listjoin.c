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

/*********************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  fts_list_t out;
  fts_list_t right;
} listjoin_t;

static void
listjoin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  ac--;
  at++;

  fts_list_init(&this->out, 0, 0);
  fts_list_init(&this->right, ac, at);
}

static void
listjoin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  fts_list_reset(&this->out);
  fts_list_reset(&this->right);
}

/*********************************************
 *
 *  methods
 *
 */

static void
listjoin_set_right_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  fts_list_set(&this->right, ac, at);
}

static void
listjoin_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int right_size = fts_list_get_size(&this->right);
  int total_size = right_size + ac;

  fts_list_set_size(&this->out, total_size);
  fts_list_set(&this->out, ac, at);
  fts_list_append(&this->out, right_size, fts_list_get_ptr(&this->right));
  
  fts_outlet_send(o, 0, fts_s_list, total_size, fts_list_get_ptr(&this->out));
}

static void
listjoin_prepend(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int right_size = fts_list_get_size(&this->right);
  int total_size = right_size + ac;

  fts_list_set_size(&this->out, total_size);
  fts_list_set(&this->out, right_size, fts_list_get_ptr(&this->right));
  fts_list_append(&this->out, ac, at);

  fts_outlet_send(o, 0, fts_s_list, total_size, fts_list_get_ptr(&this->out));
}

/*********************************************
 *
 *  class
 *
 */

static fts_status_t
listappend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listjoin_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listjoin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listjoin_delete);

  fts_method_define_varargs(cl, 0, fts_s_list, listjoin_append);

  fts_method_define_varargs(cl, 1, fts_s_int, listjoin_set_right_list);
  fts_method_define_varargs(cl, 1, fts_s_float, listjoin_set_right_list);
  fts_method_define_varargs(cl, 1, fts_s_symbol, listjoin_set_right_list);
  fts_method_define_varargs(cl, 1, fts_s_list, listjoin_set_right_list);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

static fts_status_t
listprepend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listjoin_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listjoin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listjoin_delete);

  fts_method_define_varargs(cl, 0, fts_s_list, listjoin_prepend);

  fts_method_define_varargs(cl, 1, fts_s_int, listjoin_set_right_list);
  fts_method_define_varargs(cl, 1, fts_s_float, listjoin_set_right_list);
  fts_method_define_varargs(cl, 1, fts_s_symbol, listjoin_set_right_list);
  fts_method_define_varargs(cl, 1, fts_s_list, listjoin_set_right_list);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listjoin_config(void)
{
  fts_class_install(fts_new_symbol("listappend"), listappend_instantiate);
  fts_class_install(fts_new_symbol("listprepend"), listprepend_instantiate);

  fts_class_alias(fts_new_symbol("lapp"), fts_new_symbol("listappend"));
  fts_class_alias(fts_new_symbol("lpre"), fts_new_symbol("listprepend"));
}