/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "operators.h"

fts_symbol_t op_s_recycle = 0;
fts_symbol_t op_s_inplace = 0;

fts_integer_vector_t *
op_recycle_to_integer_vector(op_t *op, int size)
{
  if(op_is_integer_vector(*op) && op_ref_can_be_reused(*op))
    {
      /* reuse integer vector */
      fts_integer_vector_t *vector = op_get_integer_vector(*op);
      fts_integer_vector_set_size(vector, size);
      return vector;
    }
  else
    {
      /* make new vector */
      fts_integer_vector_t *vector = fts_integer_vector_new(size);
      op_set_integer_vector(op, vector);
      return vector;
    }
}

fts_float_vector_t *
op_recycle_to_float_vector(op_t *op, int size)
{
  if(op_is_float_vector(*op) && op_ref_can_be_reused(*op))
    {
      /* reuse float vector */
      fts_float_vector_t *vector = op_get_float_vector(*op);
      fts_float_vector_set_size(vector, size);
      return vector;
    }
  else
    {
      /* make new vector */
      fts_float_vector_t *vector = fts_float_vector_new(size);
      op_set_float_vector(op, vector);
      return vector;
    }
}

fts_integer_vector_t *
op_recycle_to_integer_vector_replicate(op_t *op, fts_integer_vector_t *original)
{
  int size = fts_integer_vector_get_size(original);

  if(op_is_integer_vector(*op) && op_ref_can_be_reused(*op))
    {
      /* reuse integer vector */
      fts_integer_vector_t *vector = op_get_integer_vector(*op);
      fts_integer_vector_copy(original, vector);
      return vector;
    }
  else
    {
      /* make new vector */
      fts_integer_vector_t *vector = fts_integer_vector_new(size);
      fts_integer_vector_copy(original, vector);
      op_set_integer_vector(op, vector);
      return vector;
    }
}

fts_float_vector_t *
op_recycle_to_float_vector_replicate(op_t *op, fts_float_vector_t *original)
{
  int size = fts_float_vector_get_size(original);

  if(op_is_float_vector(*op) && op_ref_can_be_reused(*op))
    {
      /* reuse integer vector */
      fts_float_vector_t *vector = op_get_float_vector(*op);
      fts_float_vector_copy(original, vector);
      return vector;
    }
  else
    {
      /* make new vector */
      fts_float_vector_t *vector = fts_float_vector_new(size);
      fts_float_vector_copy(original, vector);
      op_set_float_vector(op, vector);
      return vector;
    }
}

void
operators_init(void)
{
  op_s_recycle = fts_new_symbol("recycle");
  op_s_inplace = fts_new_symbol("inplace");
}
