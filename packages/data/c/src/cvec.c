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
 */

#include <fts/fts.h>
#include <utils.h>
#include "cvec.h"
#include "fvec.h"

#include <stdlib.h>
#include <ctype.h>

fts_symbol_t cvec_symbol = 0;
fts_metaclass_t *cvec_type = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
cvec_set_size(cvec_t *vec, int size)
{
  int i;

  if(size > vec->alloc)
    {
      vec->values = (complex *)fts_realloc(vec->values, sizeof(complex) * size);
      vec->alloc = size;
    }

  /* when shortening: zero old values */
  for(i=size; i<vec->size; i++)
    vec->values[i] = CZERO;

  vec->size = size;
}

void
cvec_set_const(cvec_t *vec, complex c)
{
  complex *values = vec->values;
  int i;
  
  for(i=0; i<vec->size; i++)
    values[i] = c;
}

void
cvec_set_with_onset_from_atoms(cvec_t *vec, int offset, int ac, const fts_atom_t *at)
{
  int size = cvec_get_size(vec);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	{
	  vec->values[i + offset].re = fts_get_number_float(at + i);
	  vec->values[i + offset].im = 0.0;
	  
	}
      else
	vec->values[i + offset] = CZERO;
    }
}

void
cvec_copy(cvec_t *org, cvec_t *copy)
{
  int size = cvec_get_size(org);
  int i;

  cvec_set_size(copy, size);

  for(i=0; i<size; i++)
    copy->values[i] = org->values[i];  
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
cvec_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_object(o, 0, o);
}

static void
cvec_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  cvec_set_const(this, CZERO);
}

static void
cvec_fill(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac  > 0 && fts_is_number(at))
    {
      complex c;
      
      c.re = fts_get_number_float(at);
      c.im = 0.0;
      
      cvec_set_const(this, c);
    }
}

static void
cvec_set_elements(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 1 && fts_is_number(at))
    {
      int size = cvec_get_size(this);
      int offset = fts_get_number_int(at);

      if(offset >= 0 && offset < size)
	cvec_set_with_onset_from_atoms(this, offset, ac - 1, at + 1);
    }
}

static void
cvec_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	{
	  int old_size = this->size;
	  int i;

	  cvec_set_size(this, size);

	  /* when extending: zero new values */
	  for(i=old_size; i<size; i++)
	    this->values[i] = CZERO;
	}
    }
}

/**************************************************************************************
 *
 *  arithmetics
 *
 */

static void
cvec_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, cvec_type))
	{
	  cvec_t *right = cvec_atom_get(at);
	  int this_size = cvec_get_size(this);
	  int right_size = cvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  complex *l = cvec_get_ptr(this);
	  complex *r = cvec_get_ptr(right);
	  int i;
  
	  for(i=0; i<size; i++)
	    {
	      l[i].re += r[i].re;
	      l[i].im += r[i].im;
	    }
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
cvec_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, cvec_type))
	{
	  cvec_t *right = cvec_atom_get(at);
	  int this_size = cvec_get_size(this);
	  int right_size = cvec_get_size(right);
	  int size = (this_size <= right_size)? this_size: right_size;
	  complex *l = cvec_get_ptr(this);
	  complex *r = cvec_get_ptr(right);
	  int i;
  
	  for(i=0; i<size; i++)
	    {
	      l[i].re -= r[i].re;
	      l[i].im -= r[i].im;
	    }
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
cvec_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, fvec_type))
	{
	  fvec_t *right = fvec_atom_get(at);
	  int right_size = fvec_get_size(right);
	  int this_size = cvec_get_size(this);
	  int size = (this_size <= right_size)? this_size: right_size;
	  complex *l = cvec_get_ptr(this);
	  float *r = fvec_get_ptr(right);
	  int i;
  
	  for(i=0; i<size; i++)
	    {
	      l[i].re *= r[i];
	      l[i].im *= r[i];
	    }
	}
      if(fts_is_a(at, cvec_type))
	{
	  cvec_t *right = cvec_atom_get(at);
	  int right_size = fvec_get_size(right);
	  int this_size = cvec_get_size(this);
	  int size = (this_size <= right_size)? this_size: right_size;
	  complex *l = cvec_get_ptr(this);
	  complex *r = cvec_get_ptr(right);
	  int i;
  
	  for(i=0; i<size; i++)
	    {
	      float l_re = l[i].re;
	      float l_im = l[i].im;
	      float r_re = r[i].re;
	      float r_im = r[i].im;

	      l[i].re = l_re * r_re - l_im * r_im;
	      l[i].im = l_im * r_re + l_re * r_im;
	    }
	}
      else if(fts_is_number(at))
	{
	  float r = fts_get_number_float(at);
	  int size = cvec_get_size(this);
	  complex *p = cvec_get_ptr(this);
	  int i;
  
	  for(i=0; i<size; i++)
	    {
	      p[i].re *= r;
	      p[i].im *= r;
	    }
	}
      else
	fts_object_signal_runtime_error(o, "method not implemented for given arguments");
    }
}

static void
cvec_abs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  int size = cvec_get_size(this);
  complex *p = cvec_get_ptr(this);
  int i;
  
  for(i=0; i<size; i++)
    {
      float re = p[i].re;
      float im = p[i].im;
      
      p[i].re = sqrt(re * re + im * im);
      p[i].im = 0.0;
    }
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
cvec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  int size = cvec_get_size(this);
  complex *p = cvec_get_ptr(this);
  int i;

  post("{");

  if(size > 8)
    {
      int size8 = (size / 8) * 8;
      int i, j;

      for(i=0; i<size8; i+=8)
	{
	  post("\n  ");
	  for(j=0; j<8; j++)
	    post("%f+i%f ", p[i + j].re, p[i + j].im);
	}
	  
      if(i < size)
	{
	  post("\n  ");
	  for(; i<size; i++)
	    post("%f+i%f ", p[i].re, p[i].im);
	}

      post("\n}\n");
    }
  else if(size)
    {
      for(i=0; i<size-1; i++)
	post("%f+i%f ", p[i].re, p[i].im);

      post("%f+i%f}\n", p[i].re, p[i].im);
    }
  else
    post("}\n");
}

static void
cvec_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  cvec_t *in = cvec_atom_get(at);
  
  cvec_copy(in, this);
}

static void
cvec_set_keep(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  cvec_t *this = (cvec_t *)o;

  if(fts_is_symbol(value))
    {
      fts_symbol_t keep = fts_get_symbol(value);

      if(keep != fts_s_no)
	fts_object_signal_runtime_error(o, "no persistence for cvec");
    }
}
static void
cvec_get_keep(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  cvec_t *this = (cvec_t *)o;

  fts_set_symbol(value, this->keep);
}

static void
cvec_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

/*********************************************************
 *
 *  class
 *
 */
static void
cvec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  
  /* skip class name */
  ac--;
  at++;

  this->values = 0;
  this->size = 0;
  this->alloc = 0;
  this->keep = fts_s_no;


  if(ac == 0)
    cvec_set_size(this, 0);
  else if(ac == 1 && fts_is_int(at))
    {
      cvec_set_size(this, fts_get_int(at));
      cvec_zero(this);
    }
  else if(ac == 1 && fts_is_tuple(at))
    {
      fts_tuple_t *tup = fts_get_tuple(at);
      int size = fts_tuple_get_size(tup);
      
      cvec_set_size(this, size);
      cvec_set_with_onset_from_atoms(this, 0, size, fts_tuple_get_atoms(tup));
      this->keep = fts_s_args;
    }
  else if(ac > 1)
    {
      cvec_set_size(this, ac);
      cvec_set_with_onset_from_atoms(this, 0, ac, at);

      this->keep = fts_s_args;
    }
  else
    fts_object_set_error(o, "Wrong arguments for cvec constructor");
}

static void
cvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(this->values)
    fts_free(this->values);
}

static fts_status_t
cvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(cvec_t), 1, 1, 0);
  
  /* init / delete */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, cvec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, cvec_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, cvec_print); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, cvec_set_elements);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, cvec_set_from_instance);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_size, cvec_size);

  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, cvec_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, cvec_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, cvec_get_state);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, cvec_output);
  
  fts_method_define_varargs(cl, 0, fts_s_clear, cvec_clear);
  fts_method_define_varargs(cl, 0, fts_s_fill, cvec_fill);
  fts_method_define_varargs(cl, 0, fts_s_set, cvec_set_elements);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("add"), cvec_add);
  fts_method_define_varargs(cl, 0, fts_new_symbol("sub"), cvec_sub);
  fts_method_define_varargs(cl, 0, fts_new_symbol("mul"), cvec_mul);
  fts_method_define_varargs(cl, 0, fts_new_symbol("abs"), cvec_abs);

  fts_method_define_varargs(cl, 0, fts_s_size, cvec_size);
  
  return fts_Success;
}

/********************************************************************
 *
 *  config
 *
 */

void 
cvec_config(void)
{
  cvec_symbol = fts_new_symbol("cvec");
  cvec_type = fts_class_install(cvec_symbol, cvec_instantiate);
}