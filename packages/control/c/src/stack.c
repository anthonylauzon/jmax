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
 * Authors: Norbert Schnell.
 *
 */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_stack_t stack;
} stack_t;

/************************************************************
 *
 *  user methods
 *
 */

static void
stack_input_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stack_t *this = (stack_t *)o;
  fts_atom_t a;

  fts_set_void(&a);
  fts_atom_assign(&a, at);
  fts_stack_push(&this->stack, fts_atom_t, a);
}

static void
stack_input_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stack_t *this = (stack_t *)o;

  if(ac == 1)
    stack_input_atom(o, 0, 0, 1, at);
  else if(ac > 1)
    {
      fts_object_t *tuple = fts_object_create(fts_tuple_class, NULL, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, tuple);
      stack_input_atom(o, 0, 0, 1, &a);
    }
}

static void
stack_pop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stack_t *this = (stack_t *)o;
  fts_atom_t *stack = fts_stack_base(&this->stack);
  int top = fts_stack_top(&this->stack);
  int n = 1;

  if(ac > 0 && fts_is_number(at))
    {
      n = fts_get_number_int(at);
      
      if(n < 0)
	n = 0;
    }

  while(n-- && top >= 0)
    {
      fts_outlet_varargs(o, 0, 1, stack + top);
      fts_atom_void(stack + top);
      fts_stack_pop(&this->stack, 1);

      top = fts_stack_top(&this->stack);
    }
}

static void
stack_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stack_t *this = (stack_t *)o;
  fts_atom_t *stack = fts_stack_base(&this->stack);
  int top = fts_stack_top(&this->stack);

  while(top >= 0)
    {
      fts_outlet_varargs(o, 0, 1, stack + top);
      fts_atom_void(stack + top);
      fts_stack_pop(&this->stack, 1);
      top = fts_stack_top(&this->stack);
    }
}

static void
stack_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  stack_t *this = (stack_t *)o;
  fts_atom_t *stack = fts_stack_base(&this->stack);
  int top = fts_stack_top(&this->stack);

  while(top >= 0)
    {
      fts_atom_void(stack + top);
      fts_stack_pop(&this->stack, 1);
      top = fts_stack_top(&this->stack);
    }
}

/************************************************************
 *
 *  class
 *
 */
static void
stack_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  stack_t *this = (stack_t *)o;
  
  fts_stack_init(&this->stack, fts_atom_t);
}

static void
stack_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  stack_t *this = (stack_t *)o;

  fts_stack_destroy(&this->stack);
}

static void
stack_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(stack_t), stack_init, stack_delete);

  fts_class_inlet_number(cl, 0, stack_input_atom);
  fts_class_inlet_symbol(cl, 0, stack_input_atom);
  fts_class_inlet_varargs(cl, 0, stack_input_atoms);

  fts_class_message_varargs(cl, fts_s_bang, stack_pop);
  fts_class_message_varargs(cl, fts_new_symbol("pop"), stack_pop);
  fts_class_message_varargs(cl, fts_s_flush, stack_flush);
  fts_class_message_varargs(cl, fts_s_clear, stack_clear);

  fts_class_outlet_varargs(cl, 0);
}

void
stack_config(void)
{
  fts_class_install(fts_new_symbol("stack"), stack_instantiate);
}
