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
  fts_object_set_client_id( obj, FTS_NO_ID);
  fts_object_set_id( obj, FTS_NO_ID);
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
  /* call deconstructor */
  if(fts_class_get_deconstructor(fts_object_get_class(obj)))
    fts_class_get_deconstructor(fts_object_get_class(obj))(obj, fts_system_inlet, fts_s_delete, 0, 0);

  /* release all client components */
  fts_object_reset_client(obj);

  /* free memory */
  fts_object_free(obj);
}


/*****************************************************************************
*
*  client
*
*/

void
fts_object_upload(fts_object_t *obj)
{
  if(fts_object_has_client(obj) == 0)
  {
    fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
    
    if(container != NULL)
    {
      fts_atom_t a;
      
      fts_client_register_object(obj, -1);
      
      fts_set_object(&a, obj);
      fts_send_message(container, fts_s_member_upload, 1, &a);
    }
  }
}

void
fts_object_reset_client(fts_object_t *obj)
{
  if(fts_object_has_client(obj))
  {
    fts_send_message(obj, fts_s_closeEditor, 0, 0);
    fts_client_release_object(obj);
  }
}

/*****************************************************************************
*
*  container
*
*/

void
fts_object_set_name(fts_object_t *obj, fts_symbol_t name)
{
  fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
  
  if(container != NULL)
  {
    fts_atom_t a[2];
    
    fts_set_object(a, obj);
    fts_set_symbol(a + 1, name);
    fts_send_message(container, fts_s_member_name, 2, a);
  }
}

void
fts_object_set_persistence(fts_object_t *obj, int persistence)
{
  fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
  
  if(container != NULL)
  {
    fts_atom_t a[2];
    
    fts_set_object(a, obj);
    fts_set_int(a + 1, persistence);
    fts_send_message(container, fts_s_member_persistence, 2, a);
  }
}

void
fts_object_set_dirty(fts_object_t *obj)
{
  fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
  
  if(container != NULL)
  {
    fts_atom_t a;
    
    fts_set_object(&a, obj);
    fts_send_message(container, fts_s_member_dirty, 1, &a);
  }
}

void
fts_object_set_state_dirty(fts_object_t *obj)
{
  fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
  
  if(container != NULL)
  {
    fts_atom_t a[2];
    
    fts_set_object(a, obj);
    fts_set_symbol(a + 1, fts_s_state);
    fts_send_message(container, fts_s_member_dirty, 2, a);
  }
}

void
fts_object_name(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *container = (fts_object_t *)fts_object_get_container(o);
  
  if(container != NULL && ac > 0 && fts_is_symbol(at))
  {
    fts_atom_t a[3];
    
    fts_set_object(a, o);
    a[1] = at[0];
      
    if(ac > 1)
      a[2] = at[1];

    fts_send_message(container, fts_s_member_name, 2 + (ac > 1), a);
  }
}

void
fts_object_persistence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_number(at))
    fts_object_set_persistence(o, fts_get_number_int(at));
}
