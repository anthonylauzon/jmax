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
static fts_symbol_t sym_ne = 0;
static fts_symbol_t sym_gt = 0;
static fts_symbol_t sym_lt = 0;
static fts_symbol_t sym_ge = 0;
static fts_symbol_t sym_le = 0;

/***************************************************
 *
 *  int @ int = int
 *
 */

static op_t *
binop_ee_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l == r);

  return (op + 2);
}

static op_t *
binop_ne_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l != r);

  return (op + 2);
}

static op_t *
binop_gt_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l > r);

  return (op + 2);
}

static op_t *
binop_lt_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l < r);

  return (op + 2);
}

static op_t *
binop_ge_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l >= r);

  return (op + 2);
}

static op_t *
binop_le_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l <= r);

  return (op + 2);
}


/***************************************************
 *
 *  float @ float = float
 *
 */

static op_t *
binop_ee_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_int(&op[2], l == r);

  return (op + 2);
}

static op_t *
binop_ne_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_int(&op[2], l != r);

  return (op + 2);
}

static op_t *
binop_gt_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_int(&op[2], l > r);

  return (op + 2);
}

static op_t *
binop_lt_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_int(&op[2], l < r);

  return (op + 2);
}

static op_t *
binop_ge_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_int(&op[2], l >= r);

  return (op + 2);
}

static op_t *
binop_le_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_int(&op[2], l <= r);

  return (op + 2);
}


/***************************************************
 *
 *  number @ number = float
 *
 */

static op_t *
binop_ee_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_int(&op[2], l == r);

  return (op + 2);
}

static op_t *
binop_ne_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_int(&op[2], l != r);

  return (op + 2);
}

static op_t *
binop_gt_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_int(&op[2], l > r);

  return (op + 2);
}

static op_t *
binop_lt_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_int(&op[2], l < r);

  return (op + 2);
}

static op_t *
binop_ge_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_int(&op[2], l >= r);

  return (op + 2);
}

static op_t *
binop_le_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_int(&op[2], l <= r);

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ integer vector = integer vector
 *
 */

static op_t *
binop_ee_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ int = integer vector
 *
 */

static op_t *
binop_ee_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ float = float vector
 *
 */

static op_t *
binop_ee_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  float vector @ float vector = float vector
 *
 */

static op_t *
binop_ee_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  float vector @ number = float vector
 *
 */

static op_t *
binop_ee_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_integer_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ float vector
 *
 */

static op_t *
binop_ee_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  float vector @ integer vector
 *
 */

static op_t *
binop_ee_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l == r);
    }

  return (op + 2);
}

static op_t *
binop_ne_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l != r);
    }

  return (op + 2);
}

static op_t *
binop_gt_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l > r);
    }

  return (op + 2);
}

static op_t *
binop_lt_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l < r);
    }

  return (op + 2);
}

static op_t *
binop_ge_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l >= r);
    }

  return (op + 2);
}

static op_t *
binop_le_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_integer_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_integer_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(result_vector, i, l <= r);
    }

  return (op + 2);
}


/***************************************************
 *
 *  initialize
 *
 */

void
binop_comp_init(void)
{
  sym_ee = fts_new_symbol("==");
  sym_ne = fts_new_symbol("!=");
  sym_gt = fts_new_symbol(">");
  sym_lt = fts_new_symbol("<");
  sym_ge = fts_new_symbol(">=");
  sym_le = fts_new_symbol("<=");

  /* int @ int = int */
  binop_declare_fun(sym_ee, binop_ee_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_i_i, fts_s_int, fts_s_int, op_s_recycle);

  /* float @ float = float */
  binop_declare_fun(sym_ee, binop_ee_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_f_f, fts_s_float, fts_s_float, op_s_recycle);    

  /* int(number) @ float(number) = float */
  binop_declare_fun(sym_ee, binop_ee_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_n_n, fts_s_int, fts_s_float, op_s_recycle); 

  /* float(number) @ int(number) = float */
  binop_declare_fun(sym_ee, binop_ee_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_n_n, fts_s_float, fts_s_int, op_s_recycle);

  /* integer vector @ integer vector = integer vector */
  binop_declare_fun(sym_ee, binop_ee_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);

  /* integer vector @ int = integer vector */
  binop_declare_fun(sym_ee, binop_ee_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);

  /* integer vector @ float = float vector */
  binop_declare_fun(sym_ee, binop_ee_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);

  /* float vector @ float vector = float vector */
  binop_declare_fun(sym_ee, binop_ee_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);

  /* float vector @ int(number) = float vector */
  binop_declare_fun(sym_ee, binop_ee_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);

  /* float vector @ float(number) = float vector */
  binop_declare_fun(sym_ee, binop_ee_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle); 
  binop_declare_fun(sym_ge, binop_ge_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle); 
  binop_declare_fun(sym_le, binop_le_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle); 

  /* integer vector @ float vector = float vector */ 
  binop_declare_fun(sym_ee, binop_ee_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle); 
  binop_declare_fun(sym_le, binop_le_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);

  /* float vector @ integer vector = float vector */
  binop_declare_fun(sym_ee, binop_ee_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_ne, binop_ne_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_gt, binop_gt_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_lt, binop_lt_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_ge, binop_ge_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_le, binop_le_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
}
