/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
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
  fts_dsp_object_t head;
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
  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
vectrigon_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
vectrigon_instantiate(fts_class_t *cl, fts_method_t put_method)
{
  fts_class_init(cl, sizeof(vectrigon_t), vectrigon_init, vectrigon_delete);

  fts_class_message_varargs(cl, fts_s_put, put_method);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

static void
sin_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, sin_put);
}

static void
cos_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, cos_put);
}

static void
tan_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, tan_put);
}

static void
asin_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, asin_put);
}

static void
acos_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, acos_put);
}

static void
atan_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, atan_put);
}

static void
sinh_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, sinh_put);
}

static void
cosh_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, cosh_put);
}

static void
tanh_instantiate(fts_class_t *cl)
{
  vectrigon_instantiate(cl, tanh_put);
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
