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
 */
#ifndef _UTILS_H
#define _UTILS_H

#include <fts/fts.h>
#include <ftsconfig.h>
#include <math.h>

#ifdef WIN32
#if defined(UTILS_EXPORTS)
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif
#else
#define UTILS_API extern
#endif


#define FTS_RANDOM_MAX (0x7fffffffL)
#define FTS_RANDOM_RANGE (2147483648.)


UTILS_API void
utils_config(void);


/***************************************************************************************
 *
 *  random
 *
 */

/* 31 bit random number generator */
UTILS_API void 
fts_random_set_seed(unsigned long ul);

UTILS_API unsigned long 
fts_random(void);

/* float random between -1. and 1. */
UTILS_API float 
fts_random_float(void);


#define fts_random_range(min, max) ((min) + (((max) - (min)) * fts_random() / (double)FTS_RANDOM_MAX))




/***************************************************************************************
 *
 *  float functions
 *
 */

typedef float (*fts_float_function_t)(float);

UTILS_API int fts_ffun_exists(fts_symbol_t name);
UTILS_API void fts_ffun_new(fts_symbol_t name, float (*function)(float));
UTILS_API void fts_ffun_delete(fts_symbol_t name);
UTILS_API float fts_ffun_eval(fts_symbol_t name, float f);
UTILS_API void fts_ffun_fill(fts_symbol_t name, float *out, int size, float min, float max);
UTILS_API void fts_ffun_apply(fts_symbol_t name, float* in, float* out, int size);
UTILS_API fts_float_function_t fts_ffun_get_ptr(fts_symbol_t name);

/***************************************************************************************
 *
 *  float function tables
 *  (float arrays asssociated to fts_ffuns for function table lookups)
 *
 *     the physical size is the instantiation size + 1, so that
 *       fftab[0] = ffun(min)
 *       fftab[size] = ffun(max)
 *
 */

/* get (or create) table for existing function */
UTILS_API float *fts_fftab_get(fts_symbol_t name, int size, float min, float max);

/* get sine and cosine tables */
UTILS_API float *fts_fftab_get_sine(int size); /* size must be a multiple of 4 */
UTILS_API float *fts_fftab_get_sine_first_half(int size); /* size must be a multiple of 2 */
UTILS_API float *fts_fftab_get_sine_second_half(int size); /* size must be a multiple of 2 */
UTILS_API float *fts_fftab_get_sine_first_quarter(int size);
UTILS_API float *fts_fftab_get_sine_second_quarter(int size);
UTILS_API float *fts_fftab_get_sine_third_quarter(int size);
UTILS_API float *fts_fftab_get_sine_fourth_quarter(int size);
UTILS_API float *fts_fftab_get_cosine(int size); /* size must be a multiple of 4 */
UTILS_API float *fts_fftab_get_cosine_first_half(int size); /* size must be a multiple of 2 */
UTILS_API float *fts_fftab_get_cosine_second_half(int size); /* size must be a multiple of 2 */

/* get hanning window */
UTILS_API float *fts_fftab_get_hanning(int size); /* hanning (offset cosine) window */


/***************************************************************************************
 *
 *  cubic interpolation
 *
 */

#define FTS_CUBIC_TABLE_BITS 8
#define FTS_CUBIC_TABLE_SHIFT_BITS 24
#define FTS_CUBIC_TABLE_SIZE 256
#define FTS_CUBIC_TABLE_BIT_MASK 0xff000000

#define fts_cubic_get_table_index(i) \
   ((int)(((i).frac & FTS_CUBIC_TABLE_BIT_MASK) >> FTS_CUBIC_TABLE_SHIFT_BITS))

typedef struct
{
  float pm1;
  float p0;
  float p1;
  float p2;
} fts_cubic_coefs_t;

UTILS_API fts_cubic_coefs_t fts_cubic_table[];

#define fts_cubic_calc(x, p) \
  ((x)[-1] * (p)->pm1 + (x)[0] * (p)->p0 + (x)[1] * (p)->p1 + (x)[2] * (p)->p2)

#define fts_cubic_interpolate(p, i, y) \
  do { \
    fts_cubic_coefs_t *ft = fts_cubic_table + fts_cubic_get_table_index(i); \
    *(y) = fts_cubic_calc(p + (i).index, ft); \
  } while(0)



/*****************************************
 *
 *  ramp values
 *
 */

typedef struct _fts_ramp_value
{
  double current;
  double target;
  double incr;
} fts_ramp_value_t;

#define fts_ramp_value_zero(v) \
( \
  (v)->incr = (double)0.0, \
  (v)->current = (v)->target = (double)0.0 \
)

#define fts_ramp_value_set(v, x) \
( \
  (v)->incr = (double)0.0, \
  (v)->current = (v)->target = (x) \
)

#define fts_ramp_value_set_hold(v, x) \
( \
  (v)->incr = (double)0.0, \
  (v)->target = (x) \
)

#define fts_ramp_value_get(v) \
  ((v)->current)

#define fts_ramp_value_get_target(v) \
  ((v)->target)

#define fts_ramp_value_add(v, x) \
  ((v)->current = (v)->target += (x))

#define fts_ramp_value_set_target(v, x, n_steps) \
( \
  (v)->incr = ((x) - (v)->current) / (n_steps), \
  (v)->target = (x) \
)

#define fts_ramp_value_set_interval(v, x, n_steps) \
( \
  (v)->incr = (x) / (n_steps), \
  (v)->target = (v)->current + (x) \
)

#define fts_ramp_value_set_incr(v, x, n_steps) \
( \
  (v)->incr = (x), \
  (v)->target = (v)->current + n_steps * (x) \
)

#define fts_ramp_value_jump(v) \
( \
  (v)->incr = (double)0.0, \
  (v)->current = (v)->target \
)

#define fts_ramp_value_freeze(v) \
( \
  (v)->incr = (double)0.0, \
  (v)->target = (v)->current \
)

#define fts_ramp_value_incr(v) \
  ((v)->current += (v)->incr)

#define fts_ramp_value_incr_by(v, n) \
  ((v)->current += ((v)->incr * (n)))


/*****************************************
 *
 *  ramps
 *
 */

typedef struct _fts_ramp
{
  fts_ramp_value_t value;
  int n_steps;
} fts_ramp_t;

UTILS_API void fts_ramp_init(fts_ramp_t *ramp, float value);
UTILS_API void fts_ramp_zero(fts_ramp_t *ramp);

UTILS_API void fts_ramp_set_target(fts_ramp_t *ramp, float target, float time, float rate);
UTILS_API void fts_ramp_set_target_hold_and_jump(fts_ramp_t *ramp, float target, float time, float rate);
UTILS_API void fts_ramp_set_interval(fts_ramp_t *ramp, float interval, float time, float rate);
UTILS_API void fts_ramp_set_slope(fts_ramp_t *ramp, float slope, float time, float rate);
UTILS_API void fts_ramp_set_incr_clip(fts_ramp_t *ramp, double incr, float clip);

#define fts_ramp_running(r) ((r)->n_steps > 0)

#define fts_ramp_get_value(r) ((r)->value.current)
#define fts_ramp_get_incr(r) ((r)->value.incr)
#define fts_ramp_get_steps(r) ((r)->n_steps)

/*extern void fts_ramp_jump(fts_ramp_t *ramp);*/
UTILS_API void fts_ramp_freeze(fts_ramp_t *ramp);
UTILS_API void fts_ramp_incr(fts_ramp_t *ramp);
UTILS_API void fts_ramp_incr_by(fts_ramp_t *ramp, int n);

/* vector functions set with control rate (sr / n) */
UTILS_API void fts_ramp_vec_fill(fts_ramp_t * restrict ramp, float *out, int size);
UTILS_API void fts_ramp_vec_mul(fts_ramp_t * restrict ramp, float *in, float *out, int size);
UTILS_API void fts_ramp_vec_mul_add(fts_ramp_t * restrict ramp, float *in, float *out, int size);

/* macro implementations */
#define fts_ramp_jump(r) (fts_ramp_value_jump(&(r)->value), (r)->n_steps = 0)

#define fts_ramp_freeze(r) (fts_ramp_value_freeze(&(r)->value), (r)->n_steps = 0)

#define fts_ramp_incr(r) \
  if((r)->n_steps > 1) \
    { \
      fts_ramp_value_incr(&(r)->value); \
      (r)->n_steps--; \
    } \
  else if((r)->n_steps > 0) \
    { \
      fts_ramp_value_jump(&(r)->value); \
      (r)->n_steps = 0; \
    } \

#define fts_ramp_incr_by(r, n) \
  if((r)->n_steps > n) \
    { \
      fts_ramp_value_incr_by(&(r)->value, (n)); \
      (r)->n_steps -= n; \
    } \
  else \
    { \
      fts_ramp_value_jump(&(r)->value); \
      (r)->n_steps = 0; \
    } \


/*****************************************
 *
 *  ramp vectors
 *
 */

typedef struct _fts_framps
{
  float *value;
  float *target;
  float *incr;
  int n_steps;
  int size;
  int alloc;
} fts_framps_t;

UTILS_API fts_framps_t *fts_framps_new(int size);
UTILS_API void fts_framps_delete(fts_framps_t *ramp_vector);
UTILS_API void fts_framps_zero(fts_framps_t *ramp_vector);
UTILS_API void fts_framps_zero_tail(fts_framps_t *ramp_vector, int index);

UTILS_API void fts_framps_set_targets(fts_framps_t *ramp_vector, float *targets, int size, float time, float rate);
UTILS_API void fts_framps_set_targets_hold_and_jump(fts_framps_t *ramp_vector, float *targets, int size, float time, float rate);
UTILS_API void fts_framps_set_intervals(fts_framps_t *ramp_vector, float *intervals, int size, float time, float rate);
UTILS_API void fts_framps_set_slopes(fts_framps_t *ramp_vector, float *slopes, int size, float time, float rate);

UTILS_API void fts_framps_jump(fts_framps_t *ramp_vector);
UTILS_API void fts_framps_freeze(fts_framps_t *ramp_vector);

UTILS_API void fts_framps_incr(fts_framps_t *ramp_vector);

#define fts_framps_get_size(ramp_vector) ((ramp_vector)->size)
UTILS_API void fts_framps_set_size(fts_framps_t *ramp_vector, int size);

#define fts_framps_get(ramp_vector) ((ramp_vector)->value)


/*****************************************
 *
 *  complex numbers
 *
 */

#define MAG re
#define ARG im
#define RE re
#define IM im

typedef struct{
  float re, im;
} complex;

UTILS_API complex CZERO;


/*********************************************************************************
 *
 *    FFT computation
 *
 */

#define FTS_FFT_MIN_SIZE 16

#define FTS_FFT_MAX_SIZE_LOG2 16
#define FTS_FFT_MAX_SIZE (1 << FTS_FFT_MAX_SIZE_LOG2)

/* intitialize FFT/IFFT procedures for a specific size (returns non zero when successfull) */
UTILS_API int fts_fft_declaresize(int size);
/* check FFT/IFFT size (returns non zero for valid fft size) */
UTILS_API int fts_is_fft_size(int size);

UTILS_API void fts_cfft_inplc(complex *buf, int size);
UTILS_API void fts_cifft_inplc(complex *buf, int size);
UTILS_API void fts_rfft_inplc(float *buf, int size); /* takes real buffer of size size */
UTILS_API void fts_rifft_inplc(float *buf, int size); /* "returns" real buffer of size size */

UTILS_API void fts_cfft(complex *in, complex *out, int size);
UTILS_API void fts_cifft(complex *in, complex *out, int size);
UTILS_API void fts_rfft(float *in, complex *out, int size);
UTILS_API void fts_rifft(complex *in, float *out, int size);


/*********************************************************************************
 *
 *    idefix
 *
 */

#define FTS_IDEFIX_INDEX_BITS 31
#define FTS_IDEFIX_INDEX_MAX 2147483647 /* index is signed */

#define FTS_IDEFIX_FRAC_BITS 32
#define FTS_IDEFIX_FRAC_MAX ((unsigned int)4294967295U)
#define FTS_IDEFIX_FRAC_RANGE ((double)4294967296.0L)

typedef struct _idefix_
{
  int index;
  unsigned int frac;
} fts_idefix_t;

#define fts_idefix_get_index(x) ((int)(x).index)
#define fts_idefix_get_frac(x) ((double)((x).frac) / FTS_IDEFIX_FRAC_RANGE)

#define fts_idefix_get_float(x) ((x).index + ((double)((x).frac) / FTS_IDEFIX_FRAC_RANGE))

#define fts_idefix_set_int(x, i) ((x)->index = (i), (x)->frac = 0)
#define fts_idefix_set_float(x, f) ((x)->index = floor(f), (x)->frac = ((double)(f) - (x)->index) * FTS_IDEFIX_FRAC_RANGE)

#define fts_idefix_set_zero(x) ((x)->index = 0, (x)->frac = 0)
#define fts_idefix_set_max(x) ((x)->index = FTS_IDEFIX_INDEX_MAX, (x)->frac = FTS_IDEFIX_FRAC_MAX)

#define fts_idefix_negate(x) ((x)->index = -(x)->index - ((x)->frac > 0), (x)->frac = (FTS_IDEFIX_FRAC_MAX - (x)->frac) + 1)

#define fts_idefix_incr(x, c) ((x)->frac += (c).frac, (x)->index += ((c).index + ((x)->frac < (c).frac)))

#define fts_idefix_add(x, a, b) ((x)->frac = (a).frac + (b).frac, (x)->index = (a).index + ((b).index + ((x)->frac < (a).frac)))
#define fts_idefix_sub(x, a, b) ((x)->index = (a).index - ((b).index + ((a).frac < (b).frac)), (x)->frac = (a).frac - (b).frac)

#define fts_idefix_lshift(x, c, i) ((x)->index = ((c).index << (i)) + ((c).frac >> (FTS_IDEFIX_FRAC_BITS - (i))), (x)->frac = (c).frac << (i))

#define fts_idefix_lt(x, c) (((x).index < (c).index) || (((x).index == (c).index) && ((x).frac < (c).frac)))
#define fts_idefix_gt(x, c) (((x).index > (c).index) || (((x).index == (c).index) && ((x).frac > (c).frac)))
#define fts_idefix_eq(x, c) (((x).index == (c).index) && ((x).frac == (c).frac))
#define fts_idefix_is_zero(x) (((x).index == 0) && ((x).frac == 0))



/*********************************************************************************
 *
 *    integer phase
 *
 */

/* this type should have at least 24 bits and be efficient to be calculated */
typedef int fts_intphase_t;

#define FTS_INTPHASE_BITS (31)
#define FTS_INTPHASE_MAX (0x7fffffffL)
#define FTS_INTPHASE_RANGE (2147483648.)
#define FTS_INTPHASE_RANGE_HALF (1073741824.)
#define FTS_INTPHASE_RANGE_QUARTER (536870912.)

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
#if WORDS_BIGENDIAN

#define MSB_OFFSET 0 /* word offset to find MSW */
#define LSB_OFFSET 1 /* word offset to find LSW */
typedef long fts_int32_t; /* a data type that has 32 bits */

#else 

#define MSB_OFFSET 1 /* word offset to find MSW */
#define LSB_OFFSET 0 /* word offset to find LSW */
typedef long fts_int32_t; /* a data type that has 32 bits */

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


#endif /* _UTILS_H */
