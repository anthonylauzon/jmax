/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/object.h>
#include <ftsconfig.h>

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
 *  send message utils
 *
 */

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
static int unfold_varargs(int ac, const fts_atom_t* at, fts_atom_t* atup, const fts_atom_t** rat);

static int
unfold_atom(const fts_atom_t *at, const fts_atom_t** rat)
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
    *rat = NULL;
    return 0;
  }
  else
  {
    *rat = at;
    return 1;
  }
}

static int
unfold_varargs(int ac, const fts_atom_t* at, fts_atom_t *atup, const fts_atom_t** rat)
{
  switch(ac)
  {
  case 0:
    rat = NULL;
    return 0;
  case 1:
    return unfold_atom(at, rat);
  default:
    *rat = at;
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
  int meth_ac = 0;
  const fts_atom_t *meth_at = NULL;
  fts_atom_t atup;

  meth_ac = unfold_varargs(ac, at, &atup, &meth_at);
  (*method)(o, 0, NULL, meth_ac, meth_at);
}

void
fts_invoke_atom(fts_method_t method, fts_object_t *o, int ac, const fts_atom_t *at)
{
  int meth_ac = 0;
  const fts_atom_t *meth_at = NULL;
  fts_atom_t atup;

  fts_set_void(&atup);

  meth_ac = unfold_varargs(ac, at, &atup, &meth_at);

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
    (*method)(o, fts_system_inlet, s, !fts_is_void(at), at);

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
      (*method)(o, fts_system_inlet, s, 1, atup);
    }
    else
      (*method)(o, fts_system_inlet, s, ac, at);
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
    (*method)(o, fts_system_inlet, s, ac, at);
    return 1;
  }
  else
    return 0;
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

/***********************************************************************
 *
 * Initialization
 *
 */

void
fts_kernel_message_init(void)
{
  fts_message_class = fts_class_install(NULL, message_instantiate);
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
