/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1998/08/19 15:15:46 $
 *
 *  Eric Viara for Ircam, January 1995
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

#ifdef DEBUG 
#define INIT_CHECK_STATUS 1
#else
#define INIT_CHECK_STATUS 0
#endif

static long fts_mess_run_time_check = INIT_CHECK_STATUS;


/* Return Status */

fts_status_description_t fts_MethodNotFound = {"method not found"};
fts_status_description_t fts_ArgumentMissing = {"argument missing"};
fts_status_description_t fts_ArgumentTypeMismatch = {"argument type mismatch"};
fts_status_description_t fts_ExtraArguments = {"extra arguments"};
fts_status_description_t fts_InvalidMessage = {"invalid symbol message"};

/******************************************************************************/
/*                                                                            */
/*                  File Init function                                        */
/*                                                                            */
/******************************************************************************/

void fts_messages_init()
{
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


void fts_mess_set_run_time_check(int flag)
{
  fts_mess_run_time_check = flag;
}


int fts_mess_get_run_time_check()
{
  return fts_mess_run_time_check;
}


fts_status_t fts_send_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
    return &fts_InletOutOfRange;

  mess = fts_class_mess_inlet_get(in, s, &anything);

  if (mess)
    {
      if (fts_mess_get_run_time_check())
	{
	  status = fts_args_check(mess, ac, at);

	  if (status != fts_Success)
	    return status;
	}

      (*mess->mth)(o, winlet, s, ac, at);
      return fts_Success;
    }

  return &fts_MethodNotFound;
}


fts_status_t
fts_send_message_cache(fts_object_t *o, int winlet, fts_symbol_t s,
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
    return &fts_InletOutOfRange;


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
      if (fts_mess_get_run_time_check())
	{
	  status = fts_args_check(mess, ac, at);

	  if (status != fts_Success)
	    {
	      post("%s error for object of class %s, inlet %d, message %s arguments:",
		   status->description, fts_symbol_name(fts_object_get_class_name(o)), winlet,
		   fts_symbol_name(s));
	      post_atoms(ac, at);
	      post("\n");

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
      post("Unknown message %s for object of class %s, inlet %d\n", 
	   fts_symbol_name(s), fts_symbol_name(fts_object_get_class_name(o)), winlet);

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
    return &fts_OutletOutOfRange;

  out = &cl->outlets[woutlet];

  if (!s)
    return &fts_InvalidMessage;

  if (out->tmess.symb && out->tmess.symb != s)
    return &fts_InvalidMessage;

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
	fts_send_message_cache(conn->dst, conn->winlet, s, ac, at, &conn->symb, &conn->mth);

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
      fts_send_message(conn->dst, conn->winlet, fts_s_int, 1, &atom); 

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
      fts_send_message(conn->dst, conn->winlet, fts_s_float, 1, &atom); 

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
      fts_send_message(conn->dst, conn->winlet, fts_s_symbol, 1, &atom); 

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
      fts_send_message(conn->dst, conn->winlet, fts_s_list, ac, at); 

      conn = conn->next_same_src;
    }
}


void
fts_outlet_bang(fts_object_t *o, int woutlet)
{
  fts_connection_t *conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_send_message(conn->dst, conn->winlet, fts_s_bang, 0, 0); 

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



