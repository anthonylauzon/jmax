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
#include <fts/private/class.h>
#include <fts/private/connection.h>

/* Note that in this code there are error messages sent as blip;
   this is ok during editing, but the same error may occour while
   loading a patch in an environment where some object changed
   without keeping compatibility; we should send the messages to the 
   console in this case ?*/

/* #define TRACE_DEBUG */

static fts_heap_t *connection_heap;

/******************************************************************************/
/*                                                                            */
/*                            Connections                                     */
/*                                                                            */
/******************************************************************************/

fts_connection_t *
fts_connection_new(int id, fts_object_t *out, int woutlet, fts_object_t *in, int winlet)
{
  fts_outlet_decl_t *outlet;
  fts_inlet_decl_t *inlet;
  fts_class_mess_t *mess = 0;
  fts_connection_t *conn;
  int invalid = 0;
  int anything;

  /* first of all, if one of the two object is an error object, add the required inlets/outlets to it */
  if (fts_object_is_error(out))
    {
      fts_error_object_fit_outlet(out, woutlet);
      invalid = 1;
    }

  if (fts_object_is_error(in))
    {
      fts_error_object_fit_inlet(in, winlet);
      invalid = 1;
    }

  /* check the outlet range (should never happen, a part from loading) */
  if (woutlet >= out->head.cl->noutlets || woutlet < 0)
    {
      fts_object_blip(out, "Outlet out of range");
      return 0;
    }

  /* check againsts double connections */
  { 
    fts_connection_t *p;

    for (p = out->out_conn[woutlet]; p ; p = p->next_same_src)
    {
      if ((p->dst == in) && (p->winlet == winlet))
	{
	  /* Found, return error message */

	  fts_object_blip(out, "Double connection, cannot connect.");
	  return 0;
	}
    }
  }

  /* find the outlet and the inlet in the class structure */
  outlet = &out->head.cl->outlets[woutlet];

  if (winlet == fts_SystemInlet)
    inlet = in->head.cl->sysinlet;
  else if (winlet < in->head.cl->ninlets && winlet >= 0)
    inlet = &in->head.cl->inlets[winlet];
  else
    {
      fts_object_blip(out, "Inlet out of range, cannot connect.");
      return 0;
    }

  if(outlet->tmess.symb)
    {
      mess = fts_class_mess_inlet_get(inlet, outlet->tmess.symb, &anything);

      if(!mess || (anything && outlet->tmess.symb == fts_s_sig && !fts_object_is_thru(in)))
	{
	  fts_object_blip(out, "Type mismatch, cannot connect");
	  return 0;
	}
    }

  conn = (fts_connection_t *) fts_heap_alloc(connection_heap);

  conn->id  = id;
  conn->src = out;
  conn->woutlet = woutlet;
  conn->dst = in;
  conn->winlet = winlet;

  if(invalid)
    conn->type = fts_c_invalid;
  else
    conn->type = fts_c_anything;

  if (id != FTS_NO_ID)
    fts_connection_table_put(id, conn);

  /* pre-initialize the cache, if possible */

  if (mess)
    {
      if (fts_mess_get_run_time_check())
	{
	  conn->symb = 0;
	  conn->mth  = 0;
	}
      else if (anything)
	{
	  /* we found an anything method at the inlet:
	     we cache the method for anything here because, since the
	     outlet is typed, we are sure we will always call the anything method
	     for this type */

	  conn->symb = 0;
	  conn->mth  = mess->mth;
	}
      else
	{
	  conn->symb = mess->tmess.symb;
	  conn->mth  = mess->mth;
	}
    }
  else
    {
      conn->symb = 0;
      conn->mth  = 0;
    }

  /* add the connection to the outlet list and to the inlet list  */
  if (! out->out_conn[woutlet])
    {
      conn->next_same_src = 0;
      out->out_conn[woutlet] = conn;
    }
  else
    {
      conn->next_same_src = out->out_conn[woutlet];
      out->out_conn[woutlet] = conn;
    }

  if (! in->in_conn[winlet])
    {
      conn->next_same_dst = 0;
      in->in_conn[winlet] = conn;
    }
  else
    {
      conn->next_same_dst = in->in_conn[winlet];
      in->in_conn[winlet] = conn;
    }

  return conn;
}

static void 
fts_object_do_disconnect(fts_connection_t *conn, int do_client)
{ 
  fts_object_t *src;
  fts_object_t *dst;
  fts_connection_t **p;		/* indirect precursor */
  fts_connection_t *prev = 0;

  /* First, release the client representation of the connection, if any */
  if (do_client)
    {
      if (conn->id != FTS_NO_ID)
	fts_client_release_connection(conn);
    }

  src = conn->src;
  dst  = conn->dst;

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

  /* Unregister the connection */
  if (do_client && conn->id != FTS_NO_ID)
    fts_connection_table_remove(conn->id);

  /* Free the connection, and return */
  fts_heap_free((char *) conn, connection_heap);
}

void fts_connection_delete(fts_connection_t *conn)
{
  fts_object_do_disconnect(conn, 1);
}

static void 
fts_connection_delete_ignore_id(fts_connection_t *conn)
{
  fts_object_do_disconnect(conn, 0);
}

/*   
 * This function move the connection of the object old to
 * the object new; it delete the connections that are
 * no more pertinent; if the connection have an ID, it
 * is kept.
 */
void 
fts_object_move_connections(fts_object_t *old, fts_object_t *new, int do_client)
{
  int inlet, outlet;
  fts_atom_t at[1];
  fts_patcher_t *patcher;
  int id;

  /* reproduce in new, and delete in old,  
     all the old outgoing connections */

  for (outlet = 0; outlet < old->head.cl->noutlets; outlet++)
    {
      fts_connection_t *p;

      /* The loop work by iterating on the first connection;
	 this work because the loop destroy one connection at a time.
	 */

      if (outlet < fts_object_get_outlets_number(new))
	{	
	  for (p = old->out_conn[outlet]; p ;  p = old->out_conn[outlet])
	    {
	      fts_connection_t *new_c;

	      new_c = fts_connection_new(p->id, new, p->woutlet, p->dst, p->winlet);

	      if (new_c)
		{
		  fts_connection_delete_ignore_id(p);

		  /* Redefine the connection on the client side if needed */
		  if (do_client && (new_c->id != FTS_NO_ID))
		    fts_client_redefine_connection(new_c);
		}
	      else
		{
		  /* we got an error in redoing the connection, simply throw the old one away */
		  fts_connection_delete(p);
		}
	    }
	}
      else
	for (p = old->out_conn[outlet]; p ;  p = old->out_conn[outlet])
	  fts_connection_delete(p);

    }

  /* reproduce in new, and delete in old, all the old incoming connections */
  for (inlet = 0; inlet < old->head.cl->ninlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      if (inlet < fts_object_get_inlets_number(new))
	{
	  for (p = old->in_conn[inlet]; p; p = old->in_conn[inlet])
	    {
	      fts_connection_t *new_c;

	      new_c = fts_connection_new(p->id, p->src, p->woutlet, new, p->winlet);
	      
	      if (new_c)
		{
		  fts_connection_delete_ignore_id(p);

		  /* Redefine the connection on the client side if needed */

		  if (do_client && (new_c->id != FTS_NO_ID))
		    fts_client_redefine_connection(new_c);
		}
	      else
		{
		  /* we got an error in redoing the connection,
		     simply throw the old one away */

		  fts_connection_delete(p);
		}
	    }
	}
      else
	for (p = old->in_conn[inlet]; p; p = old->in_conn[inlet])
	  fts_connection_delete(p);
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


  for (inlet = inlets; inlet < obj->head.cl->ninlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      for (p = obj->in_conn[inlet]; p; p = obj->in_conn[inlet])
	fts_connection_delete(p);
    }
}


void 
fts_object_trim_outlets_connections(fts_object_t *obj, int outlets)
{
  int outlet;
  fts_patcher_t *patcher;


  for (outlet = outlets; outlet < obj->head.cl->noutlets; outlet++)
    {
      fts_connection_t *p;

      /* The loop work by iterating on the first connection;
	 this work because the loop destroy one connection at a time.
	 */

      for (p = obj->out_conn[outlet]; p ;  p = obj->out_conn[outlet])
	fts_connection_delete(p);
    }
}

/* (debug) print a connection as:
 * <CONNECTION fromId.outlet toId.inlet #id>
 */
void 
fprintf_connection(FILE *f, fts_connection_t *conn)
{
  if (conn != 0)
    fprintf(f, "<CONNECTION %d.%d %d.%d #%d>",
	    conn->src->head.id, conn->woutlet, conn->dst->head.id, conn->winlet, conn->id);
  else
    fprintf(f, "<CONNECTION null>");
}

void 
fts_connection_set_type(fts_connection_t *connection, fts_connection_type_t type)
{
  if(connection->type != type)
    {
      connection->type = type;
      
      if (connection->id != FTS_NO_ID)
	fts_client_redefine_connection(connection);
    }
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_connection_init()
{
  connection_heap = fts_heap_new(sizeof(fts_connection_t));
}

