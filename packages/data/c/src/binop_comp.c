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
#include "binops.h"
#include "data.h"

static fts_symbol_t sym_ee = 0;
static fts_symbol_t sym_gt = 0;
static fts_symbol_t sym_ne = 0;
static fts_symbol_t sym_ge = 0;
static fts_symbol_t sym_lt = 0;
static fts_symbol_t sym_le = 0;

/***************************************************
 *
 *  int @ int = int
 *
 */

static fts_atom_t *
binop_ee_i_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  int l = fts_get_int(left);
  int r = fts_get_int(right);

  data_atom_release(result);
  fts_set_int(result, l == r);

  return result;
}

static fts_atom_t *
binop_gt_i_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  int l = fts_get_int(left);
  int r = fts_get_int(right);

  data_atom_release(result);
  fts_set_int(result, l > r);

  return result;
}

static fts_atom_t *
binop_ne_i_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  int l = fts_get_int(left);
  int r = fts_get_int(right);

  data_atom_release(result);
  fts_set_int(result, l!=r);

  return result;
}

static fts_atom_t *
binop_ge_i_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  int l = fts_get_int(left);
  int r = fts_get_int(right);

  data_atom_release(result);
  fts_set_int(result, l >= r);

  return result;
}

static fts_atom_t *
binop_lt_i_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_gt_i_i(right, left, result);
  return result;
}

static fts_atom_t *
binop_le_i_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ge_i_i(right, left, result);
  return result;
}


/***************************************************
 *
 *  float @ float = int
 *
 */

static fts_atom_t *
binop_ee_f_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_float(left);
  float r = fts_get_float(right);

  data_atom_release(result);
  fts_set_int(result, l == r);

  return result;
}

static fts_atom_t *
binop_gt_f_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_float(left);
  float r = fts_get_float(right);

  data_atom_release(result);
  fts_set_int(result, l > r);

  return result;
}

static fts_atom_t *
binop_ne_f_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_float(left);
  float r = fts_get_float(right);

  data_atom_release(result);
  fts_set_int(result, l!=r);

  return result;
}

static fts_atom_t *
binop_ge_f_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_float(left);
  float r = fts_get_float(right);

  data_atom_release(result);
  fts_set_int(result, l >= r);

  return result;
}

static fts_atom_t *
binop_lt_f_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_gt_f_f(right, left, result);
  return result;
}

static fts_atom_t *
binop_le_f_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ge_f_f(right, left, result);
  return result;
}


/***************************************************
 *
 *  number @ number = int
 *
 */

static fts_atom_t *
binop_ee_n_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_number(left);
  float r = fts_get_number(right);

  data_atom_release(result);
  fts_set_int(result, l == r);

  return result;
}

static fts_atom_t *
binop_gt_n_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_number(left);
  float r = fts_get_number(right);

  data_atom_release(result);
  fts_set_int(result, l > r);

  return result;
}

static fts_atom_t *
binop_ne_n_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_number(left);
  float r = fts_get_number(right);

  data_atom_release(result);
  fts_set_int(result, l!=r);

  return result;
}

static fts_atom_t *
binop_ge_n_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  float l = fts_get_number(left);
  float r = fts_get_number(right);

  data_atom_release(result);
  fts_set_int(result, l >= r);

  return result;
}

static fts_atom_t *
binop_lt_n_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_gt_n_n(right, left, result);
  return result;
}

static fts_atom_t *
binop_le_n_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ge_n_n(right, left, result);
  return result;
}


/***************************************************
 *
 *  integer vector @ integer vector = integer vector
 *
 */

static fts_atom_t *
binop_ee_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return result;
}

static fts_atom_t *
binop_gt_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return result;
}

static fts_atom_t *
binop_ne_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l!=r);
    }

  return result;
}

static fts_atom_t *
binop_ge_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return result;
}

static fts_atom_t *
binop_lt_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_gt_iv_iv(right, left, result);
  return result;
}

static fts_atom_t *
binop_le_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ge_iv_iv(right, left, result);
  return result;
}


/***************************************************
 *
 *  integer vector @ int = integer vector
 *
 */

static fts_atom_t *
binop_ee_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return result;
}

static fts_atom_t *
binop_gt_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return result;
}

static fts_atom_t *
binop_ne_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l!=r);
    }

  return result;
}

static fts_atom_t *
binop_ge_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return result;
}

static fts_atom_t *
binop_lt_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return result;
}

static fts_atom_t *
binop_le_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return result;
}


/***************************************************
 *
 *  integer vector @ float = integer vector
 *
 */

static fts_atom_t *
binop_ee_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return result;
}

static fts_atom_t *
binop_gt_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return result;
}

static fts_atom_t *
binop_ne_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l!=r);
    }

  return result;
}

static fts_atom_t *
binop_ge_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return result;
}

static fts_atom_t *
binop_lt_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return result;
}

static fts_atom_t *
binop_le_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return result;
}


/***************************************************
 *
 *  float vector @ float vector = integer vector
 *
 */

static fts_atom_t *
binop_ee_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return result;
}

static fts_atom_t *
binop_gt_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return result;
}

static fts_atom_t *
binop_ne_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l!=r);
    }

  return result;
}

static fts_atom_t *
binop_ge_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return result;
}

static fts_atom_t *
binop_lt_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_gt_fv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_le_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ge_fv_fv(right, left, result);
  return result;
}


/***************************************************
 *
 *  float vector @ number = integer vector
 *
 */

static fts_atom_t *
binop_ee_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return result;
}

static fts_atom_t *
binop_gt_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return result;
}

static fts_atom_t *
binop_ne_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l!=r);
    }

  return result;
}

static fts_atom_t *
binop_ge_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return result;
}

static fts_atom_t *
binop_lt_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return result;
}

static fts_atom_t *
binop_le_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number(right);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return result;
}


/***************************************************
 *
 *  integer vector @ float vector
 *
 */

static fts_atom_t *
binop_ee_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return result;
}

static fts_atom_t *
binop_gt_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return result;
}

static fts_atom_t *
binop_ne_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l!=r);
    }

  return result;
}

static fts_atom_t *
binop_ge_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return result;
}

static fts_atom_t *
binop_lt_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return result;
}

static fts_atom_t *
binop_le_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = data_recycle_atom_integer_vector(result, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return result;
}


/***************************************************
 *
 *  float vector @ integer vector
 *
 */

static fts_atom_t *
binop_ee_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ee_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_gt_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_lt_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_ne_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ne_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_ge_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_le_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_lt_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_gt_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_le_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_ge_iv_fv(right, left, result);
  return result;
}

/***************************************************
 *
 *  initialize
 *
 */

void
binop_comp_init(void)
{
  sym_ee = fts_new_symbol("ee");
  sym_gt = fts_new_symbol("gt");
  sym_ne = fts_new_symbol("ne");
  sym_ge = fts_new_symbol("ge");
  sym_lt = fts_new_symbol("lt");
  sym_le = fts_new_symbol("le");

  /* int(number) @ int = int */
  binop_fun_declare(sym_ee, binop_ee_i_i, fts_s_int, fts_s_int);
  binop_fun_declare(sym_gt, binop_gt_i_i, fts_s_int, fts_s_int);
  binop_fun_declare(sym_ne, binop_ne_i_i, fts_s_int, fts_s_int);
  binop_fun_declare(sym_ge, binop_ge_i_i, fts_s_int, fts_s_int);
  binop_fun_declare(sym_lt, binop_lt_i_i, fts_s_int, fts_s_int);
  binop_fun_declare(sym_le, binop_le_i_i, fts_s_int, fts_s_int);

  /* int(number) @ float(number) = int */
  binop_fun_declare(sym_ee, binop_ee_n_n, fts_s_int, fts_s_float);
  binop_fun_declare(sym_gt, binop_gt_n_n, fts_s_int, fts_s_float);
  binop_fun_declare(sym_ne, binop_ne_n_n, fts_s_int, fts_s_float);
  binop_fun_declare(sym_ge, binop_ge_n_n, fts_s_int, fts_s_float);
  binop_fun_declare(sym_lt, binop_lt_n_n, fts_s_int, fts_s_float);
  binop_fun_declare(sym_le, binop_le_n_n, fts_s_int, fts_s_float);

  /* float(number) @ int(number) = int */
  binop_fun_declare(sym_ee, binop_ee_n_n, fts_s_float, fts_s_int);
  binop_fun_declare(sym_gt, binop_gt_n_n, fts_s_float, fts_s_int);
  binop_fun_declare(sym_ne, binop_ne_n_n, fts_s_float, fts_s_int);
  binop_fun_declare(sym_ge, binop_ge_n_n, fts_s_float, fts_s_int);
  binop_fun_declare(sym_lt, binop_lt_n_n, fts_s_float, fts_s_int);
  binop_fun_declare(sym_le, binop_le_n_n, fts_s_float, fts_s_int);

  /* float(number) @ float(number) = int */
  binop_fun_declare(sym_ee, binop_ee_n_n, fts_s_float, fts_s_float);
  binop_fun_declare(sym_gt, binop_gt_n_n, fts_s_float, fts_s_float);
  binop_fun_declare(sym_ne, binop_ne_n_n, fts_s_float, fts_s_float);
  binop_fun_declare(sym_ge, binop_ge_n_n, fts_s_float, fts_s_float);
  binop_fun_declare(sym_lt, binop_lt_n_n, fts_s_float, fts_s_float);
  binop_fun_declare(sym_le, binop_le_n_n, fts_s_float, fts_s_float);

  /* integer vector @ integer vector = integer vector */
  binop_fun_declare(sym_ee, binop_ee_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_gt, binop_gt_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_ne, binop_ne_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_ge, binop_ge_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_lt, binop_lt_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_le, binop_le_iv_iv, fts_s_integer_vector, fts_s_integer_vector);

  /* integer vector @ int = integer vector */
  binop_fun_declare(sym_ee, binop_ee_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_gt, binop_gt_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_ne, binop_ne_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_ge, binop_ge_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_lt, binop_lt_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_le, binop_le_iv_i, fts_s_integer_vector, fts_s_int);

  /* integer vector @ float = integer vector */
  binop_fun_declare(sym_ee, binop_ee_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_gt, binop_gt_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_ne, binop_ne_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_ge, binop_ge_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_lt, binop_lt_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_le, binop_le_iv_f, fts_s_integer_vector, fts_s_float);

  /* float vector @ float vector = integer vector */
  binop_fun_declare(sym_ee, binop_ee_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_gt, binop_gt_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_ne, binop_ne_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_ge, binop_ge_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_lt, binop_lt_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_le, binop_le_fv_fv, fts_s_float_vector, fts_s_float_vector);

  /* float vector @ int(number) = integer vector */
  binop_fun_declare(sym_ee, binop_ee_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_gt, binop_gt_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_ne, binop_ne_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_ge, binop_ge_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_lt, binop_lt_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_le, binop_le_fv_n, fts_s_float_vector, fts_s_int);

  /* float vector @ float(number) = integer vector */
  binop_fun_declare(sym_ee, binop_ee_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_gt, binop_gt_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_ne, binop_ne_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_ge, binop_ge_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_lt, binop_lt_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_le, binop_le_fv_n, fts_s_float_vector, fts_s_float);

  /* integer vector @ float vector = integer vector */
  binop_fun_declare(sym_ee, binop_ee_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_gt, binop_gt_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_ne, binop_ne_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_ge, binop_ge_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_lt, binop_lt_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_le, binop_le_iv_fv, fts_s_integer_vector, fts_s_float_vector);

  /* float vector @ integer vector = integer vector */
  binop_fun_declare(sym_ee, binop_ee_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_gt, binop_gt_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_ne, binop_ne_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_ge, binop_ge_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_lt, binop_lt_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_le, binop_le_fv_iv, fts_s_float_vector, fts_s_integer_vector);
}
