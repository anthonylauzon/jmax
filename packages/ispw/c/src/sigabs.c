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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Zack Settel.
 *
 */

#include <fts/fts.h>



typedef struct 
{
  fts_object_t obj;

} sigabs_t;

static fts_symbol_t sigabs_function = 0;

static void ftl_sigabs(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  long n = fts_word_get_long(argv + 2);
  int i;

  for (i = 0; i < n; i ++)
    {
      float f;

      f = in[i];

      out[i] = (f >= 0 ? f : (- f));
    }
}


static void
sigabs_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int  (argv + 2, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(sigabs_function, 3, argv);
}


static void
sigabs_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o); 
}


static void
sigabs_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}


static fts_status_t
sigabs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(sigabs_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigabs_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigabs_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigabs_put);

  /* Type the outlet */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  sigabs_function = fts_new_symbol("sigabs");
  dsp_declare_function(sigabs_function, ftl_sigabs);

  return fts_Success;
}

void
sigabs_config(void)
{
  fts_class_install(fts_new_symbol("abs~"),sigabs_instantiate);
}






