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

#include "fts.h"
#include "binop.h"
#include "intvec.h"
#include "floatvec.h"

/**************************************************************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t o;
  float_vector_t *right;
  float number;
  float_vector_t *res;
  fts_atom_t out;
} binop_fvec_t;

static void
binop_fvec_init_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->number = fts_get_number_float(at + 1);
  this->res = float_vector_atom_get(at + 2);
  float_vector_refer(this->res);

  float_vector_atom_set(&this->out, this->res);
}

static void
binop_fvec_init_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->right = float_vector_atom_get(at + 1);
  this->res = float_vector_atom_get(at + 2);
  float_vector_refer(this->right);
  float_vector_refer(this->res);

  float_vector_atom_set(&this->out, this->res);
}

static void
binop_fvec_delete_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  float_vector_release(this->res);
}

static void
binop_fvec_delete_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  float_vector_release(this->right);
  float_vector_release(this->res);
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
  float_vector_t *right = float_vector_atom_get(at);

  float_vector_release(this->right);
  this->right = right;
  float_vector_refer(right);
}

static void
binop_fvec_set_result(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *res = float_vector_atom_get(at);

  float_vector_release(this->res);

  this->res = res;
  float_vector_refer(res);

  float_vector_atom_set(&this->out, res);
}

/**************************************************************************************
 *
 *  float_vector (o) number
 *
 */

/* float_vector (o) number arithmetics */

static void
binop_fvec_add_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left + right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_sub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left - right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_mul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left * right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_div_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left / right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_bus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, right - left);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_vid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, right / left);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

/* float_vector (o) number comparison */

static void
binop_fvec_ee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left == right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_ne_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left != right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_gt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left > right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_ge_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left >= right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_lt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left < right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_le_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;

  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, left <= right);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

/* float_vector (o) number min/max */

static void
binop_fvec_min_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;
      
  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, (right <= left)? right: left);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_max_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float right = this->number;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(vec);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= res_size)? left_size: res_size;
  int i;
      
  for(i=0; i<size; i++)
    {
      float left = float_vector_get_element(vec, i);
      float_vector_set_element(res, i, (right >= left)? right: left);
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

/**************************************************************************************
 *
 *  fvec (o) fvec
 *
 */

/* fvec (o) fvec arithmetics */

static void
binop_fvec_add_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] + r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_sub_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] - r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_mul_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] * r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_div_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] / r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_bus_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = r[i] - l[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_vid_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = r[i] / l[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

/* fvec (o) fvec comparison  */

static void
binop_fvec_ee_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] == r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_ne_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] != r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_gt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] > r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_ge_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] >= r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_lt_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] < r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_le_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] <= r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

/* fvec (o) fvec min/max */

static void
binop_fvec_min_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = (r[i] <= l[i])? r[i]: l[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
}

static void
binop_fvec_max_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  float_vector_t *res = this->res;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int res_size = float_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  float *l = float_vector_get_ptr(left);
  float *r = float_vector_get_ptr(right);
  float *x = float_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = (r[i] >= l[i])? r[i]: l[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->out);
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
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_add_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_add_fvec);
    }
  else if(name == math_sym_sub)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_sub_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_sub_fvec);
    }
  else if(name == math_sym_mul)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_mul_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_mul_fvec);
    }
  else if(name == math_sym_div)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_div_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_div_fvec);
    }
  else if(name == math_sym_bus)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_bus_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_bus_fvec);
    }
  else if(name == math_sym_vid)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_vid_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_vid_fvec);
    }
  else if(name == math_sym_ee)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ee_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ee_fvec);
    }
  else if(name == math_sym_ne)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ne_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ne_fvec);
    }
  else if(name == math_sym_gt)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_gt_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_gt_fvec);
    }
  else if(name == math_sym_ge)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ge_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ge_fvec);
    }
  else if(name == math_sym_lt)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_lt_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_lt_fvec);
    }
  else if(name == math_sym_le)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_le_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_le_fvec);
    }
  else if(name == math_sym_min)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_min_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_min_fvec);
    }
  else if(name == math_sym_max)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_max_number);
      else
	fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_max_fvec);
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
      fts_method_define_varargs(cl, 1, float_vector_symbol, binop_fvec_set_right_vector);
    }

  fts_method_define_varargs(cl, 2, float_vector_symbol, binop_fvec_set_result);

  return fts_Success;
}
