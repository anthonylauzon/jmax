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
#ifndef _DATA_H_
#define _DATA_H_

#include "atoms.h"
#include "types.h"
#include "utils.h"
#include "complexvector.h"

extern fts_data_t *data_create(fts_symbol_t type, int ac, const fts_atom_t *at);
extern void data_destroy(fts_data_t *data);
extern fts_data_t *data_replicate(fts_data_t *data);
extern void data_release(fts_data_t *data);

extern int data_get_size(fts_data_t *data);
extern int data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at);

#endif
