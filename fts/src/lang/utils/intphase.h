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
#ifndef _INTPHASE_H_
#define _INTPHASE_H_

#define FTS_INTPHASE_BITS (24)
#define FTS_INTPHASE_RANGE (1 << FTS_INTPHASE_BITS)

/* this type should have at least (!) FTS_INTPHASE_BITS bits and be efficient to be calculated */
typedef unsigned long fts_intphase_t;

#endif
