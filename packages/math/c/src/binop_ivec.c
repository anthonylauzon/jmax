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
  int_vector_t *right;
  int number;
  int_vector_t *res;
  fts_atom_t out;
} binop_ivec_t;

static void
binop_ivec_init_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;

  this->number = fts_get_number_int(at + 1);
  this->res = int_vector_atom_get(at + 2);
  int_vector_refer(this->res);

  int_vector_atom_set(&this->out, this->res);
}

static void
binop_ivec_init_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;

  this->right = int_vector_atom_get(at + 1);
  this->res = int_vector_atom_get(at + 2);
  int_vector_refer(this->right);
  int_vector_refer(this->res);

  int_vector_atom_set(&this->out, this->res);
}

static void
binop_ivec_delete_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;

  int_vector_release(this->res);
}

static void
binop_ivec_delete_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;

  int_vector_release(this->right);
  int_vector_release(this->res);
}

/**************************************************************************************
 *
 *  user methods
 *
 */

static void
binop_ivec_set_right_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;

  this->number = fts_get_number_int(at);
}

static void
binop_ivec_set_right_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *right = int_vector_atom_get(at);

  int_vector_release(this->right);
  this->right = right;
  int_vector_refer(right);
}

static void
binop_ivec_set_result(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *res = int_vector_atom_get(at);

  int_vector_release(this->res);

  this->res = res;
  int_vector_refer(res);

  int_vector_atom_set(&this->out, res);
}

/**************************************************************************************
 *
 *  int_vector (o) number
 *
 */

/* int_vector (o) number arithmetics */

static void
binop_ivec_add_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left + right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_sub_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left - right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_mul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left * right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_div_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left / right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_bus_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, right - left);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_vid_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, right / left);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

/* int_vector (o) number comparison */

static void
binop_ivec_ee_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left == right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_ne_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left != right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_gt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left > right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_ge_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left >= right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_lt_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left < right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_le_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;

  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, left <= right);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

/* int_vector (o) number min/max */

static void
binop_ivec_min_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;
      
  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, (right <= left)? right: left);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_max_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int size = int_vector_get_size(vec);
  int right = this->number;
  int_vector_t *res = this->res;
  int i;
      
  for(i=0; i<size; i++)
    {
      int left = int_vector_get_element(vec, i);
      int_vector_set_element(res, i, (right >= left)? right: left);
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

/**************************************************************************************
 *
 *  ivec (o) ivec
 *
 */

/* ivec (o) ivec arithmetics */

static void
binop_ivec_add_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] + r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_sub_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] - r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_mul_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] * r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_div_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] / r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_bus_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = r[i] - l[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_vid_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = r[i] / l[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

/* ivec (o) ivec comparison  */

static void
binop_ivec_ee_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] == r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_ne_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] != r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_gt_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] > r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_ge_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] >= r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_lt_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] < r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_le_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = l[i] <= r[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

/* ivec (o) ivec min/max */

static void
binop_ivec_min_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = (r[i] <= l[i])? r[i]: l[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

static void
binop_ivec_max_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_ivec_t *this = (binop_ivec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  int_vector_t *right = this->right;
  int_vector_t *res = this->res;
  int left_size = int_vector_get_size(left);
  int right_size = int_vector_get_size(right);
  int res_size = int_vector_get_size(res);
  int size = (left_size <= right_size)? ((left_size <= res_size)? left_size: res_size): ((right_size <= res_size)? right_size: res_size);
  int *l = int_vector_get_ptr(left);
  int *r = int_vector_get_ptr(right);
  int *x = int_vector_get_ptr(res);
  int i;

  for(i=0; i<size; i++)
    x[i] = (r[i] >= l[i])? r[i]: l[i];

  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->out);
}

/**************************************************************************************
 *
 *  class
 *
 */

fts_status_t
binop_ivec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t name = fts_get_symbol(at);

  fts_class_init(cl, sizeof(binop_ivec_t), 3, 1, 0);

  if(name == math_sym_add)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_add_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_add_ivec);
    }
  else if(name == math_sym_sub)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_sub_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_sub_ivec);
    }
  else if(name == math_sym_mul)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_mul_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_mul_ivec);
    }
  else if(name == math_sym_div)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_div_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_div_ivec);
    }
  else if(name == math_sym_bus)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_bus_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_bus_ivec);
    }
  else if(name == math_sym_vid)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_vid_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_vid_ivec);
    }
  else if(name == math_sym_ee)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_ee_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_ee_ivec);
    }
  else if(name == math_sym_ne)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_ne_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_ne_ivec);
    }
  else if(name == math_sym_gt)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_gt_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_gt_ivec);
    }
  else if(name == math_sym_ge)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_ge_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_ge_ivec);
    }
  else if(name == math_sym_lt)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_lt_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_lt_ivec);
    }
  else if(name == math_sym_le)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_le_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_le_ivec);
    }
  else if(name == math_sym_min)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_min_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_min_ivec);
    }
  else if(name == math_sym_max)
    {
      if(fts_is_number(at + 1))
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_max_number);
      else
	fts_method_define_varargs(cl, 0, int_vector_symbol, binop_ivec_max_ivec);
    }
  
  if(fts_is_number(at + 1))
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_ivec_init_number);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_ivec_delete_number);
      fts_method_define_varargs(cl, 1, fts_s_int, binop_ivec_set_right_number);
      fts_method_define_varargs(cl, 1, fts_s_float, binop_ivec_set_right_number);
    }
  else
    {
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_ivec_init_vector);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_ivec_delete_vector);
      fts_method_define_varargs(cl, 1, int_vector_symbol, binop_ivec_set_right_vector);
    }

  fts_method_define_varargs(cl, 2, int_vector_symbol, binop_ivec_set_result);

  return fts_Success;
}
