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

static fts_symbol_t sym_add_inplace = 0;
static fts_symbol_t sym_sub_inplace = 0;
static fts_symbol_t sym_mul_inplace = 0;
static fts_symbol_t sym_div_inplace = 0;
static fts_symbol_t sym_bus_inplace = 0;
static fts_symbol_t sym_vid_inplace = 0;

/***************************************************
 *
 *  integer vector @ integer vector = integer vector
 *
 */

static fts_atom_t *
binop_add_inplace_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(left_vector, i, l + r);
    }

  return result;
}

static fts_atom_t *
binop_sub_inplace_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_mul_inplace_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(left_vector, i, l * r);
    }

  return result;
}

static fts_atom_t *
binop_div_inplace_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(left_vector, i, l / r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_sub_inplace_iv_iv(right, left, result);
  return result;
}

static fts_atom_t *
binop_vid_inplace_iv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_div_inplace_iv_iv(right, left, result);
  return result;
}


/***************************************************
 *
 *  integer vector @ int = integer vector
 *
 */

static fts_atom_t *
binop_add_inplace_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l + r);
    }

  return result;
}

static fts_atom_t *
binop_sub_inplace_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_mul_inplace_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l * r);
    }

  return result;
}

static fts_atom_t *
binop_div_inplace_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l / r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, r - l);
    }

  return result;
}

static fts_atom_t *
binop_vid_inplace_iv_i(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  int r = fts_get_int(right);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, r / l);
    }

  return result;
}


/***************************************************
 *
 *  integer vector @ float = float vector
 *
 */

static fts_atom_t *
binop_add_inplace_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return result;
}

static fts_atom_t *
binop_sub_inplace_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_mul_inplace_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return result;
}

static fts_atom_t *
binop_div_inplace_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return result;
}

static fts_atom_t *
binop_vid_inplace_iv_f(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  float r = fts_get_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return result;
}


/***************************************************
 *
 *  float vector @ float vector = float vector
 *
 */

static fts_atom_t *
binop_add_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return result;
}

static fts_atom_t *
binop_sub_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_mul_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return result;
}

static fts_atom_t *
binop_div_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_sub_inplace_fv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_vid_inplace_fv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_div_inplace_fv_fv(right, left, result);
  return result;
}


/***************************************************
 *
 *  float vector @ number = float vector
 *
 */

static fts_atom_t *
binop_add_inplace_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return result;
}

static fts_atom_t *
binop_sub_inplace_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_mul_inplace_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return result;
}

static fts_atom_t *
binop_div_inplace_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return result;
}

static fts_atom_t *
binop_vid_inplace_fv_n(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(left);
  float r = fts_get_number_float(right);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return result;
}


/***************************************************
 *
 *  integer vector @ float vector
 *
 */

static fts_atom_t *
binop_add_inplace_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return result;
}

static fts_atom_t *
binop_sub_inplace_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return result;
}

static fts_atom_t *
binop_mul_inplace_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return result;
}

static fts_atom_t *
binop_div_inplace_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return result;
}

static fts_atom_t *
binop_bus_inplace_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return result;
}

static fts_atom_t *
binop_vid_inplace_iv_fv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(left);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(right);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = (float)fts_integer_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return result;
}


/***************************************************
 *
 *  float vector @ integer vector
 *
 */

static fts_atom_t *
binop_add_inplace_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_add_inplace_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_sub_inplace_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_bus_inplace_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_mul_inplace_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_mul_inplace_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_div_inplace_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_vid_inplace_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_bus_inplace_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_sub_inplace_iv_fv(right, left, result);
  return result;
}

static fts_atom_t *
binop_vid_inplace_fv_iv(fts_atom_t *left, fts_atom_t *right, fts_atom_t *result)
{
  binop_div_inplace_iv_fv(right, left, result);
  return result;
}

/***************************************************
 *
 *  initialize
 *
 */

void
binop_arith_init(void)
{
  sym_add_inplace = fts_new_symbol("add");
  sym_sub_inplace = fts_new_symbol("sub");
  sym_mul_inplace = fts_new_symbol("mul");
  sym_div_inplace = fts_new_symbol("div");
  sym_bus_inplace = fts_new_symbol("bus");
  sym_vid_inplace = fts_new_symbol("vid");

  /* integer vector @ integer vector = integer vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector);

  /* integer vector @ int = integer vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_iv_i, fts_s_integer_vector, fts_s_int);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_iv_i, fts_s_integer_vector, fts_s_int);

  /* integer vector @ float = float vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_iv_f, fts_s_integer_vector, fts_s_float);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_iv_f, fts_s_integer_vector, fts_s_float);

  /* float vector @ float vector = float vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector);

  /* float vector @ int(number) = float vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_fv_n, fts_s_float_vector, fts_s_int);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_fv_n, fts_s_float_vector, fts_s_int);

  /* float vector @ float(number) = float vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_fv_n, fts_s_float_vector, fts_s_float);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_fv_n, fts_s_float_vector, fts_s_float);

  /* integer vector @ float vector = float vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_iv_fv, fts_s_integer_vector, fts_s_float_vector);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_iv_fv, fts_s_integer_vector, fts_s_float_vector);

  /* float vector @ integer vector = float vector */
  binop_fun_declare(sym_add_inplace, binop_add_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_sub_inplace, binop_sub_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_mul_inplace, binop_mul_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_div_inplace, binop_div_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_bus_inplace, binop_bus_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector);
  binop_fun_declare(sym_vid_inplace, binop_vid_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector);
}
