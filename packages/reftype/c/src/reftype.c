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

#include "fts.h"
#include "reftype.h"

#define MAX_N_REFTYPE 256

static fts_hash_table_t reftype_table;

static reftype_t reftype_array[MAX_N_REFTYPE];
static int retype_cnt = 0;

reftype_t *
reftype_declare(fts_symbol_t sym, reftype_dispatcher_t dispatcher, reftype_destructor_t destructor)
{
  fts_atom_t a;
  int i = retype_cnt++;

  reftype_array[i].sym = sym;
  reftype_array[i].dispatcher = dispatcher;
  reftype_array[i].destructor = destructor;

  fts_set_ptr(&a, reftype_array + i);
  fts_hash_table_insert(&reftype_table, sym, &a);

  return (reftype_array + i);
}

reftype_t *
reftype_get_by_symbol(fts_symbol_t sym)
{
  fts_atom_t a;

  if(fts_hash_table_lookup(&reftype_table, sym, &a))
    return (reftype_t *)fts_get_ptr(&a);

  return 0;
}

/**********************************************************
 *
 *  fts module
 *
 */

extern void int_vector_config(void);
extern void float_vector_config(void);
extern void vector_config(void);
extern void matrix_config(void);

static void
reftype_init(void)
{
  fts_hash_table_init(&reftype_table);

  int_vector_config();
  float_vector_config();
  vector_config();
  matrix_config();
}

fts_module_t reftype_module = {"reftype", "reftype classes", reftype_init};














