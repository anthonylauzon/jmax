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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#if HAVE_ALLOCA_H
#include <alloca.h>
#endif


/************************************************
 *
 *  tup
 *
 */

#define TUP_MAX_SIZE (sizeof(unsigned int) * 8)

static fts_symbol_t sym_all = 0;
static fts_symbol_t sym_none = 0;

static fts_symbol_t sym_any = 0;
static fts_symbol_t sym_left = 0;
static fts_symbol_t sym_right = 0;

typedef struct _tup_
{
  fts_object_t o;
  int n;
  unsigned int trigger; /* control bits: trigger on input at given inlets */
  unsigned int require; /* control bits: require input on given inlets */
  unsigned int reset; /* control bits: reset memory of given inputs after on each input */
  unsigned int wait; /* status bits: wait for input at given inlet before output */
  fts_atom_t a[TUP_MAX_SIZE];
  enum {mode_all, mode_select} mode;
} tup_t;

static void
tup_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int n = this->n;
  fts_atom_t *a = this->a;
  int i;
  
  if(ac > n)
    ac = n;

  for(i=0; i<ac; i++)
    fts_atom_assign(a + i, at + i);
}

static void
tup_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int n = this->n;
  fts_atom_t *a = this->a;
  fts_atom_t *output = (fts_atom_t *)alloca(sizeof(fts_atom_t) * n);
  int i;
  
  this->wait |= this->reset & this->require;

  for(i=0; i<n; i++)
    {
      fts_set_void(output + i);
      fts_atom_assign(output + i, a + i);
    }

  fts_outlet_send(o, 0, fts_s_list, n, output);

  for(i=0; i<n; i++)
    fts_atom_void(output + i);
}

static void
tup_input_single(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  unsigned int bit = 1 << winlet;
  
  fts_atom_assign(this->a + winlet, at);
      
  this->wait &= ~bit;
  
  if(!this->wait && (this->trigger & bit))
    tup_output(o, 0, 0, 0, 0);
}

static void
tup_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  if(ac == 1)
    tup_input_single(o, winlet, s, 1, at);
  else
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)tuple);
      tup_input_single(o, winlet, s, 1, &a);
    }
}

static void
tup_set_bits(unsigned int *bits, int n, const fts_atom_t *at, int sign)
{
  if(fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == sym_all)
	*bits = (1 << n) - 1;
      else if(mode == sym_none)
	*bits = 0;
    }
  else if(fts_is_number(at))
    {
      int in = fts_get_number_int(at) * sign;

      if(in >= 0 && in < n)
	*bits = 1 << in;
    }
  else if(fts_is_tuple(at))
    {
      fts_tuple_t *l = (fts_tuple_t *)fts_get_object(at);
      fts_atom_t *a = fts_tuple_get_atoms(l);
      int size = fts_tuple_get_size(l);
      int i;

      *bits = 0;

      for(i=0; i<size; i++)
	{
	  if(fts_is_number(a + i))
	    {
	      int in = fts_get_number_int(a + i) * sign;
	      
	      if(in >= 0 && in < n)
		*bits |= 1 << in;
	    }  
	}
    }
}

static void
tup_set_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  tup_set_bits(&this->trigger, this->n, at, 1);
}

static void
tup_set_trigger_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  tup_set_trigger(o, 0, 0, 1, value);
}

static void
tup_set_require(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  unsigned int once = 0;

  tup_set_bits(&this->require, this->n, at, 1);
  tup_set_bits(&once, this->n, at, -1);

  this->reset = ~once;
  this->wait = this->require | once;
}

static void
tup_set_require_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  tup_set_require(o, 0, 0, 1, value);
}

static void
tup_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  if(fts_is_symbol(at))
    {
      fts_symbol_t mode = fts_get_symbol(at);
      
      if(mode == sym_any)
	{
	  this->trigger = (1 << this->n) - 1;
	  this->reset = 0;
	  this->require = 0;
	}
      else if(mode == sym_all)
	this->trigger = this->require = this->reset = this->wait = (1 << this->n) - 1;
      else if(mode == sym_left)
	{
	  this->trigger = 1;
	  this->reset = 0;
	  this->require = 0;
	}
      else if(mode == sym_right)
	{
	  this->trigger = (1 << (this->n - 1));
	  this->reset = 0;
	  this->require = 0;
	}
      else if(mode == sym_none)
	{
	  this->trigger = 0;
	  this->reset = 0;
	  this->require = 0;
	}

      this->wait = this->require;
    }
}

static void
tup_set_mode_prop(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  tup_set_mode(o, 0, 0, 1, value);
}

static void
tup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int n = 0;
  int i;

  /* void state */
  for(i=0; i<TUP_MAX_SIZE; i++)
    fts_set_int(this->a + i, 0);

  switch(ac)
    {
    case 0:
      n = 2;

      break;

    case 1:
      if(fts_is_number(at))
	{
	  n = fts_get_number_int(at);
	  
	  if(n < 2) 
	    n = 2;
	  else if(n > TUP_MAX_SIZE)
	    n = TUP_MAX_SIZE;
	}
      else
	{
	  fts_object_set_error(o, "Bad argument");
	  return;
	}

      break;

    default:
      if(ac > TUP_MAX_SIZE)
	ac = TUP_MAX_SIZE;
      
      n = ac;
      
      for(i=0; i<n; i++)
	fts_atom_assign(this->a + i, at + i);
      
      break;
    }

  fts_object_set_inlets_number(o, n);

  this->n = n;
  this->trigger = 1;
  this->reset = 0;
  this->require = 0;
}

static void
tup_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int i;

  /* void state */
  for(i=0; i<this->n; i++)
    fts_set_void(this->a + i);
}

static void
tup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tup_t), tup_init, tup_delete);

  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("trigger"), tup_set_trigger_prop);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("require"), tup_set_require_prop);
  fts_class_add_daemon(cl, obj_property_put, fts_new_symbol("mode"), tup_set_mode_prop);

  fts_class_message_varargs(cl, fts_s_bang, tup_output);
  fts_class_message_varargs(cl, fts_s_set, tup_set);

  fts_class_inlet_number(cl, 0, tup_input_single);
  fts_class_inlet_symbol(cl, 0, tup_input_single);
  fts_class_inlet_varargs(cl, 0, tup_varargs);

  fts_class_inlet_number(cl, 1, tup_input_single);
  fts_class_inlet_symbol(cl, 1, tup_input_single);
  fts_class_inlet_varargs(cl, 1, tup_varargs);

  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  untup
 *
 */

typedef struct _untup_
{
  fts_object_t o;
  int n;
} untup_t;

static void
untup_input_single(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;

  fts_outlet_varargs(o, 0, 1, at);
}

static void
untup_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;
  int n = this->n;
  int i;

  if(n > ac)
    n = ac;

  for(i=n-1; i>=0; i--)
    {
      if(!fts_is_void(at + i))
	fts_outlet_varargs(o, i, 1, at + i);
    }
}

static void
untup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;
  int n = 0;

  if(ac == 1 && fts_is_int(at))
    n = fts_get_int(at);

  if(n < 2)
    n = 2;

  this->n = n;
  fts_object_set_outlets_number(o, n);
}

static void
untup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(untup_t), untup_init, NULL);

  fts_class_inlet_int(cl, 0, untup_input_single);
  fts_class_inlet_float(cl, 0, untup_input_single);
  fts_class_inlet_symbol(cl, 0, untup_input_single);
  fts_class_inlet_varargs(cl, 0, untup_varargs);

  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  cotup
 *
 */

typedef struct _cotup_
{
  fts_object_t o;
  fts_array_t array;
} cotup_t;

static void
cotup_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_clear(&this->array);
  fts_array_append(&this->array, ac, at);
}

static void
cotup_append_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_append(&this->array, ac, at);
}

static void
cotup_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;
  int size = fts_array_get_size(&this->array);
  fts_atom_t *atoms = fts_array_get_atoms(&this->array);
  fts_atom_t *output = alloca(sizeof(fts_atom_t) * size);
  int i;

  for(i=0; i<size; i++)
    {
      output[i] = atoms[i];
      fts_atom_refer(output + i);
    }

  fts_outlet_varargs(o, 0, size, output);

  for(i=0; i<size; i++)
    fts_atom_release(output + i);
}

static void
cotup_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_clear(&this->array);
}

static void
cotup_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;
  int size = fts_array_get_size(&this->array);
  fts_atom_t *atoms = fts_array_get_atoms(&this->array);
  fts_atom_t *output = alloca(sizeof(fts_atom_t) * size);
  int i;

  for(i=0; i<size; i++)
    {
      output[i] = atoms[i];
      fts_atom_refer(output + i);
    }

  fts_array_clear(&this->array);

  fts_outlet_varargs(o, 0, size, output);

  for(i=0; i<size; i++)
    fts_atom_release(output + i);
}

static void
cotup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_init(&this->array, ac, at);
}

static void
cotup_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_destroy(&this->array);
}

static void
cotup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(cotup_t), cotup_init, cotup_delete);

  fts_class_message_varargs(cl, fts_s_set, cotup_set);
  fts_class_message_varargs(cl, fts_s_bang, cotup_output);
  fts_class_message_varargs(cl, fts_s_clear, cotup_clear);
  fts_class_message_varargs(cl, fts_s_flush, cotup_flush);

  fts_class_inlet_varargs(cl, 0, cotup_append_atoms);

  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  detup
 *
 */

static void
detup_input_single(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_varargs(o, 0, 1, at);
}

static void
detup_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      if(!fts_is_void(at + i))
	fts_outlet_varargs(o, 0, 1, at + i);
    }
}

static void
detup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), NULL, NULL); 

  fts_class_inlet_int(cl, 0, detup_input_single);
  fts_class_inlet_float(cl, 0, detup_input_single);
  fts_class_inlet_symbol(cl, 0, detup_input_single);
  fts_class_inlet_varargs(cl, 0, detup_varargs);

  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  getup
 *
 */

static void
getup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_init(&this->array, 0, 0);
}

static void
getup_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  fts_array_destroy(&this->array);
}

static void
getup_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cotup_t *this = (cotup_t *)o;

  if(s == 0)
    {
      if(ac == 1)
	{
	  if(fts_is_object(at))
	    {
	      cotup_t *this = (cotup_t *)o;
	      fts_object_t *input = fts_get_object(at);
	      fts_class_t *class = fts_object_get_class(input);
	      fts_method_t method = fts_class_get_method(class, fts_s_get_array);
	      
	      if(method)
		{
		  fts_atom_t a;
		  int size;
		  fts_atom_t *atoms;
		  fts_atom_t *output;
		  int i;
		  
		  fts_array_clear(&this->array);
		  
		  /* get object state as array */
		  fts_set_pointer(&a, &this->array);
		  method(input, 0, fts_s_get_array, 1, &a);
		  
		  /* output array */
		  cotup_output(o, 0, 0, 0, 0);
		}
	      else
		fts_object_signal_runtime_error(o, "Cannot get tuple from %s object", fts_object_get_class_name(input));
	    }
	  else
	    fts_outlet_varargs(o, 0, 1, at);
	}
      else
	fts_outlet_varargs(o, 0, ac, at);
    }
  else
    {
      fts_array_clear(&this->array);
      fts_array_append_symbol(&this->array, s);
      fts_array_append(&this->array, ac, at);

      cotup_output(o, 0, 0, 0, 0);
    }
}

static void
getup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(cotup_t), getup_init, getup_delete);

  fts_class_set_default_handler(cl, getup_input);
  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  config
 *
 */

void
tup_config(void)
{
  sym_all = fts_new_symbol("all");
  sym_none = fts_new_symbol("none");
  sym_any = fts_new_symbol("any");
  sym_left = fts_new_symbol("left");
  sym_right = fts_new_symbol("right");

  fts_class_install(fts_new_symbol("tup"), tup_instantiate);
  fts_class_install(fts_new_symbol("untup"), untup_instantiate);
  fts_class_install(fts_new_symbol("cotup"), cotup_instantiate);
  fts_class_install(fts_new_symbol("detup"), detup_instantiate);
  fts_class_install(fts_new_symbol("getup"), getup_instantiate);
}
