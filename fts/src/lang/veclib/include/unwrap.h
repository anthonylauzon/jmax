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
/* unwrap.h */

#include "lang/mess.h"

/*
 *  Unwrapping of arguments of different types for FTL callable functions
 */

#define UNWRAP_VEC(idx, t_vec, vec)\
 t_vec *vec = (t_vec *)fts_word_get_ptr(argv + idx);

#define UNWRAP_PTR(idx, t_ptr, ptr)\
 t_ptr *ptr = (t_ptr *)fts_word_get_ptr(argv + idx);

#define UNWRAP_SCL(idx, t_scl, scl)\
 t_scl scl = *((t_scl*)fts_word_get_ptr(argv + idx));

#define UNWRAP_SIZE(idx)\
 int size = fts_word_get_int(argv + idx);

#define FTL_ARG fts_word_t *argv
