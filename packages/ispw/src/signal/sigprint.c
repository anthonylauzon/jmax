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


typedef struct sigprint_t {
  fts_object_t _o;
  int n_print;
  fts_symbol_t sym;
  long size;
  long init;
  long alloc;
  long index;
  float *buf;
  fts_alarm_t alarm;
} sigprint_t;

static fts_symbol_t print_dsp_function = 0;

static void
sigprint_tick(fts_alarm_t *alarm, void *o)
{
  sigprint_t *x = ((sigprint_t *)o);

  if(x->sym)
    post("%s:\n", fts_symbol_name(x->sym));

  post_vector(x->size, x->buf);

  x->n_print--;
  fts_alarm_unarm(alarm);
}


static void
sigprint_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  x->sym = fts_get_symbol_arg(ac, at, 1, fts_new_symbol(""));
  x->init = fts_get_long_arg(ac, at, 2, 0);
  x->buf = 0; /* will be allocated in _put */
  x->size = 0;
  x->alloc = 0;

  fts_alarm_init(&(x->alarm), fts_new_symbol("tick"), sigprint_tick, (void *)o);
  dsp_list_insert(o);
}

static void
sigprint_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  if(x->buf) fts_free(x->buf);

  fts_alarm_unarm(&(x->alarm));
  dsp_list_remove(o);
}

static void
sigprint_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  x->n_print = 1;
}

static void
sigprint_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  x->n_print = fts_get_long(at);
}

static void ftl_sigprint(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv);
  sigprint_t *x = (sigprint_t *)fts_word_get_ptr(argv + 1);
  long n_tick = fts_word_get_long(argv + 2);
  long index = x->index;

  fts_vecx_fcpy(in, x->buf + index, n_tick);
  index = index + n_tick;

  if(index >= x->size)
    {
      x->index = 0;

      if(x->n_print)
	{
	  fts_alarm_set_delay(&(x->alarm), 0.1f);
	  fts_alarm_arm(&(x->alarm));
	}
    }
  else
    x->index = index;
}

static void
sigprint_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  long size = x->init;

  if(size < n_tick)
    size = n_tick;
  
  size += (-size) & (n_tick - 1);
  
  if(size > x->alloc)
    {
      if(x->buf) 
	fts_free(x->buf); /* REalloc */
      x->buf = (float *)fts_zalloc(sizeof(float) * size);
      x->alloc = size;
    }

  x->size = size;
  x->index = 0;
  
  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr(argv + 1, o);
  fts_set_long(argv + 2, n_tick);
  dsp_add_funcall(print_dsp_function, 3, argv);
}

static fts_status_t
sigprint_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigprint_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigprint_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigprint_delete, 0, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigprint_put, 1, a);

  fts_method_define(cl, 0, fts_s_bang, sigprint_bang, 0, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigprint_int, 1, a);

  print_dsp_function = fts_new_symbol("print");
  dsp_declare_function(print_dsp_function, ftl_sigprint);

  dsp_sig_inlet(cl, 0);
  return fts_Success;
}

void
sigprint_config(void)
{
  fts_class_install(fts_new_symbol("print~"),sigprint_instantiate);
}
