#ifndef _IEEEWRAP_H_
#define _IEEEWRAP_H_

/*************************************************************************
 *
 *  phase wrap around tricks
 *
 *    defines:
 *
 *      fts_wapper_t ... wrapper registers
 *      
 *      void fts_wrapper_init(fts_wrapper_t *w);
 *        ... initializes internal wrapper registers
 *
 *      void fts_wrapper_set(fts_wrapper_t *w, double value, double range);
 *        ... sets value to be wrapped into range
 *
 *      int fts_wrapper_get_integer(fts_wrapper_t *w, double int_range);
 *        ... get integer part (int_range must be a power of two)
 *
 *      double fts_wrapper_get_wrapped(fts_wrapper_t *w, double range);
 *        ... gets value wrapped into range
 *
 */

#define UNITBIT32 1572864. /* 3*2^19 -- bit 32 has value 1 */

/* machine-dependent definitions of word order*/
#if defined(FTS_HAS_BIG_ENDIAN)

#define MSB_OFFSET 0    /* word offset to find MSB */
#define LSB_OFFSET 1    /* word offset to find LSB */
#define fts_int32 long  /* a data type that has 32 bits */

#elif defined(FTS_HAS_LITTLE_ENDIAN)

#define MSB_OFFSET 1    /* word offset to find MSB */
#define LSB_OFFSET 0    /* word offset to find LSB */
#define fts_int32 long  /* a data type that has 32 bits */

#else

#define MSB_OFFSET endianess_undefined
#define LSB_OFFSET endianess_undefined
#define fts_int32 int32_type_undefined

#endif

typedef union _fts_wrap_fudge
{
  double f;
  fts_int32 i[2];
} fts_wrap_fudge_t;

typedef struct _fts_wrapper
{
  union _fts_wrap_fudge fudge;
  fts_int32 normalized_msw;
} fts_wrapper_t;

#define fts_wrapper_init(wrapper, float_range) \
( \
  (wrapper)->fudge.f = UNITBIT32 * (double)(float_range), \
  (wrapper)->normalized_msw = (wrapper)->fudge.i[MSB_OFFSET] \
)

#define fts_wrapper_set_raw(wrapper, value) \
  ((wrapper)->fudge.f = (value)) \

#define fts_wrapper_set(wrapper, value, float_range)  \
  fts_wrapper_set_raw((wrapper), (value) + UNITBIT32 * (double)(float_range))

#define fts_wrapper_get_integer_raw(wrapper) \
  ((wrapper)->fudge.i[MSB_OFFSET])

#define fts_wrapper_get_integer(wrapper, int_range) \
  (wrapper)->fudge.i[MSB_OFFSET] & ((int_range) - 1)

#define fts_wrapper_get_wrapped(wrapper, float_range) \
( \
  (wrapper)->fudge.i[MSB_OFFSET] = (wrapper)->normalized_msw, \
  (wrapper)->fudge.f - UNITBIT32 * (double)(float_range) \
)

/* wrap value ones into a given range */
#define fts_wrap(wrapper, value, range) \
( \
  (wrapper)->fudge.f = (value) + UNITBIT32 * (double)(range), \
  (wrapper)->fudge.i[MSB_OFFSET] = (wrapper)->normalized_msw, \
  (wrapper)->fudge.f - UNITBIT32 * (double)(range) \
)

/*************************************************************************
 *
 *  wrap and split (int, frac) floating point values in a certain range
 *
 *    defines:
 *
 *      void fts_wrap_value_init(fts_wrap_value_t *x, double init_value);
 *        ... initializes registers of wrapped value
 *
 *      int fts_wrap_value_get_int(fts_wrap_value_t *x, long range);
 *        ... gets interger part of x inside range (power of 2)
 *            (MUST be called BEFORE fts_wrap_value_get_frac())
 *
 *      double fts_wrap_value_get_frac(fts_wrap_value_t *x);
 *        ... gets fractional part of x 
 *
 *      double fts_wrap_value_get_wrapped(fts_wrap_value_t *x, long range);
 *        ... gets index wrapped into it's range
 *
 */

typedef struct _fts_wrap_value
{
  double value;
  fts_wrapper_t wrapper;
} fts_wrap_value_t;

#define fts_wrap_value_init(wv, init) \
( \
  fts_wrapper_init(&(wv)->wrapper, 1.0), \
  (wv)->value = (init) + UNITBIT32 \
)
  
#define fts_wrap_value_incr(wv, incr) \
  ((wv)->value += (incr))

#define fts_wrap_value_set(wv) \
  (fts_wrapper_set_raw(&(wv)->wrapper, (wv)->value))

#define fts_wrap_value_set_with_offset(wv, offset) \
  (fts_wrapper_set_raw(&(wv)->wrapper, (wv)->value) + (offset))

#define fts_wrap_value_get_int(wv, range) \
  (fts_wrapper_get_integer(&(wv)->wrapper, (range)))

#define fts_wrap_value_get_frac(wv) \
  (fts_wrapper_get_wrapped(&(wv)->wrapper, 1.0))

#define fts_wrap_value_get_wrapped(wv, range) \
( \
  fts_wrapper_init(&(wv)->wrapper, (range)), \
  fts_wrapper_set(&(wv)->wrapper, (wv)->value - UNITBIT32, (range)), \
  (wv)->value = fts_wrapper_get_wrapped(&(wv)->wrapper, (range)) \
)

#endif
