#ifndef _RAMP_VECTORS_H_
#define _RAMP_VECTORS_H_

/*****************************************
 *
 *  ramp vectors
 *
 */

typedef struct _fts_float_vector_ramp
{
  float *value;
  float *target;
  float *incr;
  int n_steps;
  int size;
  int alloc;
} fts_float_vector_ramp_t;

extern fts_float_vector_ramp_t *fts_float_vector_ramp_new(int size);
extern void fts_float_vector_ramp_delete(fts_float_vector_ramp_t *ramp_vector);
extern void fts_float_vector_ramp_zero(fts_float_vector_ramp_t *ramp_vector);
extern void fts_float_vector_ramp_zero_tail(fts_float_vector_ramp_t *ramp_vector, int index);

extern void fts_float_vector_ramp_set_targets(fts_float_vector_ramp_t *ramp_vector, float *targets, int size, float time, float rate);
extern void fts_float_vector_ramp_set_targets_hold_and_jump(fts_float_vector_ramp_t *ramp_vector, float *targets, int size, float time, float rate);
extern void fts_float_vector_ramp_set_intervals(fts_float_vector_ramp_t *ramp_vector, float *intervals, int size, float time, float rate);
extern void fts_float_vector_ramp_set_slopes(fts_float_vector_ramp_t *ramp_vector, float *slopes, int size, float time, float rate);

extern void fts_float_vector_ramp_jump(fts_float_vector_ramp_t *ramp_vector);
extern void fts_float_vector_ramp_freeze(fts_float_vector_ramp_t *ramp_vector);

extern void fts_float_vector_ramp_incr(fts_float_vector_ramp_t *ramp_vector);

#define fts_float_vector_ramp_get_size(ramp_vector) ((ramp_vector)->size)
extern void fts_float_vector_ramp_set_size(fts_float_vector_ramp_t *ramp_vector, int size);

#define fts_float_vector_ramp_get(ramp_vector) ((ramp_vector)->value)

#endif
