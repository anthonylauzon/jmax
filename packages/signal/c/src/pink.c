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
 * Authors: Francois Dechelle, Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <utils.h>

#define M  (unsigned long)714025
#define M2 (M/2)
#define IA (unsigned long)1366
#define IC (unsigned long)150889

typedef struct 
{
  float b0;
  float b1;
  float b2;
  float b3;
  float b4;
  float b5;
  float b6;
} pink_data_t;

typedef struct 
{
  fts_object_t o;
  ftl_data_t data;
} pink_t;

static fts_symbol_t pink_ftl_sym = 0;

void
pink_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  pink_t *this = (pink_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t a[3];
  
  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, fts_dsp_get_output_size(dsp, 0));
  
  dsp_add_funcall(pink_ftl_sym, 3, a);
}

static void
pink_cheap_ftl(fts_word_t *argv)
{
  pink_data_t *data = (pink_data_t *)fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n = fts_word_get_int(argv + 2);
  float b0 = data->b0;
  float b1 = data->b1;
  float b2 = data->b2;
  float white;
  int i;

  for(i=0; i<n; i++)
    {
      white = fts_random_float();

      b0 = 0.99765 * b0 + white * 0.0990460;
      b1 = 0.96300 * b1 + white * 0.2965164;
      b2 = 0.57000 * b2 + white * 1.0526913;
      out[i] = b0 + b1 + b2 + white * 0.1848;
    }

  data->b0 = b0;
  data->b1 = b1;
  data->b2 = b2;
}

static void
pink_ftl(fts_word_t *argv)
{
  pink_data_t *data = (pink_data_t *)fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n = fts_word_get_int(argv + 2);
  float b0 = data->b0;
  float b1 = data->b1;
  float b2 = data->b2;
  float b3 = data->b3;
  float b4 = data->b4;
  float b5 = data->b5;
  float b6 = data->b6;
  float white;
  int i;
  
  for(i=0; i<n; i++)
    {
      white = fts_random_float();

      /* or these?
       * b0 = 0.997 * b0 + 0.029591 * white;
       * b1 = 0.985 * b1 + 0.032534 * white;
       * b2 = 0.950 * b2 + 0.048056 * white;
       * b3 = 0.850 * b3 + 0.090579 * white;
       * b4 = 0.620 * b4 + 0.108990 * white;
       * b5 = 0.250 * b5 + 0.255784 * white;
       * out[i] = b0 + b1 + b2 + b3 + b4 + b5;
       */
  
      /* or these?
       * b0 = 0.99886 * b0 + white * 0.0555179;
       * b1 = 0.99332 * b1 + white * 0.0750759;
       * b2 = 0.96900 * b2 + white * 0.1538520;
       * b3 = 0.86650 * b3 + white * 0.3104856;
       * b4 = 0.55000 * b4 + white * 0.5329522;
       * b5 = -0.7616 * b5 - white * 0.0168980;
       * out[i] = b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362;
       * b6 = white * 0.115926;
       */

      b0 = 0.99886 * b0 + white * 0.0555179;
      b1 = 0.99332 * b1 + white * 0.0750759;
      b2 = 0.96900 * b2 + white * 0.1538520;
      b3 = 0.86650 * b3 + white * 0.3104856;
      b4 = 0.55000 * b4 + white * 0.5329522;
      b5 = -0.7616 * b5 - white * 0.0168980;
      out[i] = 0.1 * (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362);
      b6 = white * 0.115926;
    }

  data->b0 = b0;
  data->b1 = b1;
  data->b2 = b2;
  data->b3 = b3;
  data->b4 = b4;
  data->b5 = b5;
  data->b6 = b6;
}

/************************************************************
 *
 *  class
 *
 */

static void
pink_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  pink_t *this = (pink_t *)o;
  pink_data_t *data;

  dsp_list_insert(o);

  this->data = ftl_data_new(pink_data_t);
  data = (pink_data_t *)ftl_data_get_ptr(this->data);

  data->b0 = 0.0;
  data->b1 = 0.0;
  data->b2 = 0.0;
  data->b3 = 0.0;
  data->b4 = 0.0;
  data->b5 = 0.0;
  data->b6 = 0.0;
}

static void
pink_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  pink_t *this = (pink_t *)o;

  ftl_data_free(this->data);
  dsp_list_remove(o);
}

static fts_status_t
pink_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(pink_t), 0, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, pink_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, pink_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, pink_put);

  pink_ftl_sym = fts_new_symbol("pink");
  dsp_declare_function(pink_ftl_sym, pink_ftl);

  dsp_sig_outlet(cl, 0);

  return fts_Success;
}

void
signal_pink_config(void)
{
  fts_metaclass_install(fts_new_symbol("pink~"), pink_instantiate, fts_always_equiv);
}

