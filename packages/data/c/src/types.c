/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "types.h"

static fts_hash_table_t data_type_id_table;
static fts_symbol_t data_type_array[MAX_N_DATA_TYPES];

int n_data_types = 0;

int
data_type_get_id(fts_symbol_t type)
{
  fts_atom_t atom;

  if(fts_hash_table_lookup(&data_type_id_table, type, &atom))
    return fts_get_int(&atom);
  else
    {
      int type_id = n_data_types;
      n_data_types++;

      fts_set_int(&atom, type_id);
      fts_hash_table_insert(&data_type_id_table, type, &atom);
      
      data_type_array[type_id] = type;
      
      return type_id;
    }
} 

fts_symbol_t 
data_type_get(int id)
{
  return data_type_array[id];
}

void
data_types_init(void)
{
  fts_hash_table_init(&data_type_id_table);
  data_type_get_id(fts_s_void); /* void is type id 0 */
}
