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

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/object.h>
#include <ftsconfig.h>

/************************************************
 *
 *  message class
 *
 */
fts_class_t *fts_message_class = 0;

void
fts_message_clear(fts_message_t *mess)
{
  mess->s = 0;
  fts_array_clear(&mess->args);
}

void
fts_message_set(fts_message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mess->s = s;
  fts_array_clear(&mess->args);
  fts_array_append(&mess->args, ac, at);
}

void
fts_message_set_from_atoms(fts_message_t *mess, int ac, const fts_atom_t *at)
{
  if(fts_is_symbol(at))
  {
    fts_symbol_t selector = fts_get_symbol(at);
    
    fts_message_set(mess, selector, ac - 1, at + 1);
  }
}

static fts_method_status_t
message_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_message_t *this = (fts_message_t *)o;

  this->s = 0;
  fts_array_init(&this->args, 0, 0);

  /* check arguments */
  if(ac > 0)
  {
    /* first arg is symbol */
    if(fts_is_symbol(at))
    {
      fts_symbol_t selector = fts_get_symbol(at);

      fts_message_set(this, selector, ac - 1, at + 1);
    }
    else
      fts_object_error(o, "first argument must be symbol");
  }
  
  return fts_ok;
}

static fts_method_status_t
message_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_message_t *this = (fts_message_t *)o;

  fts_array_destroy(&this->args);
  
  return fts_ok;
}

static void
message_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_message_t), message_init, message_delete);
}

/*****************************************************************
 *
 *  message cache
 *
 */

static fts_heap_t *message_cache_heap = NULL;

void
fts_message_cache_init(fts_message_cache_t *cache)
{
  cache->selector = NULL;
  cache->type = NULL;
  cache->method = NULL;
}

fts_message_cache_t *
fts_message_cache_new(void)
{
  fts_message_cache_t *cache;
  
  if(message_cache_heap == NULL)
    message_cache_heap = fts_heap_new(sizeof(fts_message_cache_t));
    
  cache = (fts_message_cache_t *)fts_heap_alloc(message_cache_heap);
  fts_message_cache_init(cache);
    
  return cache;
}

void
fts_message_cache_free(fts_message_cache_t *cache)
{
  fts_heap_free(cache, message_cache_heap);
}

/***********************************************************************
 *
 *  message sending API
 *
 */
void
fts_invoke_method(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  (*method)(o, NULL, ac, at, ret);
}

fts_method_t
fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_class_t *cl = fts_object_get_class(o);
  fts_method_t method = NULL;
  fts_class_t *type = NULL;
  
  switch(ac)
  {
    case 0:
      type = fts_void_class;
      break;
    case 1:
      type = fts_get_class(at);
      if(type == fts_void_class)
        ac = 0;
        break;
    default:
      break;
  }
  
  method = fts_class_get_method(cl, s, type);
  
  if(method != NULL)
    (*method)(o, s, ac, at, ret);
  
  return method;
}

fts_method_t
fts_send_message_cached(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret, fts_message_cache_t *cache)
{
  fts_class_t *cl = fts_object_get_class(o);
  fts_method_t method = NULL;
  fts_class_t *type = NULL;
  
  switch(ac)
  {
    case 0:
      type = fts_void_class;
      break;
    case 1:
      type = fts_get_class(at);
      if(type == fts_void_class)
        ac = 0;
        break;
    default:
      break;
  }
  
  if(fts_message_cache_get_selector(cache) == s && fts_message_cache_get_type(cache) == type)
    method = fts_message_cache_get_method(cache);
  else
  {
    method = fts_class_get_method(cl, s, type);
    
    fts_message_cache_set_selector(cache, s);
    fts_message_cache_set_type(cache, type);
    fts_message_cache_set_method(cache, method);
  }
  
  if(method != NULL)
    (*method)(o, s, ac, at, ret);
  
  return method;
}

/***********************************************************************
 *
 * Initialization
 *
 */

FTS_MODULE_INIT(message)
{
  fts_message_class = fts_class_install(fts_s_message, message_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
