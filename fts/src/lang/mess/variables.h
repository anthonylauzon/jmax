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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _VARIABLES_H_
#define _VARIABLES_H_

/* Lower level functions */


extern void fts_env_init(fts_env_t *env, fts_object_t *patcher);
extern void fts_binding_remove_user(fts_binding_t *var, fts_object_t *object);


/* FTS Variable handling */



/* Bind a value/object to a variable in the inner scope corresponding to the
   passed object ; cause automatic redefinition of any object
   within the scope of the variable that referred an outer variable with the same
   name.

   The owner argument is the object that actually created the binding; is it registered,
   and can be used as a way to identify variables to undefine/suspend.

   If the variable is suspended, the variable is redefined not suspended, and all
   the recursively dependent objects are redefined, and their bindings recursively
   redefined.
 */

extern void fts_variable_define(fts_patcher_t *scope, fts_symbol_t name);


/* Verify if a fts_variable_define can be issued in the passed scope.
   Note that this is not like testing if a variable is bound; the binding can be 
   inherited from a surrounding patcher.
 */

extern int fts_variable_can_define(fts_patcher_t *scope, fts_symbol_t name);

/* Return 1 if the variable exists *and* it is suspended */

extern int fts_variable_is_suspended(fts_patcher_t *scope, fts_symbol_t name);

/*
  Remove the variable bound to the name in the scope represented by the object.
   Can be called also on suspended variables; in this case, it recursively resolve
   all the suspended bindings dependent on this one; to resolve means in this
   case to redefine the object, and either redefine or undefine the object
   binding.
   */


extern void fts_variable_undefine(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *owner);

/*
  Like fts_variable_undefine, but act on all the variables in the current scope defined
  by 'owner'
  */

extern void fts_variables_undefine(fts_patcher_t *scope, fts_object_t *owner);

/*
  Suspend  the variable bound to the name in the scope represented by the object;
  To suspend means to keep the dependency structure and value, but don't consider
  the binding valid; all the binding to objects dependent on this variable are
  recursively suspended.

  Note that a variable should be suspended only temporarly; i.e. variables are
  never suspended between two user level atomic operations.
  */


extern void fts_variable_suspend(fts_patcher_t *scope, fts_symbol_t name);

/*
  Like fts_variable_suspend, but act on all the variables in the current scope defined
  by 'owner'
  */

extern void fts_variables_suspend(fts_patcher_t *scope, fts_object_t *owner);

/*
 * Undefine all the variables in the given scope that are suspended.
 * 
 */

extern void fts_variables_undefine_suspended(fts_patcher_t *scope, fts_object_t *owner);

/* Restore a variable */

extern void fts_variable_restore(fts_patcher_t *scope, fts_symbol_t name, fts_atom_t *value, fts_object_t *owner);

/* Access the value of a variable in the scope represented by an object */

extern fts_atom_t *fts_variable_get_value(fts_patcher_t *scope, fts_symbol_t name);


/*
  Add a user to a variable in a given scope.
  A user is an object that referentiate the variable and so need to 
  be redefined when the variable change value.
  */

extern void fts_variable_add_user(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *user);




#endif



