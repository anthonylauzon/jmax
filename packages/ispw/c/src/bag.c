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

/* The sustain object */

#include <fts/fts.h>

struct blink
{
  long value;
  struct blink *next;
  struct blink *prev;
};

typedef struct 
{
  fts_object_t ob;
  long in;
  struct blink *value_list;
  int multi;
} bag_t;

static void
bag_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bag_t *x = (bag_t *)o;
  long n   = (long) fts_get_int_arg(ac, at, 0, 0);
  struct blink *b;

  if (x->in)
    {
      if (!x->multi)
	{
	  for (b = x->value_list; b; b = b->next)
	    if (b->value == n)
	      return;
	}

      b = (struct blink *) fts_malloc(sizeof(struct blink));
      b->value = n;

      b->next = x->value_list;

      if (b->next)
	b->next->prev = b;

      b->prev = 0;
      x->value_list = b;
    }
  else
    {
      for (b = x->value_list; b; b = b->next)
	if (b->value == n)
	  {
	    if (x->value_list == b)
	      x->value_list = b->next;

	    if (b->next)
	      b->next->prev = b->prev;

	    if (b->prev)
	      b->prev->next = b->next;

	    fts_free(b);
	    return;
	  }
    }
}

static void
bag_cut(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bag_t *x = (bag_t *)o;
  struct blink *b;
  long value;
  
  if (!x->value_list)
    return;
  
  for (b = x->value_list; b->next; b = b->next)
    ;
  
  value = b->value;

  if (x->value_list == b)
    x->value_list = b->next;

  if (b->next)
    b->next->prev = b->prev;

  if (b->prev)
    b->prev->next = b->next;

  fts_free(b);

  fts_outlet_int(o, 0, value);
}

static void
bag_number_1(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bag_t *x = (bag_t *)o;

  x->in = fts_get_int_arg(ac, at, 0, 0);
}

/* list method */

static void
bag_list(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  if ((ac >= 2) && (fts_is_number(&at[1])))
    bag_number_1(o, s, 1, at + 1, fts_nix);

  if ((ac >= 1) && (fts_is_number(&at[0])))
    bag_number(o, s, 1, at + 0, fts_nix);
}


static void
bag_bang(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bag_t *x = (bag_t *)o;
  struct blink *b;

  for (b = x->value_list; b; b = b->next)
    fts_outlet_int(o, 0, b->value);
}

/* Also installed for $delete */

static void
bag_clear(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bag_t *x = (bag_t *)o;
  struct blink *b, *b2;

  for (b = x->value_list; b; b = b2)
    {
      b2 = b->next;
      fts_free(b);
    }

  x->value_list = 0;
}

/* Symbol optional */

static void
bag_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  bag_t *x = (bag_t *)o;
  fts_symbol_t s_arg = fts_get_symbol_arg(ac, at, 0, 0);

  x->value_list = 0;
  x->multi = (s_arg != 0);
  x->in = 0;
}


static void
bag_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(bag_t), bag_init, bag_clear);

  fts_class_inlet_bang(cl, 0, bag_bang);
  fts_class_message_varargs(cl, fts_s_clear, bag_clear);

  fts_class_message_varargs(cl, fts_new_symbol("cut"), bag_cut);

  fts_class_inlet_varargs(cl, 0, bag_list);
  fts_class_inlet_number(cl, 0, bag_number);
  fts_class_inlet_number(cl, 1, bag_number_1);

  fts_class_outlet_int(cl, 0);
}


void
bag_config(void)
{
  fts_class_install(fts_new_symbol("bag"),bag_instantiate);
}
