/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

#include "biquad.h"

#define DEF_type sym_df1

static fts_symbol_t sym_df1 = 0;
static fts_symbol_t sym_df2 = 0;

static fts_symbol_t biquad_df1_dsp_function = 0;
static fts_symbol_t biquad_df2_dsp_function = 0;

static void biquad_set_coef_a0(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);
static void biquad_set_coef_a1(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);
static void biquad_set_coef_a2(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);
static void biquad_set_coef_b1(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);
static void biquad_set_coef_b2(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);

static void biquad_df1_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);
static void biquad_df2_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at);

/****************************************
 *
 *  object
 *
 */

typedef struct
{
  fts_object_t head;
  ftl_data_t biquad_state;
  ftl_data_t biquad_coefs;
} biquad_t;

static void
biquad_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  fts_symbol_t type = (fts_symbol_t )this->head.cl->user_data;

  if(ac > 1 && fts_is_symbol(at + 1))
    {
      ac -= 1; /* skip metaclass name type specification */
      at += 1;
    }

  this->biquad_coefs = ftl_data_new(biquad_coefs_t);
  biquad_set_coef_a0(o, 0, 0, ac-1, at + 1);
  biquad_set_coef_a1(o, 0, 0, ac-2, at + 2);
  biquad_set_coef_a2(o, 0, 0, ac-3, at + 3);
  biquad_set_coef_b1(o, 0, 0, ac-4, at + 4);
  biquad_set_coef_b2(o, 0, 0, ac-5, at + 5);

  if(type == sym_df1)
    this->biquad_state = ftl_data_new(biquad_df1_state_t);  
  else if(type == sym_df2)
    this->biquad_state = ftl_data_new(biquad_df2_state_t);  

  dsp_list_insert(o);
}

static void
biquad_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;

  ftl_data_free(this->biquad_coefs);
  ftl_data_free(this->biquad_state);
  dsp_list_remove(o);
}


/****************************************
 *
 *  dsp
 *
 */

static void
biquad_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  fts_symbol_t type = (fts_symbol_t )this->head.cl->user_data;
  fts_atom_t argv[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);


  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv+2, this->biquad_state);
  fts_set_ftl_data(argv+3, this->biquad_coefs);
  fts_set_long(argv+4, fts_dsp_get_input_size(dsp, 0));

  if(type == sym_df1)
    {
      biquad_df1_state_clear(o, 0, 0, 0, 0);
      dsp_add_funcall(biquad_df1_dsp_function, 5, argv);
    }
  else if(type == sym_df2)
    {
      biquad_df2_state_clear(o, 0, 0, 0, 0);
      dsp_add_funcall(biquad_df2_dsp_function, 5, argv);
    }	
}


/****************************************
 *
 *  methods
 *
 */

static void
biquad_df1_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  const float zero = 0.0;

  ftl_data_set(biquad_df1_state_t, this->biquad_state, xnm1, &zero);
  ftl_data_set(biquad_df1_state_t, this->biquad_state, xnm2, &zero);
  ftl_data_set(biquad_df1_state_t, this->biquad_state, ynm1, &zero);
  ftl_data_set(biquad_df1_state_t, this->biquad_state, ynm2, &zero);
}

static void
biquad_df2_state_clear(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  const float zero = 0.0;

  ftl_data_set(biquad_df2_state_t, this->biquad_state, wnm1, &zero);
  ftl_data_set(biquad_df2_state_t, this->biquad_state, wnm2, &zero);
}

static void
biquad_set_coef_a0(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, a0, &f);
}

static void
biquad_set_coef_a1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, a1, &f);
}

static void
biquad_set_coef_a2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, a2, &f);
}

static void
biquad_set_coef_b1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, b1, &f);
}

static void
biquad_set_coef_b2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f);
  ftl_data_set(biquad_coefs_t, this->biquad_coefs, b2, &f);
}

static void
biquad_set_coefs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  biquad_t *this = (biquad_t *)o;
  biquad_set_coef_a0(o, 0, 0, ac - 0, at + 0);
  biquad_set_coef_a1(o, 0, 0, ac - 1, at + 1);
  biquad_set_coef_a2(o, 0, 0, ac - 2, at + 2);
  biquad_set_coef_b1(o, 0, 0, ac - 3, at + 3);
  biquad_set_coef_b2(o, 0, 0, ac - 4, at + 4);
}

/****************************************
 *
 *  class
 *
 */

static fts_status_t
biquad_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[6];
  fts_symbol_t type, sym_clear;
  fts_method_t biquad_state_clear, biquad_coef, biquad_coefs_list;
  int i;

  if(ac < 2 || !fts_is_symbol(at + 1))
    type = DEF_type;
  else
    type = fts_get_symbol(at + 1);

  sym_clear = fts_new_symbol("clear");

  /* method to clear object state */

  if(type == sym_df1)
    biquad_state_clear = biquad_df1_state_clear;
  else if(type == sym_df2)
    biquad_state_clear = biquad_df2_state_clear;
  else
    return &fts_CannotInstantiate;

  fts_class_init(cl, sizeof(biquad_t), 6, 1, (void *)type);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, biquad_set_coef_a0, 1, a);
  fts_method_define(cl, 2, fts_s_float, biquad_set_coef_a1, 1, a);
  fts_method_define(cl, 3, fts_s_float, biquad_set_coef_a2, 1, a);
  fts_method_define(cl, 4, fts_s_float, biquad_set_coef_b1, 1, a);
  fts_method_define(cl, 5, fts_s_float, biquad_set_coef_b2, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, biquad_set_coef_a0, 1, a);
  fts_method_define(cl, 2, fts_s_int, biquad_set_coef_a1, 1, a);
  fts_method_define(cl, 3, fts_s_int, biquad_set_coef_a2, 1, a);
  fts_method_define(cl, 4, fts_s_int, biquad_set_coef_b1, 1, a);
  fts_method_define(cl, 5, fts_s_int, biquad_set_coef_b2, 1, a);
  
  fts_method_define_varargs(cl, 0, fts_s_set, biquad_set_coefs);

  fts_method_define(cl, 0, sym_clear, biquad_state_clear, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, biquad_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, biquad_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, biquad_put, 1, a);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

static int
biquad_class_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  fts_symbol_t type0, type1;

  if(ac0 < 2 || !fts_is_symbol(at0 + 1))
    type0 = DEF_type;
  else 
    type0 = fts_get_symbol(at0 + 1);

  if(ac1 < 2 || !fts_is_symbol(at1 + 1))
    type1 = DEF_type;
  else 
    type1 = fts_get_symbol(at1 + 1);

  return(type0 == type1);
}

void
biquad_config(void)
{
  sym_df1 = fts_new_symbol("direct_I");  
  sym_df2 = fts_new_symbol("direct_II");  
  
  biquad_df1_dsp_function = fts_new_symbol("biquad_df1");
  biquad_df2_dsp_function = fts_new_symbol("biquad_df2");

  dsp_declare_function(biquad_df1_dsp_function, ftl_biquad_df1);
  dsp_declare_function(biquad_df2_dsp_function, ftl_biquad_df2);

  fts_metaclass_create(fts_new_symbol("biquad~"), biquad_instantiate, biquad_class_equiv);
}
