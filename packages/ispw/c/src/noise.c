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
#include <utils/c/include/utils.h>

typedef struct
{
  fts_dsp_object_t _o;
} noise_t;

static fts_symbol_t noise_dsp_symbol = 0;

static void
noise_dsp_function(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_pointer(argv);
  long int n = fts_word_get_int(argv+1);
  int i;

  for(i=0; i<n; i++)
    out[i] = fts_random_float();
}

static void
noise_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[2];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  fts_set_symbol (argv,   fts_dsp_get_output_name(dsp, 0));
  fts_set_int   (argv+1, fts_dsp_get_input_size(dsp, 0));
  fts_dsp_add_function(noise_dsp_symbol, 2, argv);
}

static void
noise_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_init((fts_dsp_object_t *)o); /* just put object in list */
}

static void
noise_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
noise_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(noise_t), noise_init, noise_delete);

  fts_class_message_varargs(cl, fts_s_put, noise_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void
noise_config(void)
{
  noise_dsp_symbol = fts_new_symbol("noise");
  fts_dsp_declare_function(noise_dsp_symbol, noise_dsp_function);

  fts_class_install(fts_new_symbol("noise~"), noise_instantiate);
}
