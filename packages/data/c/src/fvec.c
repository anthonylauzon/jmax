/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <fts/fts.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>
#include "tabeditor.h"

#include <stdlib.h>
#include <ctype.h>
#include <float.h>

static void
fvec_pick_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  fvec_t *source = (fvec_t *)fts_get_object(at);
  int source_size = fvec_get_size(source);
  int size = fvec_get_size(this);
  float *src = fvec_get_ptr(source);
  float *ptr = fvec_get_ptr(this);
  int onset = 0;
  int i;

  if(ac > 1 && fts_is_number(at + 1))
    onset = fts_get_number_int(at + 1);

  if(ac > 2 && fts_is_number(at + 2))
    size = fts_get_number_int(at + 2);

  if(onset + size > source_size)
    size = source_size - onset;

  if(size > 0)
  {
    fvec_set_size(this, size);

    for(i=0; i<size; i++)
      ptr[i] = src[onset + i];
  }
}

static void
fvec_fade(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  float *ptr = fvec_get_ptr(this);
  int fade = 0;
  float f, incr;
  int n, i;

  if(ac > 0 && fts_is_number(at))
    fade = fts_get_number_int(at);

  n = fade;
  incr = (float)(1.0 / (float)fade);

  if(n > size / 2)
    n = size / 2;
	  
  f = 0.0;
  for(i=0; i<n; i++)
    {
      ptr[i] *= f;
      ptr[size - 1 - i] *= f;
      f += incr;
    }
}

static void
fvec_get_tuple(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  float *values = fvec_get_ptr(this);
  int size = fvec_get_size(this);
  fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, 0, 0);
  fts_atom_t *atoms;
  int i;

  fts_tuple_set_size(tuple, size);
  atoms = fts_tuple_get_atoms(tuple);
  
  for(i=0; i<size; i++)
    fts_set_float(atoms + i, values[i]);

  fts_return_object((fts_object_t *)tuple);
}

static void
fvec_assign_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;

  fvec_copy((fvec_t *)fts_get_object(at), this);
}

static void
fvec_assign_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  ivec_t *ivec = (ivec_t *)fts_get_object(at);
  int *p = ivec_get_ptr(ivec);
  int size = ivec_get_size(ivec);
  int i;

  fvec_set_size(this, size);

  for(i=0; i<size; i++)
    this->values[i] = (float)p[i];
}

static void
fvec_assign_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  cvec_t *cvec = (cvec_t *)fts_get_object(at);
  complex *c = cvec_get_ptr(cvec);
  int size = cvec_get_size(cvec);
  int i;

  fvec_set_size(this, size);

  for(i=0; i<size; i++)
    this->values[i] = c[i].re;
}

static void
fvec_return_interpolated(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *this = (fvec_t *)o;
  int size = fvec_get_size(this);
  double f = fts_get_number_float(at);
  double ret = 0.0;

  if(size < 2 || f <= 0.0)
    ret = fvec_get_element(this, 0);
  else if(f >= size - 1)
    ret = fvec_get_element(this, size - 1);
  else
  {
    int i = (int)f;
    double y0 = fvec_get_element(this, i);
    double y1 = fvec_get_element(this, i + 1);

    ret = y0 + (f - i) * (y1 - y0);
  }

  fts_return_float((float)ret);
}

static void
fvec_instantiate(fts_class_t *cl)
{
  fts_class_message_varargs(cl, fts_s_get_tuple, fvec_get_tuple);

  fts_class_message(cl, fts_s_set, cl, fvec_assign_fvec);
  fts_class_message(cl, fts_s_set, ivec_type, fvec_assign_ivec);
  fts_class_message(cl, fts_s_set, cvec_type, fvec_assign_cvec);
  
  fts_class_message(cl, fts_new_symbol("pick"), cl, fvec_pick_fvec);
  fts_class_message_number(cl, fts_new_symbol("fade"), fvec_fade);

  fts_class_message_float(cl, fts_s_get_element, fvec_return_interpolated);
}

void 
fvec_config(void)
{
}
