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
#define FTS_CREATE -3
#define FTS_INVALID -4

typedef struct fts_object_patcher_data
{
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

} fts_object_patcher_data_t;

struct fts_object 
{
  fts_class_t *cl;

  /* id for the client communication */
  int client_id;
  
  /* reference counter */
  int refcnt;

  /* message cache */
  fts_symbol_t cache_selector;
  fts_class_t *cache_type;
  int cache_varargs;
  fts_method_t cache_method;

  /* name definition */
  fts_definition_t *definition;
  
  /* patcher data */
  fts_object_patcher_data_t *patcher_data;

  /* object dynamic properties */
  fts_plist_t *properties;
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

/* garbage collector handling */
#define fts_object_refer(o) (((fts_object_t *)(o))->refcnt++)
#define fts_object_release(o) ((--(((fts_object_t *)(o))->refcnt) > 0)? 0: (fts_object_destroy((fts_object_t *)(o)), 0))
#define fts_object_has_only_one_reference(o) (((fts_object_t *)(o))->refcnt == 1)

/* client id */
#define fts_object_get_id(o) ((o)->client_id)
#define fts_object_has_id(o) (fts_object_get_id(o) > FTS_NO_ID)

/* class */
#define fts_object_get_class(o) ((o)->cl)
FTS_API fts_symbol_t fts_object_get_class_name(fts_object_t *obj);

/* named objects handling */
FTS_API void fts_object_set_name(fts_object_t *obj, fts_symbol_t sym);
FTS_API fts_symbol_t fts_object_get_name(fts_object_t *obj);
FTS_API void fts_object_update_name(fts_object_t *obj);

/* message cache */
#define fts_object_message_cache_get_selector(o) ((o)->cache_selector)
#define fts_object_message_cache_get_type(o) ((o)->cache_type)
#define fts_object_message_cache_get_varargs(o) ((o)->cache_varargs)
#define fts_object_message_cache_get_method(o) ((o)->cache_method)

#define fts_object_message_cache_set_selector(o, s) ((o)->cache_selector = (s))
#define fts_object_message_cache_set_type(o, t) ((o)->cache_type = (t))
#define fts_object_message_cache_set_varargs(o, v) ((o)->cache_varargs = (v))
#define fts_object_message_cache_set_method(o, m) ((o)->cache_method = (m))

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

FTS_API void fts_object_upload(fts_object_t *obj);

/* package */
FTS_API fts_package_t *fts_object_get_package(fts_object_t *obj);
