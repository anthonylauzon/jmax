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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */



#define FTS_NO_ID -1
#define FTS_DELETE -2

struct fts_object 
{
  fts_class_t *cl;

  /* id for the client communication */
  int client_id;
  
  /* name definition */
  fts_definition_t *definition;
  
  /* the object description */
  int argc;
  fts_atom_t *argv;

  /* patcher housekeeping */
  fts_patcher_t *patcher;
  fts_object_t *next_in_patcher;

  /* names refered by the object */
  fts_list_t *name_refs;

  /* connections */
  int n_inlets;
  int n_outlets;
  fts_connection_t **in_conn;
  fts_connection_t **out_conn;

  /* object dynamic properties */
  fts_plist_t *properties;

  /* reference counter */
  int refcnt;
};

/**
 * Create an instance of a class.
 *
 * A new instance of the class is created and initialized.
 * If parent is not NULL, the created instance will be added as child to the parent object.
 *
 * @fn fts_object_t *fts_object_create( fts_class_t *cl, fts_object_t *parent, int ac, const fts_atom_t *at)
 * @param cl the class to instantiate
 * @param parent the parent of the created object
 * @param ac argument count
 * @param at the arguments
 * @return the created object, NULL if instantiation failed
 */
FTS_API fts_object_t *fts_object_create(fts_class_t *cl, fts_patcher_t *patcher, int ac, const fts_atom_t *at);
FTS_API void fts_object_destroy(fts_object_t *obj);

FTS_API fts_object_t *fts_eval_object_description(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

/* Garbage collector handling */
#define fts_object_refer(o) (((fts_object_t *)(o))->refcnt++)
#define fts_object_release(o) ((--(((fts_object_t *)(o))->refcnt) > 0)? 0: (fts_object_destroy((fts_object_t *)(o)), 0))

#define fts_object_has_only_one_reference(o) (((fts_object_t *)(o))->refcnt == 1)

/* traditional object in patcher functions */
FTS_API void fts_object_delete_from_patcher(fts_object_t *obj);

/* object description (system functions) */
FTS_API void fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv);

#define fts_object_get_description_size(o) ((o)->argc)
#define fts_object_get_description_atoms(o) ((o)->argv)

#define fts_object_get_outlets_number(o) (((fts_object_t *)(o))->n_outlets)
#define fts_object_get_inlets_number(o) (((fts_object_t *)(o))->n_inlets)
FTS_API void fts_object_set_outlets_number(fts_object_t *o, int n);
FTS_API void fts_object_set_inlets_number(fts_object_t *o, int n);

#define fts_object_get_patcher(o) (((fts_object_t *)(o))->patcher)
#define fts_object_set_patcher(o, patcher) (((fts_object_t *)(o))->patcher = (patcher))

FTS_API fts_symbol_t fts_object_get_class_name(fts_object_t *obj);

#define fts_object_get_id(o) ((o)->client_id)
#define fts_object_has_id(o) (fts_object_get_id(o) > FTS_NO_ID)
#define fts_object_get_class(o) ((o)->cl)

/* test recursively if an object is inside a patcher (or its subpatchers) */
FTS_API int fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher);

#define fts_object_inlet_is_connected(o, i) ((o)->in_conn[(i)] != 0)
#define fts_object_outlet_is_connected(o, i) ((o)->out_conn[(i)] != 0)

FTS_API fts_package_t *fts_object_get_package(fts_object_t *obj);



