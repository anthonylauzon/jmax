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


typedef struct _fts_tuple_t 
{
  fts_object_t o;
  fts_array_t args;
} fts_tuple_t;

FTS_API fts_metaclass_t *fts_tuple_metaclass;

#define fts_tuple_clear(t) (fts_array_clear(&(t)->args))
#define fts_tuple_set_size(t, n) (fts_array_set_size(&(t)->args, (n)))
#define fts_tuple_set(t, n, a) (fts_array_set(&(t)->args, (n), (a)))

#define fts_tuple_prepend(t, n, a) (fts_array_prepend(&(t)->args, (n), (a)))
#define fts_tuple_append(t, n, a) (fts_array_append(&(t)->args, (n), (a)))
#define fts_tuple_append_int(t, x) (fts_array_append_int(&(t)->args, (x)))
#define fts_tuple_append_float(t, x) (fts_array_append_float(&(t)->args, (x)))
#define fts_tuple_append_symbol(t, x) (fts_array_append_symbol(&(t)->args, (x)))

#define fts_tuple_copy(t, u) (fts_array_copy(&(t)->args, &(u)->args))

#define fts_tuple_get_atoms( t) (fts_array_get_atoms(&(t)->args))
#define fts_tuple_get_size( t) (fts_array_get_size(&(t)->args))
#define fts_tuple_get_element( t, i) (fts_array_get_element(&(t)->args, i))

#define fts_tuple_get_array( t) (&(t)->args)

#define fts_is_tuple(p) (fts_is_a(p, fts_tuple_metaclass))

#define fts_tuple_output(o, i, t) do { \
  fts_object_refer((fts_object_t *)(t)); \
  fts_outlet_atoms((o), (i), fts_tuple_get_size(t), fts_tuple_get_atoms(t)); \
  fts_object_release((fts_object_t *)(t)); \
} while(0)
