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
} binop_fvec_t;

static void
binop_fvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  this->right = float_vector_atom_get(at + 1);
  int_vector_refer(this->right);
}

static void
binop_fvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;

  float_vector_release(this->right);
}

/**************************************************************************************
 *
 *  user methods
 *
 */

static void
binop_fvec_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *right = float_vector_atom_get(at + 1);

  float_vector_release(this->right);
  this->right = right;
  float_vector_refer(right);
}

/**************************************************************************************
 *
 *  ivec (o) fvec
 *
 */

/* ivec (o) fvec arithmetics */

static void
binop_fvec_add_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (int)((float)l[i] + r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_sub_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (int)((float)l[i] - r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_mul_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (int)((float)l[i] * r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_div_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (int)((float)l[i] / r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_bus_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (int)(r[i] - (float)l[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_vid_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (int)(r[i] / (float)l[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

/* ivec (o) fvec comparison  */

static void
binop_fvec_ee_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = ((float)l[i] == r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_ne_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = ((float)l[i] != r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_gt_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = ((float)l[i] > r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_ge_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = ((float)l[i] >= r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_lt_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = ((float)l[i] < r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_le_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = ((float)l[i] <= r[i]);

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

/* ivec (o) fvec min/max  */

static void
binop_fvec_min_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    {
      if(r[i] < l[i])
	l[i] = r[i];
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
binop_fvec_max_left_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  int_vector_t *left = int_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = int_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  int * restrict l = int_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    {
      if(r[i] > l[i])
	l[i] = r[i];
    }

  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

/**************************************************************************************
 *
 *  fvec (o) fvec
 *
 */

/* fvec (o) fvec arithmetics */

static void
binop_fvec_add_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] += r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_sub_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] -= r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_mul_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] *= r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_div_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] /= r[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_bus_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = r[i] - l[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_vid_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = r[i] / l[i];

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

/* fvec (o) fvec comparison  */

static void
binop_fvec_ee_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] == r[i]);

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_ne_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] != r[i]);

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_gt_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] > r[i]);

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_ge_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] >= r[i]);

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_lt_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] < r[i]);

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_le_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    l[i] = (float)(l[i] <= r[i]);

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

/* fvec (o) fvec min/max */

static void
binop_fvec_min_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    {
      if(r[i] < l[i])
	l[i] = r[i];
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
binop_fvec_max_left_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_fvec_t *this = (binop_fvec_t *)o;
  float_vector_t *left = float_vector_atom_get(at);
  float_vector_t *right = this->right;
  int left_size = float_vector_get_size(left);
  int right_size = float_vector_get_size(right);
  int size = (left_size <= right_size)? left_size: right_size;
  float * restrict l = float_vector_get_ptr(left);
  float * restrict r = float_vector_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
    {
      if(r[i] > l[i])
	l[i] = r[i];
    }

  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
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

  fts_class_init(cl, sizeof(binop_fvec_t), 2, 1, 0);

  /* init/delete */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_fvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_fvec_delete);

  if(name == math_sym_add)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_add_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_add_left_fvec);
    }
  else if(name == math_sym_sub)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_sub_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_sub_left_fvec);
    }
  else if(name == math_sym_mul)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_mul_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_mul_left_fvec);
    }
  else if(name == math_sym_div)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_div_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_div_left_fvec);
    }
  else if(name == math_sym_bus)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_bus_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_bus_left_fvec);
    }
  else if(name == math_sym_vid)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_vid_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_vid_left_fvec);
    }
  else if(name == math_sym_ee)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_ee_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ee_left_fvec);
    }
  else if(name == math_sym_ne)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_ne_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ne_left_fvec);
    }
  else if(name == math_sym_gt)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_gt_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_gt_left_fvec);
    }
  else if(name == math_sym_ge)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_ge_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_ge_left_fvec);
    }
  else if(name == math_sym_lt)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_lt_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_lt_left_fvec);
    }
  else if(name == math_sym_le)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_le_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_le_left_fvec);
    }
  else if(name == math_sym_min)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_min_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_min_left_fvec);
    }
  else if(name == math_sym_max)
    {
      fts_method_define_varargs(cl, 0, int_vector_symbol, binop_fvec_max_left_ivec);
      fts_method_define_varargs(cl, 0, float_vector_symbol, binop_fvec_max_left_fvec);
    }

  /* right inlet: set right operand */
  fts_method_define_varargs(cl, 1, float_vector_symbol, binop_fvec_set_right);

  return fts_Success;
}
