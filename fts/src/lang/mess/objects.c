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


/* Define TRACE_DEBUG to get some debug printing during object creation.  */

/* #define TRACE_DEBUG */

#include <stdarg.h>

#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/runtime.h>
#include "messP.h"

/* stuff from runtime/client */
extern void fts_client_release_object(fts_object_t *c);
extern void fts_client_release_object_data(fts_object_t *c);
extern void fts_client_upload_object(fts_object_t *obj);
extern void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property);
extern void fts_object_reset_changed(fts_object_t *obj);

/* forward declarations  */
static void fts_object_assign(fts_symbol_t name, fts_atom_t *value, void *data);
static void fts_object_move_properties(fts_object_t *old, fts_object_t *new);
static void fts_object_send_kernel_properties(fts_object_t *obj);
static fts_symbol_t fts_object_description_get_variable_name(int ac, const fts_atom_t *at);

static void fts_object_unbind(fts_object_t *obj);
static void fts_object_free(fts_object_t *obj);

void fts_objects_init()
{
}

/******************************************************************************
 *
 *  create object
 *
 */

static fts_status_description_t fts_CannotInstantiate = {"Cannot instantiate class"};

fts_object_t *
fts_object_create(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = (fts_object_t *)fts_block_zalloc(cl->size);

  obj->head.cl = cl;
  obj->head.id = FTS_NO_ID;
  obj->properties = 0;
  obj->varname = 0;
  obj->refcnt = 0;
  
  if (cl->noutlets)
    obj->out_conn = (fts_connection_t **) fts_block_zalloc(cl->noutlets * sizeof(fts_connection_t *));
  
  if (cl->ninlets)
    obj->in_conn = (fts_connection_t **) fts_block_zalloc(cl->ninlets * sizeof(fts_connection_t *));
  
  /* &@#!@#$%*@#$ very nice hack to survive until jMax 3 (Merci Francois!) */
  fts_message_send(obj, fts_SystemInlet, fts_s_init, ac + 1, at - 1);

  return obj;
}

fts_status_t 
fts_object_new_to_patcher(fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_object_t **ret)
{
  fts_status_t status;
  fts_class_t *cl;
  fts_object_t *obj;
  int i;

  if (fts_get_symbol(at) == fts_s_patcher)
    {
      /* Patcher behave diffreentrly w.r.t. than other objects;
	 the metaclass do not depend on the arguments, but on the inlets/outlets.
	 New patchers are created with zero in zero outs, and changed later 
      */
      fts_atom_t a[3];

      fts_set_symbol(&a[0], fts_s_patcher);
      fts_set_int(&a[1], 0);
      fts_set_int(&a[2], 0);

      cl = fts_class_instantiate(3, a);
    }
  else
    cl = fts_class_instantiate(ac, at);

  if (! cl)
    {
      *ret = 0;
      return &fts_CannotInstantiate;
    }

  obj = (fts_object_t *)fts_block_zalloc(cl->size);

  obj->patcher = patcher;
  obj->head.cl = cl;
  obj->head.id = FTS_NO_ID;
  obj->properties = 0;
  obj->varname = 0;
  obj->refcnt = 0;

  if (cl->noutlets)
    obj->out_conn = (fts_connection_t **) fts_block_zalloc(cl->noutlets * sizeof(fts_connection_t *));

  if (cl->ninlets)
    obj->in_conn = (fts_connection_t **) fts_block_zalloc(cl->ninlets * sizeof(fts_connection_t *));
    
  /* send the init message */
  {
    /* force type checking during new */
    long save_check_status;

    save_check_status = fts_mess_get_run_time_check();
    fts_mess_set_run_time_check(1);

    status = fts_message_send(obj, fts_SystemInlet, fts_s_init, ac, at);
    fts_mess_set_run_time_check(save_check_status);
  }

  if(status != &fts_MethodNotFound)
    {
      if (status != fts_Success)
	{
	  /* free already allocated */
	  fts_object_free(obj);

	  /* return NULL */
	  *ret = 0;
	  
	  return status;
	}
      else
	{
	  fts_atom_t error_prop;

	  fts_object_get_prop(obj, fts_s_error, &error_prop);

	  if(!fts_is_void(&error_prop))
	    {
	      fts_atom_t error_description_prop;

	      fts_object_get_prop(obj, fts_s_error_description, &error_description_prop);
	      
	      /* free already allocated */
	      fts_object_free(obj);
	      
	      /* return NULL */
	      *ret = 0;
	  
	      if(fts_is_symbol(&error_description_prop))
		{
		  fts_symbol_t error_description = fts_get_symbol(&error_description_prop);
	      
		  return fts_new_status(fts_symbol_name(error_description));
		}
	      else
		return &fts_CannotInstantiate;
	    }
	}
    }

  fts_patcher_add_object(patcher, obj);
  fts_object_refer(obj);

  *ret = obj;
  return fts_Success;
}

/* create an object in a patcher, appling all the expression/object semantic */
fts_object_t *
fts_eval_object_description(fts_patcher_t *patcher, int aoc, const fts_atom_t *aot)
{
  fts_object_t  *obj = 0;
  fts_metaclass_t *mcl;
  fts_symbol_t  var;
  fts_expression_state_t *e = 0;
  fts_atom_t state;
  int ac = 0;
  const fts_atom_t *at = 0;
  fts_atom_t new_args[1024];

#ifdef TRACE_DEBUG
  fprintf(stderr, "Object new ");
  fprintf_atoms(stderr, aoc, aot);
  fprintf(stderr, "\n");
#endif

  /* first of all, we check if we are in the case of a object variable definition syntax */
  if (fts_object_description_defines_variable(aoc, aot))
    {
      /* extract the variable name */
      var = fts_object_description_get_variable_name(aoc, aot);

      /* if the variable already exists in this local context, make a double definition error object  */
      if(!fts_variable_can_define(patcher, var))
	{
	  /* error: redefined variable */
	  obj = fts_error_object_new(patcher, aoc, aot, "Variable %s doubly defined", fts_symbol_name(var));
	}
      else
	{
	  /* otherwise, set the ac/at pair to skip the variable */
	  ac = aoc - 2;
	  at = aot + 2;
	}
    }
  else
    {
      var = 0;
      ac = aoc;
      at = aot;
    }

  /* prepare the variable, if defined */
  if ((var != 0) && ( ! fts_variable_is_suspended(patcher, var)))
    {
      /* Define the variable, suspended;
	 this will also steal all the objects referring to the same variable name
	 in the local scope from any variable defined outside the scope */

      fts_variable_define(patcher, var);
    }

  /* Then, check for zero arguments, and produce an error object in this case */
  if ((! obj) && (aoc == 0))
    {
      /* error: zero arguments */
      obj = fts_error_object_new(patcher, aoc, aot, "Zero arguments in object");
    }

  /* 
   * The creation algorithm will try all the following techniques in
   * the given order until an object is created:
   *
   * 1 - Get the first argument; if it is a symbol, and there is an
   *     object doctor for this name, delegate the object creation to
   *     the doctor, without further evaluation; if the doctor return
   *     null, return an error object.
   *
   * 2 - Expression-evaluate the whole description; if the evaluation
   *     give an error, return an error object.
   *     If the first value is not a symbol, return an error object.
   *     Otherwise consider the result of the evaluation as the description
   *     of the object for the following.
   *
   * 3 - if an object doctor exists for the object name, delegate to it the 
   *     object creation without further evaluation; if the doctor
   *     return null, return an error object.
   * 
   * 4 - if an explicitly declared template exists, instantiate
   *     that template.
   *
   * 5 - if an explicitly declared abstraction exists, instantiate
   *     that abstraction.
   * 
   * 6 - if an FTS class exists, instantiate that class.
   * 
   * 7 - if an template exists in the template path, instantiate
   *     that template.
   *
   * 8 - if an abstraction exists in the abstraction path, instantiate
   *     that abstraction.
   *
   *   
   * 9 - Make an error object
   *
   */

  /* 1-  try the doctor */
  if ((! obj) && fts_is_symbol(&at[0]) && fts_object_doctor_exists(fts_get_symbol(&at[0])))
    {
      /* If the doctor return null, it means the doctor don't want
	 to do this particular object, so we just continue */
      obj = fts_call_object_doctor(patcher, ac, at);
    }

  /* 2- Expression evaluate */
  if (!obj)
    {
      /* Compute the expressions with the correct offset */
      e = fts_expression_eval(patcher, ac, at,  1024, new_args);
	  
      if (fts_expression_get_status(e) != FTS_EXPRESSION_OK)
	{
	  /* Error in expression */
	  obj = fts_error_object_new(patcher, aoc, aot, fts_expression_get_msg(e), fts_expression_get_err_arg(e));
	}
      else if (! fts_is_symbol(&new_args[0]))
	{
	  /* Missing class name, or class name is not a symbol */
	  obj = fts_error_object_new(patcher, aoc, aot, "The first argument should be a class name, but is not a symbol");
	}
      else
	{
	  at = new_args;
	  ac = fts_expression_get_result_count(e);
	}
    }

#ifdef TRACE_DEBUG
  fprintf(stderr, "After expression eval ");
  fprintf_atoms(stderr, ac, at);
  fprintf(stderr, "\n");
#endif

  /* 3- Retry the object doctor */
  if ((! obj) && fts_is_symbol(&at[0]) && fts_object_doctor_exists(fts_get_symbol(&at[0])))
    {
      /* If the doctor return null, it means the doctor don't want
	 to do this particular object, so we just continue */
      obj = fts_call_object_doctor(patcher, ac, at);
    }

  /* 4- explicitly declared template  */
  if (! obj)
    {
      /* First of all, try an explicitly declared abstraction */
      obj =  fts_template_new_declared(patcher, ac, at, e);
    }


  /* 5- explicitly declared abstraction  */
  if (! obj)
    {
      /* First of all, try an explicitly declared abstraction */
      obj =  fts_abstraction_new_declared(patcher, ac, at);
    }


  /* 6- Make the object if the FTS class exists */
  if ((! obj) && fts_metaclass_get_by_name(fts_get_symbol(&at[0])))
    {
      /* We have a metaclass: this prevent looking for 
	 further abstractions */
      fts_status_t ret;

      ret = fts_object_new_to_patcher(patcher, ac, at, &obj);

      if (ret != fts_Success)
	{
	  /* Standard FTS instantiation error  */
	  if (ret == &fts_CannotInstantiate)
	    obj = fts_error_object_new(patcher, aoc, aot, "Error in class instantiation");
	  else if (ret == &fts_ArgumentMissing)
	    obj = fts_error_object_new(patcher, aoc, aot, "Missing argument in object");
	  else if (ret == &fts_ExtraArguments)
	    obj = fts_error_object_new(patcher, aoc, aot, "Extra argument in object");
	  else if (ret == &fts_ArgumentTypeMismatch)
	    obj = fts_error_object_new(patcher, aoc, aot, "Argument types mismatch");
	  else
	    obj = fts_error_object_new(patcher, aoc, aot, fts_status_get_description(ret));
	}
    }

  /* 7- Try a path template  */
  if (! obj)
    obj = fts_template_new_search(patcher, ac, at, e);

  /* 8 - Try a path abstraction */
  if (! obj)
    obj = fts_abstraction_new_search(patcher, ac, at);


  /* 9- Make an error object */
  if (!obj)
    {
      /* Object not found */
      obj = fts_error_object_new(patcher, aoc, aot, "Object or template %s not found", fts_symbol_name(fts_get_symbol(at)));
    }

  /* Check if we are defining a variable *and* we have a state;
   * If yes, just store the state in the variable state for later use.
   * if not, destroy the current object, and substitute it with an error object
   * and store an error value in the variable state.
   */

  if ((! fts_object_is_error(obj)) && (var != 0))
    {
      fts_object_get_prop(obj, fts_s_state, &state);
      
      if (fts_is_void(&state))
	{
	  /* ERROR: the object cannot define a variable, it does not have a "state" property */
	  fts_object_unbind(obj);
	  fts_object_destroy(obj);
	  obj = fts_error_object_new(patcher, aoc, aot, "Object can't define a variable");
	}
    }

  if (fts_object_is_error(obj))
    fts_set_error(&state);

  /* Object created (at worst, a error object) do the last operations, like setting 
     the object description, variables and properties;
     We check if the argv exists already; a doctor may have
     changed the object definition, for persistent fixes !!
  */
  if ((fts_object_is_patcher(obj) && (! fts_patcher_is_standard((fts_patcher_t *)obj))) || (! obj->argv))
    fts_object_set_description(obj, aoc, aot);

  /* Assign the variable references to the object; do it also if
     it is an error object, because we may try to recompute, and recover,
     the object, if one of this variables have been redefined. */
  if (e)
    fts_expression_add_variables_user(e, obj);

  /* 
     If it is not an error, and not a template, assign the local variables/properties.
     Note that in case of the template, this operation has been done during the load vm code
     execution; little weird, but needed to avoid object recomputing during loading.
  */
  if (e && (! fts_object_is_error(obj)) && (! fts_object_is_template(obj)))
    {
      if (fts_object_is_patcher(obj))
	fts_expression_map_to_assignements(e, fts_patcher_assign_variable, (void *) obj);
      else
	fts_expression_map_to_assignements(e, fts_object_assign, (void *) obj);
    }

  /* Free the expression state structure if any */
  if (e)
    fts_expression_state_free(e);

  /* then, assign it to the variable if any */
  if (var != 0)
    {
      fts_variable_restore(patcher, var, &state, obj);
      obj->varname = var;
    }

  /* Finally, assign the error property;
     Always present, and always explicit for the moment,
     until we don't have global daemons again.
  */
  {
    fts_atom_t a;

    if (fts_object_is_error(obj))
      fts_set_int(&a, 1);
    else
      fts_set_int(&a, 0);

    fts_object_put_prop(obj, fts_s_error, &a);
  }

  return obj;
}


/****************************************************************
 *
 *  delete object
 *
 *    fts_object_unbind()
 *    fts_object_unconnect()
 *    fts_object_unclient()
 *    fts_object_free()
 *
 *    fts_object_destroy()
 *    fts_object_release() ... macro!
 *    fts_object_delete_from_patcher()
 *
 */

static void 
fts_object_unbind(fts_object_t *obj)
{
  int outlet, inlet;

  /* Unbind it from its variable if any */
  if (fts_object_get_variable(obj))
    fts_variable_undefine(obj->patcher, fts_object_get_variable(obj), obj);

  /* Remove it as user of its var refs */
  while (obj->var_refs)
    fts_binding_remove_user(obj->var_refs->var, obj);
}

/* remove all connections from the object (done when unplugged from the patcher) */
static void 
fts_object_unconnect(fts_object_t *obj)
{
  int outlet, inlet;

  /* delete all the survived connections starting in the object */
  for (outlet=0; outlet<obj->head.cl->noutlets; outlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons and methods can fire correctly */
      while ((p = obj->out_conn[outlet]))
	fts_connection_delete(p);
    }

  /* Delete all the survived connections ending in the object */
  for (inlet=0; inlet<obj->head.cl->ninlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons and methods  can fire correctly */
      while ((p = obj->in_conn[inlet]))
	fts_connection_delete(p);
    }
}

static void 
fts_object_unclient(fts_object_t *obj)
{
  /* tell the client to release the Java part */
  if (obj->head.id != FTS_NO_ID)
    fts_client_release_object(obj);

  /* remove the object from the object table */
  if (obj->head.id != FTS_NO_ID)
    fts_object_table_remove(obj->head.id);
}

/* delete the unbound, unconnected object already removed from the patcher */
static void 
fts_object_free(fts_object_t *obj)
{
  /* free the object properties */
  fts_properties_free(obj);

  /* free the object description */
  if (obj->argv)
    fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

  /* free the inlets and outlets */
  if (obj->out_conn)
    fts_block_free((char *)obj->out_conn, obj->head.cl->noutlets * sizeof(fts_connection_t *));
  if (obj->in_conn)
    fts_block_free((char *)obj->in_conn, obj->head.cl->ninlets * sizeof(fts_connection_t *));

  /* free the object */
  fts_block_free((char *)obj, obj->head.cl->size);
}

/* delete the unbound, unconnected object already removed from the patcher */
void 
fts_object_destroy(fts_object_t *obj)
{
  /* send delete message */
  fts_send_message(obj, fts_SystemInlet, fts_s_delete, 0, 0);

  /* take the object away from the update queue (if there) and free it */
  fts_object_reset_changed(obj);

  /* release all client components */
  fts_object_unclient(obj);

  /* free memory */
  fts_object_free(obj);
}

void 
fts_object_delete_from_patcher(fts_object_t *obj)
{
  /* unbind the objects from defined and used variables */
  fts_object_unbind(obj);
  
  /* remove connections */
  fts_object_unconnect(obj);

  /* unreference by hand */
  obj->refcnt--;

  /* send delete message */
  if(obj->refcnt == 0)
    fts_send_message(obj, fts_SystemInlet, fts_s_delete, 0, 0);

  /* remove from patcher */
  if(obj->patcher)
    fts_patcher_remove_object(obj->patcher, obj);

  /* take the object away from the update queue (if there) and free it */
  fts_object_reset_changed(obj);

  /* release all client components (no patcher, no appearance) */
  fts_object_unclient(obj);
  
  /* destroy or set no patcher */
  if(obj->refcnt == 0)
    fts_object_free(obj);
  else
    obj->patcher = 0;
}

/* return true if the object is being deleted (i.e. if the patcher or an ancestor of the patcher is being deleted) */
int 
fts_object_being_deleted(fts_object_t *obj)
{
  fts_patcher_t *p;

  p = fts_object_get_patcher(obj);

  while (p)
    {
      if (fts_patcher_being_deleted(p))
	return 1;
      
      p = fts_object_get_patcher((fts_object_t *) p);
    }

  return 0;
}

/*********************************************************************************
 * 
 * redefine object
 *
 * fts_object_redefine replace an object with a new
 * one whose definition is passed as argument, leaving the same
 * connections, properties and id.
 *
 * For the moment *no* mechanism for transferring
 * the old state to the new object is defined.
 * Waiting for a better property system, persistent/clint properties are anyway 
 * transferred to the new object before deleting the old one.
 * 
 * If the object is a patcher (either standard patcher or abstraction/template)
 * the patcher redefine function is called instead, look in the patcher.c file.
 */


fts_object_t *
fts_object_recompute(fts_object_t *old)
{
  fts_object_t *obj;

  /* If the object being redefined is a standard patcher,
     redefine it using a patcher function, otherwise with 
     the object function.
  */
  if (fts_object_is_standard_patcher(old))
    obj = (fts_object_t *) fts_patcher_redefine((fts_patcher_t *) old, old->argc, old->argv);
  else
    {
      /* If we have an object with data, data must be released,
	 because the object will be deleted */
      if (old->head.id != FTS_NO_ID)
	fts_client_release_object_data(old);

      obj = fts_object_redefine(old, old->head.id, old->argc, old->argv);

      /* Error property handling; currently it is a little bit
	 of an hack beacause we need a explit "zero"  error
	 property on a non error redefined object; actually
	 the error property daemon should be a global daemon !
      */
      if (obj->head.id != FTS_NO_ID)
	fts_object_send_kernel_properties(obj);
    }

  return obj;
}


fts_object_t *
fts_object_redefine(fts_object_t *old, int new_id, int ac, const fts_atom_t *at)
{
  int do_client;
  fts_symbol_t  var;
  fts_object_t  *new;

  /* If the new and the old id are the same, or if old do not have an id, don't do any update on the client side */
  do_client = ((old->head.id != FTS_NO_ID) && (old->head.id != new_id));

  /* check for the "var : <obj> syntax" and  extract the variable name if any */
  if (fts_object_description_defines_variable(ac, at))
    var = fts_object_description_get_variable_name(ac, at);
  else
    var = 0;

  /* if the old object define a variable, and the new definition
     do not define  the same variable, delete the variable;
     if the new object define the same variable,  just suspend it,
     but unregister the old object as definition
  */
  if (old->varname && (old->varname == var))
    fts_variable_suspend(old->patcher, old->varname);

  fts_object_unbind(old);

  /*if((old->head.id == new_id) && fts_object_description_variable_name_changed_only(fts_object_t *old, int ac, const fts_atom_t *at))*/

  /* send delete message */
  fts_send_message(old, fts_SystemInlet, fts_s_delete, 0, 0);  

  /* if old id and new id are the same, do the replace without telling the client */
  if ((old->head.id != FTS_NO_ID) && (old->head.id == new_id))
    {
      fts_object_table_remove(old->head.id);
      old->head.id = FTS_NO_ID;
    }

  /* make the new object  */
  new = fts_eval_object_description(fts_object_get_patcher(old), ac, at);
  fts_object_set_id(new, new_id);
  
  /* Update the loading vm */
  fts_vm_substitute_object(old, new);

  /* If new is an error object, assure that there are enough inlets
     and outlets for the connections */
  if (fts_object_is_error(new))
    {
      fts_error_object_fit_inlet(new, old->head.cl->ninlets - 1);
      fts_error_object_fit_outlet(new, old->head.cl->noutlets - 1);
    }

  /* 1. move the properties
   * 2. upload the object (so the properties will be known to the client)
   * 3. move the connections (the object need to be uploaded in order to move the connections) 
   */
  fts_object_move_properties(old, new);

  if(do_client)
    fts_client_upload_object(new);

  /* move the connections from the old to the new object, tell the client if needed */
  fts_object_move_connections(old, new, do_client);

  if(do_client)
    fts_object_unclient(old);

  /* remove the object from the patcher */
  if(old->patcher)
    fts_patcher_remove_object(old->patcher, old);

  fts_object_reset_changed(old);
  fts_object_free(old);

  return new;
}

/*********************************************************************************
 * 
 * object description
 *
 */

/* variable as part of object description */
int
fts_object_description_defines_variable(int ac, const fts_atom_t *at)
{
  return (ac >= 3) && fts_is_symbol(&at[0]) && fts_is_symbol(&at[1]) && (fts_get_symbol(&at[1]) == fts_s_column);
}

static fts_symbol_t
fts_object_description_get_variable_name(int ac, const fts_atom_t *at)
{
  return fts_get_symbol(&at[0]);
}

/* This is to support "changing" objects; usefull during 
 * .pat loading, where not all the information is available 
 *    at the right place; used currently explode in the fts1.5 package.
 * WARNING: user object should never call this function, otherwise they risk
 * to loose the expressions definition; but it is very usefull for doctors.
 */

void 
fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv)
{
  int i;

  if (obj->argc == argc)
    {
      /* Just copy the values, the size is correct */
      for (i = 0; i < argc; i++)
	obj->argv[i] = argv[i];
    }
  else
    {
      /* Free the old object description, if any */
      if (obj->argv)
	fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

      /* reallocate the description if argc > -0 and copy the arguments */
      obj->argc = argc;

      if (argc > 0)
	{
	  obj->argv = (fts_atom_t *) fts_block_zalloc(argc * sizeof(fts_atom_t));

	  for (i = 0; i < argc; i++)
	    obj->argv[i] = argv[i];
	}
      else
	obj->argv = 0;
    }
}


/* This is to support "changing" objects; usefull during 
 * .pat loading, where not all the information is available 
 * at the right place; used currently for explode in the fts1.5 package,
 * and for patcher redefinition.
 * 
 */

void 
fts_object_set_description_and_class(fts_object_t *obj, fts_symbol_t class_name, int argc, const fts_atom_t *argv)
{
  int i;

  if (obj->argc == argc + 1)
    {
      /* Just copy the values, the size is correct */
      fts_set_symbol(&(obj->argv[0]), class_name);

      for (i = 0; i < argc; i++)
	obj->argv[i + 1] = argv[i];
    }
  else
    {
      /* Free the old object description, if any */

      if (obj->argv)
	fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

      /* reallocate the description if argc > -0 and copy the arguments */
      obj->argc = argc + 1;
      obj->argv = (fts_atom_t *) fts_block_zalloc((argc + 1) * sizeof(fts_atom_t));

      fts_set_symbol(&(obj->argv[0]), class_name);

      for (i = 0; i < argc; i++)
	obj->argv[i + 1] = argv[i];
    }
}


/* This function delete the object description; it is intended to be used
   in object doctor that convert the object to something else *including*
   expressions, so must use fts_eval_object_description for the whole thing,
   but want to keep the original description, so after creating the object,
   they reset the description, so that the original one is used.
   */
void 
fts_object_reset_description(fts_object_t *obj)
{
  int i;

  if (obj->argv)
    {
      fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));
      
      obj->argv = 0;
      obj->argc = 0;
    }
}


/* add the id to the object (called when we know the id, usually in messtiles.c) */
void 
fts_object_set_id(fts_object_t *obj, int id)
{
  /* set the id and put the object in the object table */
  if (obj->head.id != FTS_NO_ID)
    fts_object_table_remove(obj->head.id);

  if (id != FTS_NO_ID) 
    {
      obj->head.id = id;
      fts_object_table_put(id, obj);
    }
}

/* change number of outlets */
void
fts_object_change_number_of_outlets(fts_object_t *o, int new_noutlets)
{
  int old_noutlets = fts_object_get_outlets_number(o);

  if (old_noutlets == new_noutlets)
    return;

  /* delete all the connections that will not be pertinent any more */
  fts_object_trim_outlets_connections(o, new_noutlets);

  /* reallocate and copy the outlets, incoming connections and outlets properties if needed */
  if (new_noutlets == 0)
    {
      /* no new outlets, but there are old outlets to delete */
      fts_block_free((char *)o->out_conn, old_noutlets * sizeof(fts_connection_t *));

      o->out_conn = 0;
    }
  else if (old_noutlets > 0)
    {
      /* there are new outlets and old outlets (reallocate and move) */
      int i;
      fts_outlet_t  **new_outlets;
      fts_connection_t **new_out_conn;

      new_outlets  = (fts_outlet_t **)  fts_block_alloc(new_noutlets * sizeof(fts_outlet_t *));
      new_out_conn = (fts_connection_t **) fts_block_zalloc(new_noutlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_noutlets; i++)
	{
	  new_outlets[i] = 0;

	  if (i < old_noutlets)
	    new_out_conn[i] = o->out_conn[i];
	  else
	    new_out_conn[i] = 0;
	}

      fts_block_free((char *)o->out_conn, old_noutlets * sizeof(fts_connection_t *));
	      
      o->out_conn = new_out_conn;
    }
  else 
    {
      /* there are new outlets, but there were no outlets before (allocate without copying old stuff) */
      int i;

      o->out_conn = (fts_connection_t **) fts_block_zalloc(new_noutlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_noutlets; i++)
	o->out_conn[i] = 0;
    }

  /* change the class (of course, not the metaclass). */
  {
    fts_atom_t a[2];

    fts_set_symbol(&a[0], fts_object_get_class_name(o));
    fts_set_int(&a[1], new_noutlets);
    o->head.cl = fts_class_instantiate(2, a);
  }

  if (fts_object_has_id(o))
    fts_object_property_changed(o, fts_s_noutlets);
}

/*****************************************************************************
 *
 *  object access
 *
 */

fts_symbol_t 
fts_object_get_class_name(fts_object_t *obj)
{
  return fts_get_class_name(obj->head.cl);
}

/* test recursively if an object is inside a patcher (or its subpatchers) */
int 
fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher)
{
  if (! obj)
    return 0;
  else if (obj == (fts_object_t *) patcher)
    return 1;
  else
    return fts_object_is_in_patcher((fts_object_t *) fts_object_get_patcher(obj), patcher);
}

/*****************************************************************************
 *
 *  basic property handling
 *
 */

/* utility functions to assign a property as found by the expression parser */
static void 
fts_object_assign(fts_symbol_t name, fts_atom_t *value, void *data)
{
  fts_object_t *obj = (fts_object_t *)data;
  
  fts_object_put_prop(obj, name, value); 
}

void
fts_object_send_properties(fts_object_t *obj)
{
  /* If the object have an ID (i.e. was created by the client, or a property has
     been assigned to it),
     ask the object to send the ninlets and noutlets  properties,
     and name and declaration if any. */
  if (obj->head.id != FTS_NO_ID) 
    { 
      fts_object_property_changed(obj, fts_s_x);
      fts_object_property_changed(obj, fts_s_y);
      fts_object_property_changed(obj, fts_s_height);
      fts_object_property_changed(obj, fts_s_width);

      fts_object_property_changed(obj, fts_s_font);
      fts_object_property_changed(obj, fts_s_fontSize);
      fts_object_property_changed(obj, fts_s_fontStyle);

      if (fts_object_is_patcher(obj) && (! fts_object_is_error(obj)))
	{
	  fts_object_property_changed(obj, fts_s_wx);
	  fts_object_property_changed(obj, fts_s_wy);
	  fts_object_property_changed(obj, fts_s_wh);
	  fts_object_property_changed(obj, fts_s_ww);
	}

      fts_object_property_changed(obj, fts_s_ninlets);
      fts_object_property_changed(obj, fts_s_noutlets);
      fts_object_property_changed(obj, fts_s_error);
      fts_object_property_changed(obj, fts_s_error_description);

      /* Usefull for comment or object with comments */
      
      fts_object_property_changed(obj, fts_s_comment);
      fts_object_property_changed(obj, fts_s_layer);
      fts_object_property_changed(obj, fts_s_color);
      fts_object_property_changed(obj, fts_s_flash);

      /* Ask the object to send to the client object specific properties */
      fts_send_message(obj, fts_SystemInlet, fts_s_send_properties, 0, 0);
    }
}


/* properties used by the ui (value for the moment) at run time (update related) */
void
fts_object_send_ui_properties(fts_object_t *obj)
{
  if (obj->head.id != FTS_NO_ID)
    {
      /* Ask the object to send to the client object specific UI properties */
      fts_send_message(obj, fts_SystemInlet, fts_s_send_ui_properties, 0, 0);
    }
}


/* properties to be sent after a recomputing (i.e all the non geometrical properties) */
static void
fts_object_send_kernel_properties(fts_object_t *obj)
{
  /* If the object have an ID (i.e. was created by the client, or a property has
     been assigned to it),
     ask the object to send the ninlets and noutlets  properties,
     and name and declaration if any. */

  if (obj->head.id != FTS_NO_ID) 
    { 
      fts_object_property_changed(obj, fts_s_ninlets);
      fts_object_property_changed(obj, fts_s_noutlets);
      fts_object_property_changed(obj, fts_s_error);
      fts_object_property_changed(obj, fts_s_error_description);

      /* ask the object to send to the client object specific properties */
      fts_send_message(obj, fts_SystemInlet, fts_s_send_properties, 0, 0);
    }
}

static void 
fts_move_property(fts_object_t *old, fts_object_t *new, fts_symbol_t name)
{
  fts_atom_t a;

  fts_object_get_prop(old, name, &a);

  if (! fts_is_void(&a))
    fts_object_put_prop(new, name, &a);
}

static void 
fts_object_move_properties(fts_object_t *old, fts_object_t *new)
{
  /* copy only the editor properties here, not the others !!! */

  if (fts_object_is_standard_patcher(old))
    {
      fts_move_property(old, new, fts_s_wx);
      fts_move_property(old, new, fts_s_wy);
      fts_move_property(old, new, fts_s_wh);
      fts_move_property(old, new, fts_s_ww);
    }

  fts_move_property(old, new, fts_s_x);
  fts_move_property(old, new, fts_s_y);
  fts_move_property(old, new, fts_s_height);
  fts_move_property(old, new, fts_s_width);
  fts_move_property(old, new, fts_s_font);
  fts_move_property(old, new, fts_s_fontSize);
  fts_move_property(old, new, fts_s_fontStyle);
}

/*************************************************************************************
 *
 *  blip
 *
 *  send a blip for an object (i.e. a message that will be shown in the status line)
 *
 */
void 
fts_object_blip(fts_object_t *obj, const char *format , ...)
{
  fts_atom_t a;
  va_list ap;
  char buf[512];

  va_start( ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  fts_patcher_blip(fts_object_get_patcher(obj), buf);
}

/**************************************************************************************
 *
 *  debug print 
 *
 *  object is printed as:
 *    <description> <id>
 *
 *  if no description is present
 *    <metaclass-name> <id>
 */
void 
fprintf_object(FILE *f, fts_object_t *obj)
{
  if (! obj)
    {
      fprintf(f, "<NULL OBJ>");
    }
  else if (obj->argv)
    {
      if (fts_object_is_error(obj))
	fprintf(f, "<ERROR {");
      else
	fprintf(f, "<{");

      fprintf_atoms(f, obj->argc, obj->argv);
      fprintf(f, "} #%lx(%d)>", (unsigned long) obj, obj->head.id);
    }
  else
    fprintf(f, "<\"%s\" #%lx(%d)>", fts_symbol_name(fts_object_get_class_name(obj)),
	    (unsigned long) obj, obj->head.id);
}

void 
post_object(fts_object_t *obj)
{
  if (! obj)
    {
      post("{NULL OBJ}");
    }
  else if (obj->argv)
    {
      post("{");
      post_atoms(obj->argc, obj->argv);
      post("}");
    }
  else
    post("<\"%s\" #%d>", fts_symbol_name(fts_object_get_class_name(obj)), obj->head.id);
}
