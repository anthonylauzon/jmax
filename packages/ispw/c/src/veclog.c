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

#include <math.h>
#include <fts/fts.h>

static fts_symbol_t exp_symbol;
static fts_symbol_t log_symbol;
static fts_symbol_t log10_symbol;
static fts_symbol_t expb_symbol;
static fts_symbol_t logb_symbol;

typedef struct
{
  fts_object_t head;
  float scl;
} veclog_t;

/************************************************
 *
 *    dsp
 *
 */

void 
ftl_exp(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float *out = (float *)fts_word_get_ptr(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = exp(in[i]); 
} 

void 
ftl_log(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float *out = (float *)fts_word_get_ptr(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = log(in[i]); 
} 

void 
ftl_log10(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float *out = (float *)fts_word_get_ptr(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = log10(in[i]); 
} 

void 
ftl_expb(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float scl = *((float *)fts_word_get_ptr(argv + 1 )); 
  float *out = (float *)fts_word_get_ptr(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = exp(scl * in[i]);
} 

void 
ftl_logb(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float scl = *((float *)fts_word_get_ptr(argv + 1 )); 
  float *out = (float *)fts_word_get_ptr(argv + 2); 
  int size = fts_word_get_int(argv + 3); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = scl * log(in[i]);
} 

static void
veclog_put(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t name)
{
  veclog_t *this = (veclog_t *)o;
  fts_atom_t argv[4];

  fts_set_symbol(argv + 0,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(name, 3, argv);
}

static void
veclog_put_scl(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t name)
{
  veclog_t *this = (veclog_t *)o;
  fts_atom_t argv[4];

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr(argv + 1, &this->scl);
  fts_set_symbol(argv + 2, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(argv + 3, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(name, 4, argv);
}

static void 
exp_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_put(o, (fts_dsp_descr_t *)fts_get_ptr(at), exp_symbol);
}

static void 
log_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_put(o, (fts_dsp_descr_t *)fts_get_ptr(at), log_symbol);
}

static void 
log10_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_put(o, (fts_dsp_descr_t *)fts_get_ptr(at), log10_symbol);
}

static void 
expb_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_put_scl(o, (fts_dsp_descr_t *)fts_get_ptr(at), expb_symbol);
}

static void 
logb_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_put_scl(o, (fts_dsp_descr_t *)fts_get_ptr(at), logb_symbol);
}

/************************************************
 *
 *  user methods
 *
 */
static void
veclog_set_base(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_t *obj = (veclog_t *)o;
  float f = fts_get_number_float(at);

  if(f > 0.0)
    obj->scl = log(f);
  else
    obj->scl = 1.0;
}

static void
siglog_set_base(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_t *obj = (veclog_t *)o;
  float f = fts_get_number_float(at);

  if(f > 0.0)
    obj->scl = 1.0 / log(f);
  else
    obj->scl = 1.0;
}

/************************************************
 *
 *    class
 *
 */

static void
veclog_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_t *obj = (veclog_t *)o;

  ac--;
  at++;

  if(ac > 0 && fts_is_number(at))
    veclog_set_base(o, 0, 0, 1, at);
  else
    obj->scl = 1.0;
  
  dsp_list_insert(o);  
}

static void
siglog_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  veclog_t *obj = (veclog_t *)o;

  ac--;
  at++;
 
  if(ac > 0 && fts_is_number(at))
    siglog_set_base(o, 0, 0, 1, at);
  else
    obj->scl = 1.0;
}

static void
veclog_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

static fts_status_t
veclog_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  if (ac > 0)
    {
      fts_class_init(cl, sizeof(veclog_t), 2, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, veclog_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, veclog_delete);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, expb_put);

      fts_method_define_varargs(cl, 1, fts_s_int, veclog_set_base);
      fts_method_define_varargs(cl, 1, fts_s_float, veclog_set_base);
    }
  else
    {
      fts_class_init(cl, sizeof(veclog_t), 1, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, veclog_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, veclog_delete);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, exp_put);
    }
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}


static fts_status_t
siglog_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

  if (ac > 0)
    {
      fts_class_init(cl, sizeof(veclog_t), 2, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, siglog_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, veclog_delete);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, logb_put);

      fts_method_define_varargs(cl, 1, fts_s_int, siglog_set_base);
      fts_method_define_varargs(cl, 1, fts_s_float, siglog_set_base);
    }
  else
    {
      fts_class_init(cl, sizeof(veclog_t), 1, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, veclog_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, veclog_delete);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, log_put);
    }
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

static fts_status_t
siglog10_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(veclog_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, veclog_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, veclog_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, log10_put);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void 
veclog_config(void)
{
  exp_symbol = fts_new_symbol("exp~");
  log_symbol = fts_new_symbol("log~");
  log10_symbol = fts_new_symbol("log10~");

  expb_symbol = fts_new_symbol("exp~ <base>");
  logb_symbol = fts_new_symbol("log~ <base>");

  fts_dsp_declare_function(exp_symbol, ftl_exp);
  fts_dsp_declare_function(log_symbol, ftl_log);
  fts_dsp_declare_function(log10_symbol, ftl_log10);

  fts_dsp_declare_function(expb_symbol, ftl_expb);
  fts_dsp_declare_function(logb_symbol, ftl_logb);

  fts_metaclass_install(exp_symbol, veclog_instantiate, fts_narg_equiv);
  fts_metaclass_install(log_symbol, siglog_instantiate, fts_narg_equiv);
  fts_class_install(log10_symbol, siglog10_instantiate);
}