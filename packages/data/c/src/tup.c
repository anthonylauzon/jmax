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

void
fts_outlet_array(fts_object_t *o, int woutlet, fts_array_t *array)
{
  int size = fts_array_get_size(array);
  fts_atom_t *atoms = fts_array_get_atoms(array);  
  fts_atom_t output[size];
  int i;
  
  for(i=0; i<size; i++)
    {
      fts_set_void(output + i);
      fts_atom_assign(output + i, atoms + i);
    }
  
  fts_outlet_send(o, woutlet, fts_s_list, size, output);
  
  for(i=0; i<size; i++)
    fts_set_void(output + i);
}

void
fts_outlet_atom(fts_object_t *o, int woutlet, const fts_atom_t *at)
{
  if(fts_is_array(at))
    {
      fts_array_t *array = fts_get_array(at);
      int size = fts_array_get_size(array);
      fts_atom_t *atoms = fts_array_get_atoms(array);

      if(size == 1)
	{
	  fts_atom_t *atom = fts_array_get_element(array, 0);
	  fts_outlet_send(o, woutlet, fts_get_selector(atom), 1, atom);
	}
      else
	fts_outlet_array(o, woutlet, array);
    }
  /* else if(fts_is_message(at)) {} */
  else
    fts_outlet_send(o, woutlet, fts_get_selector(at), 1, at);
}

#define SYNC_MAX_SIZE (sizeof(unsigned int))

static fts_symbol_t sym_all = 0;
static fts_symbol_t sym_none = 0;

static fts_symbol_t sym_any = 0;
static fts_symbol_t sym_left = 0;
static fts_symbol_t sym_right = 0;

typedef struct _tup_
{
  fts_object_t o;
  unsigned int trigger; /* control bits: trigger on input at given inlets */
  unsigned int require; /* control bits: require input on given inlets */
  unsigned int reset; /* control bits: reset memory of given inputs after on each input */
  unsigned int wait; /* status bits: wait for input at given inlet before output */
  fts_atom_t a[SYNC_MAX_SIZE];
  enum {mode_all, mode_select} mode;
} tup_t;

typedef struct _untup_
{
  fts_object_t o;
  int n;
} untup_t;

/************************************************
 *
 *  user methods
 *
 */

static void
tup_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  fts_array_set(&this->array, ac, at);
}

static void
tup_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  int size = fts_array_get_size(&this->array);
  fts_atom_t *atoms = fts_array_get_atoms(&this->array);
  fts_atom_t output[size];
  int i;
  
  for(i=0; i<size; i++)
    {
      fts_set_void(output + i);
      fts_atom_assign(output + i, atoms + i);
    }

  fts_outlet_send(o, 0, fts_s_list, size, output);

  for(i=0; i<size; i++)
    fts_set_void(output + i);
}

static void
tup_input_primitive_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;
  unsigned int bit = 1 << winlet;
  
  fts_atom_assign(this->a + winlet, at);
      
  this->wait &= ~bit;
  
  if(!this->wait && (this->trigger & bit))
    {
      sync_output(this);
      this->wait |= this->reset & this->require;
    }

  fts_array_set_element(&this->array, winlet, at);

  if(winlet == 0)
    tup_output(o, 0, 0, 0, 0);
}

static void
tup_input_any_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  if(ac == 1 && fts_get_selector(at) == s)
    {
      fts_array_set_element(&this->array, winlet, at);

      if(winlet == 0)
	tup_output(o, 0, 0, 0, 0);
    }
  else
    fts_object_signal_runtime_error(o, "doesn't accept message on right inlet");
}

static void
tup_input_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  switch(ac)
    {
    default:
      {
	fts_array_t *array = (fts_array_t *)fts_object_create(fts_array_class, ac, at);
	fts_atom_t a;

	fts_set_array(&a, array);
	fts_array_set_element(&this->array, winlet, &a);
      }
      break;
    case 1:
      fts_array_set_element(&this->array, winlet, at);
      break;
    case 0:
      break;
    }

  if(winlet == 0)
    tup_output(o, 0, 0, 0, 0);
}

static void
untup_input_primitive_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;

  fts_outlet_atom(o, 0, at);
}

static void
untup_input_any_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;

  if(ac == 1 && fts_get_selector(at) == s)
    fts_outlet_atom(o, 0, at);
  else
    fts_object_signal_runtime_error(o, "cannot untup accept message");
}

static void
untup_input_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;
  int n = this->n;
  int i;

  if(n < ac)
    n = ac;

  for(i=n-1; i>=0; i--)
    fts_outlet_atom(o, i, at + i);
}

/************************************************
 *
 *    class
 *
 */

static void
tup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  ac--;
  at++;

  if(ac == 1 && fts_is_int(at))
    {
      int n = fts_get_int(at);

      if(n < 2)
	n = 2;

      fts_array_init(&this->array, 0, 0);
      fts_array_set_size(&this->array, n);
    }
  else
    {
      fts_array_init(&this->array, ac, at);
      
      if(ac < 2)
	fts_array_set_size(&this->array, 2);
    }
}

static void
tup_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tup_t *this = (tup_t *)o;

  fts_array_destroy(&this->array);
}

static void
untup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  untup_t *this = (untup_t *)o;
  int n = 0;

  ac--;
  at++;

  if(ac == 1 && fts_is_int(at))
    n = fts_get_int(at);

  if(n < 2)
    n = 2;

  this->n = n;
}

static fts_status_t
tup_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n = 0;
  int i;

  ac--;
  at++;

  if(ac == 1 && fts_is_int(at))
    n = fts_get_int(at);
  else
    n = ac;

  if(n < 2)
    n = 2;

  fts_class_init(cl, sizeof(tup_t), n, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, tup_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, tup_delete);

  fts_method_define_varargs(cl, 0, fts_s_bang, tup_output);
  fts_method_define_varargs(cl, 0, fts_s_set, tup_set);

  for(i=0; i<n; i++)
    {
      fts_method_define_varargs(cl, i, fts_s_int, tup_input_primitive_value);
      fts_method_define_varargs(cl, i, fts_s_float, tup_input_primitive_value);
      fts_method_define_varargs(cl, i, fts_s_symbol, tup_input_primitive_value);
      fts_method_define_varargs(cl, i, fts_s_list, tup_input_array);
      fts_method_define_varargs(cl, i, fts_s_anything, tup_input_any_value);
    }

  return fts_Success;
}

static fts_status_t
untup_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n = 0;

  ac--;
  at++;

  if(ac == 1 && fts_is_int(at))
    n = fts_get_int(at);
  
  if(n < 2)
    n = 2;

  fts_class_init(cl, sizeof(untup_t), 1, n, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, untup_init);

  fts_method_define_varargs(cl, 0, fts_s_int, untup_input_primitive_value);
  fts_method_define_varargs(cl, 0, fts_s_float, untup_input_primitive_value);
  fts_method_define_varargs(cl, 0, fts_s_symbol, untup_input_primitive_value);
  fts_method_define_varargs(cl, 0, fts_s_anything, untup_input_any_value);
  fts_method_define_varargs(cl, 0, fts_s_list, untup_input_array);

  return fts_Success;
}

void
tup_config(void)
{
  fts_metaclass_install(fts_new_symbol("tup"), tup_instantiate, fts_arg_equiv);
  fts_class_install(fts_new_symbol("untup"), untup_instantiate);
}
