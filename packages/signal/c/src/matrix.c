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
#include <fts/fts.h>
#include <utils/c/include/utils.h>

static fts_symbol_t matrix_copy_in_ftl_symbol = 0;
static fts_symbol_t matrix_copy_out_ftl_symbol = 0;

typedef struct _matrix
{
  fts_dsp_object_t _o;
  fts_atom_t *ftl_args;
  int n_ins;
  int n_outs;
  ftl_data_t ramps;
  float def_time;
  float cr; /* control rate */
  int n_tick;
  float **bufs;
} matrix_t;

/**************************************************************
 *
 *  object
 *
 */

static void
matrix_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  
  if(ac > 1 && fts_is_number(at) && fts_is_number(at + 1))
    {
      int n_ins = fts_get_number_int(at); /* # of ins */
      int n_outs = fts_get_number_int(at + 1); /* # of outs */
      float def_time = 0.0;
      fts_ramp_t *ramps;
      int n, in, out, i;

      if(n_ins < 1)
	n_ins = 1;
      
      if(n_outs < 1)
	n_outs = 1;
      
      if(ac > 2 && fts_is_number(at + 2))
	def_time = fts_get_number_float(at + 2);

      /* buffer for FTL arguments */
      this->ftl_args = (fts_atom_t *)fts_malloc(sizeof(fts_atom_t) * (5 + n_ins + n_outs));
      
      /* get minimum of n_ins/n_outs */
      n = (n_ins < n_outs)? n_ins: n_outs;
      
      /* allocate array of buffer pointers */
      this->bufs = (float **)fts_malloc(sizeof(float *) * n);
      for(i=0; i<n; i++)
	this->bufs[i] = 0;
      
      /* allocate ramps */
      this->ramps = ftl_data_alloc(sizeof(fts_ramp_t) * n_outs * n_ins);
      ramps = (fts_ramp_t *)ftl_data_get_ptr(this->ramps);
      
      /* initialize ramps */
      for(out=0; out<n_outs; out++)
	{
	  for(in=0; in<n_ins; in++)
	    {
	      fts_ramp_t *ramp = ramps + out * n_ins + in;
	      fts_ramp_init(ramp, 0.0);
	    }
	}
      
      this->n_ins = n_ins;
      this->n_outs = n_outs;
      this->def_time = def_time;
      this->cr = 1.0f; /* will be properly set in the put routine */
      this->n_tick = 0;
      
      fts_dsp_object_init((fts_dsp_object_t *)o);
      fts_object_set_inlets_number(o, n_ins);
      fts_object_set_outlets_number(o, n_outs);      
    }
  else
    fts_object_set_error(o, "matrix dimension arguments required");
}

static void matrix_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  
  ftl_data_free((void *)this->ramps);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

/**************************************************************
 *
 *  dsp
 *
 */

static void 
matrix_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_ramp_t *ramps = (fts_ramp_t *)ftl_data_get_ptr(this->ramps);
  fts_atom_t *a = this->ftl_args;
  int n_ins = this->n_ins;
  int n_outs = this->n_outs;
  int in, out;

  this->cr = sr / (float)n_tick;

  /* reset ramps to target */
  for(out=0; out<n_outs; out++)
    for(in=0; in<n_ins; in++)
      fts_ramp_jump(ramps + out * n_ins + in);
      
  /* allocate input or output buffers */
  if(n_tick > this->n_tick)
    {
      int n = (n_ins < n_outs)? n_ins: n_outs;
      int i;

      for(i=0; i<n; i++)
	{
	  if(this->bufs[i])
	    fts_free(this->bufs[i]);

	  this->bufs[i] = fts_malloc(sizeof(float) * n_tick);
	}
    }

  this->n_tick = n_tick;

  fts_set_int(a + 0, n_ins);
  fts_set_int(a + 1, n_outs);
  fts_set_pointer(a + 2, this->bufs);
  fts_set_ftl_data(a + 3, this->ramps);
  fts_set_int(a + 4, n_tick);

  /* put inputs */
  for(in=0; in<n_ins; in++)
    fts_set_symbol(a + 5 + in, fts_dsp_get_input_name(dsp, in));

  /* put outputs */
  for(out=0; out<n_outs; out++)
    fts_set_symbol(a + 5 + n_ins + out, fts_dsp_get_output_name(dsp, out));

  if(n_ins < n_outs)
    fts_dsp_add_function(matrix_copy_in_ftl_symbol, 5 + n_ins + n_outs, a);
  else
    fts_dsp_add_function(matrix_copy_out_ftl_symbol, 5 + n_ins + n_outs, a);    
}

static void 
ftl_matrix_copy_out(fts_word_t *a)
{
  int n_ins = fts_word_get_int(a + 0);
  int n_outs = fts_word_get_int(a + 1);
  float ** restrict bufs = (float **)fts_word_get_pointer(a + 2);
  fts_ramp_t * restrict ramps = (fts_ramp_t *)fts_word_get_pointer(a + 3);
  int n_tick = fts_word_get_int(a + 4);
  int in, out;
  int i;

  for(out=0; out<n_outs; out++)
    {
      float * restrict out_buf = bufs[out];
      fts_ramp_t * restrict out_ramps = ramps + out * n_ins;
      
      for(i=0; i<n_tick; i++)
	out_buf[i] = 0.0f;
      
      for(in=0; in<n_ins; in++)
	{
	  float * restrict input = (float *)fts_word_get_pointer(a + 5 + in);
	  fts_ramp_t * restrict ramp = out_ramps + in;
	  
	  fts_ramp_vec_mul_add(ramp, input, out_buf, n_tick);
	}
    }
  
  for(out=0; out<n_outs; out++)
    {
      float * restrict out_buf = bufs[out];
      float * restrict out_sig = (float *)fts_word_get_pointer(a + 5 + n_ins + out);
      
      for(i=0; i<n_tick; i++)
	out_sig[i] = out_buf[i];
    }
}

static void 
ftl_matrix_copy_in(fts_word_t *a)
{
  int n_ins = fts_word_get_int(a + 0);
  int n_outs = fts_word_get_int(a + 1);
  float ** restrict bufs = (float **)fts_word_get_pointer(a + 2);
  fts_ramp_t * restrict ramps = (fts_ramp_t *)fts_word_get_pointer(a + 3);
  int n_tick = fts_word_get_int(a + 4);
  int in, out;
  int i;

  for(in=0; in<n_ins; in++)
    {
      float * restrict in_buf = bufs[in];
      float * restrict in_sig = (float *)fts_word_get_pointer(a + 5 + in);
      
      for(i=0; i<n_tick; i++)
	in_buf[i] = in_sig[i];
    }

  for(out=0; out<n_outs; out++)
    {
      float * restrict output = (float *)fts_word_get_pointer(a + 5 + n_ins + out);
      fts_ramp_t * restrict out_ramps = ramps + out * n_ins;
      
      for(i=0; i<n_tick; i++)
	output[i] = 0.0f;
      
      for(in=0; in<n_ins; in++)
	{
	  float * restrict in_buf = bufs[in];
	  fts_ramp_t * restrict ramp = out_ramps + in;
	  
	  fts_ramp_vec_mul_add(ramp, in_buf, output, n_tick);
	}
    }
}

/**************************************************************
 *
 *  user methods
 *
 */

static void
matrix_node(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  fts_ramp_t *ramps = (fts_ramp_t *)ftl_data_get_ptr(this->ramps);
  int in = fts_get_int(at + 0);
  int out = fts_get_int(at + 1);
  
  if(in >= 0 && in < this->n_ins && out >= 0 && out < this->n_outs)
    {
      float value = fts_get_number_float(at + 2);
      float time = fts_get_float_arg(ac, at, 3, this->def_time);
      fts_ramp_t *ramp = ramps + out * this->n_ins + in;

      fts_ramp_set_target(ramp, value, time, this->cr);
    }
}

static void
matrix_in(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  int in = fts_get_int(at + 0);
  float value = fts_get_number_float(at + 1);
  float time = fts_get_float_arg(ac, at, 2, this->def_time);
  fts_ramp_t *ramps = (fts_ramp_t *)ftl_data_get_ptr(this->ramps);
  fts_ramp_t *in_ramps = ramps + in;
  int out;

  if(in >= 0 && in < this->n_ins)
    {
      for(out=0; out<this->n_outs; out++)
	{
	  fts_ramp_t *ramp = in_ramps + out * this->n_ins;
	  fts_ramp_set_target(ramp, value, time, this->cr);
	}
    }
}

static void
matrix_out(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  int out = fts_get_int(at + 0);
  float value = fts_get_number_float(at + 1);
  float time = fts_get_float_arg(ac, at, 2, this->def_time);
  fts_ramp_t *ramps = (fts_ramp_t *)ftl_data_get_ptr(this->ramps);
  fts_ramp_t *out_ramps = ramps + out * this->n_ins;
  int in;

  if(out >= 0 && out < this->n_outs)
    {
      for(in=0; in<this->n_ins; in++)
	{
	  fts_ramp_t *ramp = out_ramps + in;
	  fts_ramp_set_target(ramp, value, time, this->cr);
	}
    }
}

static void
matrix_all(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  matrix_t *this = (matrix_t *)o;
  float value = fts_get_number_float(at + 0);
  float time = fts_get_float_arg(ac, at, 1, this->def_time);
  fts_ramp_t *ramps = (fts_ramp_t *)ftl_data_get_ptr(this->ramps);

  int in, out;
  
  for(out=0; out<this->n_outs; out++)
    {
      fts_ramp_t *out_ramps = ramps + out * this->n_ins;
      
      for(in=0; in<this->n_ins; in++)
	{
	  fts_ramp_t *ramp = out_ramps + in;
	  fts_ramp_set_target(ramp, value, time, this->cr);      
	}
    }
}

/**************************************************************
 *
 *  class
 *
 */

static void 
matrix_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(matrix_t), matrix_init, matrix_delete);

  fts_class_message_varargs(cl, fts_s_put, matrix_put);
    
  fts_class_message_varargs(cl, fts_s_set, matrix_node);
  fts_class_message_varargs(cl, fts_new_symbol("in"), matrix_in);
  fts_class_message_varargs(cl, fts_new_symbol("out"), matrix_out);
  fts_class_message_varargs(cl, fts_new_symbol("all"), matrix_all);
    
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void 
signal_matrix_config(void)
{
  matrix_copy_in_ftl_symbol = fts_new_symbol("matrix_in_copy");
  matrix_copy_out_ftl_symbol = fts_new_symbol("matrix_out_copy");

  fts_dsp_declare_function(matrix_copy_in_ftl_symbol, ftl_matrix_copy_in);
  fts_dsp_declare_function(matrix_copy_out_ftl_symbol, ftl_matrix_copy_out);

  fts_class_install(fts_new_symbol("matrix~"), matrix_instantiate);
}
