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

#include <fts/fts.h>

static fts_symbol_t sqrt_symbol;
static fts_symbol_t rsqrt_symbol;

typedef struct
{
  fts_object_t head;
} vecsqrt_t;

/************************************************
 *
 *    dsp
 *
 */

void 
ftl_sqrt(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float *out = (float *)fts_word_get_ptr(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = sqrt(in [i]);
} 

void 
ftl_rsqrt(fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_ptr(argv + 0); 
  float *out = (float *)fts_word_get_ptr(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = 1. / sqrt(in [i]);
} 

static void 
sqrt_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecsqrt_t *obj = (vecsqrt_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t argv[3];

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(sqrt_symbol, 3, argv);
}

static void 
rsqrt_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vecsqrt_t *obj = (vecsqrt_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t argv[3];

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(rsqrt_symbol, 3, argv);
}

/************************************************
 *
 *    class
 *
 */

static void 
vecsqrt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o);
}

static void 
vecsqrt_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

static fts_status_t
sqrt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(vecsqrt_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, vecsqrt_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, vecsqrt_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sqrt_put);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

static fts_status_t
rsqrt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(vecsqrt_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, vecsqrt_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, vecsqrt_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, rsqrt_put);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
vecsqrt_config(void)
{
  sqrt_symbol = fts_new_symbol("sqrt~");
  rsqrt_symbol = fts_new_symbol("rsqrt~");
  
  fts_dsp_declare_function(sqrt_symbol, ftl_sqrt);
  fts_dsp_declare_function(rsqrt_symbol, ftl_rsqrt);  

  fts_class_install(sqrt_symbol, sqrt_instantiate);
  fts_class_install(rsqrt_symbol, rsqrt_instantiate);
}
