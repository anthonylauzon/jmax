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
#define FTS_HIDDEN -2

struct fts_simple_object
{
  fts_class_t *cl;
  int id; /* id for the object */
};

struct fts_object
{
  struct fts_simple_object head;

  /* the object description */
  int argc;
  fts_atom_t *argv;

  /* patcher housekeeping */
  fts_patcher_t *patcher;
  fts_object_t  *next_in_patcher;

  /* Variable: If this object is bound to a variable, this is the variable name */
  fts_symbol_t varname;

  /* Variables referred by the object */
  fts_binding_list_t *var_refs; /* handled completely in the variable.c file */

  /* connections */
  fts_connection_t **in_conn;
  fts_connection_t **out_conn;

  /* object dynamic properties */
  fts_plist_t *properties;

  /* reference counter */
  int refcnt;
};


FTS_API fts_object_t *fts_eval_object_description(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

/* create/destroy object without patcher (attention: hack in fts_object_create()!) */
FTS_API fts_object_t *fts_object_create(fts_metaclass_t *mcl, int ac, const fts_atom_t *at);
FTS_API void fts_object_destroy(fts_object_t *obj);

#define fts_object_refer(o) (((fts_object_t *)(o))->refcnt++)
#define fts_object_release(o) ((--(((fts_object_t *)(o))->refcnt) > 0)? 0: (fts_object_destroy((fts_object_t *)(o)), 0))

#define fts_object_has_only_one_reference(o) (((fts_object_t *)(o))->refcnt == 1)

/* traditional object in patcher functions */
FTS_API fts_status_t fts_object_new_to_patcher(fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_object_t **ret);
FTS_API void fts_object_delete_from_patcher(fts_object_t *obj);

/* properties */
FTS_API void fts_object_send_properties(fts_object_t *obj);
FTS_API void fts_object_send_properties_immediately(fts_object_t *obj);
FTS_API void fts_object_send_ui_properties(fts_object_t *obj);

/* object description (system functions) */
FTS_API void fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv);
FTS_API void fts_object_set_description_and_class(fts_object_t *obj, fts_symbol_t class_name, int argc, const fts_atom_t *argv);

#define fts_object_get_description_size(o) ((o)->argc)
#define fts_object_get_description_atoms(o) ((o)->argv)

FTS_API void fts_object_reset_description(fts_object_t *obj);
FTS_API int fts_object_description_defines_variable(int ac, const fts_atom_t *at);

/* object access */
FTS_API fts_symbol_t fts_object_get_outlet_type( fts_object_t *o, int woutlet);

#define fts_object_get_outlets_number(O) (((fts_object_t *)(O))->head.cl->noutlets)
#define fts_object_get_inlets_number(O) (((fts_object_t *)(O))->head.cl->ninlets)
#define fts_object_get_patcher(O) (((fts_object_t *)(O))->patcher)
#define fts_object_set_patcher(O, patcher) (((fts_object_t *)(O))->patcher = (patcher))

FTS_API fts_symbol_t fts_object_get_class_name(fts_object_t *obj);

/* variables */
#define fts_object_get_variable(o) ((o)->varname)
#define fts_object_set_variable(o, name) ((o)->varname = (name))

FTS_API void fts_object_redefine_variable(fts_object_t *o);

#define fts_object_has_id(o) ((o)->head.id > FTS_NO_ID)
#define fts_object_get_id(o) ((o)->head.id)
#define fts_object_get_class(o) ((o)->head.cl)
#define fts_object_get_metaclass(o) ((o)->head.cl->mcl)
#define fts_object_get_user_data(o) ((o)->head.cl->user_data)

/* test recursively if an object is inside a patcher (or its subpatchers) */
FTS_API int fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher);

/* messages for the status line */
FTS_API void fts_object_blip(fts_object_t *obj, const char *format , ...);

/* change number of outlets */
FTS_API void fts_object_change_number_of_outlets(fts_object_t *o, int new_noutlets);

#define fts_object_inlet_is_connected(o, i) ((o)->in_conn[(i)] != 0)
#define fts_object_outlet_is_connected(o, i) ((o)->out_conn[(i)] != 0)

FTS_API fts_package_t *fts_object_get_package(fts_object_t *obj);



