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


/* FTS Variable handling */

/*
 * Structure description.
 *
 * fts_binding_t : is the implementation of the binding of a name to a value.
 *              i.e. the low level implementation of a variable.
 *
 * fts_env_t  : it is a list of bindings; represent an environment mappin
 *              a set of names to a set of values.
 */


#include "sys.h"
#include "lang.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/datalib.h"

/* #define TRACE_DEBUG */

/****************************************************************************/
/*                                                                          */
/*  Binding direct Manipulation                                             */
/*                                                                          */
/****************************************************************************/

static fts_heap_t *bindings_heap;
static fts_heap_t *objlist_heap;
static fts_heap_t *var_refs_heap;

/* Create a new binding in the given environment */

static fts_binding_t *fts_binding_new(fts_env_t *env, fts_symbol_t name, fts_atom_t *value)
{
  fts_binding_t *v;

  v = (fts_binding_t *) fts_heap_alloc(bindings_heap);

  v->name = name;
  v->env  = env;
  v->next  = 0;
  v->users = 0;
  v->suspended = 0;
  v->value = *value;
  v->definitions = 0;

#ifdef TRACE_DEBUG 
  fprintf(stderr, "New Binding in ");
  fprintf_object(stderr, env->patcher);
  fprintf(stderr, " for variable %s, value ", fts_symbol_name(name));
  fprintf_atoms(stderr, 1, value);
  fprintf(stderr, "\n");
#endif

  return v;
}

/* Delete a new binding in the given environment.
   Recompute all the object referring this binding.
   */


static void fts_binding_delete(fts_binding_t *var)
{
  fts_binding_t **b;		/* indirect precursor */

#ifdef TRACE_DEBUG
  fprintf(stderr, "Remove Binding for variable %s\n", fts_symbol_name(var->name));
#endif

  /* Remove the binding from the environment */

  if (var->env)
    for (b = &(var->env->first); *b; b = &((*b)->next))
      if (*b == var)
	{
	  *b = var->next;

	  break;
	}

  /* The binding is no more in the environment; we can now
     redefine the object, without any risk of inference.
     We don't free the list, because is freed by the redefined
     objects automatically.
     */


  while (var->users)
    {
      fts_object_list_t   *u;
      fts_object_t *obj;
      fts_object_list_t *tmp;

      u = var->users;

#ifdef TRACE_DEBUG
      fprintf(stderr, "\t");
      fprintf_object(stderr, u->obj);
      fprintf(stderr, "\n");
#endif

      fts_object_recompute(u->obj);
    }

#ifdef TRACE_DEBUG
  fprintf(stderr, "Done.\n");
#endif

  /* free the binding */

  fts_heap_free((char *)var, bindings_heap);
}


/* Suspend the binding; recursively suspend all the variables
   defined by objects referring this binding.
   */

static void fts_binding_suspend(fts_binding_t *var)
{
  fts_object_list_t *user;

#ifdef TRACE_DEBUG
  fprintf(stderr, "Suspend Binding for variable %s\n", fts_symbol_name(var->name));
  fprintf(stderr, "Suspending variable users:\n");
#endif

  var->suspended = 1;

  /* Recursive calls to suspend all the variables bound to users of this
     binding */

  for (user = var->users; user; user = user->next)
    if (! fts_object_is_error(user->obj))
      {
	fts_symbol_t user_var = fts_object_get_variable(user->obj);

#ifdef TRACE_DEBUG
	fprintf(stderr, "\t");
	fprintf_object(stderr, user->obj);
	fprintf(stderr, "\n");
#endif
	if(user_var)
	  fts_variable_suspend(user->obj->patcher, user_var);
      }

#ifdef TRACE_DEBUG
  fprintf(stderr, "Done.\n");
#endif
}

/*
 * Restore a binding: assign a final value to it, and recompute all the objects
 * referring to this binding.
 */

static void fts_binding_restore(fts_binding_t *var, fts_atom_t *value)
{
  fts_object_list_t *u;

  var->suspended = 0;
  var->value = *value;

#ifdef TRACE_DEBUG
  fprintf(stderr, "Restoring Binding for variable %s, value ", fts_symbol_name(var->name));
  fprintf_atoms(stderr, 1, value);
  fprintf(stderr, "\n");
#endif

  /* Recursive calls to recover all the variables bind to users of this
     binding, and redefine the objects */

#ifdef TRACE_DEBUG
  fprintf(stderr, "Recomputing: \n");
#endif

  /* this loop remove objects from the users list, while indirectly
     the calls will add objects to the user list; what we do then
     is to remove the user list from the binding, and loop
     on it; the redefinition will implicitly re-add the users to the
     user list; we free the old user list during the loop */

  u = var->users;
  var->users = 0;

  while (u)
    {
      fts_object_t *obj;
      fts_object_list_t *tmp;

      tmp = u;
      u = u->next;
	  
      obj = tmp->obj;
	  
      fts_heap_free((char *)tmp, objlist_heap);

#ifdef TRACE_DEBUG
      fprintf(stderr, "\t");
      fprintf_object(stderr, obj);
      fprintf(stderr, "\n");
#endif
      fts_object_recompute(obj);
    }

#ifdef TRACE_DEBUG
  fprintf(stderr, "Done.\n");
#endif
}


/* Check if a binding is currently suspended */

static int fts_binding_is_suspended(fts_binding_t *var)
{
  return var->suspended;
}

/* Add a user to a binding, i.e. an object that is referring the binding in its description */

static void fts_binding_add_user(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t *u;
  fts_binding_list_t *b;

  u = (fts_object_list_t *) fts_heap_alloc(objlist_heap);

  u->obj  = object;
  u->next = var->users;
  var->users = u;

  /* Add the var_refs in the object */

  b = (fts_binding_list_t *) fts_heap_alloc(var_refs_heap);

  b->next = object->var_refs;
  b->var = var;
  object->var_refs = b;

#ifdef TRACE_DEBUG
  fprintf(stderr, "New User ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s \n", fts_symbol_name(var->name));
#endif
}

/* Remove  a user to a binding, i.e. an object that is referring the binding in its description */

void fts_binding_remove_user(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t   **u;	/* indirect precursor */
  fts_binding_list_t  **b;	/* indirect precursor */

#ifdef TRACE_DEBUG
  fprintf(stderr, "Remove User ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s\n", fts_symbol_name(var->name));
#endif

  /* Remove the user from the binding; ignore the case where the binding
     is not there any more */

  for (u = &(var->users); *u; u = &((*u)->next))
    if ((*u)->obj == object)
	{
	  fts_object_list_t   *p;

	  p = *u;
	  *u = (*u)->next;

	  fts_heap_free((char *)p, objlist_heap);
	  break;
	}


  /* remove the var_refs in the object */

  for (b = &(object->var_refs); *b; b = &((*b)->next))
    if ((*b)->var == var)
	{
	  fts_binding_list_t   *p;

	  p = *b;
	  *b = (*b)->next;

	  fts_heap_free((char *)p, var_refs_heap);
	  break;
	}
}


/* Add all the users of a binding to the given object set.
   Used for the find friends of an object defining a variable */

static void fts_binding_add_users_to_set(fts_binding_t *var, fts_object_set_t *set)
{
  fts_object_list_t  *u;	

  for (u = var->users; u; u = u->next)
    fts_object_set_add(set, u->obj);
}


/* Check if a binding is defined by a given object */

static int fts_binding_defined_by(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t  *d;	

  for (d = var->definitions; d; d = d->next)
    {
      if (object == d->obj)
	return 1;
    }
  
  return 0;
}

/* Check if a binding is defined by a given object, and
   only by the given object, i.e. the binding is not a doubly
   defined variable */

static int fts_binding_defined_only_by(fts_binding_t *var, fts_object_t *object)
{
  if (var->definitions && (! var->definitions->next))
    return var->definitions->obj == object;
  else
    return 0;
}


/* Add a definition to a binding; if there is already a definition, and
   only one, recompute it, to make it an error.
   */

static void fts_binding_add_definition(fts_binding_t *var, fts_object_t *object)
{
  fts_object_t *owner = 0;
  fts_object_list_t *d;

  /* First, check if the owner is already in the definition  list,
     in this case just return  */

  for (d = var->definitions; d; d = d->next)
    {
      if (object == d->obj)
	return;
    }

  /* If there is only one definition , recompute it  after having added this one */

  if (var->definitions && (! var->definitions->next))
    {
      owner = var->definitions->obj;
    }

  d = (fts_object_list_t *) fts_heap_alloc(objlist_heap);

  d->next = var->definitions;
  d->obj = object;
  var->definitions = d;
  
#ifdef TRACE_DEBUG
  fprintf(stderr, "New Definition ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s \n", fts_symbol_name(var->name));
  fprintf(stderr, "\n");
#endif

  if (owner && (! var->suspended))
    {
#ifdef TRACE_DEBUG
      fprintf(stderr, "After double definition of  %s, recomputing ", fts_symbol_name(var->name));
      fprintf_object(stderr, owner);
      fprintf(stderr, "\n");
#endif
      fts_object_recompute(owner);
    }
}

/* Remove a definition from a binding; if only one definition is left,
   recompute it, so to reinstantiate the good object */

static void fts_binding_remove_definition(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t  **d;	/* indirect precursor */

#ifdef TRACE_DEBUG 
  fprintf(stderr, "Remove Definition ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s\n", fts_symbol_name(var->name));
#endif 

  /* Remove the definition from the binding */

  for (d = &(var->definitions); *d; d = &((*d)->next))
    if ((*d)->obj == object)
	{
	  fts_object_list_t   *p;

	  p = *d;
	  *d = (*d)->next;

	  fts_heap_free((char *)p, objlist_heap);
	  break;
	}

  /* if the binding is not suspendend, do some housekeeping */

  if (! var->suspended)
    {
      if (var->definitions && (! var->definitions->next))
	{
	  /* If there is only one definition left, recompute it */
      
	  fts_object_recompute(var->definitions->obj);
	}
      else if(! var->definitions)
	{
	  /* remove binding when last definition is deleted */
	  fts_binding_delete(var);
	}
    }
}

/* Add all the definitions of a binding to the given object set.
   Used for the find friends of an object defining a variable */

static void fts_binding_add_definitions_to_set(fts_binding_t *var, fts_object_set_t *set)
{
  fts_object_list_t  *u;	

  for (u = var->definitions; u; u = u->next)
    fts_object_set_add(set, u->obj);
}


/****************************************************************************/
/*                                                                          */
/*  Environent Manipulation                                                 */
/*                                                                          */
/****************************************************************************/

/* Initialize an environment, i.e. the list of bindings of a patcher */

void fts_env_init(fts_env_t *env, fts_object_t *patcher)
{
  env->first = 0;
  env->patcher = patcher;
}


/* Add a binding to an environment */

static fts_binding_t *fts_env_add_binding(fts_env_t *env, fts_symbol_t name, fts_atom_t *value)
{
  fts_binding_t *var;

  var = fts_binding_new(env, name, value);
  
  var->next = env->first;
  env->first = var;

  return var;
}


/* Remove a binding from an environment */

static void fts_env_remove_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t **p;		/* indirect precursor */

  p = &(env->first);

  /* Look and remove the binding */

  while (*p)
    {
      if (fts_binding_defined_only_by((*p), owner))
	{
	  fts_binding_t *var = *p;

	  *p = (*p)->next;
	  var->env = 0;		/* to prevent the binding from removing itself */
	  fts_binding_delete(var);
	}
      else if (fts_binding_defined_by((*p), owner))
	{
	  fts_binding_remove_definition((*p), owner);
	}
      else
	p = &((*p)->next);
    }
}


/* Remove all the bindings of an environment that are suspended and
   that have the owner argument as unique definitions; used for patchers */


static void fts_env_remove_suspended_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t **p;		/* indirect precursor */

  p = &(env->first);

  /* Look and remove the binding */

  while (*p)
    if (fts_binding_defined_only_by((*p), owner) && (*p)->suspended)
      {
	fts_binding_t *var = *p;

	*p = (*p)->next;
	var->env = 0;		/* to prevent the binding from removing itself */
	fts_binding_delete(var);
      }
    else
      p = &((*p)->next);
}


/* Suspend all the bindings in an environment that have the owner argument as
   unique definitions; used for patchers */

static void fts_env_suspend_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t *var = 0;

  var = env->first;

  while (var)
    {
      if (fts_binding_defined_only_by(var, owner))
	fts_binding_suspend(var);

      var = var->next;
    }
}


/* Look in the environment for a binding for a given
   variable name */

static fts_binding_t *fts_env_get_binding(fts_env_t *env, fts_symbol_t name)
{
  fts_binding_t *var = 0;

  for (var = env->first; var; var = var->next)
    if (var->name == name)
      return var;

  return 0;
}



/****************************************************************************/
/*                                                                          */
/*    Scope handlingand  High level Public API                              */
/*                                                                          */
/****************************************************************************/

static int
fts_variable_is_global(fts_symbol_t name)
{
  char c = fts_symbol_name(name)[0];

  /* variables with name starting with a capital letter are GLOBAL */
  return (c >= 'A' && c <= 'Z');

}

static fts_patcher_t *
fts_variable_get_scope(fts_patcher_t *scope, fts_symbol_t name)
{
  if(fts_variable_is_global(name))
    return fts_get_root_patcher();
  else
    return scope;
}

/* Get a binding in a given scope for a given variable name.
   Search iteratively in the containing patchers ..
   */

static fts_binding_t *fts_variable_get_binding(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_patcher_t *patcher;

  scope = fts_variable_get_scope(scope, name);

  patcher = scope;

  while (patcher)
    {
      fts_binding_t *v;

      v = fts_env_get_binding(fts_patcher_get_env(patcher), name);

      if (v)
	{
	  return v;
	}
      
      patcher = fts_object_get_patcher((fts_object_t *) patcher);
    }

  return 0;
}


/*
  Define a suspended variable in the inner scope corresponding to the
  passed object ; cause automatic stealing of any object
  within the scope of the variable that referred an outer variable with the same
  name; the actual redefinition will be made at "restore" time.

 */

void fts_variable_define(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_atom_t value;
  fts_binding_t *v, *up_v;

  scope = fts_variable_get_scope(scope, name);

  fts_set_void(&value);
  
  /* get a binding relevant for this scope (in this patcher or its parents) */
  up_v = fts_variable_get_binding(scope, name);
  
  /* check if there is already a binding in this patcher */
  v = fts_env_get_binding(fts_patcher_get_env(scope), name);
  
  if (v)
    {
      if(fts_variable_is_global(name) && fts_is_void(&v->value))
	fts_binding_suspend(v);
      else
	return; /* double definition */
    }
  else
    {
      /* add binding to this patcher */
      v = fts_env_add_binding(fts_patcher_get_env(scope), name, &value);
      
      /* suspend recursivly all bindings referring to this variable */
      fts_binding_suspend(v);

      /* does the new variable (v) shadow a variable from the parent scope (up_v)! */
      if (up_v)
	{
	  fts_object_list_t   **u;	/* indirect precursor */
	  
#ifdef TRACE_DEBUG
	  fprintf(stderr, "Found upper variable for %s\n Stealing: \n", fts_symbol_name(up_v->name));
#endif      
	  
	  /* steal all the references to the varable (users) from parent scope and suspend their references */
	  u = &(up_v->users);
	  while (*u)
	    {
	      fts_object_t *user = (* u)->obj;
	      
	      if (fts_object_is_in_patcher((*u)->obj, scope))
		{
#ifdef TRACE_DEBUG
		  fprintf(stderr, "\t");
		  fprintf_object(stderr, user);
		  fprintf(stderr, "\n");
		  fprintf(stderr, " stealed\n");
#endif
		  
		  /* remove user from old binding and add to new binding */
		  fts_binding_remove_user(up_v, user);
		  fts_binding_add_user(v, user);
		  
		  /* suspend recursivly all variables referring to the users variable */
		  if (fts_object_get_variable(user))
		    fts_variable_suspend(user->patcher, fts_object_get_variable(user));
		}
	      else
		u = &((*u)->next);
	    }
#ifdef TRACE_DEBUG
	  fprintf(stderr, "Done.\n");
#endif
	}
      else
	{
#ifdef TRACE_DEBUG
	  fprintf(stderr, " upper variable for %s not found.\n", fts_symbol_name(name));
#endif
	}
    }
}

/* Verify if a fts_variable_define can be issued in the passed scope.
   Note that this is not like testing if a variable is bound; the binding can be 
   inherited from a surrounding patcher.
 */

int fts_variable_can_define(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  if (v == 0 || fts_is_void(&v->value))
    return 1;
  else if ((v->definitions == 0) && fts_binding_is_suspended(v))
    return 1;
  else
    return 0;
}


/* Return 1 if the variable exists *and* it is suspended */

int fts_variable_is_suspended(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  return (v != 0) && fts_binding_is_suspended(v);
}


/* Remove the variable bound to the name in the scope represented by the object;
   the removing is generic enough to support any possible style of future declarations.
   cause a redefinition of all the objects that use the same variable.
   Can be called also on suspended variables; in this case, it recursively resolve
   all the suspended bindings dependent on this one; to resolve means in this
   case to redefine the object, and either redefine or undefine the object
   binding.
   */


void fts_variable_undefine(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *owner)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_remove_definition(var, owner);
}


/* Like fts_variable_undefine, but act on all the variables defined by 'owner' in the current scope */

void fts_variables_undefine(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_env_remove_bindings(fts_patcher_get_env(scope), owner);
}


/*
  Suspend  the variable bound to the name in the scope represented by the object;
  To suspend means to keep the dependency structure and value, but don't consider
  the binding valid; all the binding to objects dependent on this variable are
  recursively suspended.

  Note that a variable should be suspended only temporarly; i.e. variables are
  never suspended between two user level atomic operations.
  */

void fts_variable_suspend(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_suspend(var);
}


/* Like fts_variable_suspend, but act on all the variables defined by 'owner' in the current scope */

void fts_variables_suspend(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_env_suspend_bindings(fts_patcher_get_env(scope), owner);
}


/* Like fts_variable_suspend, but act on all the variables defined by 'owner' in the current scope and suspended. */

void fts_variables_undefine_suspended(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_env_remove_suspended_bindings(fts_patcher_get_env(scope), owner);
}


/*
 * Restore a suspended variable.
 *
 * All the recursively dependent objects are redefined, and their
 * bindings recursively redefined.
 * A variable may be restored by a different owner in case of object redefinition.
 *
 */

void fts_variable_restore(fts_patcher_t *scope, fts_symbol_t name, fts_atom_t *value, fts_object_t *owner)
{
  fts_patcher_t *patcher;
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  if (v)
    fts_binding_add_definition(v, owner);

  if (v && fts_binding_is_suspended(v))
    fts_binding_restore(v, value);
}


/* Access the value of a variable in the given scope  */

fts_atom_t *fts_variable_get_value(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);

  v = fts_variable_get_binding(scope, name);

  if (! v)
    {
      fts_atom_t a;

      /* make the root void variable here */

      fts_set_void(&a);
      v = fts_env_add_binding(fts_patcher_get_env(fts_get_root_patcher()), name, &a);
    }

  if (fts_binding_is_suspended(v))
    {
      return 0;
    }
  else
    return &(v->value);
}


/*
  Add a user to a variable in a given scope.
  A user is an object that referentiate the variable and so need to 
  be redefined when the variable change value.
  */

void fts_variable_add_user(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *user)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  fts_binding_add_user(var, user);
}


/* Support for find: find all the users and definitions of a variable visible in the given scope */

void fts_variable_find_users(fts_patcher_t *scope, fts_symbol_t name, fts_object_set_t *set)
{
  fts_binding_t *b;

  scope = fts_variable_get_scope(scope, name);

  b = fts_variable_get_binding(scope, name);

  if (b)
    {
      fts_binding_add_users_to_set(b, set);
      fts_binding_add_definitions_to_set(b, set);
    }
}


/* Module init function */

void fts_variables_init(void)
{
  bindings_heap = fts_heap_new(sizeof(fts_binding_t));
  objlist_heap  = fts_heap_new(sizeof(fts_object_list_t));
  var_refs_heap = fts_heap_new(sizeof(fts_binding_list_t));
}
