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
#include "sampbuf.h"
#include "sampwrite.h"


/******************************************************************
 *
 *    object
 *
 */
 
typedef struct
{
  fts_object_t _o;

  fts_symbol_t   tab_name;

  ftl_data_t     sampwrite_data;
} sampwrite_t;

static void
sampwrite_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 1, 0);
  sampbuf_t *null = 0;
  long l;

  this->tab_name = tab_name;
  this->sampwrite_data = ftl_data_new(sampwrite_ctl_t);

  ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &null);

  l = 0x7fffffff;
  ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);

  dsp_list_insert(o);
}

static void
sampwrite_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;

  ftl_data_free(this->sampwrite_data);
  dsp_list_remove(o);
}

/******************************************************************
 *
 *    dsp
 *
 */
 
static fts_symbol_t dsp_symbol = 0;

static void
sampwrite_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      long l;

      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &buf);

      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);

      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data(argv + 1, this->sampwrite_data);
      fts_set_long(argv + 2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(dsp_symbol, 3, argv);
    }
  else
    post("sampwrite~: %s: can't find table~\n", fts_symbol_name(this->tab_name));
}

/******************************************************************
 *
 *    user methods
 *
 */

static void
sampwrite_bang(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  const long zero = 0;

  ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &zero);
}

static void
sampwrite_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(tab_name);
  
  if (buf)
    {
      long l;

      this->tab_name = tab_name;

      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &buf);

      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);
    }
  else
    post("sampwrite~: %s: can't find table~\n", fts_symbol_name(tab_name));
}

static void
sampwrite_set_by_int(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  sampbuf_t *buf;
 
  gensampname(name_str, "sample", fts_get_long(at));
  tab_name = fts_new_symbol_copy(name_str);  
  buf = sampbuf_get(tab_name);
  
  if (buf)
    {
      long l;

      this->tab_name = tab_name;

      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &buf);

      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);
    }
  else
    post("sampwrite~: %s: can't find table~\n", fts_symbol_name(tab_name));
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sampwrite_t), 1, 0, 0);

  a[0] = fts_s_symbol; /* class */
  a[1] = fts_s_symbol; /* name of tab_name */
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sampwrite_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sampwrite_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sampwrite_put, 1, a);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_set, sampwrite_set, 1, a);
  
  fts_method_define(cl, 0, fts_s_bang, sampwrite_bang, 0, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sampwrite_set_by_int, 1, a);
  
  dsp_sig_inlet(cl, 0);

  /* DSP properties  */

  /* fts_class_put_prop(cl, fts_s_dsp_is_sink, fts_true); */
  
  return fts_Success;
}

void
sampwrite_config(void)
{
  fts_class_install(fts_new_symbol("sampwrite~"), class_instantiate);
  
  dsp_symbol = fts_new_symbol("sampwrite");
  dsp_declare_function(dsp_symbol, ftl_sampwrite);
}

