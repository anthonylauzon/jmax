#ifndef _FLOATFUNS_H_
#define _FLOATFUNS_H_

#include "fts.h"

/***************************************************************************************
 *
 *  float functions
 *
 */

typedef float (*fts_float_function_t)(float);

extern int fts_ffun_exists(fts_symbol_t name);
extern int fts_ffun_new(fts_symbol_t name, float (*function)(float));
extern void fts_ffun_delete(fts_symbol_t name);
extern float fts_ffun_eval(fts_symbol_t name, float f);
extern void fts_ffun_fill(fts_symbol_t name, float *out, int size, float min, float max);
extern void fts_ffun_apply(fts_symbol_t name, float* in, float* out, int size);
extern fts_float_function_t fts_ffun_get_ptr(fts_symbol_t name);

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
extern float *fts_fftab_get(fts_symbol_t name, int size, float min, float max);

/* get sine and cosine tables */
extern float *fts_fftab_get_sine(int size); /* size must be a multiple of 4 */
extern float *fts_fftab_get_sine_first_half(int size); /* size must be a multiple of 2 */
extern float *fts_fftab_get_sine_second_half(int size); /* size must be a multiple of 2 */
extern float *fts_fftab_get_sine_first_quarter(int size);
extern float *fts_fftab_get_sine_second_quarter(int size);
extern float *fts_fftab_get_sine_third_quarter(int size);
extern float *fts_fftab_get_sine_fourth_quarter(int size);
extern float *fts_fftab_get_cosine(int size); /* size must be a multiple of 4 */
extern float *fts_fftab_get_cosine_first_half(int size); /* size must be a multiple of 2 */
extern float *fts_fftab_get_cosine_second_half(int size); /* size must be a multiple of 2 */

#endif
