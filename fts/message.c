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
#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

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

static void
message_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
	fts_object_set_error(o, "First argument must be symbol");
    }
}

static void
message_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_message_t *this = (fts_message_t *)o;

  fts_array_destroy(&this->args);
}

static void
message_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_message_t), message_init, message_delete);
  }

void
fts_message_config(void)
{
  fts_message_metaclass = fts_class_install(NULL, message_instantiate);
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

/* return Status */
fts_status_description_t fts_ArgumentMissing = {"argument missing"};
fts_status_description_t fts_ArgumentTypeMismatch = {"argument type mismatch"};

void
fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = o->head.cl;
  fts_method_t meth = fts_class_get_method(cl, s);

  if(meth)
    {
      const fts_atom_t *a;
      int n;
      
      /* unpack tuple */
      if(ac == 1 && fts_is_tuple(at))
	{
	  fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(at);
	  
	  meth(o, fts_system_inlet, s, fts_tuple_get_size(tuple), fts_tuple_get_atoms(tuple));
	}
      else
	meth(o, fts_system_inlet, s, ac, at);
    }
}

/************************************************
 *
 *  outlet handling
 *
 */

int fts_objstack_top = 0; /* Next free slot; can overflow, must be checked */
fts_object_t *fts_objstack[FTS_OBJSTACK_SIZE];

static int
check_outlet(fts_object_t *o, int woutlet)
{
  if (woutlet >= fts_object_get_outlets_number(o) || woutlet < 0)
    {
      fts_object_signal_runtime_error(o, "outlet (%d) out of range", woutlet);
      return 0;
    }
  else
    return 1;
}

static void
output_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_connection_t *conn = o->out_conn[woutlet];

  if(!check_outlet(o, woutlet))
    return;

  if (!FTS_REACHED_MAX_CALL_DEPTH()) 
    {
      while(conn)
	{
	  if(conn->selector == s)
	    {
	      /* call cached method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*conn->method)(conn->dst, conn->winlet, s, ac, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }
	  else
	    {
	      /* get method for given selector */
	      fts_method_t method = fts_class_get_method(fts_object_get_class(conn->dst), s);
	      
	      /* get default handler */
	      if(method == NULL)
		method = fts_class_get_default_handler(fts_object_get_class(conn->dst));

	      /* update cache */
	      conn->selector = s;
	      conn->class = NULL; /* all varargs for now */
	      conn->method = method;
	      
	      /* call method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*method)(conn->dst, conn->winlet, s, ac, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }

	  conn = conn->next_same_src;
	}
    }
  else
    fts_object_signal_runtime_error(o, "Message stack overflow at inlet %d", conn->winlet);
}

/* output single value */
static void
output_value(fts_object_t *o, int woutlet, const fts_atom_t *at)
{
  fts_connection_t *conn = o->out_conn[woutlet];

  if(!check_outlet(o, woutlet))
    return;

  if (!FTS_REACHED_MAX_CALL_DEPTH()) 
    {
      fts_metaclass_t *class = fts_get_class(at);

      while(conn)
	{
	  if(conn->selector == NULL && conn->class == class)
	    {
	      /* call cached method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*conn->method)(conn->dst, conn->winlet, 0, 1, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }
	  else
	    {
	      /* get method for given class at given inlet */
	      fts_method_t method = fts_class_inlet_get_method(fts_object_get_class(conn->dst), conn->winlet, class);
	      
	      /* ... or get varargs method at given inlet */
	      if(method == NULL)
		method = fts_class_inlet_get_method(fts_object_get_class(conn->dst), conn->winlet, NULL);

	      /* ... or get default handler */
	      if(method == NULL)
		method = fts_class_get_default_handler(fts_object_get_class(conn->dst));

	      /* update cache */
	      conn->selector = NULL;
	      conn->class = class;
	      conn->method = method;
	      
	      /* call method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*method)(conn->dst, conn->winlet, 0, 1, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }

	  conn = conn->next_same_src;
	}
    }
  else
    fts_object_signal_runtime_error(o, "Message stack overflow at inlet %d", conn->winlet);
}

static void
output_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at)
{
  fts_connection_t *conn = o->out_conn[woutlet];
  fts_metaclass_t *class = fts_get_class(at);

  if(!check_outlet(o, woutlet))
    return;

  if (!FTS_REACHED_MAX_CALL_DEPTH()) 
    {
      while(conn)
	{
	  if(conn->selector == NULL && conn->class == NULL && conn->method != NULL)
	    {
	      /* call cached method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*conn->method)(conn->dst, conn->winlet, 0, ac, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }
	  else
	    {
	      /* get varargs method at given inlet */
	      fts_method_t method = fts_class_inlet_get_method(fts_object_get_class(conn->dst), conn->winlet, NULL);
	      
	      /* get default handler */
	      if(method == NULL)
		method = fts_class_get_default_handler(fts_object_get_class(conn->dst));

	      /* ... or update cache */
	      conn->selector = NULL;
	      conn->class = NULL;
	      conn->method = method;
	      
	      /* call method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*method)(conn->dst, conn->winlet, 0, ac, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }

	  conn = conn->next_same_src;
	}
    }
  else
    fts_object_signal_runtime_error(o, "Message stack overflow at inlet %d", conn->winlet);
}

void 
fts_outlet_int(fts_object_t *o, int woutlet, int n)
{
  fts_atom_t a;
  
  fts_set_int(&a, n);
  output_value(o, woutlet, &a);
}

void 
fts_outlet_float(fts_object_t *o, int woutlet, float f)
{
  fts_atom_t a;
  
  fts_set_float(&a, f);
  output_value(o, woutlet, &a);
}

void 
fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
{
  fts_atom_t a;
  
  fts_set_symbol(&a, s);
  output_value(o, woutlet, &a);
}

void 
fts_outlet_bang(fts_object_t *o, int woutlet)
{
  output_message(o, woutlet, fts_s_bang, 0, 0);
}

void
fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
{
  fts_atom_t a;
  
  fts_set_object(&a, obj);
  output_value(o, woutlet, &a);
}

void
fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(check_outlet(o, woutlet))
    {
      const fts_atom_t *a;
      int n;
      
      /* unpack tuple */
      if(ac == 1 && fts_is_tuple(at))
	{
	  fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(at);
	  
	  n = fts_tuple_get_size(tuple);
	  a = fts_tuple_get_atoms(tuple);
	}
      else
	{
	  n = ac;
	  a = at;
	}
      
      if(s == 0)
	{
	  if(ac == 1)
	    output_value(o, woutlet, at);
	  else
	    output_varargs(o, woutlet, ac, at);
	}
      else
	output_message(o, woutlet, s, ac, at);
    }
}

void
fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
{
  if(ac == 1)
    {
      if(fts_is_tuple(at))
	{
	  fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(at);
	  output_varargs(o, woutlet, fts_tuple_get_size(tuple), fts_tuple_get_atoms(tuple));
	}
      else
	output_value(o, woutlet, at);
    }
  else if(ac > 1)
    output_varargs(o, woutlet, ac, at);
}

/***********************************************************************
 *
 * return mechanism for methods/functions
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
