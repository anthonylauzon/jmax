/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"
#include "floatvec.h"

fts_symbol_t float_vector_symbol = 0;
fts_type_t float_vector_type = 0;
reftype_t *float_vector_reftype = 0;

static fts_data_class_t *float_vector_data_class = 0;

/* local */
static void
floatvec_set_size(float_vector_t *vector, int size)
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
float_vector_t *
float_vector_new(int size)
{
  float_vector_t *vector;
  int i;

  vector = (float_vector_t *)fts_malloc(sizeof(float_vector_t));

  if(size > 0)
    {
      vector->values = (float *) fts_malloc(size * sizeof(float));
      vector->size = size;
      float_vector_zero(vector);
    }
  else
    {
      vector->values = 0;
      vector->size = 0;
    }

  vector->alloc = size;

  refdata_init((refdata_t *)vector, float_vector_reftype);
  fts_data_init((fts_data_t *) vector, float_vector_data_class);

  return vector;
}

void
float_vector_delete(float_vector_t *vector)
{
  fts_data_delete((fts_data_t *) vector);
  fts_free(vector->values);
  fts_free(vector);
}

int
float_vector_get_atoms(float_vector_t *vector, int ac, fts_atom_t *at)
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
float_vector_copy(float_vector_t *in, float_vector_t *out)
{
  floatvec_set_size(out, in->size);
  fts_vec_fcpy(in->values, out->values, in->size);
}

void
float_vector_zero(float_vector_t *vector)
{
  fts_vec_fzero(vector->values, vector->size);  
}

void
float_vector_set_const(float_vector_t *vector, float c)
{
  float *values = vector->values;
  int i;
  
  for(i=0; i<vector->size; i++)
    values[i] = c;
}

void
float_vector_set_size(float_vector_t *vector, int size)
{
  int old_size = vector->size;
  int tail = size - old_size;

  floatvec_set_size(vector, size);

  if(tail > 0)
    fts_vec_fzero(vector->values + old_size, tail);
}

void 
float_vector_set_from_ptr(float_vector_t *vector, float *ptr, int size)
{
  floatvec_set_size(vector, size);
  fts_vec_fcpy(ptr, vector->values, size);
}

void
float_vector_set_from_atom_list(float_vector_t *vector, int offset, int ac, const fts_atom_t *at)
{
  int size = float_vector_get_size(vector);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(&at[i]))
	vector->values[i + offset] = fts_get_number_float(&at[i]);
      else
	vector->values[i + offset] = 0.0f;
    }
}

/* sum, min, max */
float 
float_vector_get_sum(float_vector_t *vector)
{
  float sum = 0;
  int i;

  for(i=0; i<vector->size; i++)
    sum += vector->values[i];

  return sum;
}

float
float_vector_get_sub_sum(float_vector_t *vector, int from, int to)
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
float_vector_get_min_value(float_vector_t *vector)
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
float_vector_get_max_value(float_vector_t *vector)
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
 *  fts data
 *
 */

/* remote call codes */
#define FLOAT_VECTOR_SET    1
#define FLOAT_VECTOR_UPDATE 2
#define FLOAT_VECTOR_NAME   3

static void float_vector_export_fun(fts_data_t *data)
{
  float_vector_t *vector = (float_vector_t *)data;
  fts_object_t *creator = refdata_get_creator(vector);
  int i;

  if(creator)
    {
      fts_symbol_t name = 0;

      if(fts_object_get_variable(creator))
	name = fts_object_get_variable(creator);
      else if(creator->argc > 1 && fts_is_symbol(creator->argv + 1))
	name = fts_get_symbol(creator->argv + 1);

      if(name)
	{
	  fts_data_start_remote_call(data, FLOAT_VECTOR_NAME);
	  fts_client_mess_add_symbol(name);
	  fts_data_end_remote_call();
	}
    }

  fts_data_start_remote_call(data, FLOAT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i=0; i<vector->size; i++)
    fts_client_mess_add_int((int)(vector->values[i] * 32768.0f));

  fts_data_end_remote_call();
}

static void 
float_vector_remote_set( fts_data_t *data, int ac, const fts_atom_t *at)
{
  float_vector_t *vector = (float_vector_t *)data;
  int offset;
  int nvalues;
  int i;

  /* arguments: offset, # of values, values */

  offset = fts_get_int(&at[0]);
  nvalues = fts_get_int(&at[1]);

  for (i=0; i<nvalues; i++)
    vector->values[i + offset] = (float)fts_get_int(&at[i + 2]) / 32768.0f;
}


static void 
float_vector_remote_update( fts_data_t *data, int ac, const fts_atom_t *at)
{
  float_vector_t *vector = (float_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, FLOAT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i=0; i<vector->size; i++)
    fts_client_mess_add_int((int)(vector->values[i] * 32768.0f));

  fts_data_end_remote_call();
}

/********************************************************
 *
 *  files
 *
 */

#define FLOAT_VECTOR_BLOCK_SIZE 256

static void
float_vector_grow(float_vector_t *vec, int size)
{
  int alloc = vec->alloc;

  while(size > alloc)
    alloc += FLOAT_VECTOR_BLOCK_SIZE;

  float_vector_set_size(vec, alloc);
}

int 
float_vector_read_atom_file(float_vector_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file = fts_atom_file_open(fts_symbol_name(file_name), "r");
  int n = 0;
  fts_atom_t a;
  char c;

  if(!file)
    return -1;
  
  while(fts_atom_file_read(file, &a, &c))
    {
      if(n >= vec->alloc)
	float_vector_grow(vec, n);

      if(fts_is_number(&a))
	float_vector_set_element(vec, n, fts_get_number_float(&a));
      else
	float_vector_set_element(vec, n, 0.0f);
	
      n++;
    }

  float_vector_set_size(vec, n);
  
  fts_atom_file_close(file);

  return (n);
}

int
float_vector_write_atom_file(float_vector_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = float_vector_get_size(vec);
  int i;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the vector */
  for(i=0; i<size; i++)     
    {
      fts_atom_t a;
      
      fts_set_float(&a, float_vector_get_element(vec, i));
      fts_atom_file_write(file, &a, '\n');
    }

  fts_atom_file_close(file);

  return (i);
}

/********************************************************************
 *
 *  bmax format
 *
 */

/* this is actually quite a temporary hack; there should be a real
   save standard technique for fts_data_t; it assume that is reloaded
   for a vector !!*/

void float_vector_save_bmax(float_vector_t *vector, fts_bmax_file_t *f)
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

/*********************************************************
 *
 *  reference type and expression function
 *
 */
static refdata_t *
float_vector_constructor_null(int ac, const fts_atom_t *at)
{
  return (refdata_t *)float_vector_new(0);
}

static refdata_t *
float_vector_constructor_size(int ac, const fts_atom_t *at)
{
  return (refdata_t *)float_vector_new(fts_get_int(at));
}

static refdata_t *
float_vector_constructor_atom_array(int ac, const fts_atom_t *at)
{
  float_vector_t *vec = 0;
  fts_atom_array_t *aa = fts_get_atom_array(at);
  int size = fts_atom_array_get_size(aa);
  
  vec = float_vector_new(size);      
  float_vector_set_from_atom_list(vec, 0, size, fts_atom_array_get_ptr(aa));

  return (refdata_t *)vec;
}

static refdata_t *
float_vector_constructor_atoms(int ac, const fts_atom_t *at)
{
  float_vector_t *vec = 0;

  vec = float_vector_new(ac);
  float_vector_set_from_atom_list(vec, 0, ac, at);

  return (refdata_t *)vec;
}

static reftype_constructor_t
float_vector_dispatcher(int ac, const fts_atom_t *at)
{
  if(ac == 0)
    return float_vector_constructor_null;
  else if(ac == 1 && fts_is_int(at))
    return float_vector_constructor_size;
  else if(ac == 1 && fts_is_atom_array(at))
    return float_vector_constructor_atom_array;
  else if(ac > 1)
    return float_vector_constructor_atoms;
  else
    return 0;
}

static void
float_vector_destructor(refdata_t *vec)
{
  float_vector_delete((float_vector_t *)vec);
}

static int
float_vector_function(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  float_vector_t *vec = float_vector_create(ac - 1, at + 1);
  
  if(vec)
    {
      float_vector_atom_set(result, vec);
      return FTS_EXPRESSION_OK;
    }
  else
    return FTS_EXPRESSION_SYNTAX_ERROR;
}

/********************************************************************
 *
 *  config
 *
 */
void float_vector_config(void)
{
  float_vector_symbol = fts_new_symbol("fvec");
  float_vector_type = float_vector_symbol; /* for now */

  float_vector_reftype = reftype_declare(float_vector_symbol, float_vector_dispatcher, float_vector_destructor);

  /* float_vector_data_class = fts_data_class_new(float_vector_symbol); */
  float_vector_data_class = fts_data_class_new(fts_new_symbol("fvec"));
  fts_data_class_define_export_function(float_vector_data_class, float_vector_export_fun);
  fts_data_class_define_function(float_vector_data_class, FLOAT_VECTOR_SET, float_vector_remote_set);
  fts_data_class_define_function(float_vector_data_class, FLOAT_VECTOR_UPDATE, float_vector_remote_update);

  fts_expression_declare_fun(float_vector_symbol, float_vector_function);
}








