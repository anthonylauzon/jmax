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
 */

#include <stdlib.h>
#include <ctype.h>
#include <fts/fts.h>
#include <fts/packages/utils/utils.h>
#include <fts/packages/data/data.h>

fts_symbol_t cvec_symbol = 0;
fts_class_t *cvec_type = 0;

/********************************************************
 *
 *  utility functions
 *
 */

void
cvec_set_size(cvec_t *vec, int size)
{
  int i;

  if(2 * size > vec->alloc)
    {
      vec->values = (float *)fts_realloc(vec->values, 2 * sizeof(float) * size);
      vec->alloc = 2 * size;
    }

  /* when shortening: zero old values */
  for(i=size; i<vec->m; i++)
    ((complex *)(vec->values))[i] = CZERO;

  vec->m = size;
  vec->n = 2;
}

void
cvec_set_const(cvec_t *vec, complex c)
{
  complex *values = (complex *)vec->values;
  int i;
  
  for(i=0; i<vec->m; i++)
    values[i] = c;
}

void
cvec_set_with_onset_from_atoms(cvec_t *vec, int offset, int ac, const fts_atom_t *at)
{
  complex *values = (complex *)vec->values;
  int size = cvec_get_size(vec);
  int i;
  
  if(offset + ac > size)
    ac = size - offset;
  
  for(i=0; i<ac; i++)
    {
      if(fts_is_number(at + i))
	{
	  values[i + offset].re = fts_get_number_float(at + i);
	  values[i + offset].im = 0.0;
	  
	}
      else
	values[i + offset] = CZERO;
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
cvec_return_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  fts_atom_t a;

  fts_set_int(&a, cvec_get_size(this));
  fts_return(&a);
}

static void
cvec_change_size(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 0 && fts_is_number(at))
    {
      int size = fts_get_number_int(at);
      
      if(size >= 0)
	{
	  complex *values;
	  int old_size = this->m;
	  int i;

	  cvec_set_size(this, size);

	  values = (complex *)this->values;

	  /* when extending: zero new values */
	  for(i=old_size; i<size; i++)
	    values[i] = CZERO;
	}
    }
}

/**************************************************************************************
 *
 *  arithmetics
 *
 */

static void
cvec_add_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  cvec_t *right = (cvec_t *)fts_get_object(at);
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

static void
cvec_sub_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  cvec_t *right = (cvec_t *)fts_get_object(at);
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

static void
cvec_mul_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  fvec_t *right = (fvec_t *)fts_get_object(at);
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

static void
cvec_mul_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  ivec_t *right = (ivec_t *)fts_get_object(at);
  int right_size = ivec_get_size(right);
  int this_size = cvec_get_size(this);
  int size = (this_size <= right_size)? this_size: right_size;
  complex *l = cvec_get_ptr(this);
  int *r = ivec_get_ptr(right);
  int i;

  for(i=0; i<size; i++)
  {
    l[i].re *= (float)r[i];
    l[i].im *= (float)r[i];
  }
}

static void
cvec_mul_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  cvec_t *right = (cvec_t *)fts_get_object(at);
  int right_size = cvec_get_size(right);
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

static void
cvec_mul_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
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

static void
cvec_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  int size = cvec_get_size(this);
  complex *p = cvec_get_ptr(this);
  int i;
  
  for(i=0; i<size; i++)
    {
      double re = p[i].re;
      double im = p[i].im;
      
      p[i].re = 0.5 * log(re * re + im * im);
      p[i].im = atan2(im, re);
    }
}

static void
cvec_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  int size = cvec_get_size(this);
  complex *p = cvec_get_ptr(this);
  int i;
  
  for(i=0; i<size; i++)
    {
      double m = exp(p[i].re);
      double phi = p[i].im;
      
      p[i].re = m * cos(phi);
      p[i].im = m * sin(phi);
    }
}

static void
cvec_fft_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  unsigned int fft_size = fts_get_fft_size(cvec_get_size(this));
  complex *fft_ptr;

  cvec_set_size(this, fft_size);
  fft_ptr = cvec_get_ptr(this);

  fts_cfft_inplc(fft_ptr, fft_size);
}

static void
cvec_fft_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  /* real FFT */
  fvec_t *in = (fvec_t *)fts_get_object(at);
  float *in_ptr = fvec_get_ptr(in);
  int in_size = fvec_get_size(in);
  unsigned int fft_size = fts_get_fft_size(in_size);
  float *fft_ptr;
  int i = 0;

  cvec_set_size(this, fft_size >> 1);
  fft_ptr = (float *)cvec_get_ptr(this);

  for(i=0; i<in_size; i++)
    fft_ptr[i] = in_ptr[i];

  /* zero padding */
  for(; i<fft_size; i++)
    fft_ptr[i] = 0.0;

  fts_rfft_inplc(fft_ptr, fft_size);

  return;
}

static void
cvec_fft_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  /* complex FFT */
  cvec_t *in = (cvec_t *)fts_get_object(at);
  int in_size = cvec_get_size(in);
  unsigned int fft_size = fts_get_fft_size(in_size);
  complex *fft_ptr;

  cvec_set_size(this, fft_size);
  fft_ptr = cvec_get_ptr(this);

  if(in != this)
  {
    complex *in_ptr = cvec_get_ptr(in);
    int i;

    for(i=0; i<in_size; i++)
      fft_ptr[i] = in_ptr[i];

    /* zero padding */
    for(; i<fft_size; i++)
      fft_ptr[i] = CZERO;
  }

  fts_cfft_inplc(fft_ptr, fft_size);
}

static void
cvec_ifft_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  /* inplace IFFT */
  unsigned int fft_size = fts_get_fft_size(cvec_get_size(this));
  complex *fft_ptr;

  cvec_set_size(this, fft_size);
  fft_ptr = cvec_get_ptr(this);

  fts_cifft_inplc(fft_ptr, fft_size);
}

static void
cvec_ifft_cvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  /* complex IFFT */
  cvec_t *in = (cvec_t *)fts_get_object(at);
  int in_size = cvec_get_size(in);
  unsigned int fft_size = fts_get_fft_size(in_size);
  complex *fft_ptr;

  cvec_set_size(this, fft_size);
  fft_ptr = cvec_get_ptr(this);

  if(in != this)
  {
    complex *in_ptr = cvec_get_ptr(in);
    int i;

    for(i=0; i<in_size; i++)
      fft_ptr[i] = in_ptr[i];

    /* zero padding */
    for(; i<fft_size; i++)
      fft_ptr[i] = CZERO;
  }

  fts_cifft_inplc(fft_ptr, fft_size);
}

/********************************************************************
 *
 *  system functions
 *
 */

static void
cvec_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int size = cvec_get_size(this);
  complex *p = cvec_get_ptr(this);

  if(size == 0)
    fts_spost(stream, "(:cvec)");
  else if(size <= FTS_POST_MAX_ELEMENTS)
    {
      int i;
      
      fts_spost(stream, "(:cvec", size);
      
      for(i=0; i<size; i++)
      {
        fts_spost(stream, " ");
        fts_spost_complex(stream, p[i].re, p[i].im);
      }
      
      fts_spost(stream, ")");
    }
  else
    fts_spost(stream, "(:cvec %d)", size);
}

static void
cvec_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int size = cvec_get_size(this);
  complex *p = cvec_get_ptr(this);

  if(size == 0)
    fts_spost(stream, "<empty cvec>\n");
  else if(size == 1)
    {
      fts_spost(stream, "<cvec of 1 element: ");
      fts_spost_complex(stream, p[0].re, p[0].im);
      fts_spost(stream, ">\n");
    }
  else if(size <= FTS_POST_MAX_ELEMENTS)
    {
      int i;
      
      fts_spost(stream, "<cvec of %d elements: ", size);
      
      for(i=0; i<size-1; i++)
	{
	  fts_spost_complex(stream, p[i].re, p[i].im);
	  fts_spost(stream, " ");
	}
	        
      fts_spost_complex(stream, p[i].re, p[i].im);
      fts_spost(stream, ">\n");
    }
  else
    {
      int i, j;
      
      fts_spost(stream, "<cvec of %d elements>\n", size);
      fts_spost(stream, "{\n");
      
      for(i=0; i<size; i+=FTS_POST_MAX_ELEMENTS)
	{
	  int n = size - i;

	  if(n > FTS_POST_MAX_ELEMENTS)
	    n = FTS_POST_MAX_ELEMENTS;
	  
	  fts_spost(stream, "  ");
	  
	  for(j=0; j<n; j++)
	    {
	      fts_spost_complex(stream, p[i + j].re, p[i + j].im);
	      fts_spost(stream, " ");
	    }

	  fts_spost(stream, "\n");
	}
	  
      fts_spost(stream, "}\n");
    }
}

static void
cvec_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;
  cvec_t *in = (cvec_t *)fts_get_object(at);
  
  cvec_copy(in, this);
}

static void
cvec_assign(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(ac > 0)
    {
      if(fts_is_a(at, cvec_type))
	cvec_copy((cvec_t *)fts_get_object(at), this);
      else if(fts_is_a(at, fvec_type))
	{
	  fvec_t *fvec = (fvec_t *)fts_get_object(at);
	  float *f = fvec_get_ptr(fvec);
	  int size = fvec_get_size(fvec);
	  complex *c;
	  int i;
	  
	  cvec_set_size(this, size);
	  c = cvec_get_ptr(this);

	  for(i=0; i<size; i++)
	    {
	      c[i].re = f[i];
	      c[i].im = 0.0;
	    }
	}
    }
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
  
  data_object_init(o);

  this->values = 0;
  this->m = 0;
  this->n = 2;
  this->alloc = 0;

  if(ac == 0)
    cvec_set_size(this, 0);
  else if(ac == 1 && fts_is_int(at))
    {
      cvec_set_size(this, fts_get_int(at));
      cvec_set_const(this, CZERO);
    }
  else if(ac == 1 && fts_is_tuple(at))
    {
      fts_tuple_t *tup = (fts_tuple_t *)fts_get_object(at);
      int size = fts_tuple_get_size(tup);
      
      cvec_set_size(this, size);
      cvec_set_with_onset_from_atoms(this, 0, size, fts_tuple_get_atoms(tup));

      data_object_persistence_args(o);
    }
  else if(ac > 1)
    {
      cvec_set_size(this, ac);
      cvec_set_with_onset_from_atoms(this, 0, ac, at);

      data_object_persistence_args(o);
    }
  else
    fts_object_error(o, "bad arguments for cvec constructor");
}

static void
cvec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cvec_t *this = (cvec_t *)o;

  if(this->values)
    fts_free(this->values);
}

static void
cvec_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(cvec_t), cvec_init, cvec_delete);
  
  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_dump, fts_name_dump_method);
  fts_class_message_varargs(cl, fts_s_update_gui, fts_name_gui_method);
  /* fts_class_message_varargs(cl, fts_s_persistence, data_object_persistence); */
  /* fts_class_message_varargs(cl, fts_s_update_gui, data_object_update_gui); */
  
  fts_class_message_varargs(cl, fts_s_post, cvec_post); 
  fts_class_message_varargs(cl, fts_s_print, cvec_print); 
  fts_class_message_varargs(cl, fts_s_set_from_instance, cvec_set_from_instance);

  fts_class_message_varargs(cl, fts_s_fill, cvec_fill); 
  fts_class_message_varargs(cl, fts_s_set, cvec_set_elements);

  fts_class_message_varargs(cl, fts_s_assign, cvec_assign);
 
  fts_class_message(cl, fts_new_symbol("add"), cl, cvec_add_cvec);
  fts_class_message(cl, fts_new_symbol("sub"), cl, cvec_sub_cvec);

  fts_class_message(cl, fts_new_symbol("mul"), cl, cvec_mul_cvec);
  fts_class_message(cl, fts_new_symbol("mul"), fvec_type, cvec_mul_fvec);
  fts_class_message(cl, fts_new_symbol("mul"), ivec_type, cvec_mul_ivec);
  fts_class_message_number(cl, fts_new_symbol("mul"), cvec_mul_number);

  fts_class_message_void(cl, fts_new_symbol("abs"), cvec_abs);
  fts_class_message_void(cl, fts_new_symbol("log"), cvec_log);
  fts_class_message_void(cl, fts_new_symbol("exp"), cvec_exp);

  fts_class_message_void(cl, fts_new_symbol("fft"), cvec_fft_inplace);
  fts_class_message(cl, fts_new_symbol("fft"), fvec_type, cvec_fft_fvec);
  fts_class_message(cl, fts_new_symbol("fft"), cl, cvec_fft_cvec);
  
  fts_class_message_void(cl, fts_new_symbol("ifft"), cvec_ifft_inplace);
  fts_class_message(cl, fts_new_symbol("ifft"), cl, cvec_ifft_cvec);

  fts_class_message_void(cl, fts_s_size, cvec_return_size);
  fts_class_message_number(cl, fts_s_size, cvec_change_size);
  
  fts_class_inlet_bang(cl, 0, data_object_output);

  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
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
