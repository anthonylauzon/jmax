#include "fts.h"
#include "floatvector.h"

fts_symbol_t fts_s_floatvector = 0;

void
fts_float_vector_config(void)
{
  fts_s_floatvector = fts_new_symbol("floatvector");
}

fts_float_vector_t *
fts_float_vector_new(void)
{
  fts_float_vector_t *vector;

  vector = (fts_float_vector_t *)fts_malloc(sizeof(fts_float_vector_t));
  fts_float_vector_init(vector);

  return vector;
}

void
fts_float_vector_init(fts_float_vector_t *vector)
{
  vector->values = 0;
  vector->size = vector->alloc = 0;
}

void
fts_float_vector_set_size(fts_float_vector_t *vector, long size)
{
  if(size > vector->alloc)
    {
      if(vector->alloc)
	fts_free(vector->values);

      vector->values = (float *)fts_malloc(sizeof(float) * size);
      vector->size = vector->alloc = size;
    }
  else
    vector->size = size;
}

void
fts_float_vector_delete(fts_float_vector_t *vector)
{
  fts_free(vector->values);
  fts_free(vector);
}

void
fts_float_vector_copy(fts_float_vector_t *in, fts_float_vector_t *out)
{
  fts_float_vector_set_size(out, in->size);
  fts_vec_fcpy(in->values, out->values, in->size);
}

void
fts_float_vector_zero(fts_float_vector_t *vector)
{
  fts_vec_fzero(vector->values, vector->size);  
}











