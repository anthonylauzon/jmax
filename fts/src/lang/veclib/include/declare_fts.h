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

#if defined(DECLARE_VEC_FUN)
#undef DECLARE_VEC_FUN
#endif

#define DECLARE_VEC_FUN(class, args)\
 DECLARE_FTS_VEC_ ## class ## args\
 DECLARE_FTS_VECX_ ## class ## args
