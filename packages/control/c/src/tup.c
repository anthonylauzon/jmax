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
 *  local tuple methods
 *
 */

static void
tuple_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_tuple_init(this, 0, 0);
}

static void
tuple_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_tuple_destroy(this);
}

static void
tuple_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  if(fts_tuple_get_size(this) > 0)
  {
    fts_object_t *out = fts_object_create(fts_tuple_class, 0, 0);

    fts_tuple_copy(this, (fts_tuple_t *)out);

    fts_object_refer(out);
    fts_outlet_object(o, 0, out);
    fts_object_release(out);
  }
}

/************************************************
 *
 *  tup
 *
 */

#define TUP_MAX_SIZE (sizeof(unsigned int) * 8)

static fts_symbol_t sym_mode = 0;

static fts_symbol_t sym_all = 0;
static fts_symbol_t sym_none = 0;

static fts_symbol_t sym_any = 0;
static fts_symbol_t sym_left = 0;
static fts_symbol_t sym_right = 0;

typedef struct _tup_
{
  fts_tuple_t tuple;
  unsigned int trigger; /* control bits: trigger on input at given inlets */
  unsigned int require; /* control bits: require input on given inlets */
  unsigned int reset; /* control bits: reset memory of given inputs after on each input */
  unsigned int wait; /* status bits: wait for input at given inlet before output */
} tup_t;

static void
tup_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  fts_atom_t *atoms = fts_tuple_get_atoms(&this->tuple);
  int size = fts_tuple_get_size(&this->tuple);

  if(ac > 0)
  {
    int i;

    if(ac > size)
      ac = size;

    for(i=0; i<ac; i++)
      fts_atom_assign(atoms + i, at + i);
  }
}

static void
tup_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  unsigned int bit = 1 << winlet;
  fts_atom_t *atoms = fts_tuple_get_atoms(&this->tuple);

  fts_atom_assign(atoms + winlet, at);

  this->wait &= ~bit;

  if(!this->wait && (this->trigger & bit))
  {
    this->wait |= this->reset & this->require;
    tuple_output(o, 0, 0, 0, 0);
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
  int n = fts_tuple_get_size(&this->tuple);

  tup_set_bits(&this->trigger, n, at, 1);
}

static void
tup_set_require(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  unsigned int once = 0;
  int n = fts_tuple_get_size(&this->tuple);

  tup_set_bits(&this->require, n, at, 1);
  tup_set_bits(&once, n, at, -1);

  this->reset = ~once;
  this->wait = this->require | once;
}

static void
tup_set_mode(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int n = fts_tuple_get_size(&this->tuple);
  fts_symbol_t mode = fts_get_symbol(at);

  if(mode == sym_any)
  {
    this->trigger = (1 << n) - 1;
    this->reset = 0;
    this->require = 0;
  }
  else if(mode == sym_all)
    this->trigger = this->require = this->reset = this->wait = (1 << n) - 1;
  else if(mode == sym_left)
  {
    this->trigger = 1;
    this->reset = 0;
    this->require = 0;
  }
  else if(mode == sym_right)
  {
    this->trigger = (1 << (n - 1));
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

static void
tup_set_mode_any(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int n = fts_tuple_get_size(&this->tuple);

  this->trigger = (1 << n) - 1;
  this->reset = 0;
  this->require = 0;
  this->wait = 0;
}
static void
tup_set_mode_all(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int n = fts_tuple_get_size(&this->tuple);

  this->trigger = this->require = this->reset = this->wait = (1 << n) - 1;
}

static void
tup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int i, n;

  fts_tuple_init(&this->tuple, 0, 0);
  
  switch(ac)
  {
    case 0:
      n = 2;
      fts_tuple_append_int(&this->tuple, 0);
      fts_tuple_append_int(&this->tuple, 0);

      break;

    case 1:
      if(fts_is_number(at))
      {
        n = fts_get_number_int(at);

        if(n < 2)
          n = 2;
        else if(n > TUP_MAX_SIZE)
          n = TUP_MAX_SIZE;

        for(i=0; i<n; i++)
          fts_tuple_append_int(&this->tuple, 0);
      }
      else
      {
        fts_object_error(o, "bad argument");
        return;
      }

      break;

    default:
      if(ac > TUP_MAX_SIZE)
        n = TUP_MAX_SIZE;
      else
        n = ac;

      fts_tuple_append(&this->tuple, n, at);

      break;
    }

  fts_object_set_inlets_number(o, n);

  this->trigger = 1;
  this->reset = 0;
  this->require = 0;
}

static void
tup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(tup_t), tup_init, tuple_delete);

  fts_class_message_varargs(cl, fts_s_set, tup_set);
    
  fts_class_message_symbol(cl, fts_new_symbol("mode"), tup_set_mode);
  fts_class_message_void(cl, fts_new_symbol("any"), tup_set_mode_any);
  fts_class_message_void(cl, fts_new_symbol("all"), tup_set_mode_all);

  fts_class_message_atom(cl, fts_new_symbol("trigger"), tup_set_trigger);
  fts_class_message_atom(cl, fts_new_symbol("require"), tup_set_require);

  fts_class_inlet_bang(cl, 0, tuple_output);
  
  fts_class_inlet_atom(cl, 0, tup_input);
  fts_class_inlet_atom(cl, 1, tup_input);

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
untup_primitive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_atom(o, 0, at);
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
	fts_outlet_atom(o, i, at + i);
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

  fts_class_inlet_number(cl, 0, untup_primitive);
  fts_class_inlet_symbol(cl, 0, untup_primitive);
  fts_class_inlet_varargs(cl, 0, untup_varargs);

  fts_class_outlet_atom(cl, 0);
}

/************************************************
 *
 *  cotup
 *
 */

static void
cotup_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_tuple_clear(this);
}

static void
cotup_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  
  tuple_output(o, 0, NULL, 0, NULL);
  fts_tuple_clear(this);
}

static void
cotup_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_tuple_append(this, 1, at);
}

static void
cotup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_tuple_t), tuple_init, tuple_delete);

  fts_class_message_void(cl, fts_s_clear, cotup_clear);
  fts_class_message_void(cl, fts_s_flush, cotup_flush);
  fts_class_message_atom(cl, fts_s_append, cotup_append);

  fts_class_inlet_bang(cl, 0, tuple_output);
  fts_class_inlet_atom(cl, 1, cotup_append);
  
  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  detup
 *
 */

static void
detup_primitive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_atom(o, 0, at);
}

static void
detup_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      if(!fts_is_void(at + i))
	fts_outlet_atom(o, 0, at + i);
    }
}

static void
detup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), NULL, NULL); 

  fts_class_inlet_number(cl, 0, detup_primitive);
  fts_class_inlet_symbol(cl, 0, detup_primitive);
  fts_class_inlet_varargs(cl, 0, detup_varargs);

  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  getup
 *
 */

static void
getup_object(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_is_object(at))
  {
    fts_object_t *obj = fts_get_object(at);

    fts_set_void(fts_get_return_value());

    fts_send_message_varargs(obj, fts_s_get_tuple, 0, NULL);

    /* output array */
    if(!fts_is_void(fts_get_return_value()))
    {
      fts_atom_refer(fts_get_return_value());
      fts_outlet_atom(o, 0, fts_get_return_value());
      fts_atom_release(fts_get_return_value());
    }
    else
      fts_object_error(o, "cannot get tuple from %s object", fts_object_get_class_name(obj));
  }
  else
    fts_outlet_atom(o, 0, at);
}

static void
getup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_tuple_t), tuple_init, tuple_delete);

  fts_class_inlet_atom(cl, 0, getup_object);
  fts_class_outlet_varargs(cl, 0);
}

/************************************************
 *
 *  messtup
 * 
 */ 

static void
messtup_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_is_symbol(at))
    fts_outlet_message(o, 0, fts_get_symbol(at), ac - 1, at + 1);
  else
    fts_object_error(o, "tuple doesn't start with a symbol");  
}

static void
messtup_message(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_tuple_append(this, ac, at);

  tuple_output(o, 0, NULL, 0, NULL);
}

static void
messtup_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  
  if(s == 0)
    fts_invoke_varargs(messtup_varargs, o, ac, at);
  else
  {
    /* set tuple to selector */
    fts_tuple_clear(this);
    fts_tuple_append_symbol(this, s);

    /* append the arguments  */
    fts_invoke_varargs(messtup_message, o, ac, at);
  }
}

static void
messtup_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_tuple_t), tuple_init, tuple_delete);

  fts_class_input_handler(cl, messtup_input);
  
  fts_class_outlet_thru(cl, 0);
}

/************************************************
 *
 *  config
 *
 */

void
tup_config(void)
{
  sym_mode = fts_new_symbol("mode");
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
  fts_class_install(fts_new_symbol("messtup"), messtup_instantiate);
}
