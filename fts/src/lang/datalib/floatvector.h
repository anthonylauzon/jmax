#ifndef _FLOATVECTOR_H_
#define _FLOATVECTOR_H_

typedef struct _floatvector
{
  float *values;
  long size;
  long alloc;
} fts_float_vector_t;

extern fts_symbol_t fts_s_floatvector;

extern fts_float_vector_t *fts_float_vector_new(void);
extern void fts_float_vector_init(fts_float_vector_t *vector);
extern void fts_float_vector_delete(fts_float_vector_t *vector);
extern void fts_float_vector_copy(fts_float_vector_t *in, fts_float_vector_t *out);
extern void fts_float_vector_zero(fts_float_vector_t *vector);

#define fts_float_vector_get_size(vector) ((vector)->size)
extern void fts_float_vector_set_size(fts_float_vector_t *vector, long size);

#define fts_float_vector_is_empty(vector) ((vector)->size == 0)

#define fts_float_vector_get_element(vector, index) ((vector)->values[index])
#define fts_float_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

#endif

