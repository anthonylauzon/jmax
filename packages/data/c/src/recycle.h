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

#ifndef _RECYCLE_H_
#define _RECYCLE_H_

extern fts_integer_vector_t *data_recycle_atom_integer_vector(fts_atom_t *atom, int size);
extern fts_float_vector_t *data_recycle_atom_float_vector(fts_atom_t *atom, int size);

extern fts_integer_vector_t *data_recycle_integer_vector_replicate(fts_integer_vector_t *old);
extern fts_float_vector_t *data_recycle_float_vector_replicate(fts_float_vector_t *old);

#endif
