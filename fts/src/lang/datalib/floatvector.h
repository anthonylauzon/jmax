#ifndef _FLOATVECTOR_H_
#define _FLOATVECTOR_H_

typedef struct _floatvector
{
  float *values;
  long size;
  long alloc;
} floatvector_t;

extern fts_symbol_t sym_floatvector;

extern floatvector_t *floatvector_new(void);
extern void floatvector_init(floatvector_t *vector);
extern void floatvector_delete(floatvector_t *vector);
extern void floatvector_copy(floatvector_t *in, floatvector_t *out);
extern void floatvector_zero(floatvector_t *vector);

#define floatvector_get_size(vector) ((vector)->size)
extern void floatvector_set_size(floatvector_t *vector, long size);

#define floatvector_is_empty(vector) ((vector)->size == 0)

#define floatvector_get_element(vector, index) ((vector)->values[index])
#define floatvector_set_element(vector, index, value) ((vector)->values[index] = (value))

#endif

