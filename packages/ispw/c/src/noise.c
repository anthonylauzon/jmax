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
#include <utils.h>

typedef struct noise_t {
  fts_object_t _o;
} noise_t;

static fts_symbol_t noise_dsp_symbol = 0;

static void
noise_dsp_function(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_ptr(argv);
  long int n = fts_word_get_int(argv+1);
  int i;

  for(i=0; i<n; i++)
    out[i] = fts_random_float();
}

static void
noise_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noise_t *this = (noise_t *) o;
  fts_atom_t argv[2];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol (argv,   fts_dsp_get_output_name(dsp, 0));
  fts_set_int   (argv+1, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(noise_dsp_symbol, 2, argv);
}

static void
noise_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noise_t *this = (noise_t *) o;

  dsp_list_insert(o); /* just put object in list */
}

static void
noise_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  noise_t *this = (noise_t *) o;

  dsp_list_remove(o);
}

static fts_status_t
noise_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(noise_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, noise_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, noise_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, noise_put);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  noise_dsp_symbol = fts_new_symbol("noise");
  dsp_declare_function(noise_dsp_symbol, noise_dsp_function);

  return fts_Success;
}

void
noise_config(void)
{
  fts_class_install(fts_new_symbol("noise~"), noise_instantiate);
}