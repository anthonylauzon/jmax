#ifndef _FLOATVECTOR_H_
#define _FLOATVECTOR_H_

typedef struct fts_float_vector
{
  fts_data_t dataobj;
  float *values;
  int size;
  int alloc;
} fts_float_vector_t;

extern fts_symbol_t fts_s_float_vector;

extern fts_float_vector_t *fts_float_vector_new(int size);
extern void fts_float_vector_delete(fts_float_vector_t *vector);

extern void fts_float_vector_copy(fts_float_vector_t *in, fts_float_vector_t *out);
extern void fts_float_vector_zero(fts_float_vector_t *vector);

#define fts_float_vector_get_size(vector) ((vector)->size)
extern void fts_float_vector_set_size(fts_float_vector_t *vector, long size);
#define fts_float_vector_is_empty(vector) ((vector)->size == 0)

#define fts_float_vector_get_element(vector, index) ((vector)->values[index])
#define fts_float_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

#define fts_float_vector_set_const(vector, constant) fts_vec_ffill((constant), (vector)->values, (vector)->size)

extern float fts_float_vector_get_sum(fts_float_vector_t *vector);
extern float fts_float_vector_get_sub_sum(fts_float_vector_t *vector, int from, int to);
extern float fts_float_vector_get_min_value(fts_float_vector_t *vector);
extern float fts_float_vector_get_max_value(fts_float_vector_t *vector);

extern void fts_float_vector_set_from_atom_list(fts_float_vector_t *vector, int onset, int ac, const fts_atom_t *at);

#endif

