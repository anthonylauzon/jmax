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
#include <ftsprivate/object.h>
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
fts_class_t *fts_message_class = 0;

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
	fts_object_error(o, "first argument must be symbol");
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
  fts_message_class = fts_class_install(NULL, message_instantiate);
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
  dumper->message = (fts_message_t *)fts_object_create(fts_message_class, 0, 0);

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
 *  outlet and message utils
 *
 */

/* return Status */
fts_status_description_t fts_ArgumentMissing = {"argument missing"};
fts_status_description_t fts_ArgumentTypeMismatch = {"argument type mismatch"};

int fts_objstack_top = 0; /* Next free slot; can overflow, must be checked */
fts_object_t *fts_objstack[FTS_OBJSTACK_SIZE];

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

static void
create_tuple(int ac, const fts_atom_t *at, fts_atom_t *atup)
{
  if(fts_is_void(atup))
  {
    fts_object_t *tup = fts_object_create(fts_tuple_class, ac, at);

    fts_set_object(atup, tup);
    fts_object_refer(tup);
  }
}

/*****************************************************************
*
*  unfold method args
*
*/
static int unfold_varargs(int ac, const fts_atom_t* at, fts_atom_t* atup, const fts_atom_t *rat);

static int
unfold_atom(const fts_atom_t *at, const fts_atom_t *rat)
{
  if(fts_is_tuple(at))
  {
    fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
    int tup_ac = fts_tuple_get_size(tup);
    fts_atom_t *tup_at = fts_tuple_get_atoms(tup);

    return unfold_varargs(tup_ac, tup_at, (fts_atom_t *)at, rat);
  }
  else if(fts_is_void(at))
  {
    rat = NULL;
    return 0;
  }
  else
  {
    rat = at;
    return 1;
  }
}

static int
unfold_varargs(int ac, const fts_atom_t* at, fts_atom_t *atup, const fts_atom_t *rat)
{
  switch(ac)
  {
    case 0:
      rat = NULL;
      return 0;
    case 1:
      return unfold_atom(at, rat);
    default:
      rat = at;
      return ac;
  }
}

/*****************************************************************
 *
 *  method invokation API
 *
 */

void
fts_invoke_varargs(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at)
{
  int meth_ac;
  const fts_atom_t *meth_at;
  fts_atom_t atup;

  meth_ac = unfold_varargs(ac, at, &atup, meth_at);
  (*method)(o, 0, NULL, meth_ac, meth_at);
}

void
fts_invoke_atom(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at)
{
  int meth_ac;
  const fts_atom_t *meth_at;
  fts_atom_t atup;

  fts_set_void(&atup);

  meth_ac = unfold_varargs(ac, at, &atup, meth_at);

  if(meth_ac > 0)
  {
    create_tuple(meth_ac, meth_at, &atup);
    (*method)(o, 0, NULL, 1, &atup);
  }
  else
    (*method)(o, 0, NULL, 1, meth_at);

  /* release temporary tuple (if needed) */
  fts_atom_release(&atup);
}

/*****************************************************************
 *
 *   outlet sending
 *
 */

/* output single atom (but tuple) or bang */
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

        if(handler != NULL)
          (*handler)(dst, winlet, NULL, !fts_is_void(at), at);
        else
        {
          fts_method_t method = fts_connection_cache_get_method(conn);
          
          if(fts_connection_cache_get_type(conn) != type)
	    {
	      int varargs = 0;
	      
	      method = fts_class_get_inlet_method(cl, winlet, type, &varargs);
	      
	      if(method != NULL)
		{
		  fts_connection_cache_set_type(conn, type);
		  fts_connection_cache_set_varargs(conn, 0);
		  fts_connection_cache_set_method(conn, method);
		}
	      else
		{
		  fts_object_error(dst, "no %s method for inlet %d", fts_class_get_name(type), winlet);
		  conn = fts_connection_get_next_of_same_source(conn);
		  continue;
		}
	    }

          INVOKE(method, dst, winlet, NULL, !fts_is_void(at), at);
        }

        conn = fts_connection_get_next_of_same_source(conn);
      }
    }
    else
      fts_object_error(o, "message stack overflow at outlet %d", woutlet);
  }
}

/* output tuple or varargs */
static void
outlet_tuple(fts_object_t *o, int woutlet, int ac, const fts_atom_t *at, fts_atom_t *atup)
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

        if(handler != NULL)
        {
          if(fts_is_void(atup))
            (*handler)(dst, winlet, NULL, ac, at);
          else
            (*handler)(dst, winlet, NULL, 1, atup);
        }
        else
        {
          fts_class_t *type = fts_connection_cache_get_type(conn);
          int varargs = fts_connection_cache_get_varargs(conn);
          fts_method_t method = fts_connection_cache_get_method(conn);

          if(type != fts_tuple_class)
          {
            type = fts_tuple_class;
            method = fts_class_get_inlet_method(cl, winlet, fts_tuple_class, &varargs);

            if(method != NULL)
            {
              fts_connection_cache_set_type(conn, type);
              fts_connection_cache_set_varargs(conn, varargs);
              fts_connection_cache_set_method(conn, method);
            }
            else
            {
              fts_object_error(dst, "no tuple method at inlet %d", winlet); 
	      conn = fts_connection_get_next_of_same_source(conn);
	      continue;
            }
          }

          if(varargs == 0)
          {
            create_tuple(ac, at, atup);
            INVOKE(method, dst, winlet, NULL, 1, atup);
          }
          else
            INVOKE(method, dst, winlet, NULL, ac, at);
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
 *  dispatch outlet args
 * 
 */
static void dispatch_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at, fts_atom_t* atup);

static void
dispatch_tuple(fts_object_t *o, int woutlet, const fts_atom_t *at)
{
  fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
  int tup_ac = fts_tuple_get_size(tup);
  fts_atom_t *tup_at = fts_tuple_get_atoms(tup);

  dispatch_varargs(o, woutlet, tup_ac, tup_at, (fts_atom_t *)at);
}

static void
dispatch_atom(fts_object_t *o, int woutlet, const fts_atom_t *at)
{
  if(fts_is_tuple(at))
    dispatch_tuple(o, woutlet, at);
  else
    outlet_atom(o, woutlet, at);
}

static void
dispatch_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at, fts_atom_t* atup)
{
  switch(ac)
  {
    case 0:
      outlet_atom(o, woutlet, fts_null);
      break;
    case 1:
      dispatch_atom(o, woutlet, at);
      break;
    default:
      outlet_tuple(o, woutlet, ac, at, atup);
  }
}

/*****************************************************************
 *
 *  outlet sending API
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
  dispatch_atom(o, woutlet, at);
}

void
fts_outlet_object(fts_object_t *o, int woutlet, fts_object_t *obj)
{
  fts_atom_t a;

  fts_set_object(&a, obj);
  
  if(fts_object_get_class(obj) == fts_tuple_class)
    dispatch_tuple(o, woutlet, &a);
  else
    outlet_atom(o, woutlet, &a);
}

void
fts_outlet_varargs(fts_object_t *o, int woutlet, int ac, const fts_atom_t* at)
{
  fts_atom_t atup;
  
  fts_set_void(&atup);
  dispatch_varargs(o, woutlet, ac, at, &atup);
  
  /* release temporary tuple (if needed) */
  fts_atom_release(&atup);
}

/***************************************************
 *
 *  send messages
 *
 */

/* send message with single argument or void */
static fts_method_t
send_message_atom(fts_object_t *o, fts_symbol_t s, const fts_atom_t *at)
{
  fts_class_t *cl = fts_object_get_class(o);
  fts_class_t *type = fts_get_class(at);
  fts_symbol_t cache_s = fts_object_message_cache_get_selector(o);
  fts_method_t method = fts_object_message_cache_get_method(o);

  if(cache_s != s || fts_object_message_cache_get_type(o) != type)
  {
    int varargs = 0;
    method = fts_class_get_method(cl, s, type, &varargs);

    fts_object_message_cache_set_selector(o, s);
    fts_object_message_cache_set_type(o, type);
    fts_object_message_cache_set_varargs(o, 0 );
    fts_object_message_cache_set_method(o, method);
  }

  if(method != NULL)
    INVOKE(method, o, fts_system_inlet, s, !fts_is_void(at), at);

  return method;
}

/* send message with tuple argument or varargs */
static fts_method_t
send_message_tuple(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *atup)
{
  fts_class_t *cl = fts_object_get_class(o);
  fts_symbol_t cache_s = fts_object_message_cache_get_selector(o);
  fts_class_t *type = fts_object_message_cache_get_type(o);
  int varargs = fts_object_message_cache_get_varargs(o);
  fts_method_t method = fts_object_message_cache_get_method(o);

  if(cache_s != s || type != fts_tuple_class)
  {    
    method = fts_class_get_method(cl, s, fts_tuple_class, &varargs);
    type = fts_tuple_class;

    fts_object_message_cache_set_selector(o, s);
    fts_object_message_cache_set_type(o, type);
    fts_object_message_cache_set_varargs(o, varargs);
    fts_object_message_cache_set_method(o, method);
  }

  if(method != NULL)
  {
    if(varargs == 0)
    {
      create_tuple(ac, at, atup);
      INVOKE(method, o, fts_system_inlet, s, 1, atup);
    }
    else
      INVOKE(method, o, fts_system_inlet, s, ac, at);
  }

  return method;
}

/*****************************************************************
 *
 *  dispatch message args
 *
 */
static fts_method_t dispatch_message_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t* atup);

static fts_method_t
dispatch_message_atom(fts_object_t *o, fts_symbol_t s, const fts_atom_t *at)
{
  if(fts_is_tuple(at))
  {
    fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
    int tup_ac = fts_tuple_get_size(tup);
    fts_atom_t *tup_at = fts_tuple_get_atoms(tup);

    return dispatch_message_varargs(o, s, tup_ac, tup_at, (fts_atom_t *)at);
  }
  else
    return send_message_atom(o, s, at);
}

static fts_method_t
dispatch_message_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t *atup)
{
  switch(ac)
  {
    case 0:
      return send_message_atom(o, s, fts_null);
    case 1:
      return dispatch_message_atom(o, s, at);
    default:
      return send_message_tuple(o, s, ac, at, atup);
  }
}

/***********************************************************************
 *
 * message sending API
 *
 */

int
fts_send_message(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_method_t method;
  fts_atom_t atup;
  
  fts_set_void(&atup);
  method = dispatch_message_varargs(o, s, ac, at, &atup);
  
  /* release temporary tuple (if needed) */
  fts_atom_release(&atup);

  return (method != NULL);
}

int
fts_send_message_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_method_t method = fts_class_get_method_varargs(fts_object_get_class(o), s);

  if(method != NULL)
  {
    INVOKE(method, o, fts_system_inlet, s, ac, at);
    return 1;
  }
  else
    return 0;
}

void
fts_outlet_message(fts_object_t *o, int woutlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(check_outlet(o, woutlet))
  {
    fts_connection_t *conn = fts_object_get_outlet_connections(o, woutlet);

    if (!FTS_REACHED_MAX_CALL_DEPTH())
    {
      fts_atom_t atup;
  
      fts_set_void(&atup);
  
      while(conn)
      {
        fts_object_t *dst = fts_connection_get_destination(conn);
        fts_method_t handler = fts_class_get_input_handler(fts_object_get_class(dst));

        if(handler != NULL)
          (*handler)(dst, fts_connection_get_inlet(conn), s, ac, at);
        else if(dispatch_message_varargs(dst, s, ac, at, &atup) == NULL)
	  fts_object_error(dst, "no method for message %s with given argument(s)", s);
        
        conn = fts_connection_get_next_of_same_source(conn);
      }

      /* release temporary tuple (if needed) */
      fts_atom_release(&atup);
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

void fts_return_int(int x)
{
  fts_set_int(&fts_return_value, x);
}

void fts_return_float(float x)
{
  fts_set_float(&fts_return_value, x);
}

void fts_return_symbol(fts_symbol_t x)
{
  fts_set_symbol(&fts_return_value, x);
}

void fts_return_object(fts_object_t *x)
{
  fts_set_object(&fts_return_value, x);
}

fts_atom_t *fts_get_return_value( void)
{
  return &fts_return_value;
}
