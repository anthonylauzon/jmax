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

  fvec_atom_set(&this->out, this->res);
}

static void
binop_fvec_init_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->right = fvec_atom_get(at + 1);
  this->res = fvec_atom_get(at + 2);
  fts_object_refer((fts_object_t *)this->right);
  fts_object_refer((fts_object_t *)this->res);

  fvec_atom_set(&this->out, this->res);
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

  fvec_atom_set(&this->out, res);
}

/**************************************************************************************
 *
 *  fvec x number
 *
 */

/* fvec x number arithmetics */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/* fvec x number comparison */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/* fvec x number min/max */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/**************************************************************************************
 *
 *  fvec x fvec
 *
 */

/* fvec x fvec arithmetics */
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/* fvec x fvec comparison  */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/* fvec x fvec min/max */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/**************************************************************************************
 *
 *  number x fvec
 *
 */

/* number x fvec arithmetics */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/* fvec x number comparison */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/* fvec x number min/max */

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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
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

  fts_outlet_send(o, 0, fvec_symbol, 1, &this->out);
}

/**************************************************************************************
 *
 *  class
 *
 */

fts_status_t
binop_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t name = fts_get_symbol(at);

  fts_class_init(cl, sizeof(binop_fvec_t), 3, 1, 0);

  if(name == math_sym_add)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_add_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_add_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_add_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_add_fvec);
	}
    }
  else if(name == math_sym_sub)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_sub_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_sub_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_sub_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_sub_fvec);
	}
    }
  else if(name == math_sym_mul)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_mul_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_mul_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_mul_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_mul_fvec);
	}
    }
  else if(name == math_sym_div)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_div_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_div_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_div_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_div_fvec);
	}
    }
  else if(name == math_sym_bus)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_bus_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_bus_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_bus_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_bus_fvec);
	}
    }
  else if(name == math_sym_vid)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_vid_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_vid_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_vid_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_vid_fvec);
	}
    }
  else if(name == math_sym_ee)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ee_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ee_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_ee_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_ee_fvec);
	}
    }
  else if(name == math_sym_ne)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ne_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ne_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_ee_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_ee_fvec);
	}
    }
  else if(name == math_sym_gt)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_gt_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_gt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_gt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_gt_fvec);
	}
    }
  else if(name == math_sym_ge)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ge_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ge_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_ge_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_ge_fvec);
	}
    }
  else if(name == math_sym_lt)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_lt_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_lt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_lt_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_lt_fvec);
	}
    }
  else if(name == math_sym_le)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_le_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_le_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_le_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_le_fvec);
	}
    }
  else if(name == math_sym_min)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_min_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_min_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_min_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_min_fvec);
	}
    }
  else if(name == math_sym_max)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_max_number);
      else
	{
	  fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_max_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_float, binop_fvec_number_max_fvec);
	  fts_method_define_varargs(cl, 0, fts_s_int, binop_fvec_number_max_fvec);
	}
    }
  
  if(fts_is_number(at + 1))
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

  fts_method_define_varargs(cl, 2, fvec_symbol, binop_fvec_set_result);

  return fts_Success;
}
