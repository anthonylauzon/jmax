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
#include <ftsconfig.h>

typedef void (*ftl_function_t)(fts_word_t *argv);

typedef struct
{
  fts_symbol_t symbol;
  ftl_function_t function;
} binop_function_t;

typedef struct
{
  binop_function_t signal;
  binop_function_t scalar;
  binop_function_t inplace;
  fts_method_t put;
} binop_definition_t;

typedef struct
{
  fts_dsp_object_t head;
  ftl_data_t data;
} binop_t;

static fts_symbol_t sym_add;
static fts_symbol_t sym_mul;
static fts_symbol_t sym_sub;
static fts_symbol_t sym_div;
static fts_symbol_t sym_bus;
static fts_symbol_t sym_vid;
static fts_symbol_t sym_ee;
static fts_symbol_t sym_ne;
static fts_symbol_t sym_ge;
static fts_symbol_t sym_le;
static fts_symbol_t sym_gt;
static fts_symbol_t sym_lt;

static binop_definition_t binop_add;
static binop_definition_t binop_sub;
static binop_definition_t binop_mul;
static binop_definition_t binop_div;
static binop_definition_t binop_bus;
static binop_definition_t binop_vid;

static binop_definition_t binop_ee;
static binop_definition_t binop_ne;
static binop_definition_t binop_ge;
static binop_definition_t binop_le;
static binop_definition_t binop_gt;
static binop_definition_t binop_lt;

/**************************************************************
 *
 *  dsp functions simple
 *
 */

static void 
ftl_add(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] + in2[i];
}

static void 
ftl_sub(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] - in2[i];
}

static void 
ftl_mul(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] * in2[i];
}

static void 
ftl_div(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in1[i] / in2[i];
}

static void 
ftl_bus(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in2[i] - in1[i];
}

static void 
ftl_vid(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in2[i] / in1[i];
}

/* comparison not yet implemented */
static void 
ftl_ee(fts_word_t *argv) 
{ 
  float *in0 = (float *)fts_word_get_pointer(argv + 0); 
  float *in1 = (float *)fts_word_get_pointer(argv + 1); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in0[i] == in1[i]; 
} 

static void 
ftl_ne(fts_word_t *argv) 
{ 
  float *in0 = (float *)fts_word_get_pointer(argv + 0); 
  float *in1 = (float *)fts_word_get_pointer(argv + 1); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in0[i] != in1[i]; 
} 

static void 
ftl_le(fts_word_t *argv) 
{ 
  float *in0 = (float *)fts_word_get_pointer(argv + 0); 
  float *in1 = (float *)fts_word_get_pointer(argv + 1); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in0[i] <= in1[i]; 
} 

static void 
ftl_ge(fts_word_t *argv) 
{ 
  float *in0 = (float *)fts_word_get_pointer(argv + 0); 
  float *in1 = (float *)fts_word_get_pointer(argv + 1); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in0[i] >= in1[i]; 
} 

static void 
ftl_lt(fts_word_t *argv) 
{ 
  float *in0 = (float *)fts_word_get_pointer(argv + 0); 
  float *in1 = (float *)fts_word_get_pointer(argv + 1); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in0[i] < in1[i]; 
} 

static void 
ftl_gt(fts_word_t *argv) 
{ 
  float *in0 = (float *)fts_word_get_pointer(argv + 0); 
  float *in1 = (float *)fts_word_get_pointer(argv + 1); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in0[i] > in1[i]; 
} 

/**************************************************************
 *
 *  dsp functions with scalar
 *
 */

static void 
ftl_add_scalar(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in[i] + c;
}

static void 
ftl_sub_scalar(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in[i] - c;
}

static void 
ftl_mul_scalar(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in[i] * c;
}

static void 
ftl_div_scalar(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = in[i] / c;
}

static void 
ftl_bus_scalar(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = c - in[i];
}

static void 
ftl_vid_scalar(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for (i=0; i<n; i++)
    out[i] = c / in[i];
}

static void 
ftl_ee_scalar(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i] == c; 
} 

/* comparison not yet implemented */
static void 
ftl_ne_scalar(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i] != c; 
} 

static void 
ftl_le_scalar(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i] <= c; 
} 

static void 
ftl_ge_scalar(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i] >= c; 
} 

static void 
ftl_lt_scalar(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i] < c; 
} 

static void 
ftl_gt_scalar(fts_word_t *argv)
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict in = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int n = fts_word_get_int(argv + 3);
  int i;

  for(i=0; i<n; i++)
    out[i] = in[i] > c; 
} 

/**************************************************************
 *
 *  dsp functions with scalar inplace
 *
 */

static void 
ftl_add_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] += c;
}

static void 
ftl_sub_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] -= c;
}

static void 
ftl_mul_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] *= c;
}

static void 
ftl_div_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] /= c;
}

static void 
ftl_bus_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] = c - sig[i];
}

static void 
ftl_vid_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] = c / sig[i];
}

static void 
ftl_ee_inplace(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    sig[i] = sig[i] == c; 
} 

/* comparison not yet implemented */
static void 
ftl_ne_inplace(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    sig[i] = sig[i] != c; 
} 

static void 
ftl_le_inplace(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    sig[i] = sig[i] <= c; 
} 

static void 
ftl_ge_inplace(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    sig[i] = sig[i] >= c; 
} 

static void 
ftl_lt_inplace(fts_word_t *argv) 
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    sig[i] = sig[i] < c; 
} 

static void 
ftl_gt_inplace(fts_word_t *argv)
{ 
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    sig[i] = sig[i] > c; 
} 

/************************************************
 *
 *  binop put
 *
 */

static void
binop_put(fts_object_t *o, fts_dsp_descr_t *dsp, binop_definition_t *definition)
{
  binop_t *this = (binop_t *)o;
  fts_symbol_t in0 = fts_dsp_get_input_name(dsp, 0);
  fts_symbol_t in1 = fts_dsp_get_input_name(dsp, 1);
  fts_symbol_t out0 = fts_dsp_get_output_name(dsp, 0);  
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t argv[4];

  if(fts_dsp_is_input_null(dsp, 1))
    { 
      /* scalar */
      if(in0 == out0)
	{
	  /* scalar inplace */
	  fts_set_ftl_data(argv + 0, this->data);
	  fts_set_symbol(argv + 1, out0);
	  fts_set_int(argv + 2, n_tick);
	  fts_dsp_add_function(definition->inplace.symbol, 3, argv);
	}
      else 
	{
	  /* scalar */
	  fts_set_ftl_data(argv + 0, this->data);
	  fts_set_symbol(argv + 1, in0);
	  fts_set_symbol(argv + 2, out0);
	  fts_set_int(argv + 3, n_tick);
	  fts_dsp_add_function(definition->scalar.symbol, 4, argv);
	}
    }
  else 
    {
      /* signal */
      fts_set_symbol(argv + 0, in0);
      fts_set_symbol(argv + 1, in1);
      fts_set_symbol(argv + 2, out0);
      fts_set_int(argv + 3, n_tick);
      fts_dsp_add_function(definition->signal.symbol, 4, argv);
    }
}

static void 
binop_put_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_add);
}

static void 
binop_put_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_sub);
}

static void 
binop_put_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_mul);
}

static void 
binop_put_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_div);
}

static void 
binop_put_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_bus);
}

static void 
binop_put_vid(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_vid);
}

static void 
binop_put_ee(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_ee);
}

static void 
binop_put_ne(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_ne);
}

static void 
binop_put_ge(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_ge);
}

static void 
binop_put_le(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_le);
}

static void 
binop_put_gt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_gt);
}

static void 
binop_put_lt(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), &binop_lt);
}

/************************************************
 *
 *  binop user methods
 *
 */
 
static void
binop_set_scalar(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;
  float *ptr = (float *)ftl_data_get_ptr(this->data);

  *ptr = fts_get_number_float(at);
}

static void
binop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  this->data = ftl_data_alloc(sizeof(float));

  if(ac == 0)
    {
      fts_atom_t a;
      
      fts_set_int(&a, 0);
      binop_set_scalar(o, 0, 0, 1, &a);      
    }
  else if(ac == 1 && fts_is_number(at))
    binop_set_scalar(o, 0, 0, 1, at);
  else
    fts_object_set_error(o, "bad Arguments");

  
  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
binop_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_t *this = (binop_t *)o;

  ftl_data_free(this->data);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}


static void
binop_instantiate(fts_class_t *cl, binop_definition_t *definition)
{
  fts_class_init(cl, sizeof(binop_t), binop_init, binop_delete);

  fts_class_message_varargs(cl, fts_s_put, definition->put);

  fts_class_inlet_int(cl, 1, binop_set_scalar);
  fts_class_inlet_float(cl, 1, binop_set_scalar);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
  fts_dsp_declare_outlet(cl, 0);

  fts_dsp_declare_function(definition->signal.symbol, definition->signal.function);
  fts_dsp_declare_function(definition->scalar.symbol, definition->scalar.function);
  fts_dsp_declare_function(definition->inplace.symbol, definition->inplace.function);
}

/************************************************
 *
 * instantiate
 *
 */

static void 
binop_add_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_add);
}

static void 
binop_sub_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_sub);
}

static void 
binop_mul_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_mul);
}

static void 
binop_div_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_div);
}

static void 
binop_bus_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_bus);
}

static void 
binop_vid_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_vid);
}

static void 
binop_ee_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_ee);
}

static void 
binop_ne_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_ne);
}

static void 
binop_ge_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_ge);
}

static void 
binop_le_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_le);
}

static void 
binop_gt_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_gt);
}

static void 
binop_lt_instantiate(fts_class_t *cl)
{
  binop_instantiate(cl, &binop_lt);
}

void
signal_binop_config(void)
{
  fts_class_t *bus_class, *vid_class;

  /* signal x signal */
  sym_add = fts_new_symbol("+~");
  sym_sub = fts_new_symbol("-~");
  sym_mul = fts_new_symbol("*~");
  sym_div = fts_new_symbol("/~");
  sym_bus = fts_new_symbol("-+~");
  sym_vid = fts_new_symbol("/*~");

  binop_add.signal.symbol = sym_add;
  binop_add.scalar.symbol = fts_new_symbol("+~ scalar");
  binop_add.inplace.symbol = fts_new_symbol("+~ inplace");
  binop_add.signal.function = ftl_add;
  binop_add.scalar.function = ftl_add_scalar;
  binop_add.inplace.function = ftl_add_inplace;
  binop_add.put = binop_put_add;

  binop_sub.signal.symbol = sym_sub;
  binop_sub.scalar.symbol = fts_new_symbol("-~ scalar");
  binop_sub.inplace.symbol = fts_new_symbol("-~ inplace");
  binop_sub.signal.function = ftl_sub;
  binop_sub.scalar.function = ftl_sub_scalar;
  binop_sub.inplace.function = ftl_sub_inplace;
  binop_sub.put = binop_put_sub;

  binop_mul.signal.symbol = sym_mul;
  binop_mul.scalar.symbol = fts_new_symbol("*~ scalar");
  binop_mul.inplace.symbol = fts_new_symbol("*~ inplace");
  binop_mul.signal.function = ftl_mul;
  binop_mul.scalar.function = ftl_mul_scalar;
  binop_mul.inplace.function = ftl_mul_inplace;
  binop_mul.put = binop_put_mul;

  binop_div.signal.symbol = sym_div;
  binop_div.scalar.symbol = fts_new_symbol("/~ scalar");
  binop_div.inplace.symbol = fts_new_symbol("/~ inplace");
  binop_div.signal.function = ftl_div;
  binop_div.scalar.function = ftl_div_scalar;
  binop_div.inplace.function = ftl_div_inplace;
  binop_div.put = binop_put_div;

  binop_bus.signal.symbol = sym_bus;
  binop_bus.scalar.symbol = fts_new_symbol("-+~ scalar");
  binop_bus.inplace.symbol = fts_new_symbol("-+~ inplace");
  binop_bus.signal.function = ftl_bus;
  binop_bus.scalar.function = ftl_bus_scalar;
  binop_bus.inplace.function = ftl_bus_inplace;
  binop_bus.put = binop_put_bus;

  binop_vid.signal.symbol = sym_vid;
  binop_vid.scalar.symbol = fts_new_symbol("/*~ scalar");
  binop_vid.inplace.symbol = fts_new_symbol("/*~ inplace");
  binop_vid.signal.function = ftl_vid;
  binop_vid.scalar.function = ftl_vid_scalar;
  binop_vid.inplace.function = ftl_vid_inplace;
  binop_vid.put = binop_put_vid;

  binop_ee.signal.symbol = sym_ee;
  binop_ee.scalar.symbol = fts_new_symbol("==~ scalar");
  binop_ee.inplace.symbol = fts_new_symbol("==~ inplace");
  binop_ee.signal.function = ftl_ee;
  binop_ee.scalar.function = ftl_ee_scalar;
  binop_ee.inplace.function = ftl_ee_inplace;
  binop_ee.put = binop_put_ee;

  binop_ne.signal.symbol = sym_ne;
  binop_ne.scalar.symbol = fts_new_symbol("!=~ scalar");
  binop_ne.inplace.symbol = fts_new_symbol("!=~ inplace");
  binop_ne.signal.function = ftl_ne;
  binop_ne.scalar.function = ftl_ne_scalar;
  binop_ne.inplace.function = ftl_ne_inplace;
  binop_ne.put = binop_put_ne;

  binop_ge.signal.symbol = sym_ge;
  binop_ge.scalar.symbol = fts_new_symbol(">=~ scalar");
  binop_ge.inplace.symbol = fts_new_symbol(">=~ inplace");
  binop_ge.signal.function = ftl_ge;
  binop_ge.scalar.function = ftl_ge_scalar;
  binop_ge.inplace.function = ftl_ge_inplace;
  binop_ge.put = binop_put_ge;

  binop_le.signal.symbol = sym_le;
  binop_le.scalar.symbol = fts_new_symbol("<=~ scalar");
  binop_le.inplace.symbol = fts_new_symbol("<=~ inplace");
  binop_le.signal.function = ftl_le;
  binop_le.scalar.function = ftl_le_scalar;
  binop_le.inplace.function = ftl_le_inplace;
  binop_le.put = binop_put_le;

  binop_gt.signal.symbol = sym_gt;
  binop_gt.scalar.symbol = fts_new_symbol(">~ scalar");
  binop_gt.inplace.symbol = fts_new_symbol(">~ inplace");
  binop_gt.signal.function = ftl_gt;
  binop_gt.scalar.function = ftl_gt_scalar;
  binop_gt.inplace.function = ftl_gt_inplace;
  binop_gt.put = binop_put_gt;

  binop_lt.signal.symbol = sym_lt;
  binop_lt.scalar.symbol = fts_new_symbol("<~ scalar");
  binop_lt.inplace.symbol = fts_new_symbol("<~ inplace");
  binop_lt.signal.function = ftl_lt;
  binop_lt.scalar.function = ftl_lt_scalar;
  binop_lt.inplace.function = ftl_lt_inplace;
  binop_lt.put = binop_put_lt;

  fts_class_install(sym_add, binop_add_instantiate);
  fts_class_install(sym_sub, binop_sub_instantiate);
  fts_class_install(sym_mul, binop_mul_instantiate);
  fts_class_install(sym_div, binop_div_instantiate);
  bus_class = fts_class_install(sym_bus, binop_bus_instantiate);
  vid_class = fts_class_install(sym_vid, binop_vid_instantiate);
  fts_class_install(sym_ee, binop_ee_instantiate);
  fts_class_install(sym_ne, binop_ne_instantiate);
  fts_class_install(sym_ge, binop_ge_instantiate);
  fts_class_install(sym_le, binop_le_instantiate);
  fts_class_install(sym_gt, binop_gt_instantiate);
  fts_class_install(sym_lt, binop_lt_instantiate);

  /* compatibility */
  fts_class_alias(bus_class, fts_new_symbol("inv+~"));
  fts_class_alias(vid_class, fts_new_symbol("inv*~"));
}
