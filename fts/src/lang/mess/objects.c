/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.51 $ IRCAM $Date: 1998/10/30 17:48:28 $
 *
 *  Eric Viara for Ircam, January 1995
 */

/* Define TRACE_DEBUG to get some debug printing during object creation.  */

/* #define TRACE_DEBUG */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

extern void fts_client_release_object(fts_object_t *c);
extern void fts_client_send_property(fts_object_t *obj, fts_symbol_t name);

/* forward declarations  */

static void fts_object_move_properties(fts_object_t *old, fts_object_t *new);
static void fts_object_send_kernel_properties(fts_object_t *obj);
static void fts_object_reset(fts_object_t *obj);
static void fts_object_tell_deleted(fts_object_t *obj);
static void fts_object_free(fts_object_t *obj, int release);
static void fts_object_delete_no_release(fts_object_t *obj);

/******************************************************************************/
/*                                                                            */
/*                  File Init function                                        */
/*                                                                            */
/******************************************************************************/

void fts_objects_init()
{
}

/******************************************************************************/
/*                                                                            */
/*                  Object Instantiation And Deleting                        */
/*                                                                            */
/******************************************************************************/

static fts_status_description_t fts_CannotInstantiate = {"Cannot instantiate class"};

/* A static function making the real FTS object if possible. */

fts_status_t 
fts_make_object(fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_object_t **ret)
{
  fts_status_t   status;
  fts_class_t   *cl;
  fts_object_t  *obj;
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

  obj     = (fts_object_t *)fts_block_zalloc(cl->size);
  obj->cl = cl;

  /* Other Initializations */

  obj->properties = 0;
  obj->varname    = 0;
  obj->id         = FTS_NO_ID;

  if (cl->noutlets)
    {
      obj->out_conn = (fts_connection_t **) fts_block_zalloc(cl->noutlets * sizeof(fts_connection_t *));
      obj->outlets_properties = (fts_plist_t **) fts_block_zalloc(cl->noutlets * sizeof(fts_plist_t *));
    }

  if (cl->ninlets)
    {
      obj->in_conn = (fts_connection_t **) fts_block_zalloc(cl->ninlets * sizeof(fts_connection_t *));
      obj->inlets_properties = (fts_plist_t **) fts_block_zalloc(cl->ninlets * sizeof(fts_plist_t *));
    }
    
  /* Add the object in the patcher
   * the test is only usefull during the root patcher building
   * and in rare (and wrong) cases where a service object is created
   * without a father
   */

  if (patcher)
    fts_patcher_add_object(patcher, obj);

  /* send the init message */

  {
    /* force type checking during new */

    long save_check_status;

    save_check_status = fts_mess_get_run_time_check();
    fts_mess_set_run_time_check(1);

    status = fts_message_send(obj, fts_SystemInlet, fts_s_init, ac, at);
    fts_mess_set_run_time_check(save_check_status);
  }

  if (status != fts_Success && status != &fts_MethodNotFound)
    {
      if (patcher)
	fts_patcher_remove_object(patcher, obj);

      if (obj->out_conn)
	{
	  fts_block_free((char *)obj->out_conn, obj->cl->noutlets*sizeof(fts_connection_t *));
	  fts_block_free((char *)obj->outlets_properties, obj->cl->noutlets*sizeof(fts_plist_t *));
	}

      if (obj->in_conn)
	{
	  fts_block_free((char *)obj->in_conn, obj->cl->ninlets*sizeof(fts_connection_t *));
	  fts_block_free((char *)obj->inlets_properties, obj->cl->ninlets*sizeof(fts_plist_t *));
	}

      fts_block_free((char *)obj, obj->cl->size);
      *ret = 0;

      return status;
    }

  *ret = obj;
  return fts_Success;
}


/* Utility functions to assign a property
   as found by the expression parser
   */


static void fts_object_assign_property(fts_symbol_t name, fts_atom_t *value, void *data)
{
  fts_object_t *obj = (fts_object_t *)data;

  fts_object_put_prop(obj, name, value);
}

/*
  This function create an object in a patcher, appling all the 
  expression/object semantic
 */


fts_object_t *fts_object_new(fts_patcher_t *patcher, int aoc, const fts_atom_t *aot)
{
  fts_object_t  *obj = 0;
  fts_metaclass_t *mcl;
  fts_symbol_t  var;
  fts_expression_state_t *e = 0;
  fts_atom_t state;
  int ac;
  const fts_atom_t *at;
  fts_atom_t new_args[1024]; /* Actually, the evaluated atom vector
				should be the copy for the object,
				and the expression parser should be able
				to tell how many atoms we need before
				evaluating the expression !! Yes, and Pere Noel
				should come twice a year, also.*/
#ifdef TRACE_DEBUG
  fprintf(stderr, "Object new ");
  fprintf_atoms(stderr, aoc, aot);
  fprintf(stderr, "\n");
#endif


  /* First of all, we check if we are in the case of a object variable definition syntax */

  if ((aoc >= 3) && fts_is_symbol(&aot[0]) && fts_is_symbol(&aot[1]) && (fts_get_symbol(&aot[1]) == fts_s_column))
    {
      /* foo : <obj> syntax; extract the variable name */
      
      var = fts_get_symbol(&aot[0]);

      /* If the variable already exists in this local context, make an wannabe error object  */

      if (! (fts_variable_is_suspended(patcher, var) ||
	     fts_variable_can_define(patcher, var)))
	{
	  /* Error: redefined variable */

	  obj = fts_error_object_new(patcher, aoc, aot,
				     "Variable %s is already defined", fts_symbol_name(var));
	  obj->is_wannabe = 1;

	  fts_variable_add_wannabe(patcher, var, obj);

	  var = 0;		/* forget about the variable, no binding to do */
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

      fts_variable_define(patcher, var, obj);
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
   * 1- Get the first argument; if it is a symbol, and there is an
   *    object doctor for this name, delegate the object creation to
   *    the doctor, without further evaluation; if the doctor return
   *    null, return an error object.
   *
   * 2- Expression-evaluate the whole description; if the evaluation
   *    give an error, return an error object.
   *    If the first value is not a symbol, return an error object.
   *    Otherwise consider the result of the evaluation as the description
   *    of the object for the following.
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
   * 6- if an FTS class  exists, instantiate that class.
   * 
   * 7 - if an template exists in the template path, instantiate
   *     that template.
   *
   * 8 - if an abstraction exists in the abstraction path, instantiate
   *     that abstraction.
   *
   *   
   * 9- Make an error object
   *
   */

  /* 1-  try the doctor */

  if ((! obj) && fts_is_symbol(&at[0]) && fts_object_doctor_exists(fts_get_symbol(&at[0])))
    {
      obj = fts_call_object_doctor(patcher, ac, at);

      if (! obj)
	{
	  /* Error in object doctor */

	  obj = fts_error_object_new(patcher, aoc, aot,
				     "Error in object doctor for %s", fts_symbol_name(fts_get_symbol(&at[0])));
	}
    }

  /* 2- Expression evaluate */

  if (! obj)
    {
      /* Compute the expressions with the correct offset */

      e = fts_expression_eval(patcher, ac, at,  1024, new_args);
	  
      if (fts_expression_get_status(e) != FTS_EXPRESSION_OK)
	{
	  /* Error in expression */

	  obj = fts_error_object_new(patcher, aoc, aot,
				     fts_expression_get_msg(e),
				     fts_expression_get_err_arg(e));
	}
      else if (! fts_is_symbol(&new_args[0]))
	{
	  /* Missing class name, or class name is not a symbol */

	  obj = fts_error_object_new(patcher, aoc, aot,
				     "The first argument should be a class name, but is not a symbol");
	}
      else
	{
	  at = new_args;
	  ac = fts_expression_get_count(e);
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
      obj = fts_call_object_doctor(patcher, ac, at);

      if (! obj)
	{
	  /* Error in object doctor */

	  obj = fts_error_object_new(patcher, aoc, aot,
				     "Error in object doctor for %s", fts_symbol_name(fts_get_symbol(&at[0])));
	}
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

      ret = fts_make_object(patcher, ac, at, &obj);

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
	    obj = fts_error_object_new(patcher, aoc, aot, "Error in object instantiation");
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

      obj = fts_error_object_new(patcher, aoc, aot,
				 "Object or template %s not found", fts_symbol_name(fts_get_symbol(aot)));
    }

  /* 10 - check if we are defining a variable *and* we have a state;
   * If yes, just store the state in the variable state for later use.
   * if not, destroy the current object, and substitute it with an error object
   * and store an error value in the variable state.
   */

  if ((! fts_object_is_error(obj)) && (var != 0))
    {
      fts_object_get_prop(obj, fts_s_state, &state);
      
      if (fts_is_void(&state))
	{
	  /* ERROR: the object cannot define a variable,
	     it does not have a "state" property */

	  fts_object_delete_no_release(obj);
	  obj = fts_error_object_new(patcher, aoc, aot,
				     "Object %s cannot define a variable",
				     fts_symbol_name(fts_get_symbol(aot + 2)));
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
	fts_expression_map_to_assignements(e, fts_object_assign_property, (void *) obj);
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


/*
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


fts_object_t *fts_object_recompute(fts_object_t *old)
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
      /* @@@ Later, here, handle the trasfer of a patcher data between two templates !! */

      /* If we have an object with data, data must be released,
	 because the object will be deleted */

      if (old->id != FTS_NO_ID)
	fts_client_release_object_data(old);

      obj = fts_object_redefine(old, old->id, old->argc, old->argv);

      /* Error property handling; currently it is a little bit
	 of an hack beacause we need a explit "zero"  error
	 property on a non error redefined object; actually
	 the error property daemon should be a global daemon !
	 */

      if (obj->id != FTS_NO_ID)
	fts_object_send_kernel_properties(obj);
    }

  return obj;
}


fts_object_t *fts_object_redefine(fts_object_t *old, int new_id, int ac, const fts_atom_t *at)
{
  int do_client;
  fts_symbol_t  var;
  fts_object_t  *new;

  /* If the new and the old id are the same, or if old do not have an id,
     don't do any update on the client side */

  do_client = ((old->id != FTS_NO_ID) && (old->id != new_id));

  /* check for the "var : <obj> syntax" and  extract the variable name if any */

  if ((ac >= 3) && fts_is_symbol(&at[0]) && fts_is_symbol(&at[1]) && (fts_get_symbol(&at[1]) == fts_s_column))
    var = fts_get_symbol(&at[0]);
  else
    var = 0;

  /* if the old object define a variable, and the new definition
     do not define  the same variable, delete the variable;
     if the new object define the same variable,  just suspend it.
     */

  if (old->varname)
    {
      if (old->varname == var)
	fts_variable_suspend(old->patcher, old->varname);
      else
	fts_variable_undefine(old->patcher, old->varname);

      /* Anyway, take away the name from the old object */

      old->varname = 0;
    }

  /* if old id and new id are the same, do the replace without telling the client */

  if ((old->id != FTS_NO_ID) && (old->id == new_id))
    {
      fts_object_table_remove(old->id);
      old->id = FTS_NO_ID;
    }

  /* Reset the old object, and call its delete method, so that
     it will release resources that may be needed by the new object,
     like the name for globally named objects like table */

  fts_object_reset(old);
  fts_object_tell_deleted(old);

  /* Make the new object  */

  new = fts_object_new(fts_object_get_patcher(old), ac, at);
  fts_object_set_id(new, new_id);
  
  /* Update the loading vm */

  fts_vm_substitute_object(old, new);

  /* If new is an error object, assure that there are enough inlets
   and outlets for the connections */

  if (fts_object_is_error(new))
    {
      fts_error_object_fit_inlet(new, old->cl->ninlets - 1);
      fts_error_object_fit_outlet(new, old->cl->noutlets - 1);
    }

  fts_object_move_connections(old, new, do_client);
  fts_object_move_properties(old, new);

  fts_object_free(old, do_client);

  if (do_client)
    fts_client_upload_object(new);

  return new;
}

/* This is to support "changing" objects; usefull during 
 * .pat loading, where not all the information is available 
 *    at the right place; used currently explode in the fts1.5 package.
 * WARNING: user object should never call this function, otherwise they risk
 * to loose the expressions definition.
 */

void fts_object_set_description(fts_object_t *obj, int argc, const fts_atom_t *argv)
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

void fts_object_set_description_and_class(fts_object_t *obj, fts_symbol_t class_name,
					  int argc, const fts_atom_t *argv)
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
   expressions, so must use fts_object_new for the whole thing,
   but want to keep the original description, so after creating the object,
   they reset the description, so that the original one is used.
   */
   

void fts_object_reset_description(fts_object_t *obj)
{
  int i;

  if (obj->argv)
    {
      fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));
      
      obj->argv = 0;
      obj->argc = 0;
    }
}

/* 
   Add the id to the object.
   Called when we know the id, usually in messtiles.c
   */


void fts_object_set_id(fts_object_t *obj, int id)
{
  
  /* set the id and put the object in the object table */

  if (obj->id != FTS_NO_ID)
    fts_object_table_remove(obj->id);

  if (id != FTS_NO_ID) 
    {
      obj->id = id;
      fts_object_table_put(id, obj);
    }
}

/* Delete, exported  version */

void fts_object_delete(fts_object_t *obj)
{
  fts_object_reset(obj);
  fts_object_tell_deleted(obj);
  fts_object_free(obj, 1);
}

static void fts_object_delete_no_release(fts_object_t *obj)
{
  fts_object_reset(obj);
  fts_object_tell_deleted(obj);
  fts_object_free(obj, 0);
}

/* The deleting phase 1: reset the object, i.e. take it away from
   system structures, like variables definitions, variable users,
   update fifo and so on; the object is still accessible after this
   call, and can still manipulate messages, but it will be not
   reinstantiated, all the object depending on him are already
   been recomputed.
   */

static void fts_object_reset(fts_object_t *obj)
{
  /* Unbind it from its variable if any */

  if (fts_object_get_variable(obj))
    fts_variable_undefine(obj->patcher, fts_object_get_variable(obj));

  /* Or, if the object is a wannabe, remove it from the variable */

  if (obj->is_wannabe)
    fts_variable_remove_wannabe(obj->patcher, obj->varname, obj);

  /* Remove it as user of its var refs */

  while (obj->var_refs)
    fts_binding_remove_user(obj->var_refs->var, obj);

  /* take it away from the update queue, if there */

  fts_object_reset_changed(obj);
}


/* Delete phase 2: tell the object he is going to be deleted */
     
static void fts_object_tell_deleted(fts_object_t *obj)
{
  /* tell the object we are going to delete him */

  fts_send_message(obj, fts_SystemInlet, fts_s_delete, 0, 0);
}

/* Delete phase 3: actually free the object and its system structures;
   if the release flag is set to one, release the client representation
   of the object; sometimes, the client representation can be reused for
   an other object */

static void fts_object_free(fts_object_t *obj, int release)
{
  int outlet, inlet;

  /* delete all the survived connections starting in the object */

  for (outlet = 0; outlet < obj->cl->noutlets; outlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      while (p = obj->out_conn[outlet])
	fts_connection_delete(p);
    }

  /* Delete all the survived connections ending in the object */

  for (inlet = 0; inlet < obj->cl->ninlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      while (p = obj->in_conn[inlet])
	fts_connection_delete(p);
    }


  /* Delete the object from the patcher and the patcher data !*/

  /* Some internal object don't necessarly have a patcher,
     (they should be put in the root patcher ??) */

  if (obj->patcher)
    fts_patcher_remove_object(obj->patcher, obj);

  /* Now Tell the client to release the Java part */

  if (release && (obj->id != FTS_NO_ID))
    fts_client_release_object(obj);

  /* remove the object from the object table */

  if (obj->id != FTS_NO_ID)
    fts_object_table_remove(obj->id);

  /* free the object properties */

  fts_properties_free(obj);

  /* Free the object description */

  if (obj->argv)
    fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

  /* free the object */

  if (obj->out_conn)
    {
      fts_block_free((char *)obj->out_conn, obj->cl->noutlets*sizeof(fts_connection_t *));
      fts_block_free((char *)obj->outlets_properties, obj->cl->noutlets*sizeof(fts_plist_t *));
    }

  if (obj->in_conn)
    {
      fts_block_free((char *)obj->in_conn, obj->cl->ninlets*sizeof(fts_connection_t *));
      fts_block_free((char *)obj->inlets_properties, obj->cl->ninlets*sizeof(fts_plist_t *));
    }

  fts_block_free((char *)obj, obj->cl->size);
}


/******************************************************************************/
/*                                                                            */
/*                          Object Access                                     */
/*                                                                            */
/******************************************************************************/


fts_symbol_t fts_object_get_class_name( fts_object_t *obj)
{
  return fts_get_class_name(obj->cl);
}

int fts_object_handle_message(fts_object_t *o, int winlet, fts_symbol_t s)
{
  int anything;

  if (fts_class_mess_get(o->cl, winlet, s, &anything))
    return 1;
  else
    return 0;
}

/* Test recursively if an object is inside a patcher (or its subpatchers) */

int fts_object_is_in_patcher(fts_object_t *obj, fts_patcher_t *patcher)
{
  if (! obj)
    return 0;
  else if (obj == (fts_object_t *) patcher)
    return 1;
  else
    return fts_object_is_in_patcher((fts_object_t *) fts_object_get_patcher(obj), patcher);
}


/******************************************************************************/
/*                                                                            */
/*                          Object Basic property handling                    */
/*                                                                            */
/******************************************************************************/

/* 
   All this should be somehow reviewed .... it is not completely satisfying,
   and should be more modular 
   */

void
fts_object_send_properties(fts_object_t *obj)
{
  /* If the object have an ID (i.e. was created by the client, or a property has
     been assigned to it),
     ask the object to send the ninlets and noutlets  properties,
     and name and declaration if any. */

  if (obj->id != FTS_NO_ID) 
    { 
      fts_client_send_property(obj, fts_s_x);
      fts_client_send_property(obj, fts_s_y);
      fts_client_send_property(obj, fts_s_height);
      fts_client_send_property(obj, fts_s_width);

      fts_client_send_property(obj, fts_s_font);
      fts_client_send_property(obj, fts_s_fontSize);

      if (fts_object_is_patcher(obj) && (! fts_object_is_error(obj)))
	{
	  fts_client_send_property(obj, fts_s_wx);
	  fts_client_send_property(obj, fts_s_wy);
	  fts_client_send_property(obj, fts_s_wh);
	  fts_client_send_property(obj, fts_s_ww);
	}

      fts_client_send_property(obj, fts_s_ninlets);
      fts_client_send_property(obj, fts_s_noutlets);
      fts_client_send_property(obj, fts_s_error);

      /* Ask the object to send to the client object specific properties */

      fts_send_message(obj, fts_SystemInlet, fts_s_send_properties, 0, 0);
    }
}


/* Properties used by the ui (value for the moment) at run time; update related */

void
fts_object_send_ui_properties(fts_object_t *obj)
{
  if (obj->id != FTS_NO_ID)
    {
      /* Ask the object to send to the client object specific UI properties */

      fts_send_message(obj, fts_SystemInlet, fts_s_send_ui_properties, 0, 0);
    }
}


/* Properties to be sent after a recomputing; i.e. all the non geometrical properties */

static void
fts_object_send_kernel_properties(fts_object_t *obj)
{
  /* If the object have an ID (i.e. was created by the client, or a property has
     been assigned to it),
     ask the object to send the ninlets and noutlets  properties,
     and name and declaration if any. */

  if (obj->id != FTS_NO_ID) 
    { 
      fts_client_send_property(obj, fts_s_ninlets);
      fts_client_send_property(obj, fts_s_noutlets);
      fts_client_send_property(obj, fts_s_error);

      /* Ask the object to send to the client object specific properties */

      fts_send_message(obj, fts_SystemInlet, fts_s_send_properties, 0, 0);
    }
}


static void fts_move_property(fts_object_t *old, fts_object_t *new, fts_symbol_t name)
{
  fts_atom_t a;

  fts_object_get_prop(old, name, &a);

  if (! fts_is_void(&a))
    fts_object_put_prop(new, name, &a);
}

  

static void fts_object_move_properties(fts_object_t *old, fts_object_t *new)
{
  /* Copy only the editor properties here, not the others !!! */

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
}

/* Debug print
   An object is printed as:

   <{description} #id>

   If no description is present, is printed as

   < "metaclass-name" #id>
 */


void fprintf_object(FILE *f, fts_object_t *obj)
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
      fprintf(f, "} #%lx(%d)>", (unsigned int) obj, obj->id);
    }
  else
    fprintf(f, "<\"%s\" #%lx(%d)>", fts_symbol_name(fts_object_get_class_name(obj)), (unsigned int) obj, obj->id);
}


void post_object(fts_object_t *obj)
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
    post("<\"%s\" #%d>", fts_symbol_name(fts_object_get_class_name(obj)), obj->id);
}

