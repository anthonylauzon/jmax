/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

typedef struct fts_param_listener
{
  fts_object_t *object;
  fts_method_t callback;
  struct fts_param_listener *next;
} fts_param_listener_t;

typedef struct fts_param
{
  fts_object_t o;
  int persistence;
  fts_atom_t value;
  fts_param_listener_t *listeners;
} fts_param_t;

FTS_API void fts_param_add_listener(fts_param_t *param, fts_object_t *object, fts_method_t method);
FTS_API void fts_param_remove_listener(fts_param_t *param, fts_object_t *object);

FTS_API void fts_param_set(fts_param_t *param, const fts_atom_t *value);
FTS_API void fts_param_set_int(fts_param_t *param, int i);
FTS_API void fts_param_set_float(fts_param_t *param, double f);

#define fts_param_get_value(p) (&((p)->value))

FTS_API fts_class_t *fts_param_class;

