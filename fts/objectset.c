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
#include <ftsprivate/OLDclient.h>

fts_symbol_t objectset_symbol = 0;
fts_symbol_t sym_objectset_append = 0;


/***********************************************************************
 *
 * Fts objectset functions
 *
 */

fts_objectset_t* 
fts_objectset_create()
{
  return (fts_objectset_t *) fts_object_create(fts_class_get_by_name(objectset_symbol), 0, 0);
}

void 
fts_objectset_destroy( fts_objectset_t *set)
{
  fts_object_destroy( (fts_object_t*) set);
}

void 
fts_objectset_clear( fts_objectset_t *set)
{
  fts_hashtable_clear( &set->hashtable);
}

void 
fts_objectset_add( fts_objectset_t *set, fts_object_t *object)
{
  fts_atom_t k, v;
  int exits_already;

  fts_set_ptr( &k, object);
  fts_set_void( &v);

  exits_already = fts_hashtable_put( &set->hashtable, &k, &v);

  /* inform the client if necessary */
  if (fts_object_has_id((fts_object_t *) set) && !exits_already) {

    if (!fts_object_has_id(object))
      fts_client_upload_object(object);
    
    fts_client_send_message((fts_object_t *) set, sym_objectset_append, 1, &k);
  }
}

void 
fts_objectset_remove( fts_objectset_t *set, fts_object_t *object)
{
  fts_atom_t k;

  fts_set_ptr( &k, object);
  fts_hashtable_remove( &set->hashtable, &k);
}

void 
fts_objectset_get_objects( const fts_objectset_t *set, fts_iterator_t *i)
{
  fts_hashtable_get_keys( &set->hashtable, i);
}

/***********************************************************************
 *
 * Fts class/object interface
 *
 */

static void 
fts_objectset_method_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_hashtable_init( &this->hashtable, FTS_HASHTABLE_PTR, FTS_HASHTABLE_SMALL);
}

static void 
fts_objectset_method_destroy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_objectset_t *this = (fts_objectset_t *)o;
  fts_hashtable_destroy( &this->hashtable);
}

static fts_status_t
objectset_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_objectset_t), 0, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_objectset_method_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fts_objectset_method_destroy);

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_objectset_config( void)
{
  objectset_symbol = fts_new_symbol("__objectset");
  sym_objectset_append = fts_new_symbol("append");
  fts_class_install( objectset_symbol, objectset_instantiate);
}