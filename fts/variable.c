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


/****************************************************************************
 *
 *  FTS Variable handling
 *
 *
 *  fts_binding_t
 *    A binding is the implementation of the binding of a name to a value
 *    (i.e. the low level implementation of a variable).
 *
 *  fts_env_t  
 *    The environment consists of a list of bindings.
 */


#include <fts/fts.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/variable.h>

/****************************************************************************
 *
 *  binding direct Manipulation
 *
 */

static fts_heap_t *bindings_heap;
static fts_heap_t *objlist_heap;
static fts_heap_t *var_refs_heap;

/* create a new binding in the given environment */
static fts_binding_t *fts_binding_new(fts_env_t *env, fts_symbol_t name, fts_atom_t *value)
{
  fts_binding_t *v = (fts_binding_t *) fts_heap_alloc(bindings_heap);

  v->name = name;
  v->env  = env;
  v->next  = 0;
  v->users = 0;
  v->suspended = 0;
  v->value = *value;
  v->definitions = 0;

  return v;
}

/* Delete a new binding in the given environment.
   Recompute all the object referring this binding.
   */

static void fts_binding_delete(fts_binding_t *var)
{
  fts_binding_t **b;		/* indirect precursor */

  /* remove the binding from the environment */
  if (var->env)
    for (b = &(var->env->first); *b; b = &((*b)->next))
      if (*b == var)
	{
	  *b = var->next;

	  break;
	}

  /* the binding is no more in the environment:
   * redefine the object, without any risk of inference.
   * We don't free the list, because is freed by the redefined
   * objects automatically.
   */
  while (var->users)
    {
      fts_mess_obj_list_t   *u;

      u = var->users;

      fts_object_recompute(u->obj);
    }

  /* free the binding */
  fts_heap_free((char *)var, bindings_heap);
}

/* recursively suspend all the variables defined by objects referring this binding */
static void fts_binding_suspend(fts_binding_t *var)
{
  fts_mess_obj_list_t *user;

  var->suspended = 1;

  /* recursive calls to suspend all the variables bound to users of this binding */
  for (user = var->users; user; user = user->next)
    {
      if (! fts_object_is_error(user->obj))
	{
	  fts_symbol_t user_var = fts_object_get_variable(user->obj);
	  
	  if(user_var)
	    fts_variable_suspend(user->obj->patcher, user_var);
	}
    }
}

/* assign a final value to it, and recompute all the objects referring to this binding */
static void fts_binding_restore(fts_binding_t *var, fts_atom_t *value)
{
  fts_mess_obj_list_t *u;

  var->suspended = 0;
  var->value = *value;

  /* recursive calls to recover all the variables bind to users of this binding, and redefine the objects */

  /* this loop remove objects from the users list, while indirectly
   * the calls will add objects to the user list; what we do then
   * is to remove the user list from the binding, and loop
   * on it; the redefinition will implicitly re-add the users to the
   * user list; we free the old user list during the loop
   */
  u = var->users;
  var->users = 0;

  while (u)
    {
      fts_mess_obj_list_t *freeme = u;
      fts_object_t *obj = u->obj;

      u = u->next;
	  
      fts_object_recompute(obj);

      fts_heap_free((char *)freeme, objlist_heap);
    }
}

/* check if a binding is currently suspended */
static int fts_binding_is_suspended(fts_binding_t *var)
{
  return var->suspended;
}

/* add a user to a binding (i.e. an object that is referring the binding in its description) */
static void fts_binding_add_user(fts_binding_t *var, fts_object_t *object)
{
  fts_mess_obj_list_t *u = var->users;
  fts_binding_list_t *b;

  /* return if object is already registered as user */
  while(u)
    {
      if(u->obj == object)
	return;
      
      u = u->next;
    }  

  u = (fts_mess_obj_list_t *) fts_heap_alloc(objlist_heap);
  u->obj  = object;
  u->next = var->users;
  var->users = u;

  /* add the var_refs in the object */
  b = (fts_binding_list_t *) fts_heap_alloc(var_refs_heap);

  b->next = object->var_refs;
  b->var = var;
  object->var_refs = b;
}

/* remove  a user to a binding (i.e. an object that is referring the binding in its description) */
void fts_binding_remove_user(fts_binding_t *var, fts_object_t *object)
{
  fts_mess_obj_list_t   **u;	/* indirect precursor */
  fts_binding_list_t  **b;	/* indirect precursor */

#ifdef TRACE_DEBUG
  fprintf(stderr, "Remove User ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s\n", fts_symbol_name(var->name));
#endif

  /* remove the user from the binding (ignore the case where the binding is not there any more) */
  for (u = &(var->users); *u; u = &((*u)->next))
    if ((*u)->obj == object)
	{
	  fts_mess_obj_list_t   *p;

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

/* add all the users of a binding to the given object set (find friends) */
static void fts_binding_add_users_to_set(fts_binding_t *var, fts_objectset_t *set)
{
  fts_mess_obj_list_t  *u;	

  for (u = var->users; u; u = u->next)
    {
      fts_objectset_add(set, u->obj);
    }
}

/* check if a binding is defined by a given object */
static int fts_binding_defined_by(fts_binding_t *var, fts_object_t *object)
{
  fts_mess_obj_list_t  *d;	

  for (d = var->definitions; d; d = d->next)
    {
      if (object == d->obj)
	return 1;
    }
  
  return 0;
}

/* check if a binding is (only!) defined by a given object (binding is not a doubly defined variable) */
static int fts_binding_defined_only_by(fts_binding_t *var, fts_object_t *object)
{
  if (var->definitions && (! var->definitions->next))
    return var->definitions->obj == object;
  else
    return 0;
}

/* add a definition to a binding */
static void fts_binding_add_definition(fts_binding_t *var, fts_object_t *object)
{
  fts_object_t *owner = 0;
  fts_mess_obj_list_t *d;

  /* First, check if the owner is already in the definition  list,
     in this case just return  */

  for (d = var->definitions; d; d = d->next)
    {
      if (object == d->obj)
	return;
    }

  /* if there is only one definition, recompute it after having added this one */

  if (var->definitions && (! var->definitions->next))
    owner = var->definitions->obj;

  d = (fts_mess_obj_list_t *) fts_heap_alloc(objlist_heap);

  d->next = var->definitions;
  d->obj = object;
  var->definitions = d;
  
  if (owner && (! var->suspended))
    fts_object_recompute(owner);
}

/* remove a definition from a binding */
static void fts_binding_remove_definition(fts_binding_t *var, fts_object_t *object)
{
  fts_mess_obj_list_t  **d;	/* indirect precursor */

  /* remove the definition from the binding */
  for (d = &(var->definitions); *d; d = &((*d)->next))
    if ((*d)->obj == object)
	{
	  fts_mess_obj_list_t   *p;

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
	  /* if there is only one definition left, recompute it */
	  fts_object_recompute(var->definitions->obj);
	}
      else if(! var->definitions)
	{
	  /* remove binding when last definition is deleted */
	  fts_binding_delete(var);
	}
    }
}

/* add all the definitions of a binding to the given object set (find friends) */
static void fts_binding_add_definitions_to_set(fts_binding_t *var, fts_objectset_t *set)
{
  fts_mess_obj_list_t  *u;	

  for (u = var->definitions; u; u = u->next)
    {
      fts_objectset_add(set, u->obj);
    }
}

/****************************************************************************
 *
 *  environent manipulation
 *
 */

/* initialize an environment (i.e. the list of bindings of a patcher) */
void fts_env_init(fts_env_t *env, fts_object_t *patcher)
{
  env->first = 0;
  env->patcher = patcher;
}

/* add a binding to an environment */
fts_binding_t *
fts_env_add_binding(fts_env_t *env, fts_symbol_t name, fts_atom_t *value)
{
  fts_binding_t *var;

  var = fts_binding_new(env, name, value);
  
  var->next = env->first;
  env->first = var;

  return var;
}

/* remove a binding from an environment */
static void fts_env_remove_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t **p; /* indirect precursor */

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

/* remove all the bindings of an environment that are suspended and 
 * that have the owner argument as unique definitions (used for patchers) 
 */
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
	var->env = 0; /* to prevent the binding from removing itself */
	fts_binding_delete(var);
      }
    else
      p = &((*p)->next);
}

/* suspend all the bindings in an environment that have the owner argument as 
 * unique definitions (used for patchers) 
 */
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

/* look in the environment for a binding for a given variable name */
static fts_binding_t *fts_env_get_binding(fts_env_t *env, fts_symbol_t name)
{
  fts_binding_t *var = 0;

  for (var = env->first; var; var = var->next)
    if (var->name == name)
      return var;

  return 0;
}


/****************************************************************************
 *
 *  variables
 *
 *  scope handling and high level public API
 *
 *  define:
 *    Defining a variable causes automatic stealing of any object within the scope of 
 *    the variable that referred an outer variable with the same name.
 *    Tthe actual redefinition will be made at "restore" time.
 *
 *  remove:
 *    The removing is generic enough to support any possible style of future declarations.
 *    Cause a redefinition of all the objects that use the same variable.
 *    It can be called also on suspended variables. In this case, it recursively resolve
 *    all the suspended bindings dependent on this one.
 *    To resolve means in this case to redefine the object, and either redefine or undefine 
 *    the object binding.
 *
 *  suspend:
 *    To suspend means to keep the dependency structure and value, but don't consider
 *    the binding valid. All the binding to objects dependent on this variable are
 *    recursively suspended.
 *    Note that a variable should be suspended only temporarly.
 *    Variables are never suspended between two user level atomic operations!
 *
 *  restore:
 *    All the recursively dependent objects are redefined, and their bindings recursively redefined.
 *    A variable may be restored by a different owner in case of object redefinition.
 *
 */

/* check if variable name indicates a global variable */
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

/* get a binding in a given scope for a given variable name */
static fts_binding_t *
fts_variable_get_binding(fts_patcher_t *scope, fts_symbol_t name)
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

/* define a suspended variable in the inner scope corresponding to the passed object */
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
      if(scope == fts_get_root_patcher() && fts_is_void(&v->value))
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
	  fts_mess_obj_list_t   **u;	/* indirect precursor */
	  
	  /* steal all the references to the varable (users) from parent scope and suspend their references */
	  u = &(up_v->users);
	  while (*u)
	    {
	      fts_object_t *user = (* u)->obj;
	      
	      if (fts_object_is_in_patcher((*u)->obj, scope))
		{		  
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
	}
    }
}

/* verify if a fts_variable_define can be issued in the passed scope
 * (note that this is not like testing if a variable is bound since the binding can be 
 * inherited from a surrounding patcher)
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

/* check whether the variable exists *and* it is suspended */
int fts_variable_is_suspended(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  return (v != 0) && fts_binding_is_suspended(v);
}

/* remove the variable bound to the name in the scope represented by the object */
void fts_variable_undefine(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *owner)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_remove_definition(var, owner);
}

/* like fts_variable_undefine, but act on all the variables defined by 'owner' in the current scope */
void fts_variables_undefine(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_env_remove_bindings(fts_patcher_get_env(scope), owner);
}

/* suspend the variable bound to the name in the scope represented by the object */
void fts_variable_suspend(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_suspend(var);
}

/* like fts_variable_suspend, but act on all the variables defined by 'owner' in the current scope */
void fts_variables_suspend(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_env_suspend_bindings(fts_patcher_get_env(scope), owner);
}

/* like fts_variable_suspend, but act on all the variables defined by 'owner' in the current scope and suspended. */
void fts_variables_undefine_suspended(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_env_remove_suspended_bindings(fts_patcher_get_env(scope), owner);
}

/* restore a suspended variable */
void fts_variable_restore(fts_patcher_t *scope, fts_symbol_t name, fts_atom_t *value, fts_object_t *owner)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  if (v)
    fts_binding_add_definition(v, owner);

  if (v && fts_binding_is_suspended(v))
    fts_binding_restore(v, value);
}

/* access the value of a variable in the given scope */
fts_atom_t *
fts_variable_get_value(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);
  v = fts_variable_get_binding(scope, name);

  if (!v || fts_binding_is_suspended(v) || fts_is_void(&v->value))
    return 0;
  else
    return &v->value;
}

/* access the value of a variable in the given scope or create void place holder in root patcher */
fts_atom_t *
fts_variable_get_value_or_void(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  scope = fts_variable_get_scope(scope, name);
  v = fts_variable_get_binding(scope, name);

  if(!v)
    {
      fts_atom_t a;

      /* make the root void variable here */
      fts_set_void(&a);
      v = fts_env_add_binding(fts_patcher_get_env(fts_get_root_patcher()), name, &a);
    }  

  if (fts_binding_is_suspended(v))
    return 0;
  else
    return &v->value;
}

/* add a user to a variable in a given scope.
 * (user is an object that referentiate the variable and so need to be redefined when the variable change value)
 */
void 
fts_variable_add_user(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *user)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  fts_binding_add_user(var, user);
}

void
fts_variable_remove_user(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *user)
{
  fts_binding_t *var;

  scope = fts_variable_get_scope(scope, name);

  var = fts_variable_get_binding(scope, name);

  fts_binding_remove_user(var, user);
}

/* find all the users and definitions of a variable visible in the given scope */
void fts_variable_find_users(fts_patcher_t *scope, fts_symbol_t name, fts_objectset_t *set)
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

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_variable_init(void)
{
  bindings_heap = fts_heap_new(sizeof(fts_binding_t));
  objlist_heap  = fts_heap_new(sizeof(fts_mess_obj_list_t));
  var_refs_heap = fts_heap_new(sizeof(fts_binding_list_t));
}
