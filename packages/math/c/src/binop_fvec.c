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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include <fts/fts.h>
#include "binop.h"
#include "ivec.h"
#include "fvec.h"

/**************************************************************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t o;
  fvec_t *right;
  float number;
  fvec_t *res;
  fts_atom_t out;
} binop_fvec_t;

static void
binop_fvec_init_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->number = fts_get_number_float(at + 1);
  this->res = fvec_atom_get(at + 2);
  fts_object_refer((fts_object_t *)this->res);
}

static void
binop_fvec_init_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->right = fvec_atom_get(at + 1);
  this->res = fvec_atom_get(at + 2);
  fts_object_refer((fts_object_t *)this->right);
  fts_object_refer((fts_object_t *)this->res);
}

static void
binop_fvec_delete_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  fts_object_release((fts_object_t *)this->res);
}

static void
binop_fvec_delete_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  fts_object_release((fts_object_t *)this->right);
  fts_object_release((fts_object_t *)this->res);
}

/**************************************************************************************
 *
 *  user methods
 *
 */

static void
binop_fvec_set_right_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->number = fts_get_number_float(at);
}

static void
binop_fvec_set_right_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *right = fvec_atom_get(at);

  fts_object_release((fts_object_t *)this->right);
  this->right = right;
  fts_object_refer((fts_object_t *)right);
}

static void
binop_fvec_set_result(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *res = fvec_atom_get(at);

  fts_object_release((fts_object_t *)this->res);

  this->res = res;
  fts_object_refer((fts_object_t *)res);
}

/**************************************************************************************
 *
 *  fvec x number
 *
 */

static void
binop_fvec_add_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left + right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_sub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left - right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_mul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left * right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_div_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  if(right != 0.0)
    {
      for(i=0; i<size; i++)
	{
	  float left = fvec_get_element(vec, i);
	  fvec_set_element(res, i, left / right);
	}
    }
  else
    {
      for(i=0; i<size; i++)
	fvec_set_element(res, i, 0.0);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_bus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, right - left);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_vid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      
      if(left != 0.0)
	fvec_set_element(res, i, right / left);
      else
	fvec_set_element(res, i, 0.0);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_ee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left == right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_ne_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left != right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_gt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left > right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_ge_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left >= right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_lt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left < right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_le_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, left <= right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_min_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;
      
  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, (right <= left)? right: left);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_max_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *vec = fvec_atom_get(at);
  float right = this->number;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;
      
  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float left = fvec_get_element(vec, i);
      fvec_set_element(res, i, (right >= left)? right: left);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

/**************************************************************************************
 *
 *  fvec x fvec
 *
 */

void
binop_fvec_add_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] + r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_sub_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] - r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_mul_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] * r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_div_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      if(r[i] != 0.0)
	x[i] = l[i] / r[i];
      else
	x[i] = 0.0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_bus_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = r[i] - l[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_vid_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      if(l[i] != 0.0)
	x[i] = r[i] / l[i];
      else
	x[i] = 0.0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_ee_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] == r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_ne_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] != r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_gt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] > r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_ge_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] >= r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_lt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] < r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_le_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = l[i] <= r[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_min_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = (r[i] <= l[i])? r[i]: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_max_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  fvec_t *res = this->res;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int res_size = fvec_get_size(res);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l = fvec_get_ptr(left);
  float *r = fvec_get_ptr(right);
  float *x = fvec_get_ptr(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    x[i] = (r[i] >= l[i])? r[i]: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

/**************************************************************************************
 *
 *  number x fvec
 *
 */

static void
binop_fvec_number_add_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left + right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_sub_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left - right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_mul_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left * right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_div_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);

      if(right != 0.0)
	fvec_set_element(res, i, left / right);
      else
	fvec_set_element(res, i, 0.0);	
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_bus_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, right - left);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_vid_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  if(left != 0.0)
    {
      for(i=0; i<size; i++)
	{
	  float right = fvec_get_element(vec, i);
	  fvec_set_element(res, i, right / left);
	}
    }
  else
    {
      for(i=0; i<size; i++)
	fvec_set_element(res, i, 0.0);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_ee_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left == right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_ne_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left != right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_gt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left > right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_ge_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left >= right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_lt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left < right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_le_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;

  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, left <= right);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_min_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;
      
  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, (right <= left)? right: left);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

static void
binop_fvec_number_max_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float left = fts_get_number_float(at);
  fvec_t *vec = this->right;
  fvec_t *res = this->res;
  int size = fvec_get_size(vec);
  int res_size = fvec_get_size(res);
  int i;
      
  if(res_size < size)
    fvec_set_size(res, size);

  for(i=0; i<size; i++)
    {
      float right = fvec_get_element(vec, i);
      fvec_set_element(res, i, (right >= left)? right: left);
    }

  fts_outlet_object(o, 0, (fts_object_t *)this->res);
}

/**************************************************************************************
 *
 *  fvec x fvec inplace
 *
 */

static void
binop_fvec_add_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] += r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_sub_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] -= r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_mul_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] *= r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_div_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    {
      if(r[i] != 0)
	l[i] /= r[i];
      else
	l[i] = 0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_bus_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = r[i] - l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_vid_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    {
      if(l[i] != 0)
	l[i] = r[i] / l[i];
      else
	l[i] = 0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_ee_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = l[i] == r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_ne_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = l[i] != r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_gt_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = l[i] > r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_ge_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = l[i] >= r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_lt_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = l[i] < r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_le_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = l[i] <= r[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_min_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = (r[i] <= l[i])? r[i]: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_max_fvec_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  fvec_t *left = fvec_atom_get(at);
  fvec_t *right = this->right;
  int left_size = fvec_get_size(left);
  int right_size = fvec_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float *l, *r;
  int i;
  
  l = fvec_get_ptr(left);
  r = fvec_get_ptr(right);
  
  for(i=0; i<size; i++)
    l[i] = (r[i] >= l[i])? r[i]: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

/**************************************************************************************
 *
 *  class
 *
 */

void
binop_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(fts_is_number(at))
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_fvec_init_number);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_fvec_delete_number);
      fts_method_define_varargs(cl, 1, fts_s_int, binop_fvec_set_right_number);
      fts_method_define_varargs(cl, 1, fts_s_float, binop_fvec_set_right_number);
    }
  else
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_fvec_init_vector);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_fvec_delete_vector);
      fts_method_define_varargs(cl, 1, fvec_symbol, binop_fvec_set_right_vector);
    }
  
  if(ac >= 2)
    fts_method_define_varargs(cl, 2, fvec_symbol, binop_fvec_set_result);
}

fts_status_t
binop_fvec_add_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_add_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_add_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_add_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_add_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_add_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_sub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_sub_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_sub_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_sub_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_sub_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_sub_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_mul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_mul_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_mul_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_mul_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_mul_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_mul_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_div_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_div_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_div_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_div_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_div_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_div_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_bus_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_bus_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_bus_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_bus_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_bus_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_vid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_vid_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_vid_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_vid_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_vid_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_vid_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_ee_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ee_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ee_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ee_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_ee_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_ee_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_ne_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ne_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ne_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ne_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_ne_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_ne_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_ge_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ge_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ge_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ge_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_ge_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_ge_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_gt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_gt_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_gt_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_gt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_gt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_gt_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_le_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_le_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_le_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_le_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_le_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_le_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_lt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_lt_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_lt_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_lt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_lt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_lt_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_min_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_min_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_min_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_min_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_min_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_min_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}

fts_status_t
binop_fvec_max_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_fvec_t), ac, 1, 0);

  if(fts_is_number(at + 1))
    fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_max_number);
  else
    {
      if(ac <= 2)
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_max_fvec_inplace);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_max_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_max_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_max_fvec);
	}
    }

  binop_fvec_instantiate(cl, ac, at);
}
