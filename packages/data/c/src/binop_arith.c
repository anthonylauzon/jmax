/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

static fts_symbol_t sym_add = 0;
static fts_symbol_t sym_sub = 0;
static fts_symbol_t sym_mul = 0;
static fts_symbol_t sym_div = 0;
static fts_symbol_t sym_bus = 0;
static fts_symbol_t sym_vid = 0;

/***************************************************
 *
 *  int @ int = int
 *
 */

static op_t *
binop_add_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l + r);

  return (op + 2);
}

static op_t *
binop_sub_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l - r);

  return (op + 2);
}

static op_t *
binop_mul_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l * r);

  return (op + 2);
}

static op_t *
binop_div_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], l / r);

  return (op + 2);
}

static op_t *
binop_bus_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], r - l);

  return (op + 2);
}

static op_t *
binop_vid_i_i(op_t *op)
{
  int l = op_get_int(op[0]);
  int r = op_get_int(op[1]);

  op_set_int(&op[2], r / l);

  return (op + 2);
}


/***************************************************
 *
 *  float @ float = float
 *
 */

static op_t *
binop_add_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_float(&op[2], l + r);

  return (op + 2);
}

static op_t *
binop_sub_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_float(&op[2], l - r);

  return (op + 2);
}

static op_t *
binop_mul_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_float(&op[2], l * r);

  return (op + 2);
}

static op_t *
binop_div_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_float(&op[2], l / r);

  return (op + 2);
}

static op_t *
binop_bus_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_float(&op[2], r - l);

  return (op + 2);
}

static op_t *
binop_vid_f_f(op_t *op)
{
  float l = op_get_float(op[0]);
  float r = op_get_float(op[1]);

  op_set_float(&op[2], r / l);

  return (op + 2);
}


/***************************************************
 *
 *  number @ number = float
 *
 */

static op_t *
binop_add_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_float(&op[2], l + r);

  return (op + 2);
}

static op_t *
binop_sub_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_float(&op[2], l - r);

  return (op + 2);
}

static op_t *
binop_mul_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_float(&op[2], l * r);

  return (op + 2);
}

static op_t *
binop_div_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_float(&op[2], l / r);

  return (op + 2);
}

static op_t *
binop_bus_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_float(&op[2], r - l);

  return (op + 2);
}

static op_t *
binop_vid_n_n(op_t *op)
{
  float l = op_get_number_float(op[0]);
  float r = op_get_number_float(op[1]);

  op_set_float(&op[2], r / l);

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ int = integer vector
 *
 */

static op_t *
binop_add_iv_i(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_iv_i(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_iv_i(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_iv_i(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_iv_i(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_iv_i(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ float = float vector
 *
 */

static op_t *
binop_add_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_iv_f(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  float r = op_get_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ integer vector = integer vector
 *
 */

static op_t *
binop_add_iv_iv(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_iv_iv(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_iv_iv(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_iv_iv(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_iv_iv(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_iv_iv(op_t *op)
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

      fts_integer_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}


/***************************************************
 *
 *  integer vector @ float vector = float vector
 *
 */

static op_t *
binop_add_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_iv_fv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}


/***************************************************
 *
 *  float vector @ number = float vector
 *
 */

static op_t *
binop_add_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  fts_float_vector_t *result_vector;
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}


/***************************************************
 *
 *  float vector @ integer vector = float vector
 *
 */

static op_t *
binop_add_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}


/***************************************************
 *
 *  float vector @ float vector = float vector
 *
 */

static op_t *
binop_add_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l + r);
    }

  return (op + 2);
}

static op_t *
binop_sub_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l - r);
    }

  return (op + 2);
}

static op_t *
binop_mul_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l * r);
    }

  return (op + 2);
}

static op_t *
binop_div_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, l / r);
    }

  return (op + 2);
}

static op_t *
binop_bus_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, r - l);
    }

  return (op + 2);
}

static op_t *
binop_vid_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  fts_float_vector_t *result_vector;
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  result_vector = op_recycle_to_float_vector(op + 2, size);
  
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(result_vector, i, r / l);
    }

  return (op + 2);
}

/***************************************************
 *
 *  initialize
 *
 */

void
binop_arith_init(void)
{
  sym_add = fts_new_symbol("+");
  sym_sub = fts_new_symbol("-");
  sym_mul = fts_new_symbol("*");
  sym_div = fts_new_symbol("/");
  sym_bus = fts_new_symbol("inv+");
  sym_vid = fts_new_symbol("inv*");

  /* int @ int = int */
  binop_declare_fun(sym_add, binop_add_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_i_i, fts_s_int, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_i_i, fts_s_int, fts_s_int, op_s_recycle);

  /* float @ float = float */
  binop_declare_fun(sym_add, binop_add_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_f_f, fts_s_float, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_f_f, fts_s_float, fts_s_float, op_s_recycle);    

  /* int(number) @ float(number) = float */
  binop_declare_fun(sym_add, binop_add_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_n_n, fts_s_int, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_n_n, fts_s_int, fts_s_float, op_s_recycle); 

  /* float(number) @ int(number) = float */
  binop_declare_fun(sym_add, binop_add_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_n_n, fts_s_float, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_n_n, fts_s_float, fts_s_int, op_s_recycle);

  /* integer vector @ int = integer vector */
  binop_declare_fun(sym_add, binop_add_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_iv_i, fts_s_integer_vector, fts_s_int, op_s_recycle);

  /* integer vector @ float = float vector */
  binop_declare_fun(sym_add, binop_add_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_iv_f, fts_s_integer_vector, fts_s_float, op_s_recycle);

  /* integer vector @ integer vector = integer vector */
  binop_declare_fun(sym_add, binop_add_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_recycle);

  /* integer vector @ float vector = float vector */ 
  binop_declare_fun(sym_add, binop_add_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle); 
  binop_declare_fun(sym_vid, binop_vid_iv_fv, fts_s_integer_vector, fts_s_float_vector, op_s_recycle);

  /* float vector @ int(number) = float vector */
  binop_declare_fun(sym_add, binop_add_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_fv_n, fts_s_float_vector, fts_s_int, op_s_recycle);

  /* float vector @ float(number) = float vector */
  binop_declare_fun(sym_add, binop_add_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle); 
  binop_declare_fun(sym_bus, binop_bus_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle); 
  binop_declare_fun(sym_vid, binop_vid_fv_n, fts_s_float_vector, fts_s_float, op_s_recycle); 

  /* float vector @ integer vector = float vector */
  binop_declare_fun(sym_add, binop_add_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_recycle);

  /* float vector @ float vector = float vector */
  binop_declare_fun(sym_add, binop_add_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_sub, binop_sub_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_mul, binop_mul_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_div, binop_div_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_bus, binop_bus_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
  binop_declare_fun(sym_vid, binop_vid_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_recycle);
}
