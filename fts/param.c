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


/*
 * Parameter service for all FTS.
 * 
 * A parameter is a named atom value, that can be changed thru
 * an API and thru ucs/script commands; parameters can change dynamically
 * and the API implement the parameter listener concept, i.e. a callback
 * that will be called when the value is set of is changed.
 *
 * The implementation is oriented toward simplicity against run time
 * performance; these parameters should be set by the UI, not changed
 * very fast :->
 *
 * Should be integrated with patch variables ? Don't know really,
 * this stuff is more dynamic ..
 */

#include <fts/ftsnew.h>


typedef struct fts_param
{
  fts_symbol_t name;
  fts_atom_t   value;
  struct fts_param *next;
} fts_param_t;


typedef struct fts_param_listener
{
  fts_symbol_t name;
  fts_param_listener_fun_t listener_fun;
  void *listener;
  struct fts_param_listener *next;
} fts_param_listener_t;


static fts_heap_t *param_heap;
static fts_heap_t *param_listener_heap;

static fts_param_t *param_list = 0;
static fts_param_listener_t *param_listener_list = 0;

static void fts_param_run_listeners(fts_symbol_t name, const fts_atom_t *value, const void *author);

/*
 * Note that a Parameter can't be removed.
 */

void fts_param_set(fts_symbol_t name, const fts_atom_t *value)
{
  fts_param_set_by(name, value, 0);
}

void fts_param_set_by(fts_symbol_t name, const fts_atom_t *value, const void *author)
{
  fts_param_t *p;
  int value_not_changed = 0;

  /* First, check if there, otherwise add it */
  for (p = param_list; p ; p = p->next)
    if (p->name == name)
      {
	value_not_changed = fts_atom_are_equals( &p->value, value);
	p->value = *value;
	break;
      }

  if (! p)
    {
      /* Not found, allocate a new one */
      p = (fts_param_t *) fts_heap_alloc(param_heap);
      p->name  = name;
      p->value = *value;
      p->next  = param_list;

      param_list = p;
    }

  /* Second, run the listeners */
  if ( !value_not_changed)
    {
      fts_param_run_listeners(name, value, author);
    }
}


const fts_atom_t *fts_param_get(fts_symbol_t name)
{
  fts_param_t *p;

  for (p = param_list; p ; p = p->next)
    if (p->name == name)
      return &(p->value);

  return 0;
}

/* Note that is a parameter is already set, the listener is called
   at "adding" time to tell him the current value; note also that a listener
   can be added multiple times, also for the same couple value.
   */


void fts_param_add_listener(fts_symbol_t name, void *listener, fts_param_listener_fun_t listener_fun)
{
  fts_param_listener_t *p;
  fts_param_t *pp;

  /* Add the listener to the list */

  p = (fts_param_listener_t *) fts_heap_alloc(param_listener_heap);
  p->name         = name;
  p->listener_fun = listener_fun;
  p->listener     = listener;
  p->next         = param_listener_list;

  param_listener_list = p;

  /* Run the listener if the value is already defined */

  for (pp = param_list; pp ; pp = pp->next)
    if (pp->name == name)
      {
	(* p->listener_fun)(p->listener, p->name, &(pp->value));
	break;
      }
}


static void fts_param_run_listeners(fts_symbol_t name, const fts_atom_t *value, const void *author)
{
  fts_param_listener_t *p;

  for (p = param_listener_list; p ; p = p->next)
    if ((p->name == name) && (author == 0 || p->listener != author))
      {
	(* p->listener_fun)(p->listener, name, value);
      }
}


void fts_param_remove_listener(void *listener)
{
  fts_param_listener_t **pp;	/* indirect precursor */

  pp = &(param_listener_list);

  while (*pp)
    {
      if (listener == (*pp)->listener)
	{
	  fts_param_listener_t *p;

	  p = (*pp);
	  (*pp) = (*pp)->next;
	  fts_heap_free((char *) p, param_listener_heap);
	}
      else
	pp = &((*pp)->next);
    }
}


float fts_param_get_float(fts_symbol_t name, float default_value)
{
  const fts_atom_t *v;

  v = fts_param_get(name);

  if (v && fts_is_number(v))
    return fts_get_number_float(v);
  else
    return default_value;
}


int fts_param_get_int(fts_symbol_t name, int default_value)
{
  const fts_atom_t *v;

  v = fts_param_get(name);

  if (v && fts_is_int(v))
    return fts_get_int(v);
  else
    return default_value;
}

fts_symbol_t fts_param_get_symbol(fts_symbol_t name, fts_symbol_t default_value)
{
  const fts_atom_t *v;

  v = fts_param_get(name);

  if (v && fts_is_symbol(v))
    return fts_get_symbol(v);
  else
    return default_value;
}


void fts_param_set_float(fts_symbol_t name,  float value)
{
  fts_atom_t a;

  fts_set_float(&a, value);
  fts_param_set(name, &a);
}


void fts_param_set_float_by(fts_symbol_t name,  float value, void *author)
{
  fts_atom_t a;

  fts_set_float(&a, value);
  fts_param_set_by(name, &a, author);
}


void fts_param_set_int(fts_symbol_t name,  int value)
{
  fts_atom_t a;

  fts_set_int(&a, value);
  fts_param_set(name, &a);
}


void fts_param_set_int_by(fts_symbol_t name,  int value, void *author)
{
  fts_atom_t a;

  fts_set_int(&a, value);
  fts_param_set_by(name, &a, author);
}


void fts_param_set_symbol(fts_symbol_t name,  fts_symbol_t value)
{
  fts_atom_t a;

  fts_set_symbol(&a, value);
  fts_param_set(name, &a);
}


void fts_param_set_symbol_by(fts_symbol_t name,  fts_symbol_t value, void *author)
{
  fts_atom_t a;

  fts_set_symbol(&a, value);
  fts_param_set_by(name, &a, author);
}


void fts_params_init()
{
  param_heap = fts_heap_new(sizeof(fts_param_t));
  param_listener_heap = fts_heap_new(sizeof(fts_param_listener_t));
}
