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
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/property.h>
#include <ftsprivate/template.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/variable.h>
#include <ftsprivate/package.h>

/***********************************************************************
*
* error status
*
*/

static fts_status_description_t invalid_object_description_error_description = {
  "invalid object description"
};
static fts_status_t invalid_object_description_error = &invalid_object_description_error_description;

static fts_status_description_t init_message_error_description = {
  "invalid initialization message"
};
static fts_status_t init_message_error = &init_message_error_description;

static fts_status_description_t invalid_template_error_description = {
  "invalid template"
};
static fts_status_t invalid_template_error = &invalid_template_error_description;

static fts_status_description_t unknown_class_error_description = {
  "unknown class"
};
static fts_status_t unknown_class_error = &unknown_class_error_description;

/******************************************************************************
 *
 *  patcher data
 *
 */
fts_heap_t *patcher_data_heap = NULL;

static void patcher_data_remove_description(fts_object_patcher_data_t *data);
static void patcher_data_remove_connections(fts_object_patcher_data_t *data);
static void patcher_data_remove_inoutlets(fts_object_patcher_data_t *data);
static void patcher_data_remove_bindings(fts_object_patcher_data_t *data, fts_object_t *obj);
static void patcher_data_remove_properties(fts_object_patcher_data_t *data);
static void patcher_data_remove_name(fts_object_patcher_data_t *data);

fts_object_patcher_data_t *
fts_object_get_patcher_data(fts_object_t *obj)
{
  if(obj->context == NULL)
  {
    fts_class_t *cl = fts_object_get_class(obj);
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

    obj->context = (void *)data;
  }

  return (fts_object_patcher_data_t *)obj->context;
}

void
fts_object_remove_patcher_data(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;

  if(data != NULL)
  {
    patcher_data_remove_name(data);
    patcher_data_remove_description(data);
    patcher_data_remove_inoutlets(data);
    patcher_data_remove_properties(data);

    /* remove binding to variables */
    patcher_data_remove_bindings(data, obj);

    fts_heap_free((void *)data, patcher_data_heap);
    obj->context = NULL;
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

/***********************************************************************
*
* Utility functions for creating objects
*
*/
static fts_object_t *
create_instance_in_package( fts_package_t *package, fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_status_t *status)
{
  fts_symbol_t class_name;
  fts_object_t *obj = NULL;
  fts_template_t *template;
  fts_class_t *cl;

  class_name = fts_get_symbol( at );
  ac--;
  at++;

  if ((template = fts_package_get_declared_template( package, class_name)) != NULL)
  {
    if((obj = fts_template_make_instance( template, patcher, ac, at)) == NULL)
    {
      *status = invalid_template_error;
      return NULL;
    }

    return obj;
  }

  if ((cl = fts_package_get_class( package, class_name)) != NULL)
  {
    if((obj = fts_object_create_in_patcher( cl, patcher, ac, at)) == NULL)
    {
      *status = fts_status_new(fts_get_error());
      return NULL;
    }

    return obj;
  }

  if ((template = fts_package_get_template_in_path( package, class_name)) != NULL)
  {
    if((obj = fts_template_make_instance( template, patcher, ac, at)) == NULL)
    {
      *status = invalid_template_error;
      return NULL;
    }

    return obj;
  }

  return NULL;
}

static fts_object_t *
create_instance_in_package_or_its_requires( fts_package_t *package, fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_status_t *status)
{
  fts_iterator_t iter;
  fts_object_t *obj;

  if ((obj = create_instance_in_package( package, patcher, ac, at, status)) != NULL)
    return obj;

  if (*status != fts_ok)
    return NULL;

  fts_package_get_required_packages( package, &iter);

  while ( fts_iterator_has_more( &iter))
  {
    fts_atom_t a;
    fts_package_t *required_package;

    fts_iterator_next( &iter, &a);

    required_package = fts_package_get( fts_get_symbol( &a));
    if (required_package == NULL)
      continue;

    if ((obj = create_instance_in_package( required_package, patcher, ac, at, status)) != NULL)
      return obj;

    if (*status != fts_ok)
      return NULL;
  }

  return NULL;
}

static fts_object_t *
object_or_template_create( fts_patcher_t *patcher, int ac, const fts_atom_t *at, fts_status_t *status)
{
  fts_object_t *obj;
  fts_package_t *pkg;

  /* 1) ask kernel package */
  pkg = fts_get_system_package();
  if ((obj = create_instance_in_package( pkg, patcher, ac, at, status)) != NULL)
    return obj;

  if (*status != fts_ok)
    return NULL;

  /* 2) ask the current package and its requires */
  pkg = fts_get_current_package();
  if ((obj = create_instance_in_package_or_its_requires( pkg, patcher, ac, at, status)) != NULL)
    return obj;

  if (*status != fts_ok)
    return NULL;

  /* 3) ask the current project and its requires */
  pkg = fts_project_get();
  if ((obj = create_instance_in_package_or_its_requires( pkg, patcher, ac, at, status)) != NULL)
    return obj;

  *status = unknown_class_error;

  return NULL;
}

/***********************************************************************
*
* evaluate an object description using the expression evaluator
*
*/

struct eval_data {
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
    eval_data->obj = object_or_template_create( eval_data->patcher, ac, at, &status);

    if (status == fts_ok)
    {
      /* keep object in state of creation */
      /* FIXME: see version 1.89, this code was commented out by Patrice to fix
	     error panel */
      fts_object_set_status(eval_data->obj, FTS_OBJECT_STATUS_CREATE);

      /* add object to patcher */
      fts_patcher_add_object( eval_data->patcher, eval_data->obj);

      return fts_ok;
    }

    return status;
  }
  else
  {
    if (ac > 0 && fts_is_symbol(at))
    {
      /* send message to fresh object */
      if (fts_send_message(eval_data->obj, fts_get_symbol(at), ac - 1, at + 1))
      {
        if(fts_object_get_status(eval_data->obj) == FTS_OBJECT_STATUS_INVALID)
          status = fts_status_new(fts_get_error());
      }
      else
        status = init_message_error;
    }

    if (status != fts_ok)
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

  /* FIXME: hack to support bMa2 */
  if (fts_get_symbol( at) == fts_s_colon)
  {
    at++;
    ac--;
  }

  status = fts_expression_new( ac, at, &expression);
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

  if (obj && (fts_object_get_description_size(obj) == 0 || fts_object_is_template(obj)))
    fts_object_set_description( obj, ac, at);

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
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;
  int inlet;

  for (inlet = inlets; inlet < data->n_inlets; inlet++)
  {
    fts_connection_t *p;
    
    for (p = data->in_conn[inlet]; p; p = data->in_conn[inlet])
      fts_connection_delete(p);
    
    data->in_conn[inlet] = NULL;
  }
}


static void
fts_object_trim_outlets_connections(fts_object_t *obj, int outlets)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;
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
patcher_data_remove_inoutlets(fts_object_patcher_data_t *data)
{
  patcher_data_remove_connections(data);
  
  if(data->in_conn)
    fts_free(data->in_conn);
  
  if(data->out_conn)
    fts_free(data->out_conn);
  
  data->in_conn = NULL;
  data->out_conn = NULL;
  
  data->n_inlets = 0;
  data->n_outlets = 0;
}

/*****************************************************************************
*
*  connection handling
*
*/

static void
fts_object_move_connections(fts_object_t *old, fts_object_t *new)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)old->context;
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

/* remove all connections from the object (done when unplugged from the patcher) */
static void
patcher_data_remove_connections(fts_object_patcher_data_t *data)
{
  int outlet, inlet;
  
  /* delete all the survived connections starting in the object */
  for (outlet=0; outlet<data->n_outlets; outlet++)
  {
    fts_connection_t *p;
    
    while ((p = data->out_conn[outlet]))
      fts_connection_delete(p);
  }
  
  /* Delete all the survived connections ending in the object */
  for (inlet=0; inlet<data->n_inlets; inlet++)
  {
    fts_connection_t *p;
    
    while ((p = data->in_conn[inlet]))
      fts_connection_delete(p);
  }
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

static void
fts_object_upload_connections(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;

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
patcher_data_remove_description(fts_object_patcher_data_t *data)
{
  /* free object description */
  if(data->argv != NULL)
    fts_free(data->argv);
  
  data->argc = 0;
  data->argv = NULL;
}

/*********************************************************************************
 *
 *  object name
 *
 */
void
fts_patcher_object_set_name(fts_object_t *obj, fts_symbol_t wanted_name, int global)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  fts_patcher_t *patcher = data->patcher;
  fts_atom_t a;
  
  if(patcher != NULL)
  {
    fts_symbol_t old_name = fts_patcher_object_get_name(obj);
    int old_global = fts_patcher_object_is_global(obj);
    
    /* for situations where empty string doesn't work  */
    if(wanted_name == fts_s_none || wanted_name == fts_s_unnamed)
      wanted_name = fts_s_empty_string;
    
    if(wanted_name == old_name && global == old_global)
      return;

    if(wanted_name != old_name)
    {
      /* reset current definition */
      if(data->definition != NULL)
        fts_definition_update(data->definition, fts_null);
      
      if(wanted_name != fts_s_empty_string)
      {
        fts_patcher_t *scope = fts_patcher_get_scope(patcher);
        fts_symbol_t name = fts_name_get_unused(scope, wanted_name);
        fts_definition_t *definition = fts_definition_get(scope, name);
        
        /* set new definiton */
        fts_set_object(&a, obj);
        fts_definition_update(definition, &a);
        
        /* store definition in object */
        data->definition = definition;
      }
      else
        patcher_data_remove_name(data);
    }

    if(global)
      fts_definition_set_global(data->definition);
    else
      fts_definition_set_local(data->definition);
    
    fts_patcher_set_dirty(patcher, 1);
  
    /* update gui */
    if(fts_object_has_id(obj))
    {
      fts_symbol_t name = fts_patcher_object_get_name(obj);
      int global = fts_patcher_object_is_global(obj);
      fts_atom_t a_name[2];
      
      fts_set_symbol(a_name, name);
      
      if(global != 0)
        fts_set_int(a_name + 1, global);
      
      fts_client_send_message(obj, fts_s_name, 1 + global, a_name);
    }
  }
}

static void
patcher_data_remove_name(fts_object_patcher_data_t *data)
{
  /* remove definition of named object */
  if(data->definition != NULL)
  {
    fts_definition_update(data->definition, fts_null);
    data->definition = NULL;
  }    
}

fts_symbol_t
fts_patcher_object_get_name(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;
  
  if(data != NULL && data->definition != NULL)
    return fts_definition_get_name(data->definition);
  else
    return fts_s_empty_string;
}

int
fts_patcher_object_is_global(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;
  
  if(data != NULL && data->definition != NULL)
    return fts_definition_is_global(data->definition);
  
  return 0;
}

/*********************************************************************************
*
*  object reference bindings
*
*/
void
fts_patcher_object_add_binding(fts_object_t *obj, fts_definition_t *def)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  fts_atom_t a;
  
  fts_set_pointer(&a, def);
  data->name_refs = fts_list_prepend(data->name_refs, &a);
}

static void
patcher_data_remove_bindings(fts_object_patcher_data_t *data, fts_object_t *obj)
{
  fts_list_t *list = data->name_refs;
  
  /* remove object as listener from the variables */
  while(list != NULL)
  {
    fts_definition_t *def = (fts_definition_t *)fts_get_pointer(fts_list_get(list));
    
    fts_definition_remove_listener(def, obj);
    
    list = fts_list_next(list);
  }
}

/*****************************************************************************
 *
 *  persistence
 *
 */

void
fts_patcher_object_set_state_persistence(fts_object_t *obj, int persistence)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  
  data->persistence = persistence;

  fts_patcher_set_dirty(data->patcher, 1);

  /* update gui */
  if(fts_object_has_id(obj))
  {
    fts_atom_t a;
    
    fts_set_int(&a, data->persistence);    
    fts_client_send_message(obj, fts_s_persistence, 1, &a);
  }        
}

void
fts_patcher_object_set_state_dirty(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  
  if(data->persistence > 0)
    fts_patcher_set_dirty(data->patcher, 1);
}

void
fts_patcher_object_set_dirty(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);
  
  fts_patcher_set_dirty(data->patcher, 1);
}

int
fts_patcher_object_is_persistent(fts_object_t *obj)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;
  
  if(data != NULL && data->persistence > 0)
    return 1;
  
  return 0;
}

/*****************************************************************************
*
*  basic property handling
*
*/

/* property lists */
static fts_heap_t *plist_cell_heap = NULL;

static void
plist_free(fts_plist_cell_t **plist)
{
  fts_plist_cell_t *c, *next;

  for(c = *plist; c; c = next)
  {
    next = c->next;
    fts_heap_free((char *)c, plist_cell_heap);
  }

  *plist = NULL;
}

static const fts_atom_t *
plist_get(fts_plist_cell_t **plist, fts_symbol_t property)
{
  fts_plist_cell_t *c;

  for(c = *plist; c; c = c->next)
    if (c->property == property)
      return &(c->value);

  return 0;
}

static void
plist_put(fts_plist_cell_t **plist, fts_symbol_t property, const fts_atom_t *value)
{
  fts_plist_cell_t *prev, *current, *insert;

  /* See if the property is already there */
  for(current = *plist; current; current = current->next)
  {
    if(current->property == property)
    {
      current->value = *value;
      return;
    }
  }
  
  /* It is not there, add a new list entry */
  prev = 0;
  for(current = *plist; current; current = current->next)
    prev = current;

  if(plist_cell_heap == NULL)
    plist_cell_heap = fts_heap_new(sizeof(fts_plist_cell_t));

  insert = (fts_plist_cell_t *) fts_heap_alloc(plist_cell_heap);

  insert->property =  property;
  insert->value = *value;
  insert->next = 0;

  if (prev)
    prev->next = insert;
  else
    *plist = insert;
}


static void
plist_remove(fts_plist_cell_t **plist, fts_symbol_t property)
{
  fts_plist_cell_t *prev, *current, *next;
  int found;

  prev = 0;
  found = 0;
  for(current = *plist; current; current = next)
  {
    next = current->next;
    if(current->property == property)
    {
      if(prev)
        prev->next = next;
      else
        *plist = next;

      fts_heap_free((char *)current, plist_cell_heap);
    }
    else
      prev = current;
  }
}

/* object property handling */
static void
patcher_data_remove_properties(fts_object_patcher_data_t *data)
{
  if(data->properties)
    plist_free(&data->properties);
}

static void
object_move_property(fts_object_t *old, fts_object_t *new, fts_symbol_t name)
{
  fts_atom_t a;

  fts_object_get_prop(old, name, &a);

  if (! fts_is_void(&a))
    fts_object_put_prop(new, name, &a);
}

static void
object_move_properties(fts_object_t *old, fts_object_t *new)
{
  /* copy only the editor properties here, not the others !!! */
  if (fts_object_is_standard_patcher(old))
  {
    fts_patcher_set_wx((fts_patcher_t *)new, fts_patcher_get_wx((fts_patcher_t *)old));
    fts_patcher_set_wy((fts_patcher_t *)new, fts_patcher_get_wy((fts_patcher_t *)old));
    fts_patcher_set_ww((fts_patcher_t *)new, fts_patcher_get_ww((fts_patcher_t *)old));
    fts_patcher_set_wh((fts_patcher_t *)new, fts_patcher_get_wh((fts_patcher_t *)old));
  }

  object_move_property(old, new, fts_s_x);
  object_move_property(old, new, fts_s_y);
  object_move_property(old, new, fts_s_height);
  object_move_property(old, new, fts_s_width);
  object_move_property(old, new, fts_s_font);
  object_move_property(old, new, fts_s_fontSize);
  object_move_property(old, new, fts_s_fontStyle);
}

void
fts_object_put_prop(fts_object_t *obj, fts_symbol_t property, const fts_atom_t *value)
{
  if(!fts_is_void(value) && !fts_send_message(obj, property, 1, value))
  {
    fts_object_patcher_data_t *data = fts_object_get_patcher_data(obj);

    plist_put(&data->properties, property, value);
  }
}

void
fts_object_get_prop(fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;

  if(data != NULL)
  {
    const fts_atom_t *ret = 0;

    if(data->properties)
      ret = plist_get(&data->properties, property);

    if(ret)
      *value = *ret;
    else
      fts_set_void(value);
  }
}

void
fts_object_remove_prop(fts_object_t *obj, fts_symbol_t property)
{
  fts_object_patcher_data_t *data = (fts_object_patcher_data_t *)obj->context;

  if(data != NULL)
  {
    if(data->properties)
      plist_remove(&data->properties, property);
  }
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

/*************************************************************************************
*
*  open/save Dialog
*
*/
void
fts_object_open_dialog(fts_object_t *o, fts_symbol_t callback, fts_symbol_t prompt)
{
	fts_atom_t a[2];
	
	fts_set_symbol(a, callback);
	fts_set_symbol(a + 1, prompt);
	fts_client_send_message(o, fts_s_openFileDialog, 2, a);
}

void
fts_object_save_dialog(fts_object_t *o, fts_symbol_t callback, fts_symbol_t prompt, fts_symbol_t path, fts_symbol_t default_name)
{
	fts_atom_t a[4];
	
	fts_set_symbol(a, callback);
	fts_set_symbol(a + 1, prompt);
	fts_set_symbol(a + 2, path);
	fts_set_symbol(a + 3, default_name);

	fts_client_send_message(o, fts_s_openFileDialog, 4, a);
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
  fts_object_patcher_data_t *old_data = fts_object_get_patcher_data(old);
  int old_id = fts_object_get_id( old);

  /* redefine object if not scheduled for deletion */
  if(fts_object_get_status(old) != FTS_OBJECT_STATUS_PENDING_DELETE)
  {
    fts_patcher_t *patcher = fts_object_get_patcher(old);
    fts_symbol_t name = fts_patcher_object_get_name(old);
    fts_object_t *new;
    fts_atom_t a;

    /* unbind variables from old object */
    patcher_data_remove_name(old_data);
    patcher_data_remove_bindings(old_data, old);

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
    object_move_properties(old, new);
    fts_object_move_connections(old, new);

    /* set name of new object */
    if(name != fts_s_empty_string)
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
    /* unbind name and bindings */
    patcher_data_remove_name(old_data);
    patcher_data_remove_bindings(old_data, old);
  }

  return NULL;
}
