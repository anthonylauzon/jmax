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

fts_integer_vector_t *
data_recycle_atom_integer_vector(fts_atom_t *atom, int size)
{
  fts_integer_vector_t *vector;
  fts_data_t *data;

  if(fts_is_data(atom))
    {
      data = fts_get_data(atom);

      if(fts_data_has_just_one_reference(data) && fts_data_get_class_name(data) == fts_s_integer_vector)
	{
	  /* reuse integer vector */
	  vector = (fts_integer_vector_t *)data;
	  fts_integer_vector_set_size(vector, size);
	  return vector;
	}
      
      data_release(data);
    }

  vector = fts_integer_vector_new(size);
  data = (fts_data_t *)vector;

  fts_data_refer(data);
  fts_set_data(atom, data);
  
  return vector;
}

fts_float_vector_t *
data_recycle_atom_float_vector(fts_atom_t *atom, int size)
{
  fts_float_vector_t *vector;
  fts_data_t *data;

  if(fts_is_data(atom))
    {
      data = fts_get_data(atom);

      if(fts_data_has_just_one_reference(data) && fts_data_get_class_name(data) == fts_s_float_vector)
	{
	  /* reuse float vector */
	  vector = (fts_float_vector_t *)data;
	  fts_float_vector_set_size(vector, size);
	  return vector;
	}
      
      data_release(data);
    }

  vector = fts_float_vector_new(size);
  data = (fts_data_t *)vector;

  fts_data_refer(data);
  fts_set_data(atom, data);
  
  return vector;
}

fts_integer_vector_t *
data_recycle_atom_integer_vector_replicate(fts_atom_t *atom, fts_integer_vector_t *original)
{
  fts_integer_vector_t *vector;
  fts_data_t *data;
  int size = fts_integer_vector_get_size(original);

  if(fts_is_data(atom))
    {
      data = fts_get_data(atom);

      if(fts_data_has_just_one_reference(data) && fts_data_get_class_name(data) == fts_s_integer_vector)
	{
	  /* reuse integer vector */
	  vector = (fts_integer_vector_t *)data;
	  fts_integer_vector_copy(original, vector);
	  return vector;
	}
      
      data_release(data);
    }

  vector = fts_integer_vector_new(size);
  fts_integer_vector_copy(original, vector);

  data = (fts_data_t *)vector;
  fts_data_refer(data);
  fts_set_data(atom, data);
  
  return vector;
}

fts_float_vector_t *
data_recycle_atom_float_vector_replicate(fts_atom_t *atom, fts_float_vector_t *original)
{
  fts_float_vector_t *vector;
  fts_data_t *data;
  int size = fts_float_vector_get_size(original);

  if(fts_is_data(atom))
    {
      data = fts_get_data(atom);

      if(fts_data_has_just_one_reference(data) && fts_data_get_class_name(data) == fts_s_float_vector)
	{
	  /* reuse integer vector */
	  vector = (fts_float_vector_t *)data;
	  fts_float_vector_copy(original, vector);
	  return vector;
	}
      
      data_release(data);
    }

  vector = fts_float_vector_new(size);
  fts_float_vector_copy(original, vector);

  data = (fts_data_t *)vector;
  fts_data_refer(data);
  fts_set_data(atom, data);
  
  return vector;
}

