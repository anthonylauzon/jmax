#ifndef _FLOATFUNS_H_
#define _FLOATFUNS_H_

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
 *  (float arrays asssociated to fts_ffuns for fast function lookups)
 *
 *  physical size is their instantiation size + 1
 *  fftab[0] = min
 *  fftab[size] = max
 *
 */

typedef struct _fts_fftab
{
  float *values;
  int size; /* table size must be a power of two (for fast lookups) */
  float min;
  float max;
  float range;
  struct _fts_fftab *next_in_list; /* for list of tables for one float function */
  int ref; /* reference counter */
} fts_fftab_t;

/* get (or create) table for existing function */
extern fts_fftab_t *fts_fftab_get(fts_symbol_t name, float min, float max, int size);

/* release (maybe destroy) table of given function */
extern void fts_fftab_release(fts_symbol_t name, fts_fftab_t *fftab);

#define fts_fftab_get_float_ptr(fftab) ((fftab)->values)
#define fts_fftab_get_size(fftab) ((fftab)->size)


#endif
