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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef _FTS_PATCHER_OBJECT_H_
#define _FTS_PATCHER_OBJECT_H_

typedef struct plist_cell {
  fts_symbol_t property;
  fts_atom_t value;
  struct plist_cell *next;
} fts_plist_cell_t;

typedef struct fts_object_patcher_data {
  /* the object description */
  int argc;
  fts_atom_t *argv;

  /* names refered by the object */
  fts_list_t *name_refs;

  /* patcher housekeeping */
  fts_patcher_t *patcher;
  fts_object_t *next_in_patcher;

  /* connections */
  int n_inlets;
  int n_outlets;
  fts_connection_t **in_conn;
  fts_connection_t **out_conn;

  /* properties */
  fts_plist_cell_t *properties;
  
} fts_object_patcher_data_t;

FTS_API void fts_object_set_dirty(fts_object_t *o);

/**
* Evaluate an object description.
 * An object description is supposed to begin with a class name followed
 * by the object constructor arguments.
 * fts_eval_object_description evaluates all the arguments using the
 * expression syntax defined in parser.y
 *
 * If patcher is not NULL, the created instance will be added as child of the patcher object.
 *
 * @fn fts_object_t *fts_eval_object_description(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
 * @param patcher the patcher containing of the created object
 * @param ac argument count
 * @param at the arguments (including the class name)
 */
FTS_API fts_object_t *fts_eval_object_description(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

/* object description (system functions) */
#define fts_object_get_description_size(o) (((o)->patcher_data != NULL)? ((o)->patcher_data->argc): 0)
#define fts_object_get_description_atoms(o) (((o)->patcher_data != NULL)? ((o)->patcher_data->argv): NULL)
FTS_API void fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv);

/* inlets and outlets */
#define fts_object_get_outlets_number(o) (((o)->patcher_data != NULL)? ((o)->patcher_data->n_outlets): 0)
#define fts_object_get_inlets_number(o) (((o)->patcher_data != NULL)? ((o)->patcher_data->n_inlets): 0)
FTS_API void fts_object_set_outlets_number(fts_object_t *o, int n);
FTS_API void fts_object_set_inlets_number(fts_object_t *o, int n);

/* object in patcher */
#define fts_object_get_patcher(o) (((o)->patcher_data != NULL)? ((o)->patcher_data->patcher): NULL)
FTS_API void fts_object_set_patcher(fts_object_t *o, fts_patcher_t *patcher);

/* test recursively if an object is inside a patcher (or its subpatchers) */
FTS_API int fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher);

/* inlets and outlets */
#define fts_object_inlet_is_connected(o, i) (((o)->patcher_data != NULL)? ((o)->patcher_data->in_conn[(i)] != 0): 0)
#define fts_object_outlet_is_connected(o, i) (((o)->patcher_data != NULL)? ((o)->patcher_data->out_conn[(i)] != 0): 0)

/* package */
FTS_API fts_package_t *fts_object_get_package(fts_object_t *obj);

#endif
