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
  fts_array_t array;
} listcomp_t;

static void
listcomp_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listcomp_t *this = (listcomp_t *)o;

  ac--;
  at++;

  fts_array_init(&this->array, ac, at);
}

static void
listcomp_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listcomp_t *this = (listcomp_t *)o;

  fts_array_destroy(&this->array);
}

/*********************************************
 *
 *  methods
 *
 */

static void
listcomp_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listcomp_t *this = (listcomp_t *)o;

  fts_array_append(&this->array, ac, at);
}

static void
listcomp_output_and_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listcomp_t *this = (listcomp_t *)o;

  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->array), fts_array_get_atoms(&this->array));
  fts_array_clear(&this->array);
}

static void
listcomp_append_output_and_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listcomp_t *this = (listcomp_t *)o;

  fts_array_append(&this->array, ac, at);
  fts_outlet_send(o, 0, fts_s_list, fts_array_get_size(&this->array), fts_array_get_atoms(&this->array));
  fts_array_clear(&this->array);
}

static void
listdeco_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listcomp_t *this = (listcomp_t *)o;
  fts_atom_t *atoms = fts_array_get_atoms(&this->array);
  int i;

  for(i=ac-1; i>=0; i--)
    fts_outlet_send(o, 1, fts_get_selector(at + i), 1, atoms + i);

  fts_outlet_bang(o, 0);
}

/*********************************************
 *
 *  class
 *
 */

static fts_status_t
listcompose_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listcomp_t), 2, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listcomp_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listcomp_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, listcomp_output_and_clear);
  fts_method_define_varargs(cl, 0, fts_s_int, listcomp_append_output_and_clear);
  fts_method_define_varargs(cl, 0, fts_s_float, listcomp_append_output_and_clear);
  fts_method_define_varargs(cl, 0, fts_s_symbol, listcomp_append_output_and_clear);
  fts_method_define_varargs(cl, 0, fts_s_list, listcomp_append_output_and_clear);

  fts_method_define_varargs(cl, 1, fts_s_int, listcomp_append);
  fts_method_define_varargs(cl, 1, fts_s_float, listcomp_append);
  fts_method_define_varargs(cl, 1, fts_s_symbol, listcomp_append);
  fts_method_define_varargs(cl, 1, fts_s_list, listcomp_append);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

static fts_status_t
listdecompose_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listcomp_t), 2, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listcomp_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, listcomp_delete);

  fts_method_define_varargs(cl, 1, fts_s_int, listdeco_input);
  fts_method_define_varargs(cl, 1, fts_s_float, listdeco_input);
  fts_method_define_varargs(cl, 1, fts_s_symbol, listdeco_input);
  fts_method_define_varargs(cl, 1, fts_s_list, listdeco_input);

  return fts_Success;
}

void
listcomp_config(void)
{
  fts_class_install(fts_new_symbol("listcompose"), listcompose_instantiate);
  fts_class_install(fts_new_symbol("listdecompose"), listdecompose_instantiate);

  fts_alias_install(fts_new_symbol("lcomp"), fts_new_symbol("listcompose"));
  fts_alias_install(fts_new_symbol("ldeco"), fts_new_symbol("listdecompose"));
}
