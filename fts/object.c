/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <stdarg.h>
#include <stdlib.h>

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

static fts_status_description_t invalid_object_description_error_description = {
  "invalid object description"
};
static fts_status_t invalid_object_description_error = &invalid_object_description_error_description;

static fts_status_description_t init_message_error_description = {
  "invalid initialization message"
};
static fts_status_t init_message_error = &init_message_error_description;

static void fts_object_remove_description(fts_object_t *obj);
static void fts_object_remove_connections(fts_object_t *obj);
static void fts_object_remove_inoutlets(fts_object_t *obj);
static void fts_object_remove_bindings(fts_object_t *obj);
static void fts_object_reset_client(fts_object_t *obj);

/******************************************************************************
 *
 * patcher data
 *
 */
fts_heap_t *patcher_data_heap = NULL;

fts_object_patcher_data_t *
fts_object_get_patcher_data(fts_object_t *obj)
{
  fts_class_t *cl = obj->cl;
  
  if(obj->patcher_data == NULL)
  {
    fts_object_patcher_data_t *data;
    
    if(patcher_data_heap == NULL)
      patcher_data_heap = fts_heap_new(sizeof(fts_object_patcher_data_t));

    data = fts_heap_zalloc(patcher_data_heap);

    /* allocate inlet connections */
    data->n_inlets = cl->ninlets;
    if(cl->ninlets > 0)
      data->in_conn = (fts_connection_t **) fts_zalloc(cl->ninlets * sizeof(fts_connection_t *));

    /* allocate outlet connections */
    data->n_outlets = cl->noutlets;
    if(cl->noutlets > 0)
      data->out_conn = (fts_connection_t **) fts_zalloc(cl->noutlets * sizeof(fts_connection_t *));

    obj->patcher_data = data;
  }

  return obj->patcher_data;
}

void
fts_object_remove_patcher_data(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = obj->patcher_data;
  
  if(data != NULL)
  {
    fts_object_remove_description(obj);
    fts_object_remove_inoutlets(obj);

    /* remove binding to variables */
    fts_object_remove_bindings(obj);
    
    fts_heap_free((void *)data, patcher_data_heap);
    obj->patcher_data = NULL;
  }
}

void
fts_object_set_patcher(fts_object_t *o, fts_patcher_t *patcher)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(o);

  data->patcher = patcher;
}

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
  fts_object_set_id( obj, FTS_NO_ID);
  fts_object_set_client_id( obj, FTS_NO_ID);
  fts_object_set_status( obj, FTS_OBJECT_STATUS_CREATE);

  return obj;
}

static void 
fts_object_free(fts_object_t *obj)
{
  fts_properties_free(obj);

  fts_heap_free(obj, fts_object_get_class( obj)->heap);
}

fts_object_t *
fts_object_create(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = NULL;

  if (!cl->size)
    fts_class_instantiate(cl);

  obj = fts_object_new(cl);

  /* call constructor */
  fts_class_get_constructor(cl)(obj, fts_system_inlet, NULL, ac, at);

  if( fts_object_get_status( obj) == FTS_OBJECT_STATUS_INVALID)
  {
    /* destroy invalid object */
    fts_class_get_deconstructor(cl)(obj, fts_system_inlet, NULL, 0, 0);
    fts_object_free(obj);

    return NULL;
  }

  fts_object_set_status( obj, 0);

  return obj;
}

fts_object_t *
fts_object_create_in_patcher(fts_class_t *cl, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = NULL;

  if (!cl->size)
    fts_class_instantiate(cl);

  obj = fts_object_new(cl);

  if(patcher != NULL)
    fts_object_set_patcher(obj, patcher);

  /* call constructor */
  fts_class_get_constructor(cl)(obj, fts_system_inlet, NULL, ac, at);

  if ( fts_object_get_status( obj) == FTS_OBJECT_STATUS_INVALID)
  {
    /* destroy invalid object */
    fts_class_get_deconstructor(cl)(obj, fts_system_inlet, NULL, 0, 0);
    fts_object_free(obj);

    return NULL;
  }

  fts_object_set_status( obj, 0);

  return obj;
}

/* delete the unbound, unconnected object already removed from the patcher */
void
fts_object_destroy(fts_object_t *obj)
{
  fts_object_remove_patcher_data(obj);

  /* unregister name */
  fts_object_remove_name(obj);

  /* call deconstructor */
  if(fts_class_get_deconstructor(fts_object_get_class(obj)))
    fts_class_get_deconstructor(fts_object_get_class(obj))(obj, fts_system_inlet, fts_s_delete, 0, 0);

  /* release all client components */
  fts_object_reset_client(obj);

  /* free memory */
  fts_object_free(obj);
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
  fts_status_t status = fts_ok;
    
  if (eval_data->obj == NULL)
  {
    if (ac == 1 && fts_is_object( at))
    {
      eval_data->obj = fts_get_object( at);

      /* keep object in state of creation */
      fts_object_set_status(eval_data->obj, FTS_OBJECT_STATUS_CREATE);

      /* add object to patcher */
      fts_patcher_add_object( eval_data->patcher, eval_data->obj);
      return fts_ok;
    }
    
    status = invalid_object_description_error;
  }
  else
  {
    if(ac > 0 && fts_is_symbol(at))
    {
      /* send message to fresh object */
      if(fts_send_message(eval_data->obj, fts_get_symbol(at), ac - 1, at + 1))
      {
        if(fts_object_get_status(eval_data->obj) == FTS_OBJECT_STATUS_INVALID)
          status = fts_status_new(fts_get_error());
      }
      else
        status = init_message_error;
    }

    if(status != fts_ok)
    {
      fts_patcher_remove_object(eval_data->patcher, eval_data->obj);
      eval_data->obj = NULL;
    }
  }

  return status;
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

  /* empty object */
  if (ac == 0)
  {
    fts_atom_t a;

    /* create error object */
    fts_set_symbol(&a, fts_s_empty_object);
    obj = fts_object_create_in_patcher(fts_error_object_class, patcher, 1, &a);
    fts_object_set_description(obj, ac, at);

    fts_patcher_add_object( patcher, obj);

    return obj;
  }

  data.obj = NULL;
  data.patcher = patcher;

  /* 
     FIXME ??? should it be removed ? the .jmax loader adds the :, so this is usefull only for
     the GUI evaluation ?
  */
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

    /* create error object */
    fts_set_symbol(&a, fts_status_get_description(status));
    obj = fts_object_create_in_patcher(fts_error_object_class, patcher, 1, &a);
    fts_object_set_description(obj, ac, at);

    fts_patcher_add_object(patcher, obj);
  }

  /* Add the newly created object as user of the expression's variables,
     even if it is an error object, because we may try to recompute, and recover,
     the object, if one of this variables have been redefined. */
  fts_expression_add_variables_user( expression, patcher, obj);

  if (obj && fts_object_get_description_size(obj) == 0)
    fts_object_set_description( obj, new_ac, new_at);

  fts_expression_delete(expression);

  return obj;
}

/***********************************************************************
 *
 * object inlets/outlets
 *
 */

static void
fts_object_trim_inlets_connections(fts_object_t *obj, int inlets)
{
  fts_object_patcher_data_t *data = obj->patcher_data;

  if(data != NULL)
  {
    int inlet;

    for (inlet = inlets; inlet < data->n_inlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
      and methods  can fire correctly */

      for (p = data->in_conn[inlet]; p; p = data->in_conn[inlet])
        fts_connection_delete(p);

      data->in_conn[inlet] = NULL;
    }
  }
}


static void
fts_object_trim_outlets_connections(fts_object_t *obj, int outlets)
{
  fts_object_patcher_data_t *data = obj->patcher_data;

  if(data != NULL)
  {
    int outlet;

    for (outlet = outlets; outlet < data->n_outlets; outlet++)
    {
      fts_connection_t *p;

      /* The loop work by iterating on the first connection;
      this work because the loop destroy one connection at a time.
      */

      for (p = data->out_conn[outlet]; p ;  p = data->out_conn[outlet])
        fts_connection_delete(p);

      
    data->out_conn[outlet] = NULL;
    }
  }
}

void
fts_object_set_inlets_number(fts_object_t *o, int n)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(o);
  
  if(data->n_inlets != n)
    {
      int i;

      fts_object_trim_inlets_connections(o, n);

      data->in_conn = fts_realloc(data->in_conn, n * sizeof(fts_connection_t *));
      
      for(i=data->n_inlets; i<n; i++)
	data->in_conn[i] = NULL;

      data->n_inlets = n;
    }
}

void
fts_object_set_outlets_number(fts_object_t *o, int n)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(o);

  if(data->n_outlets != n)
    {
      int i;

      fts_object_trim_outlets_connections(o, n);

      data->out_conn = fts_realloc(data->out_conn, n * sizeof(fts_connection_t *));

      for(i=data->n_outlets; i<n; i++)
	data->out_conn[i] = NULL;

      data->n_outlets = n;
    }
}

static void
fts_object_remove_inoutlets(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = obj->patcher_data;
  
  if(data != NULL)
  {
    fts_object_remove_connections(obj);
    
    if(data->in_conn)
      fts_free(data->in_conn);

    if(data->out_conn)
      fts_free(data->out_conn);

    data->in_conn = NULL;
    data->out_conn = NULL;

    data->n_inlets = 0;
    data->n_outlets = 0;
  }
}

/*****************************************************************************
*
*  connection handling
*
*/

static void
fts_object_move_connections(fts_object_t *old, fts_object_t *new)
{
  fts_object_patcher_data_t *data = old->patcher_data;

  if(data != NULL)
  {
    fts_connection_t *p;
    int i;

    for (i=0; i<fts_object_get_outlets_number(old); i++)
    {
      fts_connection_t *p;

      for (p=data->out_conn[i]; p;  p=data->out_conn[i])
      {
        if(i < fts_object_get_outlets_number(new) && p->type > fts_c_hidden)
          fts_connection_new(new, p->woutlet, p->dst, p->winlet, p->type);

        fts_connection_delete(p);
      }
    }

    for (i=0; i<fts_object_get_inlets_number(old); i++)
    {
      for (p=data->in_conn[i]; p; p=data->in_conn[i])
      {
        if(i < fts_object_get_inlets_number(new) && p->type > fts_c_hidden)
          fts_connection_new(p->src, p->woutlet, new, p->winlet, p->type);

        fts_connection_delete(p);
      }
    }
  }
}

/* remove all connections from the object (done when unplugged from the patcher) */
static void
fts_object_remove_connections(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = obj->patcher_data;

  if(data != NULL)
  {
    int outlet, inlet;

    /* delete all the survived connections starting in the object */
    for (outlet=0; outlet<data->n_outlets; outlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons and methods can fire correctly */
      while ((p = data->out_conn[outlet]))
        fts_connection_delete(p);
    }

    /* Delete all the survived connections ending in the object */
    for (inlet=0; inlet<data->n_inlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons and methods  can fire correctly */
      while ((p = data->in_conn[inlet]))
        fts_connection_delete(p);
    }
  }
}

/*********************************************************************************
*
*  object naming and bindings
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

void
fts_object_update_name(fts_object_t *obj)
{
  fts_definition_t *def = fts_object_get_definition(obj);

  if(def)
    fts_definition_update(def, &def->value);
}

fts_symbol_t
fts_object_get_name(fts_object_t *obj)
{
  fts_definition_t *def = fts_object_get_definition(obj);

  if(def)
    return fts_definition_get_name(def);

  return fts_s_empty_string;
}

void
fts_object_remove_name(fts_object_t *obj)
{
  /* remove definition of named object */
  if(obj->definition != NULL)
  {
    fts_definition_update(obj->definition, fts_null);
    obj->definition = NULL;
  }
}

void
fts_object_add_binding(fts_object_t *obj, fts_definition_t *def)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  fts_atom_t a;

  fts_set_pointer(&a, def);
  data->name_refs = fts_list_prepend(data->name_refs, &a);
}

static void
fts_object_remove_bindings(fts_object_t *obj)
{
  if(obj->patcher_data != NULL)
  {
    fts_list_t *list = obj->patcher_data->name_refs;

    /* remove object as listener from the variables */
    while(list != NULL)
    {
      fts_definition_t *def = (fts_definition_t *)fts_get_pointer(fts_list_get(list));

      fts_definition_remove_listener(def, obj);

      list = fts_list_next(list);
    }
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
*  client
*
*/

void
fts_object_upload(fts_object_t *obj)
{
  fts_object_t *parent = (fts_object_t *)fts_object_get_patcher(obj);

  if(parent != NULL)
  {
    fts_atom_t a;

    fts_client_register_object(obj, -1);

    fts_set_object(&a, obj);
    fts_send_message(parent, fts_s_upload_child, 1, &a);
  }
}

static void
fts_object_reset_client(fts_object_t *obj)
{
  if(fts_object_has_id( obj))
  {
    fts_send_message(obj, fts_s_closeEditor, 0, 0);
    fts_client_release_object(obj);
  }
}

static void
fts_object_upload_connections(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = obj->patcher_data;

  if(data != NULL)
  {
    fts_connection_t *p;
    int i;

    for(i=0; i<fts_object_get_outlets_number(obj); i++)
    {
      for (p = data->out_conn[i]; p ; p = p->next_same_src)
        if(p->type > fts_c_hidden)
          fts_object_upload((fts_object_t *)p);
    }

    for (i=0; i<fts_object_get_inlets_number(obj); i++)
    {
      for (p=data->in_conn[i]; p; p=p->next_same_dst)
        if(p->type > fts_c_hidden)
          fts_object_upload((fts_object_t *)p);
    }
  }
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
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  int i;

  if (data->argc == argc)
    {
      /* Just copy the values, the size is correct */
      for (i = 0; i < argc; i++)
	data->argv[i] = argv[i];
    }
  else
    {
      /* Free the old object description, if any */
      if (data->argv)
	fts_free( data->argv);

      /* reallocate the description if argc > -0 and copy the arguments */
      data->argc = argc;

      if (argc > 0)
	{
	  data->argv = (fts_atom_t *) fts_zalloc(argc * sizeof(fts_atom_t));

	  for (i = 0; i < argc; i++)
	    data->argv[i] = argv[i];
	}
      else
	data->argv = 0;
    }
}


static void
fts_object_remove_description(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = obj->patcher_data;

  if(data != NULL)
  {
    /* free object description */
    if(data->argv != NULL)
      fts_free(data->argv);

    data->argc = 0;
    data->argv = NULL;
  }
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

/*********************************************************************************
*
* object redefinition
*
* fts_object_redefine replace an object with a new
* one whose definition is passed as argument, leaving the same
* connections, properties and id.
*
* If the object is a patcher (either standard patcher or abstraction/template)
* the patcher redefine function is called instead, look in the patcher.c file.
*/

fts_object_t *
fts_object_recompute(fts_object_t *old)
{
  return fts_object_redefine(old, fts_object_get_description_size(old), fts_object_get_description_atoms(old));
}

fts_object_t *
fts_object_redefine(fts_object_t *old, int ac, const fts_atom_t *at)
{
  int old_id = fts_object_get_id( old);

  /* redefine object if not scheduled for deletion */
  if(fts_object_get_status(old) != FTS_OBJECT_STATUS_PENDING_DELETE)
  {
    fts_patcher_t *patcher = fts_object_get_patcher(old);
    fts_symbol_t name = fts_object_get_name(old);
    fts_object_t *new;
    fts_atom_t a;

    /* unbind variables from old object */
    fts_object_remove_name(old);
    fts_object_remove_bindings(old);

    /* create new object */
    new = fts_eval_object_description(patcher, ac, at);

    /* update the loading vm */
    fts_vm_substitute_object(old, new);

    /* remove old from client and update list */
    fts_object_reset_client(old);
    fts_update_reset(old);

    /* if old is an error object, get its name */
    if(fts_object_is_error(old))
      name = fts_error_object_get_name((fts_error_object_t *)old);
    
      /* if new is an error object, assure that there are enough inlets and outlets for the connections */
    if (fts_object_is_error(new))
    {
      fts_error_object_fit_inlet(new, fts_object_get_inlets_number(old) - 1);
      fts_error_object_fit_outlet(new, fts_object_get_outlets_number(old) - 1);
    }

    /* move graphic properties and connections from old to new object */
    fts_object_move_properties(old, new);
    fts_object_move_connections(old, new);

    /* set name of new object */
    if(name != NULL && name != fts_s_empty_string)
    {
      fts_atom_t a;

      fts_set_symbol(&a, name);
      fts_send_message_varargs(new, fts_s_name, 1, &a);
    }

    /* try to rescue the old object state to the new one */
    fts_set_object(&a, old);
    fts_send_message_varargs(new, fts_s_redefine, 1, &a);

    /* remove the old object from the patcher */
    fts_patcher_remove_object(fts_object_get_patcher(old), old);

    /* upload new object if old object was uploaded */
    if(old_id > FTS_NO_ID)
    {
      fts_object_upload(new);
      fts_object_upload_connections(new);
    }

    return new;
  }
  else
  {
    /* unbind variables */
    fts_object_remove_name(old);
    fts_object_remove_bindings(old);
  }

  return NULL;
}
