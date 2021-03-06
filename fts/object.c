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
#include <string.h>

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
fts_object_alloc( fts_class_t *cl)
{
  fts_object_t *obj;

  if(cl->size == 0)
    fts_class_instantiate(cl);
  
  obj = (fts_object_t *)fts_heap_zalloc(cl->heap);
  obj->cl = cl;
  fts_object_set_client_id( obj, FTS_NO_ID);
  fts_object_set_id( obj, FTS_NO_ID);
  fts_object_set_status( obj, FTS_OBJECT_STATUS_CREATE);

  return obj;
}

void
fts_object_free(fts_object_t *obj)
{
  fts_class_t *cl = fts_object_get_class(obj);

  fts_heap_free(obj, cl->heap);
}

static void
fts_object_reset(fts_object_t *obj)
{
  fts_class_t *cl = fts_object_get_class(obj);

  /* call deconstructor */
  if(fts_class_get_deconstructor(cl))
    fts_class_get_deconstructor(cl)(obj, fts_s_delete, 0, 0, fts_nix);
  
  /* release all client components */
  fts_object_reset_client(obj);

  fts_object_set_status( obj, FTS_OBJECT_STATUS_CREATE);
}  

void 
fts_object_snatch(fts_object_t *obj, fts_object_t *replace)
{
  fts_class_t *cl = fts_object_get_class(obj);
  
  /* empty object to be snatched */
  if(fts_object_get_status( obj) != FTS_OBJECT_STATUS_CREATE)
    fts_object_reset(obj);
  
  /* return of the body snatchers */
  memcpy(obj + 1, replace + 1, cl->size - sizeof(fts_object_t));
  
  /* free the snatcher (no return for the body snatcher) */
  fts_object_free(replace);
  
  fts_object_set_status(obj, FTS_OBJECT_STATUS_OK);
}

fts_object_t *
fts_object_create(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = fts_object_alloc(cl);
  
  /* call constructor */
  fts_class_get_constructor(cl)(obj, NULL, ac, at, fts_nix);
  
  if(fts_object_get_status(obj) == FTS_OBJECT_STATUS_INVALID)
  {
    /* destroy invalid object */
    fts_class_get_deconstructor(cl)(obj, NULL, 0, 0, fts_nix);
    fts_object_free(obj);
    
    return NULL;
  }
  
  fts_object_set_status(obj, FTS_OBJECT_STATUS_OK);
  
  return obj;
}

/* delete object */
void
fts_object_destroy(fts_object_t *obj)
{
  fts_object_reset(obj);
  fts_object_free(obj);
}

#ifdef DEBUG_REF_COUNT
void fts_object_refer(fts_object_t *obj)
{
  obj->refcnt++;
  if(fts_object_get_class_name(obj) != NULL)
    post("fts_object_refer %p type %s refcount = %d", obj, fts_symbol_name( fts_object_get_class_name(obj)), obj->refcnt);
}
#endif
/*****************************************************************************
 *
 *  client
 *
 */
void
fts_object_upload(fts_object_t *obj)
{
  if(!fts_object_has_client(obj))
  {
    fts_object_t *container = (fts_object_t *)fts_object_get_container(obj);
    
    if(container != NULL)
    {
      fts_atom_t a;

      if(!fts_object_has_client(container))
        fts_object_upload(container);

      fts_client_register_object(obj, -1);
      
      fts_set_object(&a, obj);
      fts_send_message(container, fts_s_member_upload, 1, &a, fts_nix);
    }
  }
}

void
fts_object_reset_client(fts_object_t *obj)
{
  if(fts_object_has_client(obj))
  {
    fts_send_message(obj, fts_s_closeEditor, 0, 0, fts_nix);
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
    fts_send_message(container, fts_s_member_name, 2, a, fts_nix);
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
    fts_send_message(container, fts_s_member_persistence, 2, a, fts_nix);
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
    fts_send_message(container, fts_s_member_dirty, 1, &a, fts_nix);
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
    fts_send_message(container, fts_s_member_dirty, 2, a, fts_nix);
  }
}

fts_method_status_t
fts_object_name(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_object_t *container = (fts_object_t *)fts_object_get_container(o);
  
  if(container != NULL && ac > 0 && fts_is_symbol(at))
  {
    fts_atom_t a[3];
    
    fts_set_object(a, o);
    a[1] = at[0];
      
    if(ac > 1)
      a[2] = at[1];

    fts_send_message(container, fts_s_member_name, 2 + (ac > 1), a, fts_nix);
  }

  return fts_ok;
}

fts_method_status_t
fts_object_persistence(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_number(at))
    fts_object_set_persistence(o, fts_get_number_int(at));
  
  return fts_ok;
}

/*****************************************************************************
 *
 *  listeneners
 *
 */
void
fts_object_add_listener(fts_object_t *o, void *listener, fts_object_listener_callback_t callback)
{
  fts_object_listener_t *l = (fts_object_listener_t *)fts_malloc(sizeof(fts_object_listener_t));
  
  l->listener = listener;
  l->callback = callback;
  l->next = o->listeners;
  o->listeners = l;
}

void
fts_object_remove_listener(fts_object_t *o, void *listener)
{
  fts_object_listener_t **l = &o->listeners;
  
  while(*l != NULL)
  {
    fts_object_listener_t *x = *l;
    
    if(x->listener == listener)
    {
      *l = x->next;
      fts_free(x);
    }
    else
      l = &(*l)->next;
  }
}

void
fts_object_call_listeners(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_listener_t *l;
  
  for(l=o->listeners; l!=NULL; l=l->next)
    (*l->callback)(o, l->listener, s, ac, at);
}

/*****************************************************************************
 *
 *  import/export
 *
 */

static void
object_imexport_dialog(fts_object_t *o, fts_symbol_t mode, int ac, const fts_atom_t *at)
{
  if(mode == fts_s_import)
    fts_object_open_dialog(o, mode, fts_new_symbol("Open file to import"), NULL, ac, at);
  else if(mode == fts_s_importas)
  {
    fts_symbol_t suffix = fts_get_symbol(at);
    char str[1024];
        
    snprintf(str, 1023, "Open %s file to import", fts_symbol_name(suffix));
    fts_object_open_dialog(o, mode, fts_new_symbol(str), suffix, ac - 1, at + 1);
  }
  else if (mode == fts_s_export)
  { /* export with unknown type */
    fts_object_save_dialog(o, mode, fts_new_symbol("Select file for export"), 
			   NULL, fts_project_get_dir(), 
			   fts_new_symbol("untitled.???"), ac, at);
  }
  else if (mode == fts_s_exportas)
  { /* export with type given as first argument */
    fts_symbol_t suffix = fts_get_symbol(at);
    fts_symbol_t default_name;
    char str[1024];
    
    snprintf(str, 1023, "untitled.%s", fts_symbol_name(suffix));
    default_name = fts_new_symbol(str);
    
    snprintf(str, 1023, "Select file for %s export", fts_symbol_name(suffix));
    fts_object_save_dialog(o, mode, fts_new_symbol(str), suffix, 
			   fts_project_get_dir(), default_name, ac - 1, at + 1);
  }
}


static void
object_imexport(fts_object_t *o, fts_symbol_t mode, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_class_t *cl = fts_object_get_class(o);
  fts_symbol_t suffix = fts_s_default;
  int as = (mode == fts_s_importas || mode == fts_s_exportas);
  int import = (mode == fts_s_import || mode == fts_s_importas);
  
  /* without name or "-" as name: open file chooser */
  if (ac == 0  ||  (as  &&  ac == 1))
  { 
    /* no name given */ 
    object_imexport_dialog(o, mode, ac, at);
  }
  else if(ac > 0 && fts_is_symbol(at + as))
  { /* name given */
    fts_symbol_t name = fts_get_symbol(at + as);
    
    if (name == fts_s_minus)
    { /* name is "-", skip it */
      if (as  &&  ac > 1)
      { /*  keep type for im/exportas:
        at = "type - args...", copy to a = "type args..." */
        fts_atom_t a[64];
        
        a[0] = at[0];
        memcpy(a + 1, at + 2, (ac - 1) * sizeof(fts_atom_t));
        
        object_imexport_dialog(o, mode, ac - 1, a);
      }
      else
        object_imexport_dialog(o, mode, ac - 1, at + 1);
    }
    else
    {
      fts_hashtable_t *hash = (import)? fts_class_get_import_handlers(cl): fts_class_get_export_handlers(cl);
      fts_atom_t k, v;
      
      if(as)
        suffix = fts_get_symbol(at);
      else
      {
        char *str = strrchr((char *)fts_symbol_name(name), '.');
        
        if(str != NULL)
          suffix = fts_new_symbol(str + 1);
      }
      
      fts_set_symbol(&k, suffix);
      if(fts_hashtable_get(hash, &k, &v))
      {        
        fts_method_t meth = (fts_method_t)fts_get_pointer(&v);
        (*meth)(o, suffix, ac - as, at + as, ret);
      }
      else if(suffix != fts_s_default)
      {
        fts_set_symbol(&k, fts_s_default);
        if(fts_hashtable_get(hash, &k, &v))
        {        
          fts_method_t meth = (fts_method_t)fts_get_pointer(&v);
          (*meth)(o, fts_s_default, ac - as, at + as, ret);
        }
        else
          fts_object_error(o, "cannot %s %s data as %s", (import)? "import": "export", fts_symbol_name(fts_class_get_name(cl)), fts_symbol_name(suffix));
      }
      else
        fts_object_error(o, "cannot %s %s data", (import)? "import": "export", fts_symbol_name(fts_class_get_name(cl)));
    }
  }
}

fts_method_status_t 
fts_object_import(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  object_imexport(o, fts_s_import, ac, at, ret);
  
  return fts_ok;
}

fts_method_status_t 
fts_object_export(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  object_imexport(o, fts_s_export, ac, at, ret);
  
  return fts_ok;
}

fts_method_status_t
fts_object_import_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_symbol(at))
    object_imexport(o, fts_s_importas, ac, at, ret);
  else
    fts_object_error(o, "importas: type argument missing");
  
  return fts_ok;
}

fts_method_status_t
fts_object_export_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if(ac > 0 && fts_is_symbol(at))
    object_imexport(o, fts_s_exportas, ac, at, ret);
  else
    fts_object_error(o, "exportas: type argument missing");
  
  return fts_ok;
}
