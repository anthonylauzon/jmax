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

void fts_objectset_init( fts_objectset_t *set)
{
  fts_hashtable_init( &set->hashtable, fts_t_ptr, FTS_HASHTABLE_SMALL);
}

void fts_objectset_clear( fts_objectset_t *set)
{
  fts_hashtable_clear( &set->hashtable);
}

void fts_objectset_destroy( fts_objectset_t *set)
{
  fts_hashtable_destroy( &set->hashtable);
}

void fts_objectset_add( fts_objectset_t *set, fts_object_t *object)
{
  fts_atom_t k, v;

  fts_set_ptr( &k, object);
  fts_set_void( &v);
  fts_hashtable_put( &set->hashtable, &k, &v);
}

void fts_objectset_remove( fts_objectset_t *set, fts_object_t *object)
{
  fts_atom_t k;

  fts_set_ptr( &k, object);
  fts_hashtable_remove( &set->hashtable, &k);
}

void fts_objectset_get_objects( const fts_objectset_t *set, fts_iterator_t *i)
{
  fts_hashtable_get_keys( &set->hashtable, i);
}
