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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* 
 * The value object it is stilllonger implemented using through,
 * but should de moved to value.c, using a symbol/value table.
 */


#include <fts/fts.h>

struct value_keeper
{
  fts_atom_t atom;
  fts_symbol_t sym;
  int count;
};

static fts_symbol_t sym_v = 0;
static fts_hashtable_t value_table;

static struct value_keeper *
get_keeper(fts_symbol_t s)
{
  fts_atom_t data, k;
  struct value_keeper *v;

  fts_set_symbol( &k, s);

  if (fts_hashtable_get(&value_table, &k, &data))
    v = (struct value_keeper *)fts_get_pointer(&data);
  else
    {
      v = (struct value_keeper *) fts_malloc(sizeof(struct value_keeper));
      v->sym = s;
      v->count = 0;
      fts_set_int(&(v->atom), 0);

      fts_set_pointer(&data, v);
      fts_hashtable_put(&value_table, &k, &data);
    }

  v->count++;
  return v;
}

static void
free_keeper(struct value_keeper *v)
{
  v->count--;
  
  if (v->count == 0)
    {
      fts_atom_t k;

      fts_set_symbol( &k, v->sym);
      fts_hashtable_remove( &value_table, &k);
      fts_free(v);
    }
}

typedef struct value
{
  fts_object_t ob;
  struct value_keeper *v;
  fts_symbol_t name;
} value_t;

static void
value_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *x = (value_t *)o;
  fts_atom_t a = x->v->atom;
  
  fts_outlet_varargs(o, 0, 1, &a);
}

static void
value_scalar(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *x = (value_t *)o;

  x->v->atom = at[0];
}

static void
value_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *x = (value_t *)o;
  fts_symbol_t sym = fts_get_symbol_arg(ac, at, 0, 0);

  x->v = get_keeper(sym);

  if (ac >= 2)
    x->v->atom = at[1];
  else
    fts_set_int(&x->v->atom, 0);

  x->name = sym;
}

static void
value_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *x = (value_t *)o;

  free_keeper(x->v);
}

static void
value_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(value_t), value_init, value_delete);

  fts_class_message_varargs(cl, fts_s_bang, value_bang);

  fts_class_inlet_int(cl, 0, value_scalar);
  fts_class_inlet_float(cl, 0, value_scalar);
  fts_class_inlet_symbol(cl, 0, value_scalar);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_float(cl, 0);
  fts_class_outlet_symbol(cl, 0);
}

void
ispw_value_config(void)
{
  fts_hashtable_init(&value_table, 0, FTS_HASHTABLE_MEDIUM);

  sym_v = fts_new_symbol("v");

  fts_class_install(sym_v, value_instantiate);
}
