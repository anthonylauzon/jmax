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
 */

#include <fts/fts.h>

fts_symbol_t objectset_symbol = 0;
fts_symbol_t sym_objectset_add = 0;
fts_symbol_t sym_objectset_remove = 0;
fts_symbol_t sym_objectset_clear = 0;
fts_symbol_t sym_objectset_get_objects = 0;
fts_symbol_t sym_objectset_append = 0;
fts_symbol_t sym_objectset_find = 0;
fts_symbol_t sym_objectset_find_errors = 0;

static void fts_objectset_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_hashtable_init( &this->hashtable, FTS_HASHTABLE_PTR, FTS_HASHTABLE_SMALL);
}

static void fts_objectset_destroy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;

  fts_hashtable_destroy( &this->hashtable);
  fts_free((void *)this);
}

static void fts_objectset_clear( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_hashtable_clear( &this->hashtable);
  
  fts_client_send_message((fts_object_t *)this, sym_objectset_clear, 0, 0);
}
static void fts_objectset_add( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_atom_t k, v;

  fts_object_t *obj = (fts_object_t *)fts_get_object(&at[0]);
  fts_set_ptr( &k, obj);
  fts_set_void( &v);
  fts_hashtable_put( &this->hashtable, &k, &v);

  if (!fts_object_has_id(obj))
    fts_client_upload_object(obj);

  fts_client_send_message((fts_object_t *)this, sym_objectset_append, 1, at);
}

static void fts_objectset_remove( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_atom_t k;

  fts_set_ptr( &k, (fts_object_t *)fts_get_object(&at[0]));
  fts_hashtable_remove( &this->hashtable, &k);

  fts_client_send_message((fts_object_t *)this, sym_objectset_remove, 1, at);
}

static void fts_objectset_get_objects( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;  
  fts_hashtable_get_keys(&this->hashtable, (fts_iterator_t *)fts_get_data(&at[0]));
}

static void fts_objectset_find( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_object_t *scope = fts_get_object(at);
  fts_atom_t a[256];
  int i;
  
  fts_send_message((fts_object_t *)this, fts_SystemInlet, sym_objectset_clear, 0, 0);/*???????*/

  fts_set_object(&a[0], (fts_object_t *) this);

  for (i = 1; (i < ac) && (i < 256); i++)
    a[i] = at[i];

  fts_send_message(scope, fts_SystemInlet, fts_s_find, ac, a);
}

static void fts_objectset_find_errors( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_object_t *scope = fts_get_object(at);
  fts_atom_t a[1];

  fts_send_message((fts_object_t *)this, fts_SystemInlet, sym_objectset_clear, 0, 0);/*???????*/

  fts_set_object(&a[0], (fts_object_t *) this);
  fts_send_message(scope, fts_SystemInlet, fts_s_find_errors, 1, a);
}


static fts_status_t
objectset_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_objectset_t), 0, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_objectset_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_objectset_destroy);

  fts_method_define_varargs(cl, fts_SystemInlet, sym_objectset_add, fts_objectset_add);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_objectset_remove, fts_objectset_remove);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_objectset_get_objects, fts_objectset_get_objects);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_objectset_clear, fts_objectset_clear);

  fts_method_define_varargs(cl, fts_SystemInlet, sym_objectset_find, fts_objectset_find);
  fts_method_define_varargs(cl, fts_SystemInlet, sym_objectset_find_errors, fts_objectset_find_errors);

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_objectset_init( void)
{
  objectset_symbol = fts_new_symbol("__objectset");
  sym_objectset_add = fts_new_symbol("add");
  sym_objectset_remove = fts_new_symbol("remove");
  sym_objectset_clear = fts_new_symbol("clear");
  sym_objectset_get_objects = fts_new_symbol("get_objects");

  sym_objectset_append = fts_new_symbol("append");
  sym_objectset_find = fts_new_symbol("objectset_find");
  sym_objectset_find_errors = fts_new_symbol("objectset_find_errors");
  
  fts_class_install(objectset_symbol, objectset_instantiate);
}



