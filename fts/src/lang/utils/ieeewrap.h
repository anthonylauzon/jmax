/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_IEEEWRAP_H_
#define _FTS_IEEEWRAP_H_

/*************************************************************************
 *
 *  phase wrap around tricks
 *
 *    defines:
 *
 *      fts_wapper_t ... wrapper register
 *      
 *      void fts_wrapper_set(fts_wrapper_t *w, double value, int bits);
 *        ... sets value to be wrapped into 2 ^ bits
 *
 *      int fts_wrapper_get_integer(fts_wrapper_t *w, int int_bits);
 *        ... get integer part wrapped to 2 ^ int_bits
 *
 *      double fts_wrapper_wrap(fts_wrapper_t *w, int bits);
 *        ... gets value wrapped into 2 ^ bits
 *
 */

/* machine-dependent definitions of word order*/
#if defined(FTS_HAS_BIG_ENDIAN)

#define MSB_OFFSET 0 /* word offset to find MSW */
#define LSB_OFFSET 1 /* word offset to find LSW */
typedef long fts_int32_t; /* a data type that has 32 bits */

#elif defined(FTS_HAS_LITTLE_ENDIAN)

#define MSB_OFFSET 1 /* word offset to find MSW */
#define LSB_OFFSET 0 /* word offset to find LSW */
typedef long fts_int32_t; /* a data type that has 32 bits */

#else

#define MSB_OFFSET endianess_undefined
#define LSB_OFFSET endianxess_undefined
#define fts_int32_t int32_type_undefined

#endif

#define FTS_WRAP_NORMALIZED_MSW 1094189056 /* normalized MSW for wrapping into 0...1 */
#define FTS_WRAP_NORMALIZED_MSW_BIT 1048576 /* add for each bit of range more (2 ^ 20) */

#define UNITBIT32 ((double)1572864.0) /* 3*2^19 -- bit 32 has value 1 */

typedef union _fts_wrap_fudge
{
  double f;
  fts_int32_t i[2];
} fts_wrapper_t;

/* get the normalized MSW for a certain range of 2 ^ bits */
#define fts_wrap_constant(bits) \
  (FTS_WRAP_NORMALIZED_MSW + (bits) * FTS_WRAP_NORMALIZED_MSW_BIT)

/* set wrapper with value to be wrapped to the range of 2 ^ bits */
#define fts_wrapper_set(wrapper, value, bits) \
  ((wrapper)->f = (double)(value) + UNITBIT32 * (double)(1 << bits))

/* get value back from wrapper (remove offset) */
#define fts_wrapper_get(wrapper, bits) \
  ((wrapper)->f - UNITBIT32 * (double)(1 << bits))

/* set wrapper with value to be wrapped to the range of 2 ^ bits */
#define fts_wrapper_frac_set(wrapper, value) \
  ((wrapper)->f = (double)(value) + UNITBIT32)

/* get value back from wrapper (remove offset) */
#define fts_wrapper_frac_get(wrapper) \
  ((wrapper)->f - UNITBIT32)

/* get integral part fit to the integer range */
#define fts_wrapper_get_int(wrapper, int_bits) \
  ((wrapper)->i[MSB_OFFSET] & ((1 << int_bits) - 1))

/* wrap and get it */
#define fts_wrapper_get_wrap(wrapper, bits) \
( \
  (wrapper)->i[MSB_OFFSET] = (FTS_WRAP_NORMALIZED_MSW + (bits) * FTS_WRAP_NORMALIZED_MSW_BIT), \
  (wrapper)->f - UNITBIT32 * (double)(1 << bits) \
)

/* wrap to fractional part and get it */
#define fts_wrapper_frac_get_wrap(wrapper) \
( \
  (wrapper)->i[MSB_OFFSET] = FTS_WRAP_NORMALIZED_MSW, \
  (wrapper)->f - UNITBIT32 \
)

#define fts_wrapper_copy(in_wrapper, out_wrapper) \
  ((out_wrapper)->f = (in_wrapper)->f)

#define fts_wrapper_incr(wrapper, incr) \
  (wrapper)->f += (double)incr;

/* wrap value ones into a given range (2 ^ bits) */
#define fts_wrap(wrapper, value, bits) \
( \
  (wrapper)->f = (double)(value) + UNITBIT32 * (double)(1 << bits), \
  (wrapper)->i[MSB_OFFSET] = (FTS_WRAP_NORMALIZED_MSW + (bits) * FTS_WRAP_NORMALIZED_MSW_BIT), \
  (wrapper)->f - UNITBIT32 * (double)(1 << bits) \
)

#define fts_frac(wrapper, value) \
( \
  (wrapper)->f = (double)(value) + UNITBIT32, \
  (wrapper)->i[MSB_OFFSET] = FTS_WRAP_NORMALIZED_MSW, \
  (wrapper)->f - UNITBIT32 \
)

#endif
