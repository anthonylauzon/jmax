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
#include "sys.h"
#include "lang/mess.h"
#include "lang/datalib.h"
#include "lang/veclib.h"
#include "runtime.h"	/* @@@@ what should we do ?? */

fts_symbol_t fts_s_float_vector = 0;
fts_data_class_t *fts_float_vector_data_class = 0;

/* remote call codes */

#define FLOAT_VECTOR_SET    1
#define FLOAT_VECTOR_UPDATE 2

/* local */

static void
floatvec_set_size(fts_float_vector_t *vector, int size)
{
  if(size > vector->alloc)
    {
      if(vector->alloc)
	vector->values = (float *)fts_realloc((void *)vector->values, sizeof(float) * size);
      else
	vector->values = (float *)fts_malloc(sizeof(float) * size);

      vector->alloc = size;
    }

  vector->size = size;
}

/* new/delete */

fts_float_vector_t *
fts_float_vector_new(int size)
{
  fts_float_vector_t *vector;
  int i;

  vector = (fts_float_vector_t *)fts_malloc(sizeof(fts_float_vector_t));

  if(size > 0)
    {
      vector->values = (float *) fts_malloc(size * sizeof(float));
      vector->size = size;
      fts_float_vector_zero(vector);
    }
  else
    {
      vector->values = 0;
      vector->size = 0;
    }

  vector->alloc = size;

  fts_data_init((fts_data_t *) vector, fts_float_vector_data_class);

  return vector;
}

void
fts_float_vector_delete(fts_float_vector_t *vector)
{
  fts_data_delete((fts_data_t *) vector);
  fts_free(vector->values);
  fts_free(vector);
}

int
fts_float_vector_get_atoms(fts_float_vector_t *vector, int ac, fts_atom_t *at)
{
  int i;
  int size = vector->size;

  if(ac > size)
    ac = size;

  for(i=0; i<ac; i++)
    fts_set_float(at + i, vector->values[i]);

  return size;
}

/* copy & zero */
void
fts_float_vector_copy(fts_float_vector_t *in, fts_float_vector_t *out)
{
  floatvec_set_size(out, in->size);
  fts_vec_fcpy(in->values, out->values, in->size);
}

void
fts_float_vector_zero(fts_float_vector_t *vector)
{
  fts_vec_fzero(vector->values, vector->size);  
}

void
fts_float_vector_set_size(fts_float_vector_t *vector, int size)
{
  int old_size = vector->size;
  int tail = size - old_size;

  floatvec_set_size(vector, size);

  if(tail > 0)
    fts_vec_fzero(vector->values + old_size, tail);
}

void 
fts_float_vector_set_from_ptr(fts_float_vector_t *vector, float *ptr, int size)
{
  floatvec_set_size(vector, size);
  fts_vec_fcpy(ptr, vector->values, size);
}

void
fts_float_vector_set_from_atom_list(fts_float_vector_t *vector, int offset, int ac, const fts_atom_t *at)
{
  int size = fts_float_vector_get_size(vector);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_float(&at[i]))
	vector->values[i + offset] = fts_get_float(&at[i]);
      else if(fts_is_int(&at[i]))
	vector->values[i + offset] = (float)fts_get_int(&at[i]);
    }
}

/* sum, min, max */

float
fts_float_vector_get_sum(fts_float_vector_t *vector)
{
  float sum = 0;
  int i;

  for(i=0; i<vector->size; i++)
    sum += vector->values[i];

  return sum;
}

float
fts_float_vector_get_sub_sum(fts_float_vector_t *vector, int from, int to)
{
  float sum = 0;
  int i;
  
  if(from < 0)
    from = 0;

  if(to >= vector->size)
    to = vector->size - 1;

  for(i=from; i<=to; i++)
    sum += vector->values[i];

  return sum;
}

float
fts_float_vector_get_min_value(fts_float_vector_t *vector)
{
  float min;
  int i;

  min = vector->values[0];

  for (i=1; i<vector->size; i++)
    if (vector->values[i] < min)
      min = vector->values[i];

  return min;
}


float
fts_float_vector_get_max_value(fts_float_vector_t *vector)
{
  float max;
  int i;

  max = vector->values[0];

  for (i=1; i<vector->size; i++)
    if (vector->values[i] > max)
      max = vector->values[i];

  return max;
}

/********************************************************************
 *
 *   FTS data functions
 *
 */

static void fts_float_vector_export_fun(fts_data_t *data)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, FLOAT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i = 0; i < vector->size; i++)
    fts_client_mess_add_float(vector->values[i]);

  fts_data_end_remote_call();
}

static void 
fts_float_vector_remote_set( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int offset;
  int nvalues;
  int i;

  /* arguments: offset, # of values, values */

  offset = fts_get_int(&at[0]);
  nvalues = fts_get_int(&at[1]);

  for (i=0; i<nvalues; i++)
    vector->values[i + offset] = fts_get_float(&at[i + 2]);
}


static void 
fts_float_vector_remote_update( fts_data_t *data, int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vector = (fts_float_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, FLOAT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i=0; i<vector->size; i++)
    fts_client_mess_add_float(vector->values[i]);

  fts_data_end_remote_call();
}


fts_float_vector_t *
fts_float_vector_constructor(int ac, const fts_atom_t *at)
{
  fts_float_vector_t *vec = 0;

  if(ac == 1 && fts_is_int(at))
    {
      vec = fts_float_vector_new(fts_get_int(at));
    }
  else if(ac > 1)
    {
      vec = fts_float_vector_new(ac);
      fts_float_vector_set_from_atom_list(vec, 0, ac, at);
    }
  else
    vec = fts_float_vector_new(0);
  
  return vec;
}

void fts_float_vector_config(void)
{
  fts_s_float_vector = fts_new_symbol("float_vector");
  fts_float_vector_data_class = fts_data_class_new(fts_s_float_vector);

  fts_data_class_define_export_function(fts_float_vector_data_class, fts_float_vector_export_fun);
  fts_data_class_define_function(fts_float_vector_data_class, FLOAT_VECTOR_SET, fts_float_vector_remote_set);
  fts_data_class_define_function(fts_float_vector_data_class, FLOAT_VECTOR_UPDATE, fts_float_vector_remote_update);
}

/********************************************************************
 *
 *  bmax format releated functions
 *
 */

/* vector is actually quite a temporary hack; there should be a real
   save standard technique for fts_data_t; it assume that is reloaded
   for a vector !!*/

void fts_float_vector_save_bmax(fts_float_vector_t *vector, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for(i=0; i<vector->size; i++)
    {
      fts_set_float(&av[ac], vector->values[i]);

      ac++;

      if (ac == 256)
	{
	  /* Code a push of all the values */
	  fts_bmax_code_push_atoms(f, ac, av);

	  /* Code a push of the offset */
	  fts_bmax_code_push_int(f, offset);

	  /* Code a "set" message for 256 values plus offset */
	  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);
	  offset = offset + ac;

	  /* Code a pop of all the values  */
	  fts_bmax_code_pop_args(f, ac);

	  ac = 0;
	}
    }

  if (ac != 0)
    {
      /* Code a push of all the values */
      fts_bmax_code_push_atoms(f, ac, av);

      /* Code a push of the offset */
      fts_bmax_code_push_int(f, offset);

      /* Code an "append" message for the values left */
      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_set, ac + 1);

      /* Code a pop of all the values  */
      fts_bmax_code_pop_args(f, ac);
    }
}
