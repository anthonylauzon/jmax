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

/**************************************************************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_atom_t right;
} binop_number_t;

static void
binop_number_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  this->right = at[1];
}

/**************************************************************************************
 *
 *  user methods
 *
 */

static void
binop_number_set_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  this->right = at[0];
}

/**************************************************************************************
 *
 *  integer x number
 *
 */

static void
binop_number_add_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) + fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) + fts_get_float(&this->right));
}

static void
binop_number_sub_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) - fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) - fts_get_float(&this->right));
}

static void
binop_number_mul_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) * fts_get_int(&this->right));
  else
    fts_outlet_float(o, 0, (float)fts_get_int(at) * fts_get_float(&this->right));
}

static void
binop_number_div_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    {
      int right = fts_get_int(&this->right);
      
      if(right != 0)
	fts_outlet_int(o, 0, fts_get_int(at) / right);
      else
	fts_outlet_int(o, 0, 0);
    }
  else
    {
      float right = fts_get_float(&this->right);
      
      if(right != 0.0)
	fts_outlet_float(o, 0, fts_get_int(at) / right);
      else
	fts_outlet_float(o, 0, 0.0);
    }
}

static void
binop_number_bus_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(&this->right) - fts_get_int(at));
  else
    fts_outlet_float(o, 0, fts_get_float(&this->right) - (float)fts_get_int(at));
}

static void
binop_number_vid_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  int right = fts_get_int(at);

  if(fts_is_int(&this->right))
    {
      int right = fts_get_int(at);
      
      if(right != 0)
	fts_outlet_int(o, 0, fts_get_int(&this->right) / right);
      else
	fts_outlet_int(o, 0, 0);
    }
  else
    {
      float right = (float)fts_get_int(at);
      
      if(right != 0)
	fts_outlet_float(o, 0, fts_get_float(&this->right) / right);
      else
	fts_outlet_float(o, 0, 0.0);
    }
}

static void
binop_number_ee_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) == fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) == fts_get_float(&this->right));
}

static void
binop_number_ne_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) != fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) != fts_get_float(&this->right));
}

static void
binop_number_gt_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) > fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) > fts_get_float(&this->right));
}

static void
binop_number_ge_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) >= fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) >= fts_get_float(&this->right));
}

static void
binop_number_lt_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) < fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) < fts_get_float(&this->right));
}

static void
binop_number_le_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    fts_outlet_int(o, 0, fts_get_int(at) <= fts_get_int(&this->right));
  else
    fts_outlet_int(o, 0, (float)fts_get_int(at) <= fts_get_float(&this->right));
}

static void
binop_number_min_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    {
      int left = fts_get_int(at);
      int right = fts_get_int(&this->right);

      if(right < left)
	fts_outlet_primitive(o, 0, &this->right);
      else
	fts_outlet_primitive(o, 0, at);  
    }
  else
    {
      float left = (float)fts_get_int(at);
      float right = fts_get_float(&this->right);
      
      if(right < left)
	fts_outlet_primitive(o, 0, &this->right);
      else
	fts_outlet_primitive(o, 0, at);  
    }
}

static void
binop_number_max_left_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  if(fts_is_int(&this->right))
    {
      int left = fts_get_int(at);
      int right = fts_get_int(&this->right);

      if(right > left)
	fts_outlet_int(o, 0, right);
      else
	fts_outlet_primitive(o, 0, at);  
    }
  else
    {
      float left = (float)fts_get_int(at);
      float right = fts_get_float(&this->right);
      
      if(right > left)
	fts_outlet_float(o, 0, right);
      else
	fts_outlet_primitive(o, 0, at);  
    }
}

/**************************************************************************************
 *
 *  float x number
 *
 */

static void
binop_number_add_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) + fts_get_number_float(&this->right));
}

static void
binop_number_sub_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) - fts_get_number_float(&this->right));
}

static void
binop_number_mul_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_float(at) * fts_get_number_float(&this->right));
}

static void
binop_number_div_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  float right = fts_get_number_float(&this->right);

  if(right != 0.0)
    fts_outlet_float(o, 0, fts_get_float(at) / right);
  else
    fts_outlet_float(o, 0, 0.0);
}

static void
binop_number_bus_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_float(o, 0, fts_get_number_float(&this->right) - fts_get_float(at));
}

static void
binop_number_vid_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  float right = fts_get_number_float(at);

  if(right != 0.0)
    fts_outlet_float(o, 0, fts_get_float(&this->right) / right);
  else
    fts_outlet_float(o, 0, 0.0);
}

static void
binop_number_ee_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) == fts_get_number_float(&this->right));
}

static void
binop_number_ne_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) != fts_get_number_float(&this->right));
}

static void
binop_number_gt_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) > fts_get_number_float(&this->right));
}

static void
binop_number_ge_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) >= fts_get_number_float(&this->right));
}

static void
binop_number_lt_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) < fts_get_number_float(&this->right));
}

static void
binop_number_le_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;

  fts_outlet_int(o, 0, fts_get_float(at) <= fts_get_number_float(&this->right));
}

static void
binop_number_min_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  float left = fts_get_float(at);
    
  if(fts_get_number_float(&this->right) < left)
    fts_outlet_primitive(o, 0, &this->right);
  else
    fts_outlet_primitive(o, 0, at);  
}

static void
binop_number_max_left_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  float left = fts_get_float(at);
    
  if(fts_get_number_float(&this->right) > left)
    fts_outlet_primitive(o, 0, &this->right);
  else
    fts_outlet_primitive(o, 0, at);  
}

/**************************************************************************************
 *
 *  ivec x number inplace
 *
 */

static void
binop_ivec_add_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] += right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_sub_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] -= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_mul_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] *= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_div_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    {
      if(right != 0)
	l[i] /= right;
      else
	l[i] = 0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_bus_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = right - l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_vid_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    {
      if(l[i] != 0)
	l[i] = right / l[i];
      else
	l[i] = 0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_ee_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] == right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_ne_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] != right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_gt_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] > right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_ge_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] >= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_lt_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] < right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_le_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] <= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_min_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = (right <= l[i])? right: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_ivec_max_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  ivec_t *left = ivec_atom_get(at);
  int right = fts_get_number_int(&this->right);
  int size = ivec_get_size(left);
  int *l = ivec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = (right >= l[i])? right: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

/**************************************************************************************
 *
 *  fvec x number inplace
 *
 */

static void
binop_fvec_add_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] += right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_sub_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] -= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_mul_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] *= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_div_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    {
      if(right != 0)
	l[i] /= right;
      else
	l[i] = 0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_bus_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = right - l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_vid_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    {
      if(l[i] != 0)
	l[i] = right / l[i];
      else
	l[i] = 0;
    }

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_ee_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] == right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_ne_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] != right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_gt_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] > right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_ge_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] >= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_lt_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] < right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_le_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = l[i] <= right;

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_min_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = (right <= l[i])? right: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

static void
binop_fvec_max_number_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_number_t *this = (binop_number_t *)o;
  fvec_t *left = fvec_atom_get(at);
  float right = fts_get_number_float(&this->right);
  int size = fvec_get_size(left);
  float *l = fvec_get_ptr(left);
  int i;
  
  for(i=0; i<size; i++)
    l[i] = (right >= l[i])? right: l[i];

  fts_outlet_object(o, 0, (fts_object_t *)left);
}

/**************************************************************************************
 *
 *  class
 *
 */

fts_status_t
binop_number_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t name = fts_get_symbol(at);

  fts_class_init(cl, sizeof(binop_number_t), 2, 1, 0);

  /* inlet system */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_number_init);

  if(name == math_sym_add)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_add_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_add_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_add_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_add_number_inplace);
    }
  else if(name == math_sym_sub)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_sub_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_sub_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_sub_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_sub_number_inplace);
    }
  else if(name == math_sym_mul)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_mul_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_mul_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_mul_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_mul_number_inplace);
    }
  else if(name == math_sym_div)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_div_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_div_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_div_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_div_number_inplace);
    }
  else if(name == math_sym_bus)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_bus_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_bus_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_bus_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_bus_number_inplace);
    }
  else if(name == math_sym_vid)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_vid_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_vid_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_vid_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_vid_number_inplace);
    }
  else if(name == math_sym_ee)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_ee_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_ee_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_ee_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ee_number_inplace);
    }
  else if(name == math_sym_ne)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_ne_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_ne_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_ne_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ne_number_inplace);
    }
  else if(name == math_sym_gt)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_gt_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_gt_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_gt_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_gt_number_inplace);
    }
  else if(name == math_sym_ge)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_ge_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_ge_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_ge_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_ge_number_inplace);
    }
  else if(name == math_sym_lt)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_lt_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_lt_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_lt_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_lt_number_inplace);
    }
  else if(name == math_sym_le)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_le_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_le_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_le_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_le_number_inplace);
    }
  else if(name == math_sym_min)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_min_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_min_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_min_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_min_number_inplace);
    }
  else if(name == math_sym_max)
    {
      fts_method_define_varargs(cl, 0, fts_s_int, binop_number_max_left_int);
      fts_method_define_varargs(cl, 0, fts_s_float, binop_number_max_left_float);
      fts_method_define_varargs(cl, 0, ivec_symbol, binop_ivec_max_number_inplace);
      fts_method_define_varargs(cl, 0, fvec_symbol, binop_fvec_max_number_inplace);
    }

  /* right inlet: set right operand */
  fts_method_define_varargs(cl, 1, fts_s_int, binop_number_set_right);
  fts_method_define_varargs(cl, 1, fts_s_float, binop_number_set_right);

  return fts_Success;
}
