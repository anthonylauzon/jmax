/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

#include "fts.h"


typedef struct {
  fts_object_t o;
} dsp_t;


/* Listener for the property changes */

static void dsp_on_listener(void *listener, fts_symbol_t name,  const fts_atom_t *value)
{
  dsp_t *this = (dsp_t *)listener;

  if (fts_is_int(value))
    fts_outlet_send((fts_object_t *) this, 0, fts_s_int, 1, value);
}



static void
dsp_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 0)
    fts_param_set(fts_s_vector_size, at);

  /* Switch off if already on, before switching on again */

  if (fts_param_get_int(fts_s_dsp_on, 0))
    fts_param_set_int(fts_s_dsp_on, 0);

  fts_param_set_int(fts_s_dsp_on, 1);
}

static void
dsp_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_set_int(fts_s_dsp_on, 0);
}

static void
dsp_on_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_get_long_arg(ac, at, 0, 0))
    fts_param_set_int(fts_s_dsp_on, 1);
  else
    fts_param_set_int(fts_s_dsp_on, 0);
}

static void
dsp_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}

static void
dsp_save(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *f;
  
  if (fts_is_symbol(at))
    {
      const char *filename;

      filename = fts_symbol_name(fts_get_symbol(at));

      f = fopen(filename, "w");

      if (f)
	dsp_chain_fprint(f);

      fclose(f);
    }
}

static void
dsp_print_signals(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post_signals();
}


static void
dsp_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_add_listener(fts_s_dsp_on, o, dsp_on_listener);
}

static void
dsp_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_remove_listener(o);
}


static fts_status_t
dsp_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(dsp_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, dsp_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, dsp_delete, 0, 0);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("start"), dsp_start, 1, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("stop"), dsp_stop, 0, a);
 
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, dsp_on_off, 1, a);
  
  fts_method_define(cl, 0, fts_s_bang, dsp_print, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("print"), dsp_print, 0, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("save"), dsp_save, 1, a);

  fts_method_define(cl, 0, fts_new_symbol("print-signals"), dsp_print_signals, 0, 0);
  
  return fts_Success;
}

void
dsp_config(void)
{
  fts_class_install(fts_new_symbol("dsp"),dsp_instantiate);
  fts_class_alias(fts_new_symbol("dsp~"), fts_new_symbol("dsp"));
}

