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
#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include "fts.h"
#include "types.h"
#include "recycle.h"

typedef fts_atom_t * (* operator_fun_t) (fts_atom_t *);
typedef operator_fun_t (*operator_matrix_t)[MAX_N_DATA_TYPES];

#endif
