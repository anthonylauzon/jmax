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

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/variable.h>

/******************************************************************************
*
* create an object from a class and arguments
*
*/
fts_object_t *
fts_object_new( fts_class_t *cl)
{
  fts_object_t *obj = (fts_object_t *)fts_heap_zalloc(cl->heap);

  obj->cl = cl;
  fts_object_set_id( obj, FTS_NO_ID);
  fts_object_set_client_id( obj, FTS_NO_ID);
  fts_object_set_status( obj, FTS_OBJECT_STATUS_CREATE);

  return obj;
}

void
fts_object_free(fts_object_t *obj)
{
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

/*********************************************************************************
*
*  object name
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

    if(sym != fts_s_empty_string && sym != fts_s_none)
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
    else 
    {
	/* we cleared the name since we have an empty string */
	fts_object_remove_name(obj);
	if(fts_object_has_id(obj))
	{
	    fts_atom_t a;
	    
	    fts_set_symbol(&a, fts_s_empty_string);
	    fts_client_send_message(obj, fts_s_name, 1, &a);
	}
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

void
fts_object_reset_client(fts_object_t *obj)
{
  if(fts_object_has_id( obj))
  {
    fts_send_message(obj, fts_s_closeEditor, 0, 0);
    fts_client_release_object(obj);
  }
}

