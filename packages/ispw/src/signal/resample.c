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

/* resample.c */

#include <string.h>

#include "fts.h"


/* ------------------------------ sigup~ ------------------------------- */
static fts_symbol_t sigup_function = 0;

typedef struct
{
  float last[3];
} up_state_t;

typedef struct up
{
  fts_object_t _o;

  ftl_data_t up_ftl_data;
} sigup_t;


static void
ftl_up(fts_word_t *argv)
{
  float * restrict in  = (float *)fts_word_get_ptr(argv);
  float * restrict out = (float *)fts_word_get_ptr(argv+1);
  up_state_t *x = (up_state_t *)fts_word_get_ptr(argv+2);
  int n = fts_word_get_long(argv+3);
  int i, j;
  float x0;
  float x1 = x->last[0];
  float x2 = x->last[1];
  float x3 = x->last[2];

  for (i = 0, j = 0; i < n; i++, j += 2)
    {
      x0 = x1;
      x1 = x2;
      x2 = x3;
      x3 = in[i];
      out[j]  = .5625f * (x1 + x2) - .0625f * (x0 + x3);
      out[j+1] = x2;
    }

  x->last[0] = x1;
  x->last[1] = x2;
  x->last[2] = x3;
}


static void
sigup_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigup_t *this = (sigup_t *)o;
  fts_atom_t argv[4];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  const float zero = 0.0f;

  ftl_data_set(up_state_t, this->up_ftl_data, last[0], &zero);
  ftl_data_set(up_state_t, this->up_ftl_data, last[1], &zero);
  ftl_data_set(up_state_t, this->up_ftl_data, last[2], &zero);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));

  fts_set_ftl_data(argv+2, this->up_ftl_data);
  fts_set_long(argv+3, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(sigup_function, 4, argv);
}

static void
sigup_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigup_t *this = (sigup_t *)o;

  this->up_ftl_data = ftl_data_new(up_state_t);
  dsp_list_insert(o);
}

static void
sigup_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigup_t *this = (sigup_t *)o;

  ftl_data_free(this->up_ftl_data);
  dsp_list_remove(o);
}

static fts_status_t
sigup_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(sigup_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigup_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigup_delete, 0, 0);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigup_put, 1, a);

  /* declare the object upsampling for the DSP compiler */

  {
    fts_atom_t av;

    fts_set_long(&av, 1);
    fts_class_put_prop(cl, fts_s_dsp_upsampling, &av);
  }

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  sigup_function = fts_new_symbol("up");
  dsp_declare_function(sigup_function, ftl_up);
  
  return fts_Success;
}

void
sigup_config(void)
{
  fts_class_install(fts_new_symbol("up~"),sigup_instantiate);
}

/* ------------------------------ sigdown~ ------------------------------- */
static fts_symbol_t sigdown_function = 0;

typedef struct down
{
  fts_object_t _o;
} sigdown_t;


static void
ftl_down(fts_word_t *argv)
{
  float * restrict in = (float *)fts_word_get_ptr(argv);
  float * restrict out = (float *)fts_word_get_ptr(argv+1);
  long int n = fts_word_get_long(argv+2);
  int i, j;

  for (i = 0, j = 0; i < n; i++, j += 2)
    out[i] = in[j];
}


static void
sigdown_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0) >> 1);

  dsp_add_funcall(sigdown_function, 3, argv);
}

static void
sigdown_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o);
}

static void
sigdown_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

static fts_status_t
sigdown_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(sigdown_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigdown_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigdown_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigdown_put, 1, a);

  /* declare the object downsampling for the DSP compiler */

  {
    fts_atom_t av;

    fts_set_long(&av, 1);
    fts_class_put_prop(cl, fts_s_dsp_downsampling, &av);
  }


  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  sigdown_function = fts_new_symbol("down");
  dsp_declare_function(sigdown_function, ftl_down);

  return fts_Success;
}

void
sigdown_config(void)
{
  fts_class_install(fts_new_symbol("down~"),sigdown_instantiate);
}
