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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

fts_symbol_t dsp_add;
fts_symbol_t dsp_mul;

fts_symbol_t dsp_64_add;
fts_symbol_t dsp_64_mul;

fts_symbol_t dsp_2ops_add;
fts_symbol_t dsp_2ops_mul;

fts_symbol_t dsp_64_2ops_add;
fts_symbol_t dsp_64_2ops_mul;

fts_symbol_t dsp_1ops_add;
fts_symbol_t dsp_1ops_mul;

fts_symbol_t dsp_64_1ops_add;
fts_symbol_t dsp_64_1ops_mul;

/* We do it only for mul and add, plain and for size == 64  */

static void ftl_add(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_ptr(argv+0);
  float * restrict in2 = (float *)fts_word_get_ptr(argv+1);
  float * restrict out = (float *)fts_word_get_ptr(argv+2);
  int n = fts_word_get_long(argv+3);
  int i;

  for (i = 0; i < n; i++)
    out[i] = in1[i] + in2[i];
}

static void ftl_mul(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_ptr(argv+0);
  float * restrict in2 = (float *)fts_word_get_ptr(argv+1);
  float * restrict out = (float *)fts_word_get_ptr(argv+2);
  int n = fts_word_get_long(argv+3);
  int i;

  for (i = 0; i < n; i++)
    out[i] = in1[i] * in2[i];
}


static void ftl_add_64(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_ptr(argv+0);
  float * restrict in2 = (float *)fts_word_get_ptr(argv+1);
  float * restrict out = (float *)fts_word_get_ptr(argv+2);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = in1[i] + in2[i];
}

static void ftl_mul_64(fts_word_t *argv)
{
  float * restrict in1 = (float *)fts_word_get_ptr(argv+0);
  float * restrict in2 = (float *)fts_word_get_ptr(argv+1);
  float * restrict out = (float *)fts_word_get_ptr(argv+2);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = in1[i] * in2[i];
}

/* versions with two args: out = out + in */

static void ftl_add_2ops(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv+0);
  float * restrict out = (float *)fts_word_get_ptr(argv+1);
  int n = fts_word_get_long(argv+2);
  int i;

  for (i = 0; i < n; i++)
    out[i] = out[i] + in[i];
}

static void ftl_mul_2ops(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv+0);
  float * restrict out = (float *)fts_word_get_ptr(argv+1);
  int n = fts_word_get_long(argv+2);
  int i;

  for (i = 0; i < n; i++)
    out[i] = out[i] * in[i];
}

static void ftl_add_64_2ops(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv+0);
  float * restrict out = (float *)fts_word_get_ptr(argv+1);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = out[i] + in[i];
}

static void ftl_mul_64_2ops(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv+0);
  float * restrict out = (float *)fts_word_get_ptr(argv+1);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = out[i] * in[i];
}

/* version with one arg ( v = v + v, v = v * v)
   Probabily useless, but needed for consitency of
   the ivdep thing */

static void ftl_add_1ops(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_ptr(argv+0);
  int n = fts_word_get_long(argv+1);
  int i;

  for (i = 0; i < n; i++)
    v[i] = v[i] + v[i];
}

static void ftl_mul_1ops(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_ptr(argv+0);
  int n = fts_word_get_long(argv+1);
  int i;

  for (i = 0; i < n; i++)
    v[i] = v[i] * v[i];
}

static void ftl_add_64_1ops(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_ptr(argv+0);
  int n = fts_word_get_long(argv+1);
  int i;

  for (i = 0; i < 64; i++)
    v[i] = v[i] + v[i];
}

static void ftl_mul_64_1ops(fts_word_t *argv)
{
  float * restrict v = (float *)fts_word_get_ptr(argv+0);
  int i;

  for (i = 0; i < 64; i++)
    v[i] = v[i] * v[i];
}


/* the class */


#define DEFINE_PUT_VEC_VEC_FUN(name)\
  static void vecvecbinop_put_vec_vec_ ## name\
    (fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)\
    {vecvecbinop_put_vec_vec(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), ftl_sym.name.f.vec.vec);}

#define DEFINE_PUT_VEC_SCL_FUN(name)\
  static void vecsclbinop_put_vec_scl_ ## name\
    (fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)\
    {vecsclbinop_put_vec_scl(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), ftl_sym.name.f.vec.scl);}


/************************************************
 *
 *    vecvec object
 *
 */

typedef struct
{
  fts_object_t obj;

} vecvecbinop_t;

/* objects creation arguments */

static void
vecvecbinop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o);
}

static void
vecvecbinop_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}


/************************************************
 *
 *  vecscl object
 *
 */


typedef struct
{
  fts_object_t head;

  ftl_data_t scl_ftl_data;
} vecsclbinop_t;


static void
vecsclbinop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecsclbinop_t *this = (vecsclbinop_t *)o;
  float scl;

  scl = fts_get_float_arg(ac, at, 1, 0.0f);

  this->scl_ftl_data = ftl_data_new(float);
  ftl_data_copy(float, this->scl_ftl_data, &scl);

  dsp_list_insert(o);
}

static void
vecsclbinop_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecsclbinop_t *this = (vecsclbinop_t *)o;
  ftl_data_free(this->scl_ftl_data);

  dsp_list_remove(o);
}


/************************************************
 *
 *    vec vec dsp
 *
 */

/* here the value of the enum element is important,
   should be expressed also if the default is ok 
*/

enum
{
  DSP_ARG_in0 = 0,
  DSP_ARG_in1 = 1,
  DSP_ARG_out = 2,
  DSP_ARG_n_tick = 3,
  N_DSP_ARGS = 4
};


static void
vecvecbinop_put_vec_vec(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t dsp_function)
{
  fts_atom_t argv[N_DSP_ARGS];

  fts_set_symbol(argv + DSP_ARG_in0,    fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + DSP_ARG_in1,    fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol(argv + DSP_ARG_out,    fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + DSP_ARG_n_tick, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(dsp_function, N_DSP_ARGS, argv);
}

/* a single put fun for each class (two per operator) */

/* optimize "+" without in0 connected */

static void
vecvecbinop_put_vec_vec_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  if (fts_dsp_is_input_null(dsp, 0))
    {
      if (fts_dsp_get_input_name(dsp, 1) != fts_dsp_get_output_name(dsp, 0))
      {
	fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 1));
	fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	fts_set_long   (argv + 2, fts_dsp_get_input_size(dsp, 0));
	dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
      }
    }
  else if (fts_dsp_is_input_null(dsp, 1))
    {
      if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol (argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol (argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_long   (argv + 2, fts_dsp_get_input_size(dsp, 0));
	  dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
	}
    }
  else
    {
      if (fts_dsp_get_input_size(dsp, 0) == 64)
	{
	  if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	      (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      dsp_add_funcall(dsp_64_1ops_add, 1, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      dsp_add_funcall(dsp_64_2ops_add, 2, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      dsp_add_funcall(dsp_64_2ops_add, 2, argv);
	    }
	  else
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	      dsp_add_funcall(dsp_64_add, 3, argv);
	    }
	}
      else
	{
	  if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	      (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_int(argv + 1, fts_dsp_get_input_size(dsp, 0));
	      dsp_add_funcall(dsp_1ops_add, 2, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
	      dsp_add_funcall(dsp_2ops_add, 3, argv);
	    }
	  else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
	      dsp_add_funcall(dsp_2ops_add, 3, argv);
	    }
	  else
	    {
	      fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	      fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	      fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	      fts_set_int(argv + 3, fts_dsp_get_input_size(dsp, 0));
	      dsp_add_funcall(dsp_add, 4, argv);
	    }
	}
    }
}

static void
vecvecbinop_put_vec_vec_mul(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[N_DSP_ARGS];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_symbol_t dsp_function = ftl_sym.mul.f.vec.vec;

  if (fts_dsp_get_input_size(dsp, 0) == 64)
    {
      if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	  (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	  dsp_add_funcall(dsp_64_1ops_mul, 1, argv);
	}
      else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  dsp_add_funcall(dsp_64_2ops_mul, 2, argv);
	}
      else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  dsp_add_funcall(dsp_64_2ops_mul, 2, argv);
	}
      else
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	  dsp_add_funcall(dsp_64_mul, 3, argv);
	}
    }
  else
    {
      if ((fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0)) && 
	  (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0)))
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_int   (argv + 1, fts_dsp_get_input_size(dsp, 0));
	  dsp_add_funcall(dsp_1ops_mul, 2, argv);
	}
      else if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int   (argv + 2, fts_dsp_get_input_size(dsp, 0));
	  dsp_add_funcall(dsp_2ops_mul, 3, argv);
	}
      else if (fts_dsp_get_input_name(dsp, 1) == fts_dsp_get_output_name(dsp, 0))
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int   (argv + 2, fts_dsp_get_input_size(dsp, 0));
	  dsp_add_funcall(dsp_2ops_mul, 3, argv);
	}
      else
	{
	  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
	  fts_set_symbol(argv + 1, fts_dsp_get_input_name(dsp, 1));
	  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
	  fts_set_int   (argv + 3, fts_dsp_get_input_size(dsp, 0));
	  dsp_add_funcall(dsp_mul, 4, argv);
	}
    }
}

  DEFINE_PUT_VEC_VEC_FUN(sub)
  DEFINE_PUT_VEC_VEC_FUN(div)
  DEFINE_PUT_VEC_VEC_FUN(bus)
  DEFINE_PUT_VEC_VEC_FUN(vid)


/************************************************
 *
 *    vec scl dsp
 *
 */


/* here the value of the enum element is important,
   should be expressed also if the default is ok 
*/

static void
vecsclbinop_put_vec_scl(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t dsp_symbol)
{
  vecsclbinop_t *this = (vecsclbinop_t *)o;
  fts_atom_t argv[N_DSP_ARGS];

  fts_set_symbol(argv + DSP_ARG_in0,    fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data(argv + DSP_ARG_in1,  this->scl_ftl_data);
  fts_set_symbol(argv + DSP_ARG_out,    fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + DSP_ARG_n_tick, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(dsp_symbol, N_DSP_ARGS, argv);
}

/* a single put fun for each class (two per operator) */


  DEFINE_PUT_VEC_SCL_FUN(add)
  DEFINE_PUT_VEC_SCL_FUN(sub)
  DEFINE_PUT_VEC_SCL_FUN(mul)
  DEFINE_PUT_VEC_SCL_FUN(div)
  DEFINE_PUT_VEC_SCL_FUN(bus)
  DEFINE_PUT_VEC_SCL_FUN(vid)


/************************************************
 *
 *    vec scl user methods
 *
 */
 
static void
vecsclbinop_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecsclbinop_t *this = (vecsclbinop_t *)o;
  float scl;

  scl = fts_get_float_arg(ac, at, 0, 0.0f);

  ftl_data_copy(float, this->scl_ftl_data, &scl);
}


/************************************************
 *
 *    vecvec class
 *
 */

static fts_status_t
vecvec_instantiate_realize(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t vec_vec_mth)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(vecvecbinop_t), 2, 1, 0);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, vecvecbinop_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, vecvecbinop_delete, 0, 0);

  a[0] = fts_s_ptr;

  fts_method_define(cl, fts_SystemInlet, fts_s_put, vec_vec_mth, 1, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1);
  dsp_sig_outlet(cl, 0);

  return fts_Success;
}
  
/************************************************
 *
 *    class
 *
 */

static fts_status_t
vecscl_instantiate_realize(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t vec_scl_mth)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(vecsclbinop_t), 2, 1, 0);
  
  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, vecsclbinop_init, 2, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, vecsclbinop_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, vec_scl_mth, 1, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
    
  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, vecsclbinop_number, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, vecsclbinop_number, 1, a);

  return fts_Success;
}



static fts_status_t 
vecadd_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 1)
    return vecvec_instantiate_realize(cl, ac, at, vecvecbinop_put_vec_vec_add);
  else if (ac == 2)
    return vecscl_instantiate_realize(cl, ac, at, vecsclbinop_put_vec_scl_add);
  else
    return &fts_CannotInstantiate;
}


static fts_status_t 
vecsub_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 1)
    return vecvec_instantiate_realize(cl, ac, at, vecvecbinop_put_vec_vec_sub);
  else if (ac == 2)
    return vecscl_instantiate_realize(cl, ac, at, vecsclbinop_put_vec_scl_sub);
  else
    return &fts_CannotInstantiate;
}


static fts_status_t 
vecmul_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 1)
    return vecvec_instantiate_realize(cl, ac, at, vecvecbinop_put_vec_vec_mul);
  else if (ac == 2)
    return vecscl_instantiate_realize(cl, ac, at, vecsclbinop_put_vec_scl_mul);
  else
    return &fts_CannotInstantiate;
}


static fts_status_t 
vecdiv_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 1)
    return vecvec_instantiate_realize(cl, ac, at, vecvecbinop_put_vec_vec_div);
  else if (ac == 2)
    return vecscl_instantiate_realize(cl, ac, at, vecsclbinop_put_vec_scl_div);
  else
    return &fts_CannotInstantiate;
}

static fts_status_t 
vecbus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 1)
    return vecvec_instantiate_realize(cl, ac, at, vecvecbinop_put_vec_vec_bus);
  else if (ac == 2)
    return vecscl_instantiate_realize(cl, ac, at, vecsclbinop_put_vec_scl_bus);
  else
    return &fts_CannotInstantiate;
}

static fts_status_t 
vecvid_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if (ac == 1)
    return vecvec_instantiate_realize(cl, ac, at, vecvecbinop_put_vec_vec_vid);
  else if (ac == 2)
    return vecscl_instantiate_realize(cl, ac, at, vecsclbinop_put_vec_scl_vid);
  else
    return &fts_CannotInstantiate;
}

void
vecarith_config(void)
{
  dsp_add = fts_new_symbol("add_vec");
  dsp_declare_function(dsp_add, ftl_add);

  dsp_mul = fts_new_symbol("mul_vec");
  dsp_declare_function(dsp_mul, ftl_mul);

  dsp_64_add = fts_new_symbol("add_vec_64");
  dsp_declare_function(dsp_64_add, ftl_add_64);

  dsp_64_mul = fts_new_symbol("mul_vec_64");
  dsp_declare_function(dsp_64_mul, ftl_mul_64);

  dsp_2ops_add = fts_new_symbol("add_vec_2ops");
  dsp_declare_function(dsp_2ops_add, ftl_add_2ops);

  dsp_2ops_mul = fts_new_symbol("mul_vec_2ops");
  dsp_declare_function(dsp_2ops_mul, ftl_mul_2ops);

  dsp_64_2ops_add = fts_new_symbol("add_vec_64_2ops");
  dsp_declare_function(dsp_64_2ops_add, ftl_add_64_2ops);

  dsp_64_2ops_mul = fts_new_symbol("mul_vec_64_2ops");
  dsp_declare_function(dsp_64_2ops_mul, ftl_mul_64_2ops);

  dsp_1ops_add = fts_new_symbol("add_vec_1ops");
  dsp_declare_function(dsp_1ops_add, ftl_add_1ops);

  dsp_1ops_mul = fts_new_symbol("mul_vec_1ops");
  dsp_declare_function(dsp_1ops_mul, ftl_mul_1ops);

  dsp_64_1ops_add = fts_new_symbol("add_vec_64_1ops");
  dsp_declare_function(dsp_64_1ops_add, ftl_add_64_1ops);

  dsp_64_1ops_mul = fts_new_symbol("mul_vec_64_1ops");
  dsp_declare_function(dsp_64_1ops_mul, ftl_mul_64_1ops);

  fts_metaclass_install(fts_new_symbol("+~"),  vecadd_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("-~"),  vecsub_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("*~"),  vecmul_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("/~"),  vecdiv_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("inv+~"), vecbus_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("inv*~"), vecvid_instantiate, fts_narg_equiv);
}


