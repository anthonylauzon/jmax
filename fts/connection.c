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

/* Note that in this code there are error messages sent as blip;
   this is ok during editing, but the same error may occour while
   loading a patch in an environment where some object changed
   without keeping compatibility; we should send the messages to the 
   console in this case ?*/

/* #define TRACE_DEBUG */

/******************************************************************************/
/*                                                                            */
/*                            Connections                                     */
/*                                                                            */
/******************************************************************************/
  
fts_metaclass_t *fts_connection_type = 0;

fts_connection_t *
fts_connection_new(int id, fts_object_t *src, int woutlet, fts_object_t *dst, int winlet, fts_connection_type_t type)
{
  fts_outlet_decl_t *outlet;
  fts_inlet_decl_t *inlet;
  fts_class_mess_t *mess = 0;
  fts_connection_t *conn;
  int invalid = 0;
  int anything;

  /* first of all, if one of the two object is an error object, add the required inlets/outlets to it */
  if (fts_object_is_error(src))
    {
      /* don't create hidden connections between error objects */
      if(type <= fts_c_hidden)
	return NULL;

      fts_error_object_fit_outlet(src, woutlet);
      invalid = 1;
    }
  
  if (fts_object_is_error(dst))
    {
      /* don't create hidden connections between error objects */
      if(type <= fts_c_hidden)
	return NULL;

      fts_error_object_fit_inlet(dst, winlet);
      invalid = 1;
    }

  /* check the outlet range (should never happen, a part from loading) */
  if (woutlet >= src->head.cl->noutlets || woutlet < 0)
    {
      fts_object_blip(src, "Outlet out of range");
      return NULL;
    }

  /* check againsts double connections */
  { 
    fts_connection_t *p;
    
    for (p = src->out_conn[woutlet]; p ; p = p->next_same_src)
      {
	if ((p->dst == dst) && (p->winlet == winlet))
	  {
	    /* Found, return error message */
	    
	    fts_object_blip(src, "Double connection, cannot connect.");
	    return NULL;
	  }
      }
  }
  
  /* find the outlet and the inlet in the class structure */
  outlet = &src->head.cl->outlets[woutlet];

  if (winlet == fts_SystemInlet)
    inlet = dst->head.cl->sysinlet;
  else if (winlet < dst->head.cl->ninlets && winlet >= 0)
    inlet = &dst->head.cl->inlets[winlet];
  else
    {
      fts_object_blip(src, "Inlet out of range, cannot connect.");
      return NULL;
    }

  if(outlet->tmess.symb)
    {
      mess = fts_class_mess_inlet_get(inlet, outlet->tmess.symb, &anything);

      if(!mess || (anything && outlet->tmess.symb == fts_s_sig && !fts_object_is_thru(dst)))
	{
	  fts_object_blip(src, "Type mismatch, cannot connect");
	  return NULL;
	}
    }

  conn = (fts_connection_t *) fts_object_create(fts_connection_type, 0, 0);

  conn->id = id;
  conn->src = src;
  conn->woutlet = woutlet;
  conn->dst = dst;
  conn->winlet = winlet;

  if(invalid && type > fts_c_hidden)
    conn->type = fts_c_invalid;
  else
    conn->type = type;

  ((fts_object_t *)conn)->patcher = conn->src->patcher;

  /* pre-initialize the cache, if possible */
  if(mess)
    {
      if (anything)
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
  if (! src->out_conn[woutlet])
    {
      conn->next_same_src = 0;
      src->out_conn[woutlet] = conn;
    }
  else
    {
      conn->next_same_src = src->out_conn[woutlet];
      src->out_conn[woutlet] = conn;
    }

  if (! dst->in_conn[winlet])
    {
      conn->next_same_dst = 0;
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
  fts_connection_t **p;		/* indirect precursor */
  fts_connection_t *prev = 0;

  /* First, release the client representation of the connection, if any */
  if ( fts_object_has_id( (fts_object_t *)conn) && conn->type > fts_c_hidden)
    fts_patcher_release_connection((fts_object_t *)conn->src->patcher, conn);

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
  int inlet, outlet;

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
	      fts_connection_new(p->id, new, p->woutlet, p->dst, p->winlet, p->type);
	      fts_connection_delete(p);
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
	      fts_connection_new(p->id, p->src, p->woutlet, new, p->winlet, p->type);
	      fts_connection_delete(p);
	    }
	}
      else
	for (p = old->in_conn[inlet]; p; p = old->in_conn[inlet])
	  fts_connection_delete(p);
    }
}

void 
fts_object_upload_connections(fts_object_t *obj)
{
  int inlet, outlet;
  fts_connection_t *p;

  for (outlet = 0; outlet < obj->head.cl->noutlets; outlet++)
    {
      for (p = obj->out_conn[outlet]; p ; p = p->next_same_src)
	fts_client_upload_object((fts_object_t *)p, -1);
    }

  for (inlet = 0; inlet < obj->head.cl->ninlets; inlet++)
    {
      for (p = obj->in_conn[inlet]; p; p = p->next_same_dst)
	fts_client_upload_object((fts_object_t *)p, -1);
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

void 
fts_connection_set_type(fts_connection_t *connection, fts_connection_type_t type)
{
  if(connection->type != type)
    {
      connection->type = type;
      
      if ( fts_object_has_id( (fts_object_t *)connection))
	fts_patcher_redefine_connection((fts_object_t *)connection->src->patcher, connection);
    }
}

static fts_status_t
connection_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_connection_t), 0, 0, 0); 

  return fts_Success;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_connection_init()
{
  fts_connection_type = fts_class_install(fts_s_connection, connection_instantiate);
}

