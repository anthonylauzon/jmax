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


#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/object.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/patcher.h>

/* #define TRACE_DEBUG */

fts_class_t *fts_connection_class = 0;
fts_symbol_t sym_setType = 0;

static int
connection_check(fts_object_t *src, int woutlet, fts_object_t *dst, int winlet)
{
  fts_iterator_t iter;
  int n_types = fts_class_outlet_get_declarations(fts_object_get_class(src), woutlet, &iter);
  int varargs = 0;

  if(n_types == 0)
  {
    /* no declaration is always good */
    return 1;
  }
  else if(n_types == 1 && fts_class_outlet_has_type(fts_object_get_class(src), woutlet, fts_dsp_signal_class) &&
          fts_class_get_method_varargs(fts_object_get_class(dst), fts_s_propagate_input) == NULL)
  {
    /* pur signal outlet connects to signal inlet or thru object (implements fts_s_propagate_input) only */
    return (fts_class_get_inlet_method(fts_object_get_class(dst), winlet, fts_dsp_signal_class, &varargs) != NULL);
  }
  else
  {
    /* connection to class with declared default handler is always good */
    if(fts_class_get_input_handler(fts_object_get_class(dst)) != NULL)
      return 1;

    while( fts_iterator_has_more( &iter))
    {
      fts_atom_t a;

      fts_iterator_next(&iter, &a);

      if(fts_is_pointer(&a))
      {
        fts_class_t *type = (fts_class_t *)fts_get_pointer(&a);

        /* outlets connects to everything (message or varargs) */
        if(type == NULL)
          return 1;

        /* found inlet type match or varargs method */
        if(fts_class_get_inlet_method(fts_object_get_class(dst), winlet, type, &varargs) != NULL)
          return 1;
      }
    }
  }
      
  return 0; /* no match found */
}

fts_connection_t *
fts_connection_new(fts_object_t *src, int woutlet, fts_object_t *dst, int winlet, fts_connection_type_t type)
{
  fts_patcher_t *patcher = fts_object_get_patcher(src);
  fts_connection_t *conn;
  fts_connection_t **p;
  int valid = 1;

  /* make sure that patcher data is allocated */
  fts_object_get_patcher_data(src);
  fts_object_get_patcher_data(dst);

  /* first of all, if one of the two object is an error object, add the required inlets/outlets to it */
  if (fts_object_is_error(src))
    {
      /* don't create a hidden connection between error objects */
      if(type <= fts_c_hidden)
	return NULL;

      fts_error_object_fit_outlet(src, woutlet);
      valid = 0;
    }
  
  if (fts_object_is_error(dst))
    {
      /* don't create a hidden connection between error objects */
      if(type <= fts_c_hidden)
	return NULL;

      fts_error_object_fit_inlet(dst, winlet);
      valid = 0;
    }

  /* check the outlet range of source (should never happen, apart from loading) */
  if (woutlet >= fts_object_get_outlets_number(src) || woutlet < 0)
  {
    fts_object_error(dst, "outlet out of range");
    return NULL;
  }

  /* check the inlet range of destination (should never happen, apart from loading) */
  if (winlet >= fts_object_get_inlets_number(dst) || winlet < 0)
    {
      fts_object_error(dst, "inlet out of range");
      return NULL;
    }

  /* check source for double connections */
  for (conn = fts_object_get_outlet_connections(src, woutlet); conn ; conn = conn->next_same_src)
  {
    if ((conn->dst == dst) && (conn->winlet == winlet))
    {
      fts_object_error(dst, "double connection ignored");
      return NULL;
    }
  }

  /* check destination for double connections */
  for (conn = fts_object_get_inlet_connections(dst, winlet); conn ; conn = conn->next_same_dst)
  {
    if ((conn->src == src) && (conn->woutlet == woutlet))
    {
      fts_object_error(src, "double connection ignored");
      return NULL;
    }
  }

  /* connections are at least anything between valid objects */
  if(valid && type == fts_c_null) 
    type = fts_c_anything;

  /* check connection */
  if(connection_check(src, woutlet, dst, winlet) == 0)
    {
      fts_object_error(dst, "type mismatch (cannot connect)");
      return NULL;
    }

  /* set patcher */
  if(patcher != fts_object_get_patcher(dst))
    patcher = NULL;

  /* init connection */
  conn = (fts_connection_t *)fts_object_create(fts_connection_class, patcher, 0, 0);
  conn->src = src;
  conn->woutlet = woutlet;
  conn->dst = dst;
  conn->winlet = winlet;
  conn->type = type;
  conn->next_same_src = NULL;
  conn->next_same_dst = NULL;

  /* init cache */
  conn->cache_type = NULL;
  conn->cache_method = NULL;

  /* add connection to source outlet */
  p = &fts_object_get_outlet_connections(src, woutlet);
  conn->next_same_src = *p;
  *p = conn;

  /* add connection to destination inlet */
  p = &fts_object_get_inlet_connections(dst, winlet);
  conn->next_same_dst = *p;
  *p = conn;

  return conn;
}

void 
fts_connection_delete(fts_connection_t *conn)
{
  fts_object_t *src = conn->src;
  fts_object_t *dst = conn->dst;
  int woutlet = conn->woutlet;
  int winlet = conn->winlet;
  fts_connection_t **p;

  /* release the client representation of the connection */
  if (fts_object_has_id( (fts_object_t *)conn) && conn->type > fts_c_hidden)
    fts_client_release_object((fts_object_t *)conn);

  /* look for the connection in the output list of src, and remove it */
  for (p = &fts_object_get_outlet_connections(src, woutlet); *p ; p = &((*p)->next_same_src))
    {
      if ((*p) == conn)
	{
	  *p = (*p)->next_same_src;
	  break;
	}
    }

  /* look for it  in the input list of in, and remove it*/
  for (p = &fts_object_get_inlet_connections(dst, winlet); *p ; p = &((*p)->next_same_dst))
    if ((*p) == conn)
      {
	*p = (*p)->next_same_dst;
	break;
      }
}

fts_connection_t *
fts_connection_get(fts_object_t *src, int woutlet, fts_object_t *dst, int winlet)
{
  fts_connection_t *c;

  if(fts_object_has_patcher_data(src))
  {
    for(c=fts_object_get_outlet_connections(src, woutlet); c; c=c->next_same_src)
    {
      if(c->dst == dst && c->winlet == winlet)
        return c;
    }
  }
  
  return NULL;
}

void 
fts_connection_set_type(fts_connection_t *connection, fts_connection_type_t type)
{
  if(connection->type != type)
    {
      connection->type = type;
      
      if(fts_object_has_id( (fts_object_t *)connection))
	{
	  fts_atom_t a;

	  fts_set_int(&a, type);
	  fts_client_send_message((fts_object_t *)connection, sym_setType, 1, &a);
	}
    }
}

static void
connection_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_connection_t), 0, 0); 
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_connection_init()
{
  fts_connection_class = fts_class_install(fts_s_connection, connection_instantiate);

  sym_setType = fts_new_symbol("setType");
}

