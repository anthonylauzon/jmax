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

  if(n_types == 0)
    {
      /* no declaration is always good */
      return 1;
    }
  else if(n_types == 1 && fts_class_outlet_has_type(fts_object_get_class(src), woutlet, fts_dsp_signal_class) &&
	  fts_class_get_method(fts_object_get_class(dst), fts_s_propagate_input) == NULL)
    {
      /* pur signal outlet connects to signal inlet or thru object (implements fts_s_propagate_input) only */
      return (fts_class_inlet_get_method(fts_object_get_class(dst), winlet, fts_dsp_signal_class) != NULL);
    }
  else 
    {
      /* connection to class with declared default handler is always good */
      if(fts_class_get_default_handler(fts_object_get_class(dst)) != fts_class_default_error_handler)
	return 1;

      while( fts_iterator_has_more( &iter)) 
	{
	  fts_atom_t a;

	  fts_iterator_next(&iter, &a);

	  if(fts_is_pointer(&a))
	    {
	      fts_class_t *class = (fts_class_t *)fts_get_pointer(&a);

	      /* varargs connects to everything (we could be more severe here) */
	      if(class == NULL)
		return 1; 
	      
	      /* found inlet type match or varargs method */
	      if(fts_class_inlet_get_method(fts_object_get_class(dst), winlet, class) != NULL)
		return 1;
	      else if(fts_class_inlet_get_method(fts_object_get_class(dst), winlet, NULL) != NULL)
		return 1;
	    }	
	  else if(fts_is_symbol(&a))
	    {
	      fts_symbol_t selector = fts_get_symbol(&a);

	      if(fts_class_get_method(fts_object_get_class(dst), selector) != NULL)
		return 1; /* found message match */
	    }
	}
    }
      
  return 0; /* no match found */
}

fts_connection_t *
fts_connection_new(fts_object_t *src, int woutlet, fts_object_t *dst, int winlet, fts_connection_type_t type)
{
  int valid = 1;
  fts_connection_t *conn;
  fts_connection_t *p;

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

  /* check the inlet range (should never happen, a part from loading) */
  if (winlet >= fts_object_get_inlets_number(dst) || winlet < 0)
    {
      fts_object_signal_runtime_error(dst, "inlet out of range");
      return NULL;
    }

  /* check the outlet range (should never happen, a part from loading) */
  if (winlet >= fts_object_get_inlets_number(dst) || winlet < 0)
    {
      fts_object_signal_runtime_error(dst, "inlet out of range");
      return NULL;
    }

  /* check the outlet range (should never happen, a part from loading) */
  if (woutlet >= fts_object_get_outlets_number(src) || woutlet < 0)
    {
      fts_object_signal_runtime_error(src, "outlet out of range");
      return NULL;
    }

  /* check for double connections */
  for (p = src->out_conn[woutlet]; p ; p = p->next_same_src)
    {
      if ((p->dst == dst) && (p->winlet == winlet))
	{
	  fts_object_signal_runtime_error(dst, "double connection ignored");
	  return NULL;
	}
    }
  
  /* connections are at least anything between valid objects */
  if(valid && type == fts_c_null) 
    type = fts_c_anything;

  /* check connection */
  if(connection_check(src, woutlet, dst, winlet) == 0)
    {
      fts_object_signal_runtime_error(dst, "type mismatch (cannot connect)");
      return NULL;
    }

  /* init connection */
  conn = (fts_connection_t *)fts_object_create(fts_connection_class, NULL, 0, 0);
  conn->src = src;
  conn->woutlet = woutlet;
  conn->dst = dst;
  conn->winlet = winlet;
  conn->type = type;

  /* set patcher */
  ((fts_object_t *)conn)->patcher = conn->src->patcher;

  /* init cache */
  conn->selector = NULL;
  conn->class = NULL;
  conn->method = NULL;

  /* add the connection to the outlet list and to the inlet list  */
  if (src->out_conn[woutlet] == NULL)
    {
      conn->next_same_src = NULL;
      src->out_conn[woutlet] = conn;
    }
  else
    {
      conn->next_same_src = src->out_conn[woutlet];
      src->out_conn[woutlet] = conn;
    }

  if (dst->in_conn[winlet] == NULL)
    {
      conn->next_same_dst = NULL;
      dst->in_conn[winlet] = conn;
    }
  else
    {
      conn->next_same_dst = dst->in_conn[winlet];
      dst->in_conn[winlet] = conn;
    }

  return conn;
}

void 
fts_connection_delete(fts_connection_t *conn)
{
  fts_object_t *src;
  fts_object_t *dst;
  fts_connection_t **p; /* indirect precursor */

  /* release the client representation of the connection */
  if (fts_object_has_id( (fts_object_t *)conn) && conn->type > fts_c_hidden)
    fts_client_release_object((fts_object_t *)conn);

  src = conn->src;
  dst = conn->dst;

  /* look for the connection in the output list of src, and remove it */
  for (p = &src->out_conn[conn->woutlet]; *p ; p = &((*p)->next_same_src))
    {
      if ((*p) == conn)
	{
	  *p = (*p)->next_same_src;
	  break;
	}
    }

  /* look for it  in the input list of in, and remove it*/
  for (p = &dst->in_conn[conn->winlet]; *p ; p = &((*p)->next_same_dst))
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

  for(c=src->out_conn[woutlet]; c; c=c->next_same_src)
    {
      if(c->dst == dst && c->winlet == winlet)
	return c;
    }

  return NULL;
}

/*   
 * This function move the connection of the object old to
 * the object new; it delete the connections that are
 * no more pertinent; if the connection have an ID, it
 * is kept.
 */
void 
fts_object_move_connections(fts_object_t *old, fts_object_t *new)
{
  fts_connection_t *p;
  int i;

  for (i=0; i<fts_object_get_outlets_number(old); i++)
    {
      fts_connection_t *p;

      for (p=old->out_conn[i]; p;  p=old->out_conn[i])
	{
	  if(i < fts_object_get_outlets_number(new) && p->type > fts_c_hidden)
	    fts_connection_new(new, p->woutlet, p->dst, p->winlet, p->type);
	  
	  fts_connection_delete(p);
	}
    }

  for (i=0; i<fts_object_get_inlets_number(old); i++)
    {
      for (p=old->in_conn[i]; p; p=old->in_conn[i])
	{
	  if(i < fts_object_get_inlets_number(new) && p->type > fts_c_hidden)
	    fts_connection_new(p->src, p->woutlet, new, p->winlet, p->type);
	  
	  fts_connection_delete(p);
	}
    }
}

/*   
 * Assuming that the number of inlets or outlets
 * of the object will become as specified by the arguments,
 * delete all the connections that will be not pertinent
 * anymore; tell the client also !!
 */
void 
fts_object_trim_inlets_connections(fts_object_t *obj, int inlets)
{
  int inlet;


  for (inlet = inlets; inlet < fts_object_get_inlets_number(obj); inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      for (p = obj->in_conn[inlet]; p; p = obj->in_conn[inlet])
	fts_connection_delete(p);

      obj->in_conn[inlet] = NULL; 
    }
}


void 
fts_object_trim_outlets_connections(fts_object_t *obj, int outlets)
{
  int outlet;

  for (outlet = outlets; outlet < fts_object_get_outlets_number(obj); outlet++)
    {
      fts_connection_t *p;

      /* The loop work by iterating on the first connection;
	 this work because the loop destroy one connection at a time.
	 */

      for (p = obj->out_conn[outlet]; p ;  p = obj->out_conn[outlet])
	fts_connection_delete(p);


      obj->out_conn[outlet] = NULL; 
    }
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

