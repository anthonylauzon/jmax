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
#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_

#include "fts.h"

#define MAX_N_DATA_TYPES 64

/* system defined primitive types */
enum data_type_primitive {data_type_id_void, data_type_id_int, data_type_id_float, data_type_id_symbol, data_type_n_primitive};
/* system defined complex types */
enum data_type_complex {data_type_id_integer_vector = data_type_n_primitive, data_type_id_float_vector};

extern int n_data_types;

extern int data_type_get_id_by_name(fts_symbol_t type);
extern fts_symbol_t data_type_get(int id);

#define data_type_id_is_primitive(id) ((id) < data_type_n_primitive)

typedef fts_atom_t (* data_type_create_fun_t) (fts_atom_t *);
typedef void (* data_type_destroy_fun_t) (fts_atom_t *);
typedef void (* data_type_load_fun_t) (fts_atom_t *);

typedef struct _data_atom_load_fun_table_
{
  fts_hash_table_t table;
} data_type_load_fun_table_t;

typedef struct _data_type_
{
  fts_symbol_t name;
  data_type_create_fun_t create;
  data_type_destroy_fun_t destroy;
  data_type_load_fun_table_t load_fun_table;
} data_type_t;

extern void data_type_declare_create(fts_symbol_t type, data_type_create_fun_t fun);
extern void data_type_declare_destroy(fts_symbol_t type, data_type_destroy_fun_t fun);
extern void data_type_declare_load(fts_symbol_t type, data_type_load_fun_t fun);

extern fts_atom_t data_type_create(fts_symbol_t type, int ac, const fts_atom_t *at);
extern void data_type_destroy(fts_atom_t *atom);
extern fts_atom_t data_type_load(fts_symbol_t *file_name);


extern data_type_t data_type_table[MAX_N_DATA_TYPES];

#endif
