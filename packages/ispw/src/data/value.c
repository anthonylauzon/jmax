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

static fts_hash_table_t value_table;

static struct value_keeper *
get_keeper(fts_symbol_t s)
{
  fts_atom_t data;
  struct value_keeper *v;

  if (fts_hash_table_lookup(&value_table, s, &data))
    v = (struct value_keeper *)fts_get_ptr(&data);
  else
    {
      v = (struct value_keeper *) fts_malloc(sizeof(struct value_keeper));
      v->sym = s;
      v->count = 0;
      fts_set_long(&(v->atom), 0);
      fts_set_ptr(&data, v);
      fts_hash_table_insert(&value_table, s, &data);
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
      fts_hash_table_remove(&value_table, v->sym);
      fts_free(v);
    }
}


/******************************************************************************/
/*                                                                            */
/*        The VALUE object                                                    */
/*                                                                            */
/******************************************************************************/

/* remember a value. Use a private hash table, so  a
 private name space for values; in MP versions, the values
 are local to a processor.*/


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
  
  fts_outlet_send(o, 0, fts_get_type(&a), 1, &a);
}

/* Installed for int, floats and symbols */

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
  fts_symbol_t sym = fts_get_symbol_arg(ac, at, 1, 0);

  x->v = get_keeper(sym);

  if (ac >= 3)
    x->v->atom = at[2];
  else
    fts_set_long(&x->v->atom, 0);

  x->name = sym;
}

static void
value_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  value_t *x = (value_t *)o;

  free_keeper(x->v);
}

static fts_status_t
value_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(value_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_anything;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, value_init, 3, a, 2);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, value_delete, 0, 0);

  /* Value args */
  fts_method_define(cl, 0, fts_s_bang, value_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, value_scalar, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, value_scalar, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_symbol, value_scalar, 1, a);

  return fts_Success;
}


void
value_config(void)
{
  fts_hash_table_init(&value_table);

  fts_class_install(fts_new_symbol("value"),value_instantiate);
  fts_class_alias(fts_new_symbol("v"), fts_new_symbol("value"));
}

