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
 */

#include <math.h>
#include <fts/fts.h>
#include "utils.h"
#include "fvec.h"
#include "cvec.h"

static fts_symbol_t sym_complex = 0;
static fts_symbol_t sym_real = 0;

typedef struct 
{
  fts_object_t o;
  int size;
  cvec_t *target;
} fft_t;

static void
fft_calculate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *this = (fft_t *)o;  
  
  if(fts_is_a(at, fvec_type))
    {
      float *fft_ptr = (float *)cvec_get_ptr(this->target);
      int fft_size = 2 * this->size;
      fvec_t *in = fvec_atom_get(at); 
      float *in_ptr = fvec_get_ptr(in);
      int in_size = fvec_get_size(in);
      int size = (in_size < fft_size)? in_size: fft_size;
      int i = 0;
      
      for(i=0; i<size; i++)
	fft_ptr[i] = in_ptr[i];

      /* zero padding */
      for(; i<fft_size; i++)
	fft_ptr[i] = 0.0;
  
      fts_rfft_inplc(fft_ptr, fft_size);
      fts_outlet_object(o, 0, (fts_object_t *)this->target);

      cvec_set_size(this->target, fft_size >> 1);
    }
  else if(fts_is_a(at, cvec_type))
    {
      cvec_t *in = cvec_atom_get(at);
      complex *fft_ptr = cvec_get_ptr(this->target);
      int fft_size = this->size;

      if(in != this->target)
	{
	  complex *in_ptr = cvec_get_ptr(in);
	  int in_size = cvec_get_size(in);
	  int size = (in_size < fft_size)? in_size: fft_size;
	  int i;
	  
	  for(i=0; i<size; i++)
	    fft_ptr[i] = in_ptr[i];
	  
	  /* zero padding */
	  for(; i<fft_size; i++)
	    fft_ptr[i] = CZERO;      
	}
      
      fts_cfft_inplc((complex *)fft_ptr, fft_size);
      fts_outlet_object(o, 0, (fts_object_t *)this->target);
    }
}

static void
fft_set_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *this = (fft_t *)o;
  cvec_t *cvec = cvec_atom_get(at);

  if(this->target != NULL)
    fts_object_release((fts_object_t *)this->target);

  this->target = cvec;
  fts_object_refer((fts_object_t *)cvec);

  cvec_set_size(this->target, this->size);
}
  
/************************************************************
 *
 *  class
 *
 */

static void
fft_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fft_t *this = (fft_t *)o;

  ac--;
  at++;
  
  this->size = 512;
  this->target = NULL;

  if(ac > 0)
    {
      if(fts_is_number(at))
	{
	  this->size = fts_get_number_int(at);
	  
	  if(this->size < 0)
	    this->size = 0;
	}
      else if(fts_is_a(at, cvec_type))
	{
	  cvec_t *cvec = cvec_atom_get(at);

	  this->size = cvec_get_size(cvec);
	  fft_set_target(o, 0, 0, 1, at);
	}
      else
	fts_object_set_error(o, "Wrong argument");
    }
 

  if(!fts_fft_declaresize(2 * this->size) || !fts_fft_declaresize(this->size))
    fts_object_set_error(o, "Invalid FFT size");
  else if(this->target == NULL)
    {
      this->target = (cvec_t *)fts_object_create(cvec_type, 0, 0);
      cvec_set_size(this->target, this->size);
      fts_object_refer((fts_object_t *)this->target);
    }
}

static void
fft_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fft_t *this = (fft_t *)o;

  if(this->target != NULL)
    fts_object_release((fts_object_t *)this->target);
}

static fts_status_t
fft_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fft_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fft_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fft_delete);

  fts_method_define_varargs(cl, 0, fvec_symbol, fft_calculate);
  fts_method_define_varargs(cl, 0, cvec_symbol, fft_set_target);

  return fts_Success;
}

void
fft_config(void)
{
  sym_complex = fts_new_symbol("complex");
  sym_real = fts_new_symbol("real");

  fts_class_install(fts_new_symbol("fft"), fft_instantiate);
}
