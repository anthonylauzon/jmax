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
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/outlet.h>
#include <ftsconfig.h>

int fts_objstack_top = 0;
fts_object_t *fts_objstack[FTS_OBJSTACK_SIZE];

/************************************************
 *
 *  outlet utils
 *
 */
#define INVOKE(m, o, i, s, n, a) \
do { \
  FTS_OBJSTACK_PUSH(o); \
    (*m)((o), (i), (s), (n), (a)); \
      FTS_OBJSTACK_POP(o); \
} while(0)

static int
check_outlet(fts_object_t *o, int woutlet)
{
  if (woutlet >= fts_object_get_outlets_number(o) || woutlet < 0)
  {
    fts_object_error(o, "outlet (%d) out of range", woutlet);
    return 0;
  }
  else
    return 1;
}

/*****************************************************************
 *
 *   outlet sending
 *
 */

/* output single atom or bang */
static void
outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t *at)
{
  if(check_outlet(o, woutlet))
  {
    fts_connection_t *conn = fts_object_get_outlet_connections(o, woutlet);
    
    if (!FTS_REACHED_MAX_CALL_DEPTH())
    {
      fts_class_t *type = fts_get_class(at);
      
      while(conn)
      {
        fts_object_t *dst = fts_connection_get_destination(conn);
        int winlet = fts_connection_get_inlet(conn);
        fts_class_t *cl = fts_object_get_class(dst);
        fts_method_t handler = fts_class_get_input_handler(fts_object_get_class(dst));

        fts_object_set_message_inlet(dst, winlet);

        if(handler != NULL)
          (*handler)(dst, NULL, !fts_is_void(at), at, fts_nix);
        else
        {
          fts_method_t method = fts_message_cache_get_method(&conn->cache);
          
          if(fts_message_cache_get_selector(&conn->cache) != NULL || fts_message_cache_get_type(&conn->cache) != type)
          {
            method = fts_class_get_inlet_method(cl, winlet, type);
            
            if(method != NULL)
            {
              fts_message_cache_set_selector(&conn->cache, NULL);
              fts_message_cache_set_type(&conn->cache, type);
              fts_message_cache_set_method(&conn->cache, method);
            }
            else
            {
              fts_object_error(dst, "no %s method for inlet %d", fts_class_get_name(type), winlet);
              conn = fts_connection_get_next_of_same_source(conn);
              continue;
            }
          }
          
          INVOKE(method, dst, NULL, !fts_is_void(at), at, fts_nix);
        }
        
        conn = fts_connection_get_next_of_same_source(conn);
      }
    }
    else
      fts_object_error(o, "message stack overflow at outlet %d", woutlet);
  }
}

/* output varargs */
static void
outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at)
{
  if(check_outlet(o, woutlet))
  {
    fts_connection_t *conn = fts_object_get_outlet_connections(o, woutlet);
    
    if(!FTS_REACHED_MAX_CALL_DEPTH())
    {
      while(conn)
      {
        fts_object_t *dst = fts_connection_get_destination(conn);
        int winlet = fts_connection_get_inlet(conn);
        fts_class_t *cl = fts_object_get_class(dst);
        fts_method_t handler = fts_class_get_input_handler(fts_object_get_class(dst));
        
        fts_object_set_message_inlet(dst, winlet);
        
        if(handler != NULL)
          (*handler)(dst, NULL, ac, at, fts_nix);
        else
        {
          fts_symbol_t selector = fts_message_cache_get_selector(&conn->cache);
          fts_class_t *type = fts_message_cache_get_type(&conn->cache);
          fts_method_t method = fts_message_cache_get_method(&conn->cache);
          
          if(selector != NULL || method == NULL || type != NULL)
          {
            type = NULL;
            method = fts_class_get_inlet_method(cl, winlet, NULL);
            
            if(method != NULL)
            {
              fts_message_cache_set_selector(&conn->cache, NULL);
              fts_message_cache_set_type(&conn->cache, type);
              fts_message_cache_set_method(&conn->cache, method);
            }
            else
            {
              fts_object_error(dst, "no varargs method at inlet %d", winlet); 
              conn = fts_connection_get_next_of_same_source(conn);
              continue;
            }
          }
          
          INVOKE(method, dst, NULL, ac, at, fts_nix);
        }
        
        conn = fts_connection_get_next_of_same_source(conn);
      }
    }
    else
      fts_object_error(o, "message stack overflow at outlet %d", woutlet);
  }
}

/*****************************************************************
 *
 *  outlet atoms
 * 
 */ 

void
fts_outlet_bang(fts_object_t *o, int woutlet)
{
  outlet_atom(o, woutlet, fts_null);
}

void 
fts_outlet_int(fts_object_t *o, int woutlet, int n)
{
  fts_atom_t a;
  
  fts_set_int(&a, n);
  outlet_atom(o, woutlet, &a);
}

void 
fts_outlet_float(fts_object_t *o, int woutlet, double f)
{
  fts_atom_t a;
  
  fts_set_float(&a, f);
  outlet_atom(o, woutlet, &a);
}

void 
fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
{
  fts_atom_t a;
  
  fts_set_symbol(&a, s);
  outlet_atom(o, woutlet, &a);
}

void
fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* at)
{
  outlet_atom(o, woutlet, at);
}

void
fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
{
  fts_atom_t a;

  fts_set_object(&a, obj);
  outlet_atom(o, woutlet, &a);
}

void
fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
{
  switch(ac)
  {
    case 0:
      outlet_atom(o, woutlet, fts_null);
      break;
    case 1:
      outlet_atom(o, woutlet, at);
      break;
    default:
      outlet_varargs(o, woutlet, ac, at);
  }
}

/***************************************************
 *
 *  outlet messages
 *
 */
void
fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(check_outlet(o, woutlet))
  {
    fts_connection_t *conn = fts_object_get_outlet_connections(o, woutlet);
    
    if (!FTS_REACHED_MAX_CALL_DEPTH())
    {
      while(conn)
      {
        fts_object_t *dst = fts_connection_get_destination(conn);
        fts_method_t handler = fts_class_get_input_handler(fts_object_get_class(dst));
        int winlet = fts_connection_get_inlet(conn);
        
        fts_object_set_message_inlet(dst, winlet);
        
        if(handler != NULL)
          (*handler)(dst, s, ac, at, fts_nix);
        else if(fts_send_message_cached(dst, s, ac, at, fts_nix, &conn->cache) == NULL)
          fts_object_error(dst, "no method for message %s with given argument(s)", s);
        
        conn = fts_connection_get_next_of_same_source(conn);
      }
    }
    else
      fts_object_error(o, "message stack overflow at outlet %d", woutlet);
  }
}

void
fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(s == 0)
    fts_outlet_varargs(o, woutlet, ac, at);
  else
    fts_outlet_message(o, woutlet, s, ac, at);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
