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

#include "iir.h"

static fts_symbol_t iir_dsp_function[5] = {0, 0, 0, 0, 0};

/****************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t head;
  ftl_data_t state;
  ftl_data_t coefs;  
  long n_order;
} iir_t;

static void
iir_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  float *coefs;
  float zero = 0.0f;
  long n_order;
  long i;

  n_order = ac - 1;

  this->n_order = n_order;
  this->coefs = ftl_data_alloc(sizeof(float) * n_order);
  this->state = ftl_data_alloc(sizeof(float) * n_order);

  coefs = (float *)ftl_data_get_ptr(this->coefs);
  for(i=0; i<n_order; i++)
    coefs[i] = fts_get_float_arg(ac, at, i + 1, 0.0f);
  
  dsp_list_insert(o);
 
}


static void
iir_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;

  if(this->n_order > 0)
    {
      ftl_data_free(this->coefs);
      ftl_data_free(this->state);
    }

  dsp_list_remove(o);
}


/****************************************
 *
 *   dsp
 *
 */

static void
iir_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  fts_atom_t argv[6];
 
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_vec_fzero((float *)ftl_data_get_ptr(this->state), this->n_order);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 2, this->state);
  fts_set_ftl_data(argv + 3, this->coefs);
  fts_set_long(argv + 4, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(iir_dsp_function[this->n_order], 5, argv);
}


/****************************************
 *
 *   user methods
 *
 */

static void
iir_clear(fts_object_t *o, int inlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  
  fts_vec_fzero((float *)ftl_data_get_ptr(this->state), this->n_order);
}

static void
iir_set_coef(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  float *coefs = (float *)ftl_data_get_ptr(this->coefs);

  coefs[i-1] = fts_get_float_arg(ac, at, 0, 0.0f);
}

/****************************************
 *
 *   class
 *
 */

static fts_status_t
iir_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  fts_symbol_t sym_clear;
  int i;
 
  if(ac <= 1 || ac > 5)
    return &fts_CannotInstantiate;

  fts_class_init(cl, sizeof(iir_t), ac, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, iir_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, iir_delete, 0, a);

  a[0] = fts_s_ptr; 
  fts_method_define(cl, fts_SystemInlet, fts_s_put, iir_put, 1, a);
  
  /* user methods */
  a[0] = fts_s_float;
  for(i=1; i<ac; i++)
    fts_method_define(cl, i, fts_s_float, iir_set_coef, 1, a);

  a[0] = fts_s_int;
  for(i=1; i<ac; i++)
    fts_method_define(cl, i, fts_s_int, iir_set_coef, 1, a);

  fts_method_define(cl, 0, fts_new_symbol("clear"), iir_clear, 0, 0);

  /* dsp in/outlets */
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
iir_config(void)
{ 
  iir_dsp_function[1] = fts_new_symbol("iir_1");
  iir_dsp_function[2] = fts_new_symbol("iir_2");
  iir_dsp_function[3] = fts_new_symbol("iir_3");
  iir_dsp_function[4] = fts_new_symbol("iir_4");

  dsp_declare_function(iir_dsp_function[1], ftl_iir_1);
  dsp_declare_function(iir_dsp_function[2], ftl_iir_2);
  dsp_declare_function(iir_dsp_function[3], ftl_iir_3);
  dsp_declare_function(iir_dsp_function[4], ftl_iir_4);

  fts_metaclass_create(fts_new_symbol("iir~"), iir_instantiate, fts_narg_equiv);
}
