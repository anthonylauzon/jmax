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





/* the class */
enum{
  INLET_sig = 0,
  INLET_base = 1,
  N_INLETS
};

enum{
  OUTLET_sig = 0,
  N_OUTLETS
};

#define DEFINE_PUT_VEC_FUN(name)\
  static void dsp_put_ ## name\
    (fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)\
    {dsp_put_vec(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), ftl_sym.name.vec);}

#define DEFINE_PUT_VEC_SCL_FUN(name)\
  static void dsp_put_ ## name\
    (fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)\
    {dsp_put_vec_scl(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), ftl_sym.name.vec);}

/************************************************
 *
 *    object
 *
 */

typedef struct{
  fts_object_t head;
  float scl; /* scalar in computation */
} obj_t;

/* objects creation arguments */
enum
{
  OBJ_ARG_class,
  OBJ_ARG_base,
  N_OBJ_ARGS
};

static void
sigexp_init_vec_scl(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  float base = fts_get_float_arg(ac, at, OBJ_ARG_base, 0.0f);
  
  dsp_list_insert(o);
  
  obj->scl = log(base);
}

static void
siglog_init_vec_scl(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  float base = fts_get_float_arg(ac, at, OBJ_ARG_base, 0.0f);
  
  dsp_list_insert(o);
  
  obj->scl = 1.0f/log(base);
}

static void
obj_init_vec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  dsp_list_insert(o);
}

static void
obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

/************************************************
 *
 *    dsp
 *
 */

static void
dsp_put_vec(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t dsp_symbol)
{
  fts_atom_t argv[4];

  fts_set_symbol(argv + 0,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(dsp_symbol, 3, argv);
}

static void
dsp_put_vec_scl(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t dsp_symbol)
{
  obj_t *obj = (obj_t *)o;
  fts_atom_t argv[4];

  fts_set_symbol(argv + 0,   fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr   (argv + 1, &obj->scl);
  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + 3, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(dsp_symbol, 4, argv);
}

/* a single put fun for each dsp function class */
  DEFINE_PUT_VEC_FUN(exp)
  DEFINE_PUT_VEC_FUN(log)
  DEFINE_PUT_VEC_FUN(log10)
  DEFINE_PUT_VEC_SCL_FUN(expb)
  DEFINE_PUT_VEC_SCL_FUN(logb)

/************************************************
 *
 *    user methods
 *
 */
static void
meth_float_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  obj->scl = log(fts_get_float_arg(ac, at, 0, 0.0f));
}

static void
meth_int_exp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  obj->scl = log(fts_get_long_arg(ac, at, 0, 0));
}

static void
meth_float_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  obj->scl = 1.0f/log(fts_get_float_arg(ac, at, 0, 0.0f));
}

static void
meth_int_log(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  obj_t *obj = (obj_t *)o;
  obj->scl =  1.0f/log(fts_get_long_arg(ac, at, 0, 0));
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
sigexp_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
 
  if (ac > 1)
    {
      fts_class_init(cl, sizeof(obj_t), 2, 1, 0); /* 2 inlets + 1 outlet */

      a[OBJ_ARG_class] = fts_s_symbol; /* class name */
      a[OBJ_ARG_base] = fts_s_number; /* base */
      fts_method_define(cl, fts_SystemInlet, fts_s_init, sigexp_init_vec_scl, 2, a);
      fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_put, dsp_put_expb, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, INLET_base, fts_s_int, meth_int_exp, 1, a);

      a[0] = fts_s_float;
      fts_method_define(cl, INLET_base, fts_s_float, meth_float_exp, 1, a);
    }
  else
    {
      fts_class_init(cl, sizeof(obj_t), 1, 1, 0); /* 1 inlet + 1 outlet */
      a[OBJ_ARG_class] = fts_s_symbol; /* class name */
      fts_method_define(cl, fts_SystemInlet, fts_s_init, obj_init_vec, 1, a);
      fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_put, dsp_put_exp, 1, a);
    }
  
  /* in any case */
  dsp_sig_inlet(cl, INLET_sig);
  dsp_sig_outlet(cl, OUTLET_sig);
  
  return fts_Success;
}


static fts_status_t
siglog_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
 
  if (ac > 1)
    {
      fts_class_init(cl, sizeof(obj_t), 2, 1, 0); /* 2 inlets + 1 outlet */

      a[OBJ_ARG_class] = fts_s_symbol; /* class name */
      a[OBJ_ARG_base] = fts_s_number; /* base */
      fts_method_define(cl, fts_SystemInlet, fts_s_init, siglog_init_vec_scl, 2, a);
      fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_put, dsp_put_logb, 1, a);

      a[0] = fts_s_int;
      fts_method_define(cl, INLET_base, fts_s_int, meth_int_log, 1, a);

      a[0] = fts_s_float;
      fts_method_define(cl, INLET_base, fts_s_float, meth_float_log, 1, a);
    }
  else
    {
      fts_class_init(cl, sizeof(obj_t), 1, 1, 0); /* 1 inlet + 1 outlet */
      a[OBJ_ARG_class] = fts_s_symbol; /* class name */
      fts_method_define(cl, fts_SystemInlet, fts_s_init, obj_init_vec, 1, a);
      fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

      a[0] = fts_s_ptr;
      fts_method_define(cl, fts_SystemInlet, fts_s_put, dsp_put_log, 1, a);
    }
  
  /* in any case */
  dsp_sig_inlet(cl, INLET_sig);
  dsp_sig_outlet(cl, OUTLET_sig);
  
  return fts_Success;
}

static fts_status_t
siglog10_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];

  fts_class_init(cl, sizeof(obj_t), 1, 1, 0); /* 1 inlet + 1 outlet */
  a[OBJ_ARG_class] = fts_s_symbol; /* class name */
  fts_method_define(cl, fts_SystemInlet, fts_s_init, obj_init_vec, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, dsp_put_log10, 1, a);
  
  /* in any case */

  dsp_sig_inlet(cl, INLET_sig);
  dsp_sig_outlet(cl, OUTLET_sig);
  
  return fts_Success;
}

void veclog_config(void)
{
  fts_metaclass_install(fts_new_symbol("exp~"), sigexp_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("log~"), siglog_instantiate, fts_narg_equiv);
  fts_metaclass_install(fts_new_symbol("log10~"), siglog10_instantiate, fts_narg_equiv);
}
