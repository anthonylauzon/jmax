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

extern int n_data_types;

extern int data_type_get_id(fts_symbol_t type);
extern fts_symbol_t data_type_get(int id);

#endif
