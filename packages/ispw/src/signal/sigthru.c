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



typedef struct sigthrough
{
  fts_object_t  o;
} sigthru_t;

static void
sigthru_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  fts_outlet_send(o, 1, s, ac, av); 
}

static void
sigthru_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  dsp_list_insert(o);
}

static void
sigthru_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  dsp_list_remove(o);
}

static void
sigthru_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    {
      fts_set_symbol( argv,   fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_long( argv+2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
    }
}

static fts_status_t
sigthru_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  /* initialize the class */

  fts_class_init(cl, sizeof(sigthru_t), 1, 2, 0);

  /* define the init system method */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigthru_init, 1, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigthru_put, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigthru_delete, 0, 0);

  /* define the application  methods */

  fts_method_define_varargs(cl, 0, fts_s_anything, sigthru_anything);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  return fts_Success;
}

void
sigthru_config(void)
{
  fts_metaclass_create(fts_new_symbol("thru~"), sigthru_instantiate, fts_always_equiv);
}
