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

/* primitive types */
enum data_type_primitive {data_type_id_void, data_type_id_int, data_type_id_float, data_type_id_symbol, data_type_n_primitive};
enum data_type_complex {data_type_id_integer_vector = data_type_n_primitive, data_type_id_float_vector};

extern int n_data_types;

extern int data_type_get_id(fts_symbol_t type);
extern fts_symbol_t data_type_get(int id);

#define data_type_id_is_primitive(id) ((id) < data_type_n_primitive)

#endif
