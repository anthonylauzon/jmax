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

static fts_atom_t *
binop_add_inplace_iv_iv(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_sub_inplace_iv_iv(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_mul_inplace_iv_iv(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_div_inplace_iv_iv(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_bus_inplace_iv_iv(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_vid_inplace_iv_iv(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}


/***************************************************
 *
 *  integer vector @ int = integer vector
 *
 */

static fts_atom_t *
binop_add_inplace_iv_i(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  int r = fts_get_int(at + 1);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l + r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_sub_inplace_iv_i(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  int r = fts_get_int(at + 1);
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l - r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_mul_inplace_iv_i(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  int r = fts_get_int(at + 1);
  int size = fts_integer_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l * r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_div_inplace_iv_i(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  int r = fts_get_int(at + 1);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, l / r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_bus_inplace_iv_i(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  int r = fts_get_int(at + 1);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, r - l);
    }

  return (at + 2);
}

static fts_atom_t *
binop_vid_inplace_iv_i(fts_atom_t *at)
{
  fts_integer_vector_t *left_vector = (fts_integer_vector_t *)fts_get_data(at + 0);
  int r = fts_get_int(at + 1);
  int size = fts_integer_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      int l = fts_integer_vector_get_element(left_vector, i);

      fts_integer_vector_set_element(left_vector, i, r / l);
    }

  return (at + 2);
}


/***************************************************
 *
 *  float vector @ float vector = float vector
 *
 */

static fts_atom_t *
binop_add_inplace_fv_fv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_sub_inplace_fv_fv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_mul_inplace_fv_fv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_div_inplace_fv_fv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_bus_inplace_fv_fv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_vid_inplace_fv_fv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_float_vector_t *right_vector = (fts_float_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}


/***************************************************
 *
 *  float vector @ number = float vector
 *
 */

static fts_atom_t *
binop_add_inplace_fv_n(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  float r = fts_get_number(at + 1);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l + r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_sub_inplace_fv_n(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  float r = fts_get_number(at + 1);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l - r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_mul_inplace_fv_n(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  float r = fts_get_number(at + 1);
  int size = fts_float_vector_get_size(left_vector);
  int i;

  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l * r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_div_inplace_fv_n(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  float r = fts_get_number(at + 1);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, l / r);
    }

  return (at + 2);
}

static fts_atom_t *
binop_bus_inplace_fv_n(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  float r = fts_get_number(at + 1);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r - l);
    }

  return (at + 2);
}

static fts_atom_t *
binop_vid_inplace_fv_n(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  float r = fts_get_number(at + 1);
  int size = fts_float_vector_get_size(left_vector);
  int i;
		
  for(i=0; i<size; i++)
    {
      float l = fts_float_vector_get_element(left_vector, i);

      fts_float_vector_set_element(left_vector, i, r / l);
    }

  return (at + 2);
}


/***************************************************
 *
 *  float vector @ integer vector
 *
 */

static fts_atom_t *
binop_add_inplace_fv_iv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_sub_inplace_fv_iv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_mul_inplace_fv_iv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_div_inplace_fv_iv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_bus_inplace_fv_iv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}

static fts_atom_t *
binop_vid_inplace_fv_iv(fts_atom_t *at)
{
  fts_float_vector_t *left_vector = (fts_float_vector_t *)fts_get_data(at + 0);
  fts_integer_vector_t *right_vector = (fts_integer_vector_t *)fts_get_data(at + 1);
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

  return (at + 2);
}


/***************************************************
 *
 *  initialize
 *
 */

void
binop_arith_inplace_init(void)
{
  sym_add = fts_new_symbol("add");
  sym_sub = fts_new_symbol("sub");
  sym_mul = fts_new_symbol("mul");
  sym_div = fts_new_symbol("div");
  sym_bus = fts_new_symbol("bus");
  sym_vid = fts_new_symbol("vid");

  /* integer vector @ integer vector = integer vector */
  binop_declare_fun(sym_add, binop_add_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_iv_iv, fts_s_integer_vector, fts_s_integer_vector, binops_s_inplace);

  /* integer vector @ int = integer vector */
  binop_declare_fun(sym_add, binop_add_inplace_iv_i, fts_s_integer_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_iv_i, fts_s_integer_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_iv_i, fts_s_integer_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_iv_i, fts_s_integer_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_iv_i, fts_s_integer_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_iv_i, fts_s_integer_vector, fts_s_int, binops_s_inplace);

  /* float vector @ float vector = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, binops_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, binops_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, binops_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, binops_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, binops_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_fv, fts_s_float_vector, fts_s_float_vector, binops_s_inplace);

  /* float vector @ int(number) = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_n, fts_s_float_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_n, fts_s_float_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_n, fts_s_float_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_n, fts_s_float_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_n, fts_s_float_vector, fts_s_int, binops_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_n, fts_s_float_vector, fts_s_int, binops_s_inplace);

  /* float vector @ float(number) = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_n, fts_s_float_vector, fts_s_float, binops_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_n, fts_s_float_vector, fts_s_float, binops_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_n, fts_s_float_vector, fts_s_float, binops_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_n, fts_s_float_vector, fts_s_float, binops_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_n, fts_s_float_vector, fts_s_float, binops_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_n, fts_s_float_vector, fts_s_float, binops_s_inplace);

  /* float vector @ integer vector = float vector */
  binop_declare_fun(sym_add, binop_add_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_sub, binop_sub_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_mul, binop_mul_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_div, binop_div_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_bus, binop_bus_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, binops_s_inplace);
  binop_declare_fun(sym_vid, binop_vid_inplace_fv_iv, fts_s_float_vector, fts_s_integer_vector, binops_s_inplace);
}
