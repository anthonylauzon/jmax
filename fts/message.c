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

static fts_status_t
message_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_message_t), 0, 0, 0);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, message_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, message_delete);
  
  return fts_ok;
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

/* Return Status */
fts_status_description_t fts_MethodNotFound = {"method not found"};
fts_status_description_t fts_StackOverflow = {"method stack overflow"};
fts_status_description_t fts_ArgumentMissing = {"argument missing"};
fts_status_description_t fts_ArgumentTypeMismatch = {"argument type mismatch"};
fts_status_description_t fts_InvalidMessage = {"invalid symbol message"};

/* The object stack */

int fts_objstack_top = 0; /* Next free slot; can overflow, must be checked */
fts_object_t *fts_objstack[FTS_OBJSTACK_SIZE];

fts_status_t 
fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = o->head.cl;
  fts_method_t meth = fts_class_get_method(cl, s);

  if (meth && !FTS_REACHED_MAX_CALL_DEPTH())
    {
      FTS_OBJSTACK_PUSH(o);
      meth(o, 0, s, ac, at);
      FTS_OBJSTACK_POP(o);

      return fts_ok;
    }

  return &fts_MethodNotFound;
}

static fts_status_t
message_outlet(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = o->head.cl;
  fts_symbol_t selector = fts_class_outlet_get_selector(cl, woutlet);
  fts_connection_t *conn = o->out_conn[woutlet];

  if (selector != NULL && selector != s)
    return &fts_InvalidMessage;

  if (!FTS_REACHED_MAX_CALL_DEPTH()) 
    {
      while(conn)
	{
	  if ((conn->symb == s) || (conn->symb == NULL && conn->mth != NULL))
	    {
	      /* call cashed method */
	      FTS_OBJSTACK_PUSH(conn->dst);
	      (*conn->mth)(conn->dst, conn->winlet, s, ac, at);
	      FTS_OBJSTACK_POP(conn->dst);
	    }
	  else
	    {
	      /* search method and cache if found */
	      fts_method_t meth = fts_class_inlet_get_method(fts_object_get_class(conn->dst), conn->winlet, s);
	      
	      if(meth)
		{
		  conn->symb = s;
		  conn->mth = meth;
		  
		  FTS_OBJSTACK_PUSH(o);
		  (*meth)(conn->dst, conn->winlet, s, ac, at);
		  FTS_OBJSTACK_POP(o);
		  
		  return fts_ok;
		}
	      else
		{
		  fts_object_signal_runtime_error(o, "Received unknown message %s at inlet %d", s, conn->winlet);
		  return &fts_MethodNotFound;
		}
	    }
	  
	  conn = conn->next_same_src;
	}
    }
  else
    {
      fts_object_signal_runtime_error(o, "Message stack overflow at inlet %d", conn->winlet);
      return &fts_StackOverflow;
    }

  return fts_ok;
}


fts_status_t
fts_outlet_send(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = o->head.cl;
  const fts_atom_t *a;
  int n;

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

  if (woutlet >= fts_object_get_outlets_number(o) || woutlet < 0)
    return &fts_OutletOutOfRange;

  return message_outlet(o, woutlet, s, n, a);
}

void 
fts_outlet_int(fts_object_t *o, int woutlet, int n)
{
  fts_atom_t a;

  fts_set_int(&a, n);
  message_outlet(o, woutlet, fts_s_int, 1, &a);
}

void 
fts_outlet_float(fts_object_t *o, int woutlet, float f)
{
  fts_atom_t a;

  fts_set_float(&a, f);
  message_outlet(o, woutlet, fts_s_float, 1, &a);
}

void 
fts_outlet_symbol(fts_object_t *o, int woutlet, fts_symbol_t s)
{
  fts_atom_t a;

  fts_set_symbol(&a, s);
  message_outlet(o, woutlet, fts_s_symbol, 1, &a);
}

void 
fts_outlet_bang(fts_object_t *o, int woutlet)
{
  message_outlet(o, woutlet, fts_s_bang, 0, 0);
}

void
fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
{
  fts_atom_t a;

  fts_set_object(&a, obj);

  fts_object_refer(obj);
  message_outlet(o, woutlet, fts_metaclass_get_selector(fts_object_get_metaclass(obj)), 1, &a);
  fts_object_release(obj);
}

void
fts_outlet_primitive(fts_object_t *o, int woutlet, const fts_atom_t *a)
{
  message_outlet(o, woutlet, fts_get_selector(a), 1, a);
}

void
fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t* a)
{
  if(fts_is_tuple(a))
    fts_tuple_output(o, woutlet, (fts_tuple_t *)fts_get_object(a));
  else if(!fts_is_void(a))
    message_outlet(o, woutlet, fts_get_selector(a), 1, a);
}

void
fts_outlet_atoms(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
{
  if(ac == 1)
    fts_outlet_atom(o, woutlet, at);
  else if(ac > 1)
   message_outlet(o, woutlet, fts_s_list, ac, at);
}

void
fts_outlet_atoms_copy(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
{
  if(ac == 1)
    {
      fts_atom_t output;

      output = *at;
      fts_atom_refer(&output);

      fts_outlet_atom(o, woutlet, &output);

      fts_atom_release(&output);
    }
  else if(ac > 1)
    {
      fts_atom_t *output = alloca(sizeof(fts_atom_t) * ac);
      int i;
      
      for(i=0; i<ac; i++)
	{
	  output[i] = at[i];
	  fts_atom_refer(output + i);
	}
      
      message_outlet(o, woutlet, fts_s_list, ac, at);
      
      for(i=0; i<ac; i++)
	fts_atom_release(output + i);
    }
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
