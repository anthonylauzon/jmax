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

typedef struct
{
  fts_object_t head;
} binop_t;

typedef struct
{
  fts_object_t head;
  ftl_data_t data;
} binop_const_t;

static fts_symbol_t sym_add;
static fts_symbol_t sym_mul;
static fts_symbol_t sym_sub;
static fts_symbol_t sym_div;
static fts_symbol_t sym_bus;
static fts_symbol_t sym_vid;

static fts_symbol_t sym_add_64;
static fts_symbol_t sym_mul_64;

static fts_symbol_t sym_add_inplace;
static fts_symbol_t sym_mul_inplace;

static fts_symbol_t sym_add_inplace_64;
static fts_symbol_t sym_mul_inplace_64;

static fts_symbol_t sym_add_self;
static fts_symbol_t sym_mul_self;

static fts_symbol_t sym_add_self_64;
static fts_symbol_t sym_mul_self_64;

static fts_symbol_t sym_add_const;
static fts_symbol_t sym_mul_const;
static fts_symbol_t sym_sub_const;
static fts_symbol_t sym_div_const;
static fts_symbol_t sym_bus_const;
static fts_symbol_t sym_vid_const;

static fts_symbol_t sym_add_const_inplace;
static fts_symbol_t sym_mul_const_inplace;
static fts_symbol_t sym_sub_const_inplace;
static fts_symbol_t sym_div_const_inplace;
static fts_symbol_t sym_bus_const_inplace;
static fts_symbol_t sym_vid_const_inplace;

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
ftl_ee(fts_word_t *argv ) 
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
ftl_ne(fts_word_t *argv ) 
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
ftl_le(fts_word_t *argv ) 
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
ftl_ge(fts_word_t *argv ) 
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
ftl_lt(fts_word_t *argv ) 
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
ftl_gt(fts_word_t *argv ) 
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
 *  dsp functions for a tick size of 64 
 *
 */

static void ftl_add_64(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = in1[i] + in2[i];
}

static void ftl_mul_64(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_pointer(argv + 0);
  float * restrict in2 = (float *)fts_word_get_pointer(argv + 1);
  float * restrict out = (float *)fts_word_get_pointer(argv + 2);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = in1[i] * in2[i];
}

/**************************************************************
 *
 *  dsp functions inplace
 *
 */

static void ftl_add_inplace(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    out[i] = out[i] + in[i];
}

static void ftl_mul_inplace(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    out[i] = out[i] * in[i];
}

static void ftl_add_inplace_64(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = out[i] + in[i];
}

static void ftl_mul_inplace_64(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_pointer(argv + 0);
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = out[i] * in[i];
}

/**************************************************************
 *
 *  dsp functions for v = v + v and v = v * v
 *
 */

static void ftl_add_self(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_pointer(argv + 0);
  int n = fts_word_get_int(argv + 1);
  int i;

  for (i=0; i<n; i++)
    v[i] = v[i] + v[i];
}

static void ftl_mul_self(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_pointer(argv + 0);
  int n = fts_word_get_int(argv + 1);
  int i;

  for (i=0; i<n; i++)
    v[i] = v[i] * v[i];
}

static void ftl_add_self_64(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_pointer(argv + 0);
  int n = fts_word_get_int(argv + 1);
  int i;

  for (i = 0; i < 64; i++)
    v[i] = v[i] + v[i];
}

static void ftl_mul_self_64(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_pointer(argv + 0);
  int i;

  for (i = 0; i < 64; i++)
    v[i] = v[i] * v[i];
}


/**************************************************************
 *
 *  dsp functions with scalar (const)
 *
 */

static void 
ftl_add_const(fts_word_t *argv)
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
ftl_sub_const(fts_word_t *argv)
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
ftl_mul_const(fts_word_t *argv)
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
ftl_div_const(fts_word_t *argv)
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
ftl_bus_const(fts_word_t *argv)
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
ftl_vid_const(fts_word_t *argv)
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
ftl_ee_const (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float scl = *((float *)fts_word_get_pointer(argv + 1 )); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in[i] == scl; 
} 

/* comparison not yet implemented */
static void 
ftl_ne_const (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float scl = *((float *)fts_word_get_pointer(argv + 1 )); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in[i] != scl; 
} 

static void 
ftl_le_const (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float scl = *((float *)fts_word_get_pointer(argv + 1 )); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in[i] <= scl; 
} 

static void 
ftl_ge_const (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float scl = *((float *)fts_word_get_pointer(argv + 1 )); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in[i] >= scl; 
} 

static void 
ftl_lt_const (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float scl = *((float *)fts_word_get_pointer(argv + 1 )); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in[i] < scl; 
} 

static void 
ftl_gt_const (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float scl = *((float *)fts_word_get_pointer(argv + 1 )); 
  float *out = (float *)fts_word_get_pointer(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = in[i] > scl; 
} 

/**************************************************************
 *
 *  dsp functions with scalar (const)
 *
 */

static void 
ftl_add_const_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] += c;
}

static void 
ftl_sub_const_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] -= c;
}

static void 
ftl_mul_const_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] *= c;
}

static void 
ftl_div_const_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] /= c;
}

static void 
ftl_bus_const_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] = c - sig[i];
}

static void 
ftl_vid_const_inplace(fts_word_t *argv)
{
  float c = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict sig = (float *)fts_word_get_pointer(argv + 1);
  int n = fts_word_get_int(argv + 2);
  int i;

  for (i=0; i<n; i++)
    sig[i] = c / sig[i];
}

/************************************************
 *
 *  binop put
 *
 */

static void
binop_put(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t sym)
{
  fts_atom_t argv[4];

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(argv + 3, fts_dsp_get_output_size(dsp, 0));

  fts_dsp_add_function(sym, 4, argv);
}

static void
binop_put_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  if (fts_dsp_is_input_null(dsp, 0))
    {
      /* no first input */

      if (fts_dsp_get_input_name(dsp, 1) != fts_dsp_get_output_name(dsp, 0))
	fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 1), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_output_size(dsp, 0));
    }
  else if (fts_dsp_is_input_null(dsp, 1))
    {
      /* no second input */

      if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
	fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 0), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_output_size(dsp, 0));
    }
  else
    {
      /* all input connected */

      if (fts_dsp_get_output_size(dsp, 0) == 64)
	{
	  /* tick size of 64 */

	  if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	      (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	    {
	      /* all signals equal (for tick size of 64) */
	  
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_dsp_add_function(sym_add_self_64, 1, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* first input == output (for tick size of 64) */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_dsp_add_function(sym_add_inplace_64, 2, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* second input == output (for tick size of 64) */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_dsp_add_function(sym_add_inplace_64, 2, argv);
	    }
	  else
	    {
	      /* no inplace (for tick size of 64) */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	      fts_dsp_add_function(sym_add_64, 3, argv);
	    }
	}
      else
	{
	  if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	      (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	    {
	      /* all signals equal */
	  
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_int(argv + 1, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_add_self, 2, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* first input == output */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 2, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_add_inplace, 3, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* second input == output */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 2, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_add_inplace, 3, argv);
	    }
	  else
	    {
	      /* no inplace */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 3, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_add, 4, argv);
	    }
	}
    }
}

static void 
binop_put_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_sub);
}

static void
binop_put_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  if (fts_dsp_is_input_null(dsp, 0))
    {
      /* no first input */

      if (fts_dsp_get_input_name(dsp, 1) != fts_dsp_get_output_name(dsp, 0))
	fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 1), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_output_size(dsp, 0));
    }
  else if (fts_dsp_is_input_null(dsp, 1))
    {
      /* no second input */

      if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
	fts_dsp_add_function_copy(fts_dsp_get_input_name(dsp, 0), fts_dsp_get_output_name(dsp, 0), fts_dsp_get_output_size(dsp, 0));
    }
  else
    {
      /* all input connected */

      if (fts_dsp_get_output_size(dsp, 0) == 64)
	{
	  /* tick size of 64 */

	  if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	      (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	    {
	      /* all signals equal (for tick size of 64) */
	  
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_dsp_add_function(sym_mul_self_64, 1, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* first input == output (for tick size of 64) */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_dsp_add_function(sym_mul_inplace_64, 2, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* second input == output (for tick size of 64) */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_dsp_add_function(sym_mul_inplace_64, 2, argv);
	    }
	  else
	    {
	      /* no inplace (for tick size of 64) */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	      fts_dsp_add_function(sym_mul_64, 3, argv);
	    }
	}
      else
	{
	  if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	      (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	    {
	      /* all signals equal */
	  
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_int(argv + 1, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_mul_self, 2, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* first input == output */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 2, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_mul_inplace, 3, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      /* second input == output */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 2, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_mul_inplace, 3, argv);
	    }
	  else
	    {
	      /* no inplace */

	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 3, fts_dsp_get_output_size(dsp, 0));
	      fts_dsp_add_function(sym_mul, 4, argv);
	    }
	}
    }
}

static void 
binop_put_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_div);
}

static void 
binop_put_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_bus);
}

static void 
binop_put_vid(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_vid);
}

/************************************************
 *
 *  binop const put
 *
 */

static void
binop_const_put(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t sym, fts_symbol_t sym_inplace)
{
  binop_const_t *this = (binop_const_t *)o;
  fts_atom_t argv[4];

  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      /* inplace */
      fts_set_ftl_data(argv + 0, this->data);
      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(argv + 2, fts_dsp_get_output_size(dsp, 0));
      
      fts_dsp_add_function(sym_inplace, 3, argv);
    }
  else
    {
      fts_set_ftl_data(argv + 0, this->data);
      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(argv + 3, fts_dsp_get_output_size(dsp, 0));
      
      fts_dsp_add_function(sym, 4, argv);
    }
}

static void 
binop_const_put_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_add_const, sym_add_const_inplace);
}

static void 
binop_const_put_sub(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_sub_const, sym_sub_const_inplace);
}

static void 
binop_const_put_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_mul_const, sym_mul_const_inplace);
}

static void 
binop_const_put_div(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_div_const, sym_div_const_inplace);
}

static void 
binop_const_put_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_bus_const, sym_bus_const_inplace);
}

static void 
binop_const_put_vid(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sym_vid_const, sym_vid_const_inplace);
}

/************************************************
 *
 *  binop user methods
 *
 */
 
static void
binop_set_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_t *this = (binop_const_t *)o;
  float *ptr = (float *)ftl_data_get_ptr(this->data);

  *ptr = fts_get_number_float(at);
}

/************************************************
 *
 *  binop init delete instantiate
 *
 */

static void
binop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_add_object(o);
}

static void
binop_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}

static fts_status_t
binop_instantiate_realize(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t mth)
{
  fts_class_init(cl, sizeof(binop_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, mth);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
  fts_dsp_declare_outlet(cl, 0);

  return fts_Success;
}
  
/************************************************
 *
 *  binop const init delete instantiate
 *
 */

static void
binop_const_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_t *this = (binop_const_t *)o;
  float *data;

  this->data = ftl_data_alloc(sizeof(float));

  binop_set_const(o, 0, 0, 1, at + 1);
  
  fts_dsp_add_object(o);
}

static void
binop_const_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_const_t *this = (binop_const_t *)o;

  ftl_data_free(this->data);

  fts_dsp_remove_object(o);
}


static fts_status_t
binop_const_instantiate_realize(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t mth)
{
  fts_class_init(cl, sizeof(binop_const_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_const_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, binop_const_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, mth);

  fts_method_define_varargs(cl, 1, fts_s_int, binop_set_const);
  fts_method_define_varargs(cl, 1, fts_s_float, binop_set_const);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
    
  return fts_Success;
}

/************************************************
 *
 * instantiate
 *
 */

static fts_status_t 
binop_add_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 0)
    return binop_instantiate_realize(cl, ac, at, binop_put_add);
  else if (ac == 1)
    return binop_const_instantiate_realize(cl, ac, at, binop_const_put_add);
  else
    return &fts_CannotInstantiate;
}


static fts_status_t 
binop_sub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 0)
    return binop_instantiate_realize(cl, ac, at, binop_put_sub);
  else if (ac == 1)
    return binop_const_instantiate_realize(cl, ac, at, binop_const_put_sub);
  else
    return &fts_CannotInstantiate;
}


static fts_status_t 
binop_mul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 0)
    return binop_instantiate_realize(cl, ac, at, binop_put_mul);
  else if (ac == 1)
    return binop_const_instantiate_realize(cl, ac, at, binop_const_put_mul);
  else
    return &fts_CannotInstantiate;
}


static fts_status_t 
binop_div_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 0)
    return binop_instantiate_realize(cl, ac, at, binop_put_div);
  else if (ac == 1)
    return binop_const_instantiate_realize(cl, ac, at, binop_const_put_div);
  else
    return &fts_CannotInstantiate;
}

static fts_status_t 
binop_bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 0)
    return binop_instantiate_realize(cl, ac, at, binop_put_bus);
  else if (ac == 1)
    return binop_const_instantiate_realize(cl, ac, at, binop_const_put_bus);
  else
    return &fts_CannotInstantiate;
}

static fts_status_t 
binop_vid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 0)
    return binop_instantiate_realize(cl, ac, at, binop_put_vid);
  else if (ac == 1)
    return binop_const_instantiate_realize(cl, ac, at, binop_const_put_vid);
  else
    return &fts_CannotInstantiate;
}

void
signal_binop_config(void)
{
  /* signal x signal */

  sym_add = fts_new_symbol("add");
  fts_dsp_declare_function(sym_add, ftl_add);

  sym_sub = fts_new_symbol("sub");
  fts_dsp_declare_function(sym_sub, ftl_sub);

  sym_mul = fts_new_symbol("mul");
  fts_dsp_declare_function(sym_mul, ftl_mul);

  sym_div = fts_new_symbol("div");
  fts_dsp_declare_function(sym_div, ftl_div);

  sym_bus = fts_new_symbol("bus");
  fts_dsp_declare_function(sym_bus, ftl_bus);

  sym_vid = fts_new_symbol("vid");
  fts_dsp_declare_function(sym_vid, ftl_vid);

  /* signal x signal (tick size of 64) */

  sym_add_64 = fts_new_symbol("add_64");
  fts_dsp_declare_function(sym_add_64, ftl_add_64);

  sym_mul_64 = fts_new_symbol("mul_64");
  fts_dsp_declare_function(sym_mul_64, ftl_mul_64);

  /* inplace */

  sym_add_inplace = fts_new_symbol("add_inplace");
  fts_dsp_declare_function(sym_add_inplace, ftl_add_inplace);

  sym_mul_inplace = fts_new_symbol("mul_inplace");
  fts_dsp_declare_function(sym_mul_inplace, ftl_mul_inplace);

  sym_add_inplace_64 = fts_new_symbol("add_inplace_64");
  fts_dsp_declare_function(sym_add_inplace_64, ftl_add_inplace_64);

  sym_mul_inplace_64 = fts_new_symbol("mul_inplace_64");
  fts_dsp_declare_function(sym_mul_inplace_64, ftl_mul_inplace_64);

  /* v = v x v */

  sym_add_self = fts_new_symbol("add_self");
  fts_dsp_declare_function(sym_add_self, ftl_add_self);

  sym_mul_self = fts_new_symbol("mul_self");
  fts_dsp_declare_function(sym_mul_self, ftl_mul_self);

  sym_add_self_64 = fts_new_symbol("add_self_64");
  fts_dsp_declare_function(sym_add_self_64, ftl_add_self_64);

  sym_mul_self_64 = fts_new_symbol("mul_self_64");
  fts_dsp_declare_function(sym_mul_self_64, ftl_mul_self_64);

  /* signal x const */

  sym_add_const = fts_new_symbol("add_const");
  fts_dsp_declare_function(sym_add_const, ftl_add_const);

  sym_sub_const = fts_new_symbol("sub_const");
  fts_dsp_declare_function(sym_sub_const, ftl_sub_const);

  sym_mul_const = fts_new_symbol("mul_const");
  fts_dsp_declare_function(sym_mul_const, ftl_mul_const);

  sym_div_const = fts_new_symbol("div_const");
  fts_dsp_declare_function(sym_div_const, ftl_div_const);

  sym_bus_const = fts_new_symbol("bus_const");
  fts_dsp_declare_function(sym_bus_const, ftl_bus_const);

  sym_vid_const = fts_new_symbol("vid_const");
  fts_dsp_declare_function(sym_vid_const, ftl_vid_const);

  /* signal x const inplace */

  sym_add_const_inplace = fts_new_symbol("add_const_inplace");
  fts_dsp_declare_function(sym_add_const_inplace, ftl_add_const_inplace);

  sym_sub_const_inplace = fts_new_symbol("sub_const_inplace");
  fts_dsp_declare_function(sym_sub_const_inplace, ftl_sub_const_inplace);

  sym_mul_const_inplace = fts_new_symbol("mul_const_inplace");
  fts_dsp_declare_function(sym_mul_const_inplace, ftl_mul_const_inplace);

  sym_div_const_inplace = fts_new_symbol("div_const_inplace");
  fts_dsp_declare_function(sym_div_const_inplace, ftl_div_const_inplace);

  sym_bus_const_inplace = fts_new_symbol("bus_const_inplace");
  fts_dsp_declare_function(sym_bus_const_inplace, ftl_bus_const_inplace);

  sym_vid_const_inplace = fts_new_symbol("vid_const_inplace");
  fts_dsp_declare_function(sym_vid_const_inplace, ftl_vid_const_inplace);

  fts_metaclass_install(fts_new_symbol("+~"), binop_add_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("-~"), binop_sub_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("*~"), binop_mul_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("/~"), binop_div_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("-+~"), binop_bus_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("/*~"), binop_vid_instantiate, fts_narg_equiv);
  
  /* compatibility */
  fts_metaclass_install(fts_new_symbol("inv+~"), binop_bus_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("inv*~"), binop_vid_instantiate, fts_narg_equiv);
}
