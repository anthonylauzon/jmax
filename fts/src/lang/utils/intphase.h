/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

/* this type should have at least 24 bits and be efficient to be calculated */
typedef int fts_intphase_t;

#define FTS_INTPHASE_BITS (31)
#define FTS_INTPHASE_MAX (0x7fffffffL)
#define FTS_INTPHASE_RANGE (2147483648.)

#define fts_intphase_wrap(phi) \
  ((phi) & FTS_INTPHASE_MAX)

#define fts_intphase_get_index(phi, bits) \
  ((phi) >> (FTS_INTPHASE_BITS - (bits)))

#define fts_intphase_get_frac(phi, bits) \
  ((float)((phi) & ((1 << (FTS_INTPHASE_BITS - (bits))) - 1)) * (float)(1.0 / (1 << (FTS_INTPHASE_BITS - (bits)))))

#define fts_intphase_float(phi) \
  ((float)(phi) * (float)(1.0 / FTS_INTPHASE_RANGE))

#define fts_intphase_double(phi) \
  ((double)((phi) * (double)(1.0 / FTS_INTPHASE_RANGE)))

#define fts_intphase_lookup(phi, array, bits) \
  ((array)[fts_intphase_get_index((phi), (bits))])

#endif
