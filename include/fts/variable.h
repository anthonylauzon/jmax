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

typedef struct fts_binding_list fts_binding_list_t;
typedef struct fts_mess_obj_list fts_mess_obj_list_t;
typedef struct fts_binding fts_binding_t;
typedef struct fts_env fts_env_t;

/* Commodity structure to keep a list of objects */
struct fts_mess_obj_list
{
  fts_object_t *obj;

  struct fts_mess_obj_list *next;
};


/* Commodity structure to keep a list of bindings */
struct fts_binding_list
{
  fts_binding_t *var;

  struct fts_binding_list *next;
};


/* Variable structure */
struct fts_binding
{
  fts_symbol_t   name;
  int            suspended;
  fts_atom_t     value;
  fts_mess_obj_list_t *users;	/* object that use this variables */
  fts_mess_obj_list_t *definitions;/* object that want to redefine this variables locally, if any*/
  struct fts_env  *env;		/* back pointer to the environment where the variable is stored */
  fts_binding_t *next;		/* next in the environent */
};


/* Variable environment */
struct fts_env
{
  fts_binding_t *first;
  fts_object_t *patcher;
};

/* Access the value of a variable in the scope represented by an object */
FTS_API fts_atom_t *fts_variable_get_value(fts_patcher_t *scope, fts_symbol_t name);
/* Access the value of a variable in the given scope or create void place holder in root patcher */
FTS_API fts_atom_t *fts_variable_get_value_or_void(fts_patcher_t *scope, fts_symbol_t name);

/* Add a user to a variable in a given scope.
 * A user is an object that referentiate the variable and so need to 
 * be redefined when the variable change value.
 */
FTS_API void fts_variable_add_user(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *user);
FTS_API void fts_variable_remove_user(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *user);

