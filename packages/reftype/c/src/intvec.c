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
#include "intvec.h"

fts_symbol_t int_vector_symbol = 0;
fts_type_t int_vector_type = 0;
reftype_t *int_vector_reftype = 0;

static fts_data_class_t *int_vector_data_class = 0;

/* local */
static void 
intvec_set_size(int_vector_t *vector, int size)
{
  if(size > vector->alloc)
    {
      if(vector->alloc)
	vector->values = (int *)fts_realloc((void *)vector->values, sizeof(int) * size);
      else
	vector->values = (int *)fts_malloc(sizeof(int) * size);

      vector->alloc = size;
    }

  vector->size = size;
}

/* new/delete */
int_vector_t *
int_vector_new(int size)
{
  int_vector_t *vector;
  int i;

  vector = (int_vector_t *)fts_malloc(sizeof(int_vector_t));

  if(size > 0)
    {
      vector->values = (int *) fts_malloc(size * sizeof(int));
      vector->size = size;
      fts_vec_izero(vector->values, vector->size);  
    }
  else
    {
      vector->values = 0;
      vector->size = 0;
    }

  vector->alloc = size;

  fts_data_init((fts_data_t *)vector, int_vector_data_class);
  refdata_init((refdata_t *)vector, int_vector_reftype);

  return vector;
}

void 
int_vector_delete(int_vector_t *vector)
{
  fts_data_delete((fts_data_t *) vector);
  fts_free((void *)vector->values);
  fts_free((void *)vector);
}

int
int_vector_get_atoms(int_vector_t *vector, int ac, fts_atom_t *at)
{
  int i;
  int size = vector->size;

  if(ac > size)
    ac = size;

  for(i=0; i<ac; i++)
    fts_set_int(at + i, vector->values[i]);

  return size;
}

/* copy & zero */
void
int_vector_copy(int_vector_t *in, int_vector_t *out)
{
  intvec_set_size(out, in->size);
  fts_vec_icpy(in->values, out->values, in->size);
}

void
int_vector_zero(int_vector_t *vector)
{
  fts_vec_izero(vector->values, vector->size);  
}

/* set the size of the vector */
void
int_vector_set_size(int_vector_t *vector, int size)
{
  int old_size = vector->size;
  int tail = size - old_size;

  intvec_set_size(vector, size);

  if(tail > 0)
    fts_vec_izero(vector->values + old_size, tail);
}

void 
int_vector_set_from_ptr(int_vector_t *vector, int *ptr, int size)
{
  intvec_set_size(vector, size);
  fts_vec_icpy(ptr, vector->values, size);
}

void
int_vector_set_from_atom_list(int_vector_t *vector, int offset, int ac, const fts_atom_t *at)
{
  int size = int_vector_get_size(vector);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(&at[i]))
	vector->values[i + offset] = fts_get_number_int(&at[i]);
      else
	vector->values[i + offset] = 0;
    }
}

void
int_vector_set_const(int_vector_t *vector, int c)
{
  int *values = vector->values;
  int i;
  
  for(i=0; i<vector->size; i++)
    values[i] = c;
}

/* sum, min, max */
int
int_vector_get_sum(int_vector_t *vector)
{
  int sum = 0;
  int i;

  for(i=0; i<vector->size; i++)
    sum += vector->values[i];

  return sum;
}

int
int_vector_get_sub_sum(int_vector_t *vector, int from, int to)
{
  int sum = 0;
  int i;
  
  if(from < 0)
    from = 0;

  if(to >= vector->size)
    to = vector->size - 1;

  for (i=from; i<=to; i++)
    sum +=vector->values[i];

  return sum;
}

int
int_vector_get_min_value(int_vector_t *vector)
{
  int min;
  int i;

  min = vector->values[0];

  for (i = 1; i < vector->size; i++)
    if (vector->values[i] < min)
      min = vector->values[i];

  return min;
}


int
int_vector_get_max_value(int_vector_t *vector)
{
  int max;
  int i;

  max = vector->values[0];

  for (i = 1; i < vector->size; i++)
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
#define INT_VECTOR_SET    1
#define INT_VECTOR_UPDATE 2
#define INT_VECTOR_NAME   3

static void int_vector_export_fun(fts_data_t *data)
{
  int_vector_t *vector = (int_vector_t *)data;
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
	  fts_data_start_remote_call(data, INT_VECTOR_NAME);
	  fts_client_mess_add_symbol(name);
	  fts_data_end_remote_call();
	}
    }

  fts_data_start_remote_call(data, INT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i = 0; i < vector->size; i++)
    fts_client_mess_add_int(vector->values[i]);

  fts_data_end_remote_call();
}

static void 
int_vector_remote_set( fts_data_t *data, int ac, const fts_atom_t *at)
{
  int_vector_t *vector = (int_vector_t *)data;
  int offset;
  int nvalues;
  int i;

  /* arguments: offset, # of values, values */

  offset = fts_get_int(&at[0]);
  nvalues = fts_get_int(&at[1]);

  for (i=0; i<nvalues; i++)
    vector->values[i + offset] = fts_get_int(&at[i + 2]);
}


static void 
int_vector_remote_update( fts_data_t *data, int ac, const fts_atom_t *at)
{
  int_vector_t *vector = (int_vector_t *)data;
  int i;

  fts_data_start_remote_call(data, INT_VECTOR_SET);
  fts_client_mess_add_int(vector->size);

  for (i=0; i<vector->size; i++)
    fts_client_mess_add_int(vector->values[i]);

  fts_data_end_remote_call();
}

/********************************************************
 *
 *  files
 *
 */

#define INT_VECTOR_BLOCK_SIZE 256

static void
int_vector_grow(int_vector_t *vec, int size)
{
  int alloc = vec->alloc;

  while(size > alloc)
    alloc += INT_VECTOR_BLOCK_SIZE;

  int_vector_set_size(vec, alloc);
}

int 
int_vector_read_atom_file(int_vector_t *vec, fts_symbol_t file_name)
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
	int_vector_grow(vec, n);

      if(fts_is_number(&a))
	int_vector_set_element(vec, n, fts_get_number_int(&a));
      else
	int_vector_set_element(vec, n, 0.0f);
	
      n++;
    }

  int_vector_set_size(vec, n);
  
  fts_atom_file_close(file);

  return (n);
}

int
int_vector_write_atom_file(int_vector_t *vec, fts_symbol_t file_name)
{
  fts_atom_file_t *file;
  int size = int_vector_get_size(vec);
  int i;

  file = fts_atom_file_open(fts_symbol_name(file_name), "w");

  if(!file)
    return -1;

  /* write the content of the vector */
  for(i=0; i<size; i++)     
    {
      fts_atom_t a;
      
      fts_set_int(&a, int_vector_get_element(vec, i));
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

/* vector is actually quite a temporary hack; there should be a real
   save standard technique for fts_data_t; it assume that is reloaded
   for a vector !!*/

void int_vector_save_bmax(int_vector_t *vector, fts_bmax_file_t *f)
{
  fts_atom_t av[256];
  int ac = 0;
  int i;
  int offset = 0;

  for  (i = 0; i < vector->size; i++)
    {
      fts_set_int(&av[ac], vector->values[i]);

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
int_vector_constructor_null(int ac, const fts_atom_t *at)
{
  return (refdata_t *)int_vector_new(0);
}

static refdata_t *
int_vector_constructor_size(int ac, const fts_atom_t *at)
{
  return (refdata_t *)int_vector_new(fts_get_int(at));
}

static refdata_t *
int_vector_constructor_atom_array(int ac, const fts_atom_t *at)
{
  int_vector_t *vec = 0;
  fts_atom_array_t *aa = fts_get_atom_array(at);
  int size = fts_atom_array_get_size(aa);
  
  vec = int_vector_new(size);      
  int_vector_set_from_atom_list(vec, 0, size, fts_atom_array_get_ptr(aa));

  return (refdata_t *)vec;
}

static refdata_t *
int_vector_constructor_atoms(int ac, const fts_atom_t *at)
{
  int_vector_t *vec = 0;

  vec = int_vector_new(ac);
  int_vector_set_from_atom_list(vec, 0, ac, at);

  return (refdata_t *)vec;
}

static reftype_constructor_t
int_vector_dispatcher(int ac, const fts_atom_t *at)
{
  if(ac == 0)
    return int_vector_constructor_null;
  else if(ac == 1 && fts_is_int(at))
    return int_vector_constructor_size;
  else if(ac == 1 && fts_is_atom_array(at))
    return int_vector_constructor_atom_array;
  else if(ac > 1)
    return int_vector_constructor_atoms;
  else
    return 0;
}

static void
int_vector_destructor(refdata_t *vec)
{
  int_vector_delete((int_vector_t *)vec);
}

static int
int_vector_function(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  int_vector_t *vec = int_vector_create(ac - 1, at + 1);
  
  if(vec)
    {
      int_vector_atom_set(result, vec);
      return FTS_EXPRESSION_OK;
    }
  else
    return FTS_EXPRESSION_SYNTAX_ERROR;
}

/************************************************
 *
 *  config 
 *
 */
void int_vector_config(void)
{
  int_vector_symbol = fts_new_symbol("ivec");
  int_vector_type = int_vector_symbol; /* for now */

  int_vector_reftype = reftype_declare(int_vector_symbol, int_vector_dispatcher, int_vector_destructor);

  int_vector_data_class = fts_data_class_new(int_vector_type);
  fts_data_class_define_export_function(int_vector_data_class, int_vector_export_fun);
  fts_data_class_define_function(int_vector_data_class, INT_VECTOR_SET, int_vector_remote_set);
  fts_data_class_define_function(int_vector_data_class, INT_VECTOR_UPDATE, int_vector_remote_update);

  fts_expression_declare_fun(int_vector_symbol, int_vector_function);
}
