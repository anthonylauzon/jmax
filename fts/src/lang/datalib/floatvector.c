#include "fts.h"
#include "floatvector.h"

fts_symbol_t sym_floatvector = 0;

void
floatvector_config(void)
{
  sym_floatvector = fts_new_symbol("floatvector");
}

floatvector_t *
floatvector_new(void)
{
  floatvector_t *vector;

  vector = (floatvector_t *)fts_malloc(sizeof(floatvector_t));
  floatvector_init(vector);

  return vector;
}

void
floatvector_init(floatvector_t *vector)
{
  vector->values = 0;
  vector->size = vector->alloc = 0;
}

void
floatvector_set_size(floatvector_t *vector, long size)
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
floatvector_delete(floatvector_t *vector)
{
  fts_free(vector->values);
  fts_free(vector);
}

void
floatvector_copy(floatvector_t *in, floatvector_t *out)
{
  floatvector_set_size(out, in->size);
  fts_vec_fcpy(in->values, out->values, in->size);
}

void
floatvector_zero(floatvector_t *vector)
{
  fts_vec_fzero(vector->values, vector->size);  
}
