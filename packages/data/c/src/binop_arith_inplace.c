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
 *  integer vector @ integer vector = integer vector
 *
 */

static op_t *
binop_add_inplace_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_sub_inplace_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_mul_inplace_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_div_inplace_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_bus_inplace_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(left_vector, i, r - l);
    }

  return (op + 0);
}

static op_t *
binop_vid_inplace_iv_iv(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_integer_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);
      int r = fts_integer_vector_get_element(right_vector, i);

      fts_integer_vector_set_element(left_vector, i, r / l);
    }

  return (op + 0);
}


/***************************************************
 *
 *  integer vector @ int = integer vector
 *
 */

static op_t *
binop_add_inplace_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l + r);
    }

  return (op + 0);
}

static op_t *
binop_sub_inplace_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l - r);
    }

  return (op + 0);
}

static op_t *
binop_mul_inplace_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l * r);
    }

  return (op + 0);
}

static op_t *
binop_div_inplace_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l / r);
    }

  return (op + 0);
}

static op_t *
binop_bus_inplace_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, r - l);
    }

  return (op + 0);
}

static op_t *
binop_vid_inplace_iv_i(op_t *op)
{
  fts_integer_vector_t *left_vector = op_get_integer_vector(op[0]);
  int r = op_get_int(op[1]);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, r / l);
    }

  return (op + 0);
}


/***************************************************
 *
 *  float vector @ float vector = float vector
 *
 */

static op_t *
binop_add_inplace_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_sub_inplace_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_mul_inplace_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_div_inplace_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
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

  return (op + 0);
}

static op_t *
binop_bus_inplace_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return (op + 0);
}

static op_t *
binop_vid_inplace_fv_fv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_float_vector_t *right_vector = op_get_float_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_float_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = fts_float_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return (op + 0);
}


/***************************************************
 *
 *  float vector @ number = float vector
 *
 */

static op_t *
binop_add_inplace_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return (op + 0);
}

static op_t *
binop_sub_inplace_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return (op + 0);
}

static op_t *
binop_mul_inplace_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return (op + 0);
}

static op_t *
binop_div_inplace_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return (op + 0);
}

static op_t *
binop_bus_inplace_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return (op + 0);
}

static op_t *
binop_vid_inplace_fv_n(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  float r = op_get_number_float(op[1]);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return (op + 0);
}


/***************************************************
 *
 *  float vector @ integer vector
 *
 */

static op_t *
binop_add_inplace_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return (op + 0);
}

static op_t *
binop_sub_inplace_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return (op + 0);
}

static op_t *
binop_mul_inplace_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return (op + 0);
}

static op_t *
binop_div_inplace_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return (op + 0);
}

static op_t *
binop_bus_inplace_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return (op + 0);
}

static op_t *
binop_vid_inplace_fv_iv(op_t *op)
{
  fts_float_vector_t *left_vector = op_get_float_vector(op[0]);
  fts_integer_vector_t *right_vector = op_get_integer_vector(op[1]);
  int left_size = fts_float_vector_get_size(left_vector);
  int size = fts_integer_vector_get_size(right_vector);
  int i;
		
  if(left_size < size)
    size = left_size;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);
      float r = (float)fts_integer_vector_get_element(right_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return (op + 0);
}


/***************************************************
 *
 *  initialize
 *
 */

void
binop_arith_inplace_init(void)
{
  sym_add = fts_new_symbol("+");
  sym_sub = fts_new_symbol("-");
  sym_mul = fts_new_symbol("*");
  sym_div = fts_new_symbol("/");
  sym_bus = fts_new_symbol("_");
  sym_vid = fts_new_symbol("\\");

  /* integer vector @ integer vector = integer vector */
  binop_declare_fun(sym_add, binop_add_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, op_s_inplace);

  /* integer vector @ int = integer vector */
  binop_declare_fun(sym_add, binop_add_inplace_iv_i, fts_s_integer_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_iv_i, fts_s_integer_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_iv_i, fts_s_integer_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_iv_i, fts_s_integer_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_iv_i, fts_s_integer_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_iv_i, fts_s_integer_vector, fts_s_int, op_s_inplace);

  /* float vector @ float vector = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, op_s_inplace);

  /* float vector @ int(number) = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_n, fts_s_float_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_n, fts_s_float_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_n, fts_s_float_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_n, fts_s_float_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_n, fts_s_float_vector, fts_s_int, op_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_n, fts_s_float_vector, fts_s_int, op_s_inplace);

  /* float vector @ float(number) = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_n, fts_s_float_vector, fts_s_float, op_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_n, fts_s_float_vector, fts_s_float, op_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_n, fts_s_float_vector, fts_s_float, op_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_n, fts_s_float_vector, fts_s_float, op_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_n, fts_s_float_vector, fts_s_float, op_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_n, fts_s_float_vector, fts_s_float, op_s_inplace);

  /* float vector @ integer vector = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, op_s_inplace);
}
