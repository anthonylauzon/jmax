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

static fts_symbol_t sym_abs_tilda = 0;

static void
abs_put(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));

  fts_dsp_add_function(sym_abs_tilda, 3, argv);
}

static void 
ftl_abs(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  float *out = (float *)fts_word_get_pointer(argv + 1);
  long n = fts_word_get_int(argv + 2);
  int i;

  for(i=0; i<n; i++)
    {
      float f = in[i];

      out[i] = (f >= 0 ? f : (- f));
    }
}

static void
abs_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_object_init((fts_dsp_object_t *)o); 
}


static void
abs_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
abs_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_dsp_object_t), abs_init, abs_delete);

  fts_class_message_varargs(cl, fts_s_put, abs_put);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
}

void
signal_abs_config(void)
{
  sym_abs_tilda = fts_new_symbol("abs~");
  fts_dsp_declare_function(sym_abs_tilda, ftl_abs);

  fts_class_install(sym_abs_tilda, abs_instantiate);
}






