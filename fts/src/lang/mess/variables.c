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

/* #define TRACE_DEBUG  */

/* Errors */

static fts_status_description_t fts_redefinedVariable = {"Redefined variable"};

  
/****************************************************************************/
/*                                                                          */
/*  Binding direct Manipulation                                             */
/*                                                                          */
/****************************************************************************/

static fts_heap_t *bindings_heap;
static fts_heap_t *objlist_heap;
static fts_heap_t *var_refs_heap;

static fts_binding_t *fts_binding_new(fts_env_t *env, fts_symbol_t name, fts_atom_t *value, fts_object_t *owner)
{
  fts_binding_t *v;

  v = (fts_binding_t *) fts_heap_alloc(bindings_heap);

  v->name = name;
  v->env  = env;
  v->owner = owner;
  v->next  = 0;
  v->users = 0;
  v->suspended = 0;
  v->value = *value;
  v->wannabes = 0;

#ifdef TRACE_DEBUG
  fprintf(stderr, "New Binding in ");
  fprintf_object(stderr, env->patcher);
  fprintf(stderr, " for variable %s, value ", fts_symbol_name(name));
  fprintf_atoms(stderr, 1, value);
  fprintf(stderr, " owner ");
  fprintf_object(stderr, owner);
  fprintf(stderr, "\n");
#endif

  return v;
}


static void fts_binding_delete(fts_binding_t *var)
{
  fts_binding_t **b;		/* indirect precursor */

#ifdef TRACE_DEBUG
  if (var->env)
    {
      fprintf(stderr, "Remove Binding in ");
      fprintf_object(stderr, var->env->patcher);
    }
  else
    {
      fprintf(stderr, "Remove Binding for variable %s\n", fts_symbol_name(var->name));
      fprintf(stderr, "Recomputing :\n");
    }
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


static void fts_binding_suspend(fts_binding_t *var)
{
  fts_object_list_t   *u;

#ifdef TRACE_DEBUG
  if (var->env)
    {
      fprintf(stderr, "Suspend Binding in ");
      fprintf_object(stderr, var->env->patcher);
      fprintf(stderr, " for variable %s\n", fts_symbol_name(var->name));
    }
  else
    {
      fprintf(stderr, "Suspend Binding for variable %s\n", fts_symbol_name(var->name));
    }

  fprintf(stderr, "Suspending variable users:\n");
#endif

  var->suspended = 1;

  /* Recursive calls to suspend all the variables bind to users of this
     binding */

  for (u = var->users; u; u = u->next)
    if (! fts_object_is_error(u->obj))
      {
#ifdef TRACE_DEBUG
	fprintf(stderr, "\t");
	fprintf_object(stderr, u->obj);
	fprintf(stderr, "\n");
#endif

	fts_variable_suspend(u->obj->patcher, fts_object_get_variable(u->obj));
      }

#ifdef TRACE_DEBUG
  fprintf(stderr, "Done.\n");
#endif
}

/*
 * NOTE: If an variable had an error or void value, and is restored to an error or
 * void value, it do no propagation; this to stop loop propagation.
 */

static void fts_binding_restore(fts_binding_t *var, fts_atom_t *value, fts_object_t *owner)
{
  int do_propagation;

  if (fts_is_object(&(var->value)) && fts_object_is_error(fts_get_object(&(var->value))) &&
      fts_is_object(value) && fts_object_is_error(fts_get_object(value)))
    do_propagation = 0;
  else
    do_propagation = 1;

  var->suspended = 0;
  var->owner = owner;
  var->value = *value;

#ifdef TRACE_DEBUG
  fprintf(stderr, "Restoring Binding in ");
  fprintf_object(stderr, var->env->patcher);
  fprintf(stderr, " for variable %s, value ", fts_symbol_name(var->name));
  fprintf_atoms(stderr, 1, value);
  fprintf(stderr, " owner ");
  fprintf_object(stderr, owner);
  fprintf(stderr, "\n");
#endif

  /* Recursive calls to recover all the variables bind to users of this
     binding, and redefine the objects */

  if (do_propagation)
    {
      fts_object_list_t *u;

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
}


static int fts_binding_is_suspended(fts_binding_t *var)
{
  return var->suspended;
}


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
  fprintf(stderr, " for variable %s in ", fts_symbol_name(var->name));
  fprintf_object(stderr, var->env->patcher);
  fprintf(stderr, "\n");
#endif

}


void fts_binding_remove_user(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t   **u;	/* indirect precursor */
  fts_binding_list_t  **b;	/* indirect precursor */

#ifdef TRACE_DEBUG
  fprintf(stderr, "Remove User ");
  fprintf_object(stderr, object);

  if (var->env)
    {
      fprintf(stderr, " for variable %s in ", fts_symbol_name(var->name));
      fprintf_object(stderr, var->env->patcher);
      fprintf(stderr, "\n");
    }
  else
    {
      fprintf(stderr, " for variable %s\n", fts_symbol_name(var->name));
    }
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

  /* Then, if no user are left, and if there are no owner (i.e. undefined variable)
     delete the binding */

  if ((var->users == 0) && (var->owner == 0))
    fts_binding_delete(var);
}


static void fts_binding_add_users_to_set(fts_binding_t *var, fts_object_set_t *set)
{
  fts_object_list_t  *u;	

  fprintf(stderr, "Adding users\n");
  for (u = var->users; u; u = u->next)
    {
      fprintf(stderr, "\nAdding user %lx (%d)\n", u->obj, u->obj->id);
      fts_object_set_add(set, u->obj);
      
    }
  fprintf(stderr, "Done Adding users\n");
}

static void fts_binding_add_wannabe(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t *w;

  w = (fts_object_list_t *) fts_heap_alloc(objlist_heap);

  w->next = var->wannabes;
  w->obj = object;
  var->wannabes = w;
  
#ifdef TRACE_DEBUG
  fprintf(stderr, "New Wannabe ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s in ", fts_symbol_name(var->name));
  fprintf_object(stderr, var->env->patcher);
  fprintf(stderr, "\n");
#endif
}


void fts_binding_remove_wannabe(fts_binding_t *var, fts_object_t *object)
{
  fts_object_list_t   **w;	/* indirect precursor */


#ifdef TRACE_DEBUG
  fprintf(stderr, "Remove Wannabe ");
  fprintf_object(stderr, object);
  fprintf(stderr, " for variable %s in ", fts_symbol_name(var->name));
  fprintf_object(stderr, var->env->patcher);
  fprintf(stderr, "\n");
#endif

  /* Remove the wannabe from the binding */

  for (w = &(var->wannabes); *w; w = &((*w)->next))
    if ((*w)->obj == object)
	{
	  fts_object_list_t   *p;

	  p = *w;
	  *w = (*w)->next;

	  fts_heap_free((char *)p, objlist_heap);
	  break;
	}
}


/****************************************************************************/
/*                                                                          */
/*  Environent Manipulation                                                 */
/*                                                                          */
/****************************************************************************/

void fts_env_init(fts_env_t *env, fts_object_t *patcher)
{
  env->first = 0;
  env->patcher = patcher;
}


static fts_binding_t *fts_env_add_binding(fts_env_t *env, fts_symbol_t name, fts_atom_t *value, fts_object_t *owner)
{
  fts_binding_t *var;

  var = fts_binding_new(env, name, value, owner);
  
  var->next = env->first;
  env->first = var;

  return var;
}


static void fts_env_remove_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t **p;		/* indirect precursor */

  p = &(env->first);

  /* Look and remove the binding */

  while (*p)
    if ((*p)->owner == owner)
      {
	fts_binding_t *var = *p;

	*p = (*p)->next;
	var->env = 0;		/* to prevent the binding from removing itself */
	fts_binding_delete(var);
      }
    else
      p = &((*p)->next);
}


static void fts_env_remove_suspended_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t **p;		/* indirect precursor */

  p = &(env->first);

  /* Look and remove the binding */

  while (*p)
    if (((*p)->owner == owner) && (*p)->suspended)
      {
	fts_binding_t *var = *p;

	*p = (*p)->next;
	var->env = 0;		/* to prevent the binding from removing itself */
	fts_binding_delete(var);
      }
    else
      p = &((*p)->next);
}


static void fts_env_suspend_bindings(fts_env_t *env, fts_object_t *owner)
{
  fts_binding_t *var = 0;

  var = env->first;

  while (var)
    {
      if (var->owner == owner)
	fts_binding_suspend(var);

      var = var->next;
    }
}


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


static fts_binding_t *fts_variable_get_binding(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_patcher_t *patcher;

  patcher = scope;

  while (patcher)
    {
      fts_binding_t *v;

      v = fts_env_get_binding(fts_patcher_get_env(patcher), name);

      if (v)
	return v;
      
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


void fts_variable_define(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *owner)
{
  fts_atom_t value;
  fts_binding_t *v, *up_v;

  fts_set_void(&value);


  up_v = fts_variable_get_binding(scope, name);

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  if (v)
    {
      /* suspended variable case; variable already defined do nothing
       * for now.
       * Note that redefinitions should be check before calling this
       * function using fts_variable_can_define.
       */

      return;
    }
  else
    {
      v = fts_env_add_binding(fts_patcher_get_env(scope), name, &value, owner);
      fts_binding_suspend(v);
    }


  /* Steal and suspends all the references, in the local scope, to 
     variables with the same name */

  if (up_v)
    {
      fts_object_list_t   **u;	/* indirect precursor */

#ifdef TRACE_DEBUG
      fprintf(stderr, "Found upper variable for %s\n Stealing: \n", fts_symbol_name(up_v->name));
#endif      

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

	      fts_binding_remove_user(up_v, user);
	      fts_binding_add_user(v, user);
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

/* Verify if a fts_variable_define can be issued in the passed scope.
   Note that this is not like testing if a variable is bound; the binding can be 
   inherited from a surrounding patcher.
 */

int fts_variable_can_define(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  return (v == 0);
}

/* Return 1 if the variable exists *and* it is suspended */

int fts_variable_is_suspended(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

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


void fts_variable_undefine(fts_patcher_t *scope, fts_symbol_t name)
{
  /* if a wanna be exist, just suspend the binding,
     don't delete it, and recover it with the wannabe as value
     by recomputing the wannabe.
     No need to scope manipulation, we are guaranteed that the 
     wannabe is in the same patcher of the original object, so the user
     list is consistent.
   */

  fts_binding_t *var;

  var = fts_variable_get_binding(scope, name);

  if (var)
    {
      if (var->wannabes)
	{
	  fts_object_list_t *w;
	  fts_object_t *wannabe;

	  w = var->wannabes;
	  var->wannabes = w->next;
	  wannabe = w->obj;
	  fts_heap_free((char *)w, objlist_heap);
	  
	  fts_binding_suspend(var);

	  fts_object_recompute(wannabe);
	}
      else
	fts_binding_delete(var);
    }
}

/*
  Like fts_variable_undefine, but act on all the variables in the current scope defined
  by 'owner'
  */

void fts_variables_undefine(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_binding_t *v;

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

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_suspend(var);
}


/*
  Like fts_variable_suspend, but act on all the variables in the current scope defined
  by 'owner'
  */


void fts_variables_suspend(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_binding_t *v;

  fts_env_suspend_bindings(fts_patcher_get_env(scope), owner);
}

void fts_variables_undefine_suspended(fts_patcher_t *scope, fts_object_t *owner)
{
  fts_binding_t *v;

  fts_env_remove_suspended_bindings(fts_patcher_get_env(scope), owner);
}


/*
 * Restore a suspended variable.
 *
 * All the recursively dependent objects are redefined, and their
 * bindings recursively redefined.
 * A variable may be restored by a different owner in case of object ridefinition.
 *
 */

void fts_variable_restore(fts_patcher_t *scope, fts_symbol_t name, fts_atom_t *value, fts_object_t *owner)
{
  fts_patcher_t *patcher;
  fts_binding_t *v;

  v = fts_env_get_binding(fts_patcher_get_env(scope), name);

  if (v && fts_binding_is_suspended(v))
    fts_binding_restore(v, value, owner);
}


/* Access the value of a variable in the scope represented by an object */

fts_atom_t *fts_variable_get_value(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_binding_t *v;

  v = fts_variable_get_binding(scope, name);

  if (! v)
    {
      /* make the root void variable here */

      fts_atom_t a;

      fts_set_void(&a);
      v = fts_env_add_binding(fts_patcher_get_env(fts_get_root_patcher()), name, &a, 0);
    }

  if (fts_binding_is_suspended(v))
    return 0;
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

  var = fts_variable_get_binding(scope, name);

  fts_binding_add_user(var, user);
}



/*
  A variable wannabe is an object that want to redefine that variable.
  If the original definer of the variable is delete, one wannabe
  should be selected to become the new variable definer.
  */

void fts_variable_add_wannabe(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *wannabe)
{
  fts_binding_t *var;

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_add_wannabe(var, wannabe);
}


void fts_variable_remove_wannabe(fts_patcher_t *scope, fts_symbol_t name, fts_object_t *wannabe)
{
  fts_binding_t *var;

  var = fts_variable_get_binding(scope, name);

  if (var)
    fts_binding_remove_wannabe(var, wannabe);
}

/* Support for find  */

void fts_variable_find_users(fts_patcher_t *scope, fts_symbol_t name, fts_object_set_t *set)
{
  fts_binding_t *b;

  b = fts_variable_get_binding(scope, name);

  if (b)
    fts_binding_add_users_to_set(b, set);
}

void fts_variable_assign(fts_patcher_t *scope, fts_symbol_t name, fts_atom_t *value)
{
  if (! fts_variable_is_suspended(scope, name))
    fts_variable_define(scope, name, (fts_object_t *)scope);

  fts_variable_restore(scope, name, value, (fts_object_t *)scope);
}

/* Module init function */

void fts_variables_init(void)
{
  bindings_heap = fts_heap_new(sizeof(fts_binding_t));
  objlist_heap  = fts_heap_new(sizeof(fts_object_list_t));
  var_refs_heap = fts_heap_new(sizeof(fts_binding_list_t));
}















