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

#ifdef DEBUG 
#define INIT_CHECK_STATUS 1
#else
#define INIT_CHECK_STATUS 0
#endif

/************************************************
 *
 *  message class
 *
 */
fts_metaclass_t *fts_message_metaclass = 0;

static int 
is_token(fts_symbol_t s)
{
  return ((s == fts_s_dot) ||
	  (s == fts_s_comma) ||
	  (s == fts_s_semi) ||
	  (s == fts_s_colon) ||
	  (s == fts_s_double_colon) ||
	  (s == fts_s_quote) ||
	  (s == fts_s_comma));
}

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

static void
message_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_message_t *this = (fts_message_t *)o;

  this->s = 0;
  fts_array_init(&this->args, 0, 0);

  /* check arguments */
  if(ac > 0)
    {
      int i;

      /* check for separators and lists */
      for(i=0; i<ac; i++)
	{
	  if(fts_is_symbol(at + i) && is_token(fts_get_symbol(at + i)))
	    {
	      fts_object_set_error(o, "Syntax error in message or constant");
	      return;
	    }
	  else if(fts_is_tuple(at + i))
	    {
	      fts_object_set_error(o, "List cannot be argument of a message or constructor");
	      return;
	    }
	}

      /* first arg is symbol */
      if(fts_is_symbol(at))
	{
	  fts_symbol_t name = fts_get_symbol(at);
	  fts_class_t *cl;

	  fts_message_set(this, name, ac - 1, at + 1); /* message format: <selector> [<value> ...] (any message) */
	}
      else if(ac == 1)
	fts_message_set(this, fts_get_selector(at), 1, at); /* value format: <non symbol value> (without type specifyer) */
      else
	fts_message_set(this, fts_s_list, ac, at); /* implicit list format: <non symbol value> [<value> ...] */
    }
}

static void
message_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_message_t *this = (fts_message_t *)o;

  fts_array_destroy(&this->args);
}

static fts_status_t
message_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_message_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, message_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, message_delete);
  
  return fts_Success;
}

void
fts_message_config(void)
{
  fts_message_metaclass = fts_class_install(fts_s_message, message_instantiate);
}

/************************************************
 *
 *  message dumper
 *
 */

void
fts_dumper_init(fts_dumper_t *dumper, fts_method_t send)
{
  dumper->send = send;
  dumper->message = (fts_message_t *)fts_object_create(fts_message_metaclass, 0, 0);

  fts_object_refer(dumper->message);
}

void
fts_dumper_destroy(fts_dumper_t *dumper)
{
  fts_object_release(dumper->message);
}

fts_message_t *
fts_dumper_message_new(fts_dumper_t *dumper, fts_symbol_t selector)
{
  fts_message_set(dumper->message, selector, 0, 0);

  return dumper->message;
}

void
fts_dumper_message_send(fts_dumper_t *dumper, fts_message_t *message)
{
  fts_symbol_t s = fts_message_get_selector(message);
  int ac = fts_message_get_ac(message);
  const fts_atom_t *at = fts_message_get_at(message);

  dumper->send((fts_object_t *)dumper, 0, s, ac, at);
}

void
fts_dumper_send(fts_dumper_t *dumper, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dumper->send((fts_object_t *)dumper, 0, s, ac, at);  
}

/************************************************
 *
 *  message handling
 *
 */

/* Return Status */
fts_status_description_t fts_MethodNotFound = {"method not found"};
fts_status_description_t fts_ArgumentMissing = {"argument missing"};
fts_status_description_t fts_ArgumentTypeMismatch = {"argument type mismatch"};
fts_status_description_t fts_ExtraArguments = {"extra arguments"};
fts_status_description_t fts_InvalidMessage = {"invalid symbol message"};

/* The object stack */

int fts_objstack_top = 0; /* Next free slot; can overflow, must be checked */
fts_object_t *fts_objstack[FTS_OBJSTACK_SIZE];

fts_status_t 
fts_send_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_decl_t *in;
  fts_class_mess_t *mess;
  fts_class_t *cl = o->head.cl;
  int anything;

  if (winlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    return &fts_InletOutOfRange;

  mess = fts_class_mess_inlet_get(in, s, &anything);  /* @@@anything */

  if (mess && !FTS_REACHED_MAX_CALL_DEPTH())
    {
      FTS_OBJSTACK_PUSH(o);
      (*mess->mth)(o, winlet, s, ac, at);
      FTS_OBJSTACK_POP(o);

      return fts_Success;
    }

  return &fts_MethodNotFound;
}


fts_status_t
fts_send_message_cache(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at, fts_symbol_t *symb_cache, fts_method_t *mth_cache)
{
  fts_inlet_decl_t *in;
  fts_class_mess_t *mess;
  fts_class_t *cl = o->head.cl;
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

  if (mess && !FTS_REACHED_MAX_CALL_DEPTH())
    {
      FTS_OBJSTACK_PUSH(o);
      (*mess->mth)(o, winlet, s, ac, at);
      FTS_OBJSTACK_POP(o);

      return fts_Success;
    }
  else
    {
      fts_object_signal_runtime_error(o, "Unknown message %s for object of class %s, inlet %d", 
				      s, fts_object_get_class_name(o), winlet);

      return &fts_MethodNotFound;
    }
}


/* All the call to this  Function are overwritten to macro in case of optimization.
   The function is left here so that a user can compile an object with -g to test it
*/

#undef fts_outlet_send
fts_status_t
fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s,
		int ac, const fts_atom_t *at)
{
  fts_connection_t *conn;
  fts_class_t *cl = o->head.cl;
  fts_outlet_decl_t *out;

  if (woutlet >= cl->noutlets || woutlet < 0)
    return &fts_OutletOutOfRange;

  out = &cl->outlets[woutlet];

  if (!s)
    return &fts_InvalidMessage;

  if (out->tmess.symb && out->tmess.symb != s)
    return &fts_InvalidMessage;

  conn = o->out_conn[woutlet];

  if (!FTS_REACHED_MAX_CALL_DEPTH()) {

    while(conn)
      {
	if ((conn->symb == s) || (!conn->symb && conn->mth))
	  {
	    /* call cashed method */
	    FTS_OBJSTACK_PUSH(conn->dst);
	    (*conn->mth)(conn->dst, conn->winlet, s, ac, at);
	    FTS_OBJSTACK_POP(conn->dst);
	  }
	else
	  fts_send_message_cache(conn->dst, conn->winlet, s, ac, at, &conn->symb, &conn->mth);
	
	conn = conn->next_same_src;
      }
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

#undef fts_outlet_int
void fts_outlet_int(fts_object_t *o, int woutlet, int n)
{
  fts_connection_t *conn;
  fts_atom_t atom;

  fts_set_int(&atom, n);

  conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_send_message(conn->dst, conn->winlet, fts_s_int, 1, &atom); 

      conn = conn->next_same_src;
    }
}

#undef fts_outlet_float
void fts_outlet_float(fts_object_t *o, int woutlet, float f)
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

#undef fts_outlet_symbol
void fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
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

#undef fts_outlet_bang
void fts_outlet_bang(fts_object_t *o, int woutlet)
{
  fts_connection_t *conn = o->out_conn[woutlet];

  while(conn)
    {
      fts_send_message(conn->dst, conn->winlet, fts_s_bang, 0, 0); 

      conn = conn->next_same_src;
    }
}

void
fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
{
  fts_atom_t a;

  fts_set_object(&a, obj);

  fts_object_refer(obj);
  fts_outlet_send(o, woutlet, fts_metaclass_get_selector(fts_object_get_metaclass(obj)), 1, &a);
  fts_object_release(obj);
}

void
fts_outlet_primitive(fts_object_t *o, int woutlet, const fts_atom_t *a)
{
  fts_outlet_send(o, woutlet, fts_get_selector(a), 1, a);
}

void
fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* a)
{
  if(fts_is_tuple(a))
    fts_tuple_output(o, woutlet, fts_get_tuple(a));
  else
    fts_outlet_send(o, woutlet, fts_get_selector(a), 1, a);
}

void
fts_outlet_atoms(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
{
  if(ac == 1)
    fts_outlet_atom(o, woutlet, at);
  else if(ac > 0)
    fts_outlet_send(o, woutlet, fts_s_list, ac, at);
}


/* **********************************************************************
 *
 * Return mechanism for methods/functions
 *
 */

static fts_atom_t fts_return_value;

void fts_return( fts_atom_t *p)
{
  fts_return_value = *p;
}

fts_atom_t *fts_get_return_value( void)
{
  return &fts_return_value;
}
