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
 */

#include <fts/fts.h>
#include <math.h>

static fts_symbol_t sin_symbol;
static fts_symbol_t cos_symbol;
static fts_symbol_t tan_symbol;

static fts_symbol_t asin_symbol;
static fts_symbol_t acos_symbol;
static fts_symbol_t atan_symbol;

static fts_symbol_t sinh_symbol;
static fts_symbol_t cosh_symbol;
static fts_symbol_t tanh_symbol;

typedef struct
{
  fts_object_t head;
} vectrigon_t;

/************************************************
 *
 *  dsp
 *
 */

static void
ftl_sin (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = sin(in[i]); 
} 

static void
ftl_cos (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = cos(in[i]); 
} 

static void
ftl_tan (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = tan(in[i]); 
} 

static void
ftl_asin (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = asin(in[i]); 
} 

static void
ftl_acos (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = acos(in[i]); 
} 

static void
ftl_atan (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = atan(in[i]); 
} 

static void
ftl_sinh (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = sinh(in[i]); 
} 

static void
ftl_cosh (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = cosh(in[i]); 
} 

static void
ftl_tanh (fts_word_t *argv ) 
{ 
  float *in = (float *)fts_word_get_pointer(argv + 0); 
  float *out = (float *)fts_word_get_pointer(argv + 1); 
  int size = fts_word_get_int(argv + 2); 
  int i; 

  for(i=0; i<size; i++)
    out[i] = tanh(in[i]); 
} 

static void
vectrigon_put(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t name)
{
  fts_atom_t argv[3];

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int  (argv + 2, fts_dsp_get_input_size(dsp, 0));
  fts_dsp_add_function(name, 3, argv);
}

static void 
sin_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sin_symbol);
}

static void 
cos_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), cos_symbol);
}

static void 
tan_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), tan_symbol);
}

static void 
asin_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), asin_symbol);
}

static void 
acos_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), acos_symbol);
}

static void 
atan_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), atan_symbol);
}

static void 
sinh_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), sinh_symbol);
}

static void 
cosh_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), cosh_symbol);
}

static void 
tanh_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vectrigon_put(o, (fts_dsp_descr_t *)fts_get_pointer(at), tanh_symbol);
}

/************************************************
 *
 *    class
 *
 */

static void
vectrigon_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_add_object(o);
}

static void
vectrigon_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_remove_object(o);
}

static fts_status_t
vectrigon_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t put_method)
{
  fts_class_init(cl, sizeof(vectrigon_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, vectrigon_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, vectrigon_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, put_method);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  
  return fts_Success;
}

static fts_status_t
sin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, sin_put);
}

static fts_status_t
cos_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, cos_put);
}

static fts_status_t
tan_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, tan_put);
}

static fts_status_t
asin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, asin_put);
}

static fts_status_t
acos_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, acos_put);
}

static fts_status_t
atan_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, atan_put);
}

static fts_status_t
sinh_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, sinh_put);
}

static fts_status_t
cosh_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, cosh_put);
}

static fts_status_t
tanh_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return vectrigon_instantiate(cl, ac, at, tanh_put);
}

void
vectrigon_config(void)
{
  sin_symbol = fts_new_symbol("sin~");
  cos_symbol = fts_new_symbol("cos~");
  tan_symbol = fts_new_symbol("tan~");

  asin_symbol = fts_new_symbol("asin~");
  acos_symbol = fts_new_symbol("acos~");
  atan_symbol = fts_new_symbol("atan~");

  sinh_symbol = fts_new_symbol("sinh~");
  cosh_symbol = fts_new_symbol("cosh~");
  tanh_symbol = fts_new_symbol("tanh~");

  fts_dsp_declare_function(sin_symbol, ftl_sin);
  fts_dsp_declare_function(cos_symbol, ftl_cos);
  fts_dsp_declare_function(tan_symbol, ftl_tan);

  fts_dsp_declare_function(asin_symbol, ftl_asin);
  fts_dsp_declare_function(acos_symbol, ftl_acos);
  fts_dsp_declare_function(atan_symbol, ftl_atan);

  fts_dsp_declare_function(sinh_symbol, ftl_sinh);
  fts_dsp_declare_function(cosh_symbol, ftl_cosh);
  fts_dsp_declare_function(tanh_symbol, ftl_tanh);

  fts_class_install(sin_symbol, sin_instantiate);
  fts_class_install(cos_symbol, cos_instantiate);
  fts_class_install(tan_symbol, tan_instantiate);

  fts_class_install(asin_symbol, asin_instantiate);
  fts_class_install(acos_symbol, acos_instantiate);
  fts_class_install(atan_symbol, atan_instantiate);

  fts_class_install(sinh_symbol, sinh_instantiate);
  fts_class_install(cosh_symbol, cosh_instantiate);
  fts_class_install(tanh_symbol, tanh_instantiate);
}
