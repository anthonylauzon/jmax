/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.16 $ IRCAM $Date: 1998/04/29 14:44:38 $
 *
 *  Eric Viara for Ircam, January 1995
 */

#include <stdio.h>		/* TMp @@@ */


#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"


extern void fprintf_atoms(FILE *f, int ac, const fts_atom_t *at); /* @@@ */
static fts_heap_t connection_heap;

#ifdef DEBUG 
#define INIT_CHECK_STATUS 1
#else
#define INIT_CHECK_STATUS 0
#endif

static long fts_mess_run_time_check = INIT_CHECK_STATUS;


/* Return Status */

fts_status_description_t fts_MethodNotFound = {"method not found"};
fts_status_description_t fts_ObjectCannotConnect = {"object cannot connect"};
fts_status_description_t fts_NoSuchConnection = {"no such connection"};
fts_status_description_t fts_ArgumentMissing = {"argument missing"};
fts_status_description_t fts_ArgumentTypeMismatch = {"argument type mismatch"};
fts_status_description_t fts_ExtraArguments = {"extra arguments"};
fts_status_description_t fts_InvalidMessage = {"invalid symbol message"};
fts_status_description_t fts_DoubleConnection = {"the connection already exists"};

/******************************************************************************/
/*                                                                            */
/*                  File Init function                                        */
/*                                                                            */
/******************************************************************************/

void fts_objects_init()
{
  fts_heap_init(&connection_heap, sizeof(fts_connection_t), 256);
}

/******************************************************************************/
/*                                                                            */
/*                  Object Instantiation And Deleting                        */
/*                                                                            */
/******************************************************************************/


/* the new function now create an object in a patcher */


fts_object_t *
fts_object_new(fts_patcher_t *patcher, long id, int ac, const fts_atom_t *at)
{
  fts_status_t   status;
  fts_class_t   *cl;
  fts_object_t  *obj;
  int i;

  if (! fts_is_symbol(&at[0]))
    {
      fprintf(stderr,"Non symbol class name in object creation\n"); /* @@@@ ERROR !!! */
      return 0;
    }

  cl = fts_class_instantiate(ac, at);

  /* Class not found, try with old style abstraction */

  if (! cl)
    {
      obj =  fts_abstraction_new(patcher, id, ac, at);

      return obj;
    }

  obj     = (fts_object_t *)fts_block_zalloc(cl->size);
  obj->cl = cl;

  /* Copying the arguments */

  obj->argc = ac;
  obj->argv = (fts_atom_t *) fts_block_zalloc(ac * sizeof(fts_atom_t));

  for (i = 0; i < ac; i++)
    obj->argv[i] = at[i];

  /* Other Initializations */

  obj->properties = 0;

  if (cl->noutlets)
    {
      obj->out_conn = (fts_connection_t **) fts_block_zalloc(cl->noutlets * sizeof(fts_connection_t *));
      obj->outlets_properties = (fts_plist_t **) fts_block_zalloc(cl->noutlets * sizeof(fts_plist_t *));
    }

  if (cl->ninlets)
    {
      obj->in_conn = (fts_connection_t **) fts_block_zalloc(cl->ninlets * sizeof(fts_connection_t *));
      obj->inlets_properties = (fts_plist_t **) fts_block_zalloc(cl->ninlets * sizeof(fts_plist_t *));
    }
    
  /* Add the object in the patcher
   * the test is only usefull during the root patcher building
   */

  if (patcher)
    fts_patcher_add_object(patcher, obj);

  /* set the id and put the object in the object table */

  obj->id = id;
  fts_object_table_put(id, obj);

  /* send the init message */

  {
    /* force type checking during new */

    long save_check_status;

    save_check_status = fts_mess_run_time_check;
    fts_mess_run_time_check = 1;

    status = fts_message_send(obj, fts_SystemInlet, fts_s_init, ac, at);
    fts_mess_run_time_check = save_check_status;
  }

  if (status != fts_Success && status != &fts_MethodNotFound)
    {
      if (patcher)
	fts_patcher_remove_object(patcher, obj);

      if (obj->out_conn)
	{
	  fts_block_free((char *)obj->out_conn, obj->cl->noutlets*sizeof(fts_connection_t *));
	  fts_block_free((char *)obj->outlets_properties, obj->cl->noutlets*sizeof(fts_plist_t *));
	}

      if (obj->in_conn)
	{
	  fts_block_free((char *)obj->in_conn, obj->cl->ninlets*sizeof(fts_connection_t *));
	  fts_block_free((char *)obj->inlets_properties, obj->cl->ninlets*sizeof(fts_plist_t *));
	}

      fts_block_free((char *)obj, obj->cl->size);


      fts_object_table_remove(id);

      return 0;
    }


  return obj;
}

/* This is to support "changing" objects; usefull during 
   .pat loading, where not all the information is available 
   at the right place; used currently explode in the fts1.5 package.
   */

void fts_object_change_description(fts_object_t *obj, int argc, const fts_atom_t *argv)
{
  int i;

  if (obj->argc == argc)
    {
      /* Just copy the values, the size is correct */
      
      for (i = 0; i < argc; i++)
	obj->argv[i] = argv[i];
    }
  else
    {
      /* Free the old object description */

      fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

      /* reallocate the description and copy the arguments */

      obj->argc = argc;
      obj->argv = (fts_atom_t *) fts_block_zalloc(argc * sizeof(fts_atom_t));

      for (i = 0; i < argc; i++)
	obj->argv[i] = argv[i];
    }
}



void
fts_object_send_properties(fts_object_t *obj)
{
  /* If the object have an ID (i.e. was created by the client, or a property has
     been assigned to it),
     ask the object to send the ninlets and noutlets  properties,
     and name and declaration if any. */

  if (obj->id != FTS_NO_ID) 
    { 
      fts_object_property_changed_urgent(obj, fts_s_x);
      fts_object_property_changed_urgent(obj, fts_s_y);
      fts_object_property_changed_urgent(obj, fts_s_height);
      fts_object_property_changed_urgent(obj, fts_s_width);

      if (fts_object_is_patcher(obj))
	{
	  fts_object_property_changed_urgent(obj, fts_s_autorouting);
	  fts_object_property_changed_urgent(obj, fts_s_wx);
	  fts_object_property_changed_urgent(obj, fts_s_wy);
	  fts_object_property_changed_urgent(obj, fts_s_wh);
	  fts_object_property_changed_urgent(obj, fts_s_ww);
	}

      fts_object_property_changed_urgent(obj, fts_s_ninlets);
      fts_object_property_changed_urgent(obj, fts_s_noutlets);
      fts_object_property_changed_urgent(obj, fts_s_name);

      fts_object_property_changed_urgent(obj, fts_s_min_value);
      fts_object_property_changed_urgent(obj, fts_s_max_value);

      /* Declarations are not yet really supported */

      /* fts_object_property_changed_urgent(obj, fts_new_symbol("declaration")); */
    }
}



void
fts_object_delete(fts_object_t *obj)
{
  int outlet, inlet;

  /* tell the object we are going to delete him */

  fts_message_send(obj, fts_SystemInlet, fts_s_delete, 0, 0);

  /* take it away from the update queue, if there */

  fts_object_reset_changed(obj);

  /* delete all the survived connections starting in the object */

  for (outlet = 0; outlet < obj->cl->noutlets; outlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      for (p = obj->out_conn[outlet]; p ;  p = obj->out_conn[outlet])
	fts_object_disconnect(p->src, p->woutlet, p->dst, p->winlet);
    }

  /* Delete all the survived connections ending in the object */

  for (inlet = 0; inlet < obj->cl->ninlets; inlet++)
    {
      fts_connection_t *p;

      /* must call the real disconnect function, so that all the daemons
	 and methods  can fire correctly */

      for (p = obj->in_conn[inlet]; p; p = obj->in_conn[inlet])
	fts_object_disconnect(p->src, p->woutlet, p->dst, p->winlet);
    }

  /* Delete the object from the patcher */
  /* Some internal object don't necessarly have a patcher,
     like through (they should be put in the root patcher ??) */

  if (obj->patcher)
    fts_patcher_remove_object(obj->patcher, obj);

  /* remove the object from the object table */

  if (obj->id != FTS_NO_ID)
    fts_object_table_remove(obj->id);

  /* free the object properties */

  fts_properties_free(obj);

  /* Free the object description */

  fts_block_free((char *)obj->argv, obj->argc * sizeof(fts_atom_t));

  /* free the object */

  if (obj->out_conn)
    {
      fts_block_free((char *)obj->out_conn, obj->cl->noutlets*sizeof(fts_connection_t *));
      fts_block_free((char *)obj->outlets_properties, obj->cl->noutlets*sizeof(fts_plist_t *));
    }

  if (obj->in_conn)
    {
      fts_block_free((char *)obj->in_conn, obj->cl->ninlets*sizeof(fts_connection_t *));
      fts_block_free((char *)obj->inlets_properties, obj->cl->ninlets*sizeof(fts_plist_t *));
    }

  fts_block_free((char *)obj, obj->cl->size);
}

/******************************************************************************/
/*                                                                            */
/*                            Connections                                     */
/*                                                                            */
/******************************************************************************/

fts_status_t
fts_object_connect(fts_object_t *out, int woutlet, fts_object_t *in, int winlet)
{
  fts_outlet_decl_t *outlet;
  fts_inlet_decl_t *inlet;
  fts_class_mess_t *mess = 0;
  fts_connection_t *outconn;
  int anything;

  /* check the range */

  if (woutlet >= out->cl->noutlets || woutlet < 0)
    {
      /*
	fprintf(stderr,"fts_object_connect: outlet %d out of range %d for object %s(%d)\n",
	woutlet,	out->cl->noutlets, 
	fts_symbol_name(fts_get_class_name(out->cl)), fts_object_get_id(out));
      */

      return &fts_OutletOutOfRange;
    }


  /* check againsts double connections */
  { 
    fts_connection_t *p;

    for (p = out->out_conn[woutlet]; p ; p = p->next_same_src)
    {
      if ((p->dst == in) && (p->winlet == winlet))
	{
	  /* Found, return error message */

	  return &fts_DoubleConnection;
	}
    }
  }

  /* find the outlet and the inlet in the class structure */

  outlet = &out->cl->outlets[woutlet];

  if (winlet == fts_SystemInlet)
    inlet = in->cl->sysinlet;
  else if (winlet < in->cl->ninlets && winlet >= 0)
    inlet = &in->cl->inlets[winlet];
  else
    {
      /* 
      fprintf(stderr,"fts_object_connect: inlet %d out of range %d for object %s(%d)\n",
	      winlet, in->cl->ninlets, 
	      fts_symbol_name(fts_get_class_name(in->cl)), fts_object_get_id(in));
      */

      return &fts_InletOutOfRange;
    }

  if (outlet->tmess.symb)
    {
      mess = fts_class_mess_inlet_get(inlet, outlet->tmess.symb, &anything);

      if (! mess)
	{
	  post("fts_object_connect: cannot connect %s(%d) #%d to %s(%d) #%d\n", 
		  fts_symbol_name(fts_get_class_name(out->cl)),
		  fts_object_get_id(out),
		  woutlet,
		  fts_symbol_name(fts_get_class_name(in->cl)),
		  fts_object_get_id(in),
		  winlet
		  );/* @@@@ ERROR !!! */

	  /*
	  fprintf(stderr, "fts_object_connect: method for message %s not found\n",
		  fts_symbol_name(outlet->tmess.symb));
	  */


	  return &fts_ObjectCannotConnect;
	}
    }

  outconn = (fts_connection_t *) fts_heap_alloc(&connection_heap);

  outconn->src = out;
  outconn->woutlet = woutlet;
  outconn->dst = in;
  outconn->winlet = winlet;

  /* pre-initialize the cache, if possible */

  if (mess)
    {
      if (fts_mess_run_time_check)
	{
	  outconn->symb = 0;
	  outconn->mth  = 0;
	}
      else if (anything)
	{
	  /* we cache the method for anything here because, since the
	   outlet is typed, we are sure we will always call the anything method
	   for this type */

	  outconn->symb = 0;
	  outconn->mth  = mess->mth;
	}
      else
	{
	  outconn->symb = mess->tmess.symb;
	  outconn->mth  = mess->mth;
	}
    }
  else
    {
      outconn->symb = 0;
      outconn->mth  = 0;
    }

  /* add the connection to the outlet list and to the inlet list  */

  if (! out->out_conn[woutlet])
    {
      outconn->next_same_src = 0;
      out->out_conn[woutlet] = outconn;
    }
  else
    {
      outconn->next_same_src = out->out_conn[woutlet];
      out->out_conn[woutlet] = outconn;
    }

  if (! in->in_conn[winlet])
    {
      outconn->next_same_dst = 0;
      in->in_conn[winlet] = outconn;
    }
  else
    {
      outconn->next_same_dst = in->in_conn[winlet];
      in->in_conn[winlet] = outconn;
    }

  return fts_Success;
}

/* disconnect return fts_NoSuchConnection if the connection didn't existed */

fts_status_t
fts_object_disconnect(fts_object_t *out, int woutlet, fts_object_t *in, int winlet)
{ 
  fts_connection_t **p;		/* indirect precursor */
  fts_connection_t *conn = 0;
  fts_connection_t *prev = 0;

  /* look for the connection in the output list of out */

  for (p = &out->out_conn[woutlet]; *p ; p = &((*p)->next_same_src))
    {
      if (((*p)->dst == in) && ((*p)->winlet == winlet))
	{
	  conn = *p;

	  *p = (*p)->next_same_src;
	  break;
	}
    }

  /* if found, look for it  in the input list of in */

  if (conn)
    {
      for (p = &in->in_conn[winlet]; *p ; p = &((*p)->next_same_dst))
	if ((*p) == conn)
	  {
	    *p = (*p)->next_same_dst;
	    break;
	  }

      fts_heap_free((char *) conn, &connection_heap);

      return fts_Success;
    }
  else
    return &fts_NoSuchConnection;
}


/******************************************************************************/
/*                                                                            */
/*                            Messaging                                       */
/*                                                                            */
/******************************************************************************/

static fts_status_t
fts_args_check(fts_class_mess_t *mess, int ac, const fts_atom_t *at)
{
  fts_mess_type_t *tmess = &(mess->tmess);
  int i;

  if (tmess->mandatory_args == FTS_VAR_ARGS)
    return fts_Success;

  if (ac < tmess->mandatory_args)
    return &fts_ArgumentMissing;

  if (ac > tmess->nargs)
    return &fts_ExtraArguments;

  for (i = 0; i < ac; i++)
    {
      if (tmess->arg_types[i] == fts_s_anything)
	continue;
      else if ((tmess->arg_types[i] == fts_s_number) &&
	       ((fts_get_type(&at[i]) == fts_s_int) || 
		(fts_get_type(&at[i]) == fts_s_float)))
	continue;
      else if (fts_get_type(&at[i]) == tmess->arg_types[i])
	continue;
      else
	return &fts_ArgumentTypeMismatch;
    }

  return fts_Success;
}

void
fts_mess_set_run_time_check(int flag)
{
  fts_mess_run_time_check = flag;
}

fts_status_t
fts_message_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_status_t status;

  fts_inlet_decl_t *in;
  fts_class_mess_t *mess;
  fts_class_t *cl = o->cl;
  int anything;

  if (winlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    {
      fprintf(stderr,"Inlet %d out of range, for message %s to an object `%s'\n", winlet,
	   (s ? fts_symbol_name(s) : "(null)"), fts_symbol_name(fts_object_get_class_name(o)));  /* @@@@ ERROR !!! */
      return &fts_InletOutOfRange;
    }

  mess = fts_class_mess_inlet_get(in, s, &anything);

  if (mess)
    {
      if (fts_mess_run_time_check)
	{
	  status = fts_args_check(mess, ac, at);

	  if (status != fts_Success)
	    {
	      fprintf(stderr,"%s error for object of class %s, inlet %d, message %s arguments:",
		   status->description, fts_symbol_name(fts_object_get_class_name(o)), 
		   winlet,  fts_symbol_name(s)); /* @@@@ ERROR !!! */

	      fprintf_atoms(stderr, ac, at);
	      fprintf(stderr,"\n");/* @@@@ ERROR !!! */

	      return status;
	    }
	}

      (*mess->mth)(o, winlet, s, ac, at);
      return fts_Success;
    }

  return &fts_MethodNotFound;
}


fts_status_t
fts_message_send_cache(fts_object_t *o, int winlet, fts_symbol_t s,
		 int ac, const fts_atom_t *at, fts_symbol_t *symb_cache, fts_method_t *mth_cache)
{
  fts_status_t status;
  fts_inlet_decl_t *in;
  fts_class_mess_t *mess;
  fts_class_t *cl = o->cl;
  fts_class_mess_t **messtable;
  int i;

  if (winlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    {
      fprintf(stderr,"Inlet %d out of range, for message %s to an object `%s'\n", winlet,
	   (s ? fts_symbol_name(s) : "(null)"), fts_symbol_name(fts_object_get_class_name(o))); /* @@@@ ERROR !!! */
      return &fts_InletOutOfRange;
    }


  messtable = in->messlist;

  if ((in->nmess == 1) && (messtable[0]->tmess.symb == fts_s_anything))
    {
      /* special case for the anything as unique method */
      
      mess = messtable[0];
      *symb_cache = 0;
      *mth_cache  = mess->mth;
    }
  else
    {
      mess = 0;
      for (i = 0; i < in->nmess; i++)
	{
	  if (messtable[i]->tmess.symb == s)
	    {
	      mess = messtable[i];

	      *symb_cache = mess->tmess.symb;
	      *mth_cache = mess->mth;

	      break;
	    }
	  else if (messtable[i]->tmess.symb == fts_s_anything)
	    {
	      /* found and temporary stored a method for anything;
		 since it is not unique, we do not cache it */

	      mess = messtable[i];
	    }
	}
    }

  if (mess)
    {
      if (fts_mess_run_time_check)
	{
	  status = fts_args_check(mess, ac, at);

	  if (status != fts_Success)
	    {
	      fprintf(stderr,"%s error for object of class %s, inlet %d, message %s arguments:",
		   status->description, fts_symbol_name(fts_object_get_class_name(o)), winlet,
		   fts_symbol_name(s));  /* @@@@ ERROR !!! */
	      fprintf_atoms(stderr, ac, at);
	      fprintf(stderr,"\n");/* @@@@ ERROR !!! */

	      return status;
	    }

	  /* empty the connection cache if check is active */

	  *symb_cache = 0;
	  *mth_cache = 0;
	}

      (*mess->mth)(o, winlet, s, ac, at);

      return fts_Success;
    }
  else
    {
      fprintf(stderr,"Unknown message %s for object of class %s, inlet %d\n", 
	   fts_symbol_name(s), fts_symbol_name(fts_object_get_class_name(o)), winlet); /* @@@@ ERROR !!! */

      return &fts_MethodNotFound;
    }
}


/* All the call to this  Function are overwritten to macro in case of optimization.
   The function is left here so that a user can compile an object with -g to test it
*/

#ifdef DEBUG

fts_status_t
fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s,
		int ac, const fts_atom_t *at)
{
  fts_connection_t *conn;
  fts_class_t *cl = o->cl;
  fts_outlet_decl_t *out;
  fts_status_t status;

  if (woutlet >= cl->noutlets || woutlet < 0)
    {
      fprintf(stderr,"fts_outlet_send: outlet out of range #%d for object of class `%s'\n", woutlet, 
	   fts_symbol_name(fts_object_get_class_name(o))); /* @@@@ ERROR !!! */

      return &fts_OutletOutOfRange;
    }

  out = &cl->outlets[woutlet];

  if (!s)
    {
      fprintf(stderr,"fts_outlet_send: invalid message null symbol on outlet #%d for object of class `%s'\n", 
	   fts_symbol_name(s), woutlet, fts_symbol_name(fts_object_get_class_name(o))); /* @@@@ ERROR !!! */

      return &fts_InvalidMessage;
    }

  if (out->tmess.symb && out->tmess.symb != s)
    {
      fprintf(stderr, "fts_outlet_send: invalid message symbol `%s' on outlet #%d for object of class `%s'\n",
	   fts_symbol_name(s), woutlet, fts_symbol_name(fts_object_get_class_name(o)));	/* @@@ ERROR !!! */

      return &fts_InvalidMessage;
    }

  conn = o->out_conn[woutlet];

  while(conn)
    {
      /* second test is for the anything case */

      if ((conn->symb == s) || (!conn->symb && conn->mth))
	{
	  fts_class_mess_t *mess;
	  int anything;

	  mess = fts_class_mess_get(conn->dst->cl, conn->winlet, s, &anything);

	  if ((status = fts_args_check(mess, ac, at)) != fts_Success)
	    return status;

	  (*conn->mth)(conn->dst, conn->winlet, s, ac, at);
	}
      else
	fts_message_send_cache(conn->dst, conn->winlet, s, ac, at, &conn->symb, &conn->mth);

      conn = conn->next_same_src;
    }

  return fts_Success;
}


/* Utility functions */

/*
   The fts_outlet_* functions call will be overwritten by macro
   expansion in case of -O optimization; the functions are 
   always compiled, also to allow user object to be compiled with
   -g also with -O compiled libraries.
*/

void
fts_outlet_int(fts_object_t *o, int woutlet, int n)
{
  fts_connection_t *conn;
  fts_atom_t atom;

  fts_set_long(&atom, n);

  conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_message_send(conn->dst, conn->winlet, fts_s_int, 1, &atom); 

      conn = conn->next_same_src;
    }
}

void
fts_outlet_float(fts_object_t *o, int woutlet, float f)
{
  fts_connection_t *conn;
  fts_atom_t atom;

  fts_set_float(&atom, f);

  conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_message_send(conn->dst, conn->winlet, fts_s_float, 1, &atom); 

      conn = conn->next_same_src;
    }
}

void
fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
{
  fts_connection_t *conn;
  fts_atom_t atom;

  fts_set_symbol(&atom, s);

  conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_message_send(conn->dst, conn->winlet, fts_s_symbol, 1, &atom); 

      conn = conn->next_same_src;
    }
}

void
fts_outlet_list(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at)
{
  fts_connection_t *conn;

  conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_message_send(conn->dst, conn->winlet, fts_s_list, ac, at); 

      conn = conn->next_same_src;
    }
}


void
fts_outlet_bang(fts_object_t *o, int woutlet)
{
  fts_connection_t *conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_message_send(conn->dst, conn->winlet, fts_s_bang, 0, 0); 

      conn = conn->next_same_src;
    }
}

#endif

/* 
   function to get values by name;  later, argument by name will be supported in
   class discrimination function, using keywords ...

 */


long
fts_get_int_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, int def)
{
  int i;

  for (i = 0; i < (argc - 1); i++)
    if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == name) && fts_is_long(&at[i+1]))
      return fts_get_long(&at[i+1]);

  return def;
}


float
fts_get_float_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, float def)
{
  int i;

  for (i = 0; i < (argc - 1); i++)
    if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == name) && fts_is_float(&at[i+1]))
      return fts_get_float(&at[i+1]);

  return def;
}


fts_symbol_t
fts_get_symbol_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, fts_symbol_t def)
{
  int i;

  for (i = 0; i < (argc - 1); i++)
    if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == name) && fts_is_symbol(&at[i+1]))
      return fts_get_symbol(&at[i+1]);

  return def;
}


/* Boolean as 1 or 0: simply require that the symbol exists in the arguments,
   otherwise return the default, that should be 0, in the current implementation.
   In the future will do a smarter parsing, this is why the default argument is
   present
 */

long
fts_get_boolean_by_name(int argc, const fts_atom_t *at, fts_symbol_t name, int def)
{
  int i;

  for (i = 0; i < argc; i++)
    if (fts_is_symbol(&at[i]) && (fts_get_symbol(&at[i]) == name))
      return 1;

  return def;
}



/******************************************************************************/
/*                                                                            */
/*                          Object Access                                     */
/*                                                                            */
/******************************************************************************/


fts_symbol_t 
fts_object_get_class_name( fts_object_t *obj)
{
  return fts_get_class_name(obj->cl);
}

int 
fts_object_handle_message(fts_object_t *o, int winlet, fts_symbol_t s)
{
  int anything;

  if (fts_class_mess_get(o->cl, winlet, s, &anything))
    return 1;
  else
    return 0;
}

