#ifndef _INTVEC_H_
#define _INTVEC_H_

typedef struct fts_integer_vector
{
  fts_data_t dataobj;
  int *values;
  int size;	
  int alloc;
} fts_integer_vector_t;

extern fts_symbol_t fts_s_integer_vector;

extern fts_integer_vector_t *fts_integer_vector_new(int size);
extern void fts_integer_vector_init(fts_integer_vector_t *vector, int size);
extern void fts_integer_vector_delete(fts_integer_vector_t *this);

extern void fts_integer_vector_copy(fts_integer_vector_t *in, fts_integer_vector_t *out);
extern void fts_integer_vector_zero(fts_integer_vector_t *vector);

#define fts_integer_vector_get_size(vector) ((vector)->size)
extern void fts_integer_vector_set_size(fts_integer_vector_t *vector, int size);
#define fts_integer_vector_is_empty(vector) ((vector)->size == 0)

#define fts_integer_vector_get_element(vector, index) ((vector)->values[index])
#define fts_integer_vector_set_element(vector, index, value) ((vector)->values[index] = (value))

#define fts_float_vector_get_ptr(vector) ((vector)->values)

extern int fts_integer_vector_get_sum(fts_integer_vector_t *vector);
extern int fts_integer_vector_get_sub_sum(fts_integer_vector_t *vector, int from, int to);
extern int fts_integer_vector_get_min_value(fts_integer_vector_t *vector);
extern int fts_integer_vector_get_max_value(fts_integer_vector_t *vector);

extern void fts_integer_vector_set(fts_integer_vector_t *vector, int *ptr, int size);
extern void fts_integer_vector_set_from_atom_list(fts_integer_vector_t *vector, int offset, int ac, const fts_atom_t *at);
#define fts_integer_vector_set_const(vector, constant) fts_vec_ifill((constant), (long *)((vector)->values), (vector)->size)

extern void fts_integer_vector_save_bmax(fts_integer_vector_t *this, fts_bmax_file_t *f);

#endif
