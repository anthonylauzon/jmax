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

#include <stdarg.h>

#include <ftsconfig.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/abstraction.h>
#include <ftsprivate/client.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/property.h>
#include <ftsprivate/template.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/variable.h>

/* forward declarations  */
static void fts_object_move_properties(fts_object_t *old, fts_object_t *new);

static fts_status_description_t class_not_found_error_description = {
  "object or template not found"
};
static fts_status_t class_not_found_error = &class_not_found_error_description;

static fts_status_description_t class_instantiation_error_description = {
  "class instantiation error"
};
static fts_status_t class_instantiation_error = &class_instantiation_error_description;

static fts_status_description_t invalid_class_name_error_description = {
  "invalid class name"
};
static fts_status_t invalid_class_name_error = &invalid_class_name_error_description;


/******************************************************************************
 *
 * create an object from a class and arguments
 *
 */

static fts_object_t *
fts_object_new( fts_class_t *cl)
{
  fts_object_t *obj = (fts_object_t *)fts_heap_zalloc(cl->heap);

  obj->cl = cl;
  obj->client_id = FTS_NO_ID;

  obj->n_inlets = cl->ninlets;
  if (cl->ninlets)
    obj->in_conn = (fts_connection_t **) fts_zalloc(cl->ninlets * sizeof(fts_connection_t *));
    
  obj->n_outlets = cl->noutlets;
  if (cl->noutlets)
    obj->out_conn = (fts_connection_t **) fts_zalloc(cl->noutlets * sizeof(fts_connection_t *));

  return obj;
}

static void 
fts_object_free(fts_object_t *obj)
{
  fts_properties_free(obj);

  if (obj->argv)
    fts_free( obj->argv);

  if (obj->out_conn)
    fts_free( obj->out_conn);
  if (obj->in_conn)
    fts_free( obj->in_conn);

  fts_heap_free(obj, fts_object_get_class( obj)->heap);
}

fts_object_t *
fts_object_create( fts_class_t *cl, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;

  /* Is class instantiated ? We can test that with the 'size' member 
     because it is set by the instantiate function and cannot be set to 0 */
  if (!cl->size)
    fts_class_instantiate(cl);

  obj = fts_object_new( cl);

  fts_object_set_patcher(obj, patcher);

  /* this would be even more elegant!
  if (parent)
    {
      fts_atom_t a;

      fts_set_object( &a, obj);
      fts_class_get_add_child( fts_object_get_class( parent))( parent, fts_system_inlet, 1, &a);
    }
  */

  fts_class_get_constructor(cl)(obj, fts_system_inlet, fts_s_init, ac, at); 

  if(fts_object_get_error(obj) != NULL)
    {
      fts_class_get_deconstructor(cl)(obj, fts_system_inlet, fts_s_delete, 0, 0); 
      fts_object_free(obj);
      
      return NULL;
    }
 
  return obj;
}

/***********************************************************************
 *
 * evaluate an object description using the expression evaluator
 *
 */

struct eval_data 
{
  fts_object_t *obj;
  fts_patcher_t *patcher;
};

static fts_status_t
eval_object_description_expression_callback( int ac, const fts_atom_t *at, void *data)
{
  struct eval_data *eval_data = (struct eval_data *)data;

  if (eval_data->obj == NULL)
    {
      if (ac == 1 && fts_is_object( at))
	{
	  eval_data->obj = fts_get_object( at);

	  return fts_ok;
	}

      return class_instantiation_error;
    }
  else
    {
      if(ac > 0 && fts_is_symbol(at))
	{
	  fts_class_t *cl = fts_object_get_class(eval_data->obj);
	  fts_symbol_t selector = fts_get_symbol(at);
	  fts_method_t meth = fts_class_get_method(cl, selector);

	  if(meth)
	    {
	      (*meth)(eval_data->obj, fts_system_inlet, selector, ac - 1, at + 1);

	      if(fts_object_get_error(eval_data->obj) == NULL)
		return fts_ok;
	    }
	}

      fts_object_destroy(eval_data->obj);
      eval_data->obj = NULL;

      return class_instantiation_error;
    }

  return fts_ok;
}

fts_object_t *
fts_eval_object_description( fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = 0;
  fts_expression_t *expression;
  struct eval_data data;
  fts_status_t status;
  int new_ac;
  fts_atom_t *new_at;

  if (ac == 0)
    {
      /* empty object */
      fts_atom_t a;

      fts_set_symbol(&a, fts_s_empty_object);
      obj = fts_object_create(fts_error_object_class, patcher, 1, &a);
      fts_object_set_description(obj, ac, at);

      fts_patcher_add_object( patcher, obj);    

      return obj;
    }

  data.obj = NULL;
  data.patcher = patcher;

  if ( (fts_is_symbol( at) && fts_get_symbol( at) == fts_s_colon) 
       || (ac >= 2 && fts_is_symbol( at+1) && fts_get_symbol( at+1) == fts_s_colon))
    {
      new_ac = ac;
      new_at = (fts_atom_t *)at;
    }
  else
    {
      int i;

      new_ac = ac+1;
      new_at = alloca( new_ac * sizeof (fts_atom_t));

      fts_set_symbol( new_at, fts_s_colon);
      for ( i = 0; i < ac; i++)
	new_at[i+1] = at[i];
    }

  status = fts_expression_new( new_ac, new_at, &expression);
  if (status == fts_ok)
    {
      status = fts_expression_reduce( expression, patcher, 0, 0, eval_object_description_expression_callback, &data);

      if (status == fts_ok)
	obj = data.obj;
    }

  if (status != fts_ok)
    {
      fts_atom_t a;

      fts_set_symbol(&a, fts_status_get_description( status));
      obj = fts_object_create(fts_error_object_class, patcher, 1, &a);
    }

  fts_patcher_add_object( patcher, obj);    

  /* Add the newly created object as user of the expression's variables,
     even if it is an error object, because we may try to recompute, and recover,
     the object, if one of this variables have been redefined. */
  fts_expression_add_variables_user( expression, patcher, obj);

  if(fts_object_get_description_atoms(obj) == NULL)
    fts_object_set_description(obj, new_ac, new_at);

  fts_expression_delete(expression);

  return obj;
}


/***********************************************************************
 *
 * Object inlets/outlets
 *
 */

void
fts_object_set_inlets_number(fts_object_t *o, int n)
{
  if(o->n_inlets != n)
    {
      int i;

      fts_object_trim_inlets_connections(o, n);

      o->in_conn = fts_realloc(o->in_conn, n * sizeof(fts_connection_t *));
      
      for(i=o->n_inlets; i<n; i++)
	o->in_conn[i] = NULL;

      o->n_inlets = n;
    }
}

void
fts_object_set_outlets_number(fts_object_t *o, int n)
{
  if(o->n_outlets != n)
    {
      int i;

      fts_object_trim_outlets_connections(o, n);

      o->out_conn = fts_realloc(o->out_conn, n * sizeof(fts_connection_t *));

      for(i=o->n_outlets; i<n; i++)
	o->out_conn[i] = NULL;

      o->n_outlets = n;
    }
}

/*********************************************************************************
 * 
 *  set name
 *
 */

void
fts_object_set_name(fts_object_t *obj, fts_symbol_t sym)
{
  fts_patcher_t *patcher = fts_object_get_patcher(obj);

  if(patcher != NULL)
    {
      /* reset current definition */
      if(fts_object_get_definition(obj) != NULL)
	fts_definition_update(fts_object_get_definition(obj), fts_null);
      
      if(sym != fts_s_empty_string)
	{
	  fts_patcher_t *scope = fts_patcher_get_scope(patcher);
	  fts_symbol_t name = fts_name_get_unused(scope, sym);
	  fts_definition_t *def = fts_definition_get(scope, name);
	  fts_atom_t a;
	  
	  /* set new definiton */
	  fts_set_object(&a, obj);
	  fts_definition_update(def, &a);
	  
	  /* store definition in object */
	  fts_object_set_definition(obj, def);
	  
	  /* set name of object */
	  if(fts_object_has_id(obj))
	    {
	      fts_set_symbol(&a, name);
	      fts_client_send_message(obj, fts_s_name, 1, &a);
	    }
	}
      else if(fts_object_has_id(obj))
	{
	  fts_atom_t a;

	  fts_set_symbol(&a, fts_s_empty_string);
	  fts_client_send_message(obj, fts_s_name, 1, &a);	  
	}
    }
}

fts_symbol_t 
fts_object_get_name(fts_object_t *obj)
{
  fts_definition_t *def = fts_object_get_definition(obj);

  if(def)
    return fts_definition_get_name(def);

  return NULL;
}

/****************************************************************
 *
 *  delete object
 *
 *    fts_object_unconnect()
 *    fts_object_unbind()
 *    fts_object_unname()
 *    fts_object_unclient()
 *    fts_object_free()
 *
 *    fts_object_destroy()
 *    fts_object_release() ... macro!
 *
 */

/* remove all connections from the object (done when unplugged from the patcher) */
void 
fts_object_unconnect(fts_object_t *obj)
{
  int outlet, inlet;

  /* delete all the survived connections starting in the object */
  for (outlet=0; outlet<obj->n_outlets; outlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons and methods can fire correctly */
      while ((p = obj->out_conn[outlet]))
	fts_connection_delete(p);
    }

  /* Delete all the survived connections ending in the object */
  for (inlet=0; inlet<obj->n_inlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons and methods  can fire correctly */
      while ((p = obj->in_conn[inlet]))
	fts_connection_delete(p);
    }
}

void 
fts_object_unbind(fts_object_t *obj)
{
  fts_list_t *list = obj->name_refs;

  /* remove object as listener from the variables */
  while(list != NULL) 
    {
      fts_definition_t *def = (fts_definition_t *)fts_get_pointer(fts_list_get(list));
      
      fts_definition_remove_listener(def, obj);
      
      list = fts_list_next(list);
    }
}

static void 
fts_object_unname(fts_object_t *obj)
{
  /* remove definition of named object */
  if(obj->definition != NULL)
    {
      fts_definition_update(obj->definition, fts_null);
      obj->definition = NULL;
    }
}

static void 
fts_object_unclient(fts_object_t *obj)
{
  if ( fts_object_get_id( obj) > FTS_NO_ID)
    fts_client_release_object(obj);
}

/* delete the unbound, unconnected object already removed from the patcher */
void 
fts_object_destroy(fts_object_t *obj)
{
  /* unregister name */
  fts_object_unname(obj);

  /* call deconstructor */
  if(fts_class_get_deconstructor(fts_object_get_class(obj)))
    fts_class_get_deconstructor(fts_object_get_class(obj))(obj, fts_system_inlet, fts_s_delete, 0, 0);

  /* release all client components */
  fts_object_unclient(obj);

  /* free memory */
  fts_object_free(obj);
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
  return fts_object_redefine(old, old->argc, old->argv);
}

fts_object_t *
fts_object_redefine(fts_object_t *old, int ac, const fts_atom_t *at)
{
  int old_id = fts_object_get_id( old);

  /* redefine object if not scheduled for removal */
  if(old_id != FTS_DELETE)
    {
      fts_symbol_t name = fts_object_get_name(old);
      fts_object_t *new;
      
      /* unbind variables from old object */
      fts_object_unname(old);
      fts_object_unbind(old);

      /* call deconstructor of old object */
      if(old->refcnt == 1)
	fts_class_get_deconstructor( fts_object_get_class( old))(old, fts_system_inlet, fts_s_delete, 0, 0);
      
      /* make the new object  */
      new = fts_eval_object_description(fts_object_get_patcher(old), ac, at);
      
      /* update the loading vm */
      fts_vm_substitute_object(old, new);
      
      /* if new is an error object, assure that there are enough inlets and outlets for the connections */
      if (fts_object_is_error(new))
	{
	  fts_error_object_fit_inlet(new, old->n_inlets - 1);
	  fts_error_object_fit_outlet(new, old->n_outlets - 1);
	}
      
      /* move graphic properties from old to new object */
      fts_object_move_properties(old, new);
      
      /* move the connections from the old to the new object */
      fts_object_move_connections(old, new);
      
      /* set name of new object */
      if(name != NULL)
	{
	  fts_atom_t a;

	  fts_set_symbol(&a, name);
	  fts_send_message(new, fts_s_name, 1, &a);
	}

      /* remove old from client and update list */
      fts_object_unclient(old);
      fts_update_reset(old);
      
      /* assure that object won't be destroyed when removed from patcher */
      old->refcnt++;

      /* remove the object from the patcher */
      if(old->patcher)
	fts_patcher_remove_object(old->patcher, old);
      
      /* destroy or  */
      if(old->refcnt == 1)
	fts_object_free(old);
      else
	{
	  old->refcnt--;
	  old->patcher = NULL;
	}

      /* upload new object if old object was uploaded */
      if(old_id != FTS_NO_ID)
	{
	  fts_client_upload_object(new, -1);
	  fts_client_upload_object_connections(new);
	}
      
      return new;
    }
  else
    {
      /* unbind variables */
      fts_object_unname(old);
      fts_object_unbind(old);
    }      

  return NULL;
}

/*********************************************************************************
 * 
 *  object description
 *
 */

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
	fts_free( obj->argv);

      /* reallocate the description if argc > -0 and copy the arguments */
      obj->argc = argc;

      if (argc > 0)
	{
	  obj->argv = (fts_atom_t *) fts_zalloc(argc * sizeof(fts_atom_t));

	  for (i = 0; i < argc; i++)
	    obj->argv[i] = argv[i];
	}
      else
	obj->argv = 0;
    }
}


/*****************************************************************************
 *
 *  object access
 *
 */

fts_symbol_t 
fts_object_get_class_name(fts_object_t *obj)
{
  return fts_class_get_name( fts_object_get_class( obj));
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
 *  package
 *
 */
fts_package_t *
fts_object_get_package(fts_object_t *obj)
{
  if(fts_object_is_template(obj))
    return fts_template_get_package(fts_patcher_get_template((fts_patcher_t *)obj));
  else
    return fts_class_get_package(fts_object_get_class(obj));
}
