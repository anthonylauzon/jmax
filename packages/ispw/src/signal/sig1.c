/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"




/* Don't call thjios tyep sig_t, it conflict with system
   types in a lot of systems
*/

static fts_symbol_t sig_dsp_function = 0;
static fts_symbol_t sig_64_dsp_function = 0;

#define CLASS_NAME "sig~"

/**********************************************************
 *
 *    object
 *
 */
 
typedef struct 
{
  fts_object_t _o;

  ftl_data_t sig_ftl_data;
} sigobj_t;


static void
sig_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  int down = fts_get_int_arg(ac, at, 2, 0);
  float value = (float) fts_get_float_arg(ac, at, 1, 0.0f);

  this->sig_ftl_data = ftl_data_new(float);
  ftl_data_copy(float, this->sig_ftl_data, &value);

  if (down > 0)
    {
      fts_atom_t a;

      fts_set_long(&a, down);
      fts_object_put_prop(o, fts_s_dsp_downsampling, &a);
    }

  dsp_list_insert(o); /* just put object in list */
}


static void
sig_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;

  ftl_data_free(this->sig_ftl_data);
  dsp_list_remove(o);
}

/**********************************************************
 *
 *    dsp
 *
 */

static void ftl_sig(fts_word_t *argv)
{
  float f = *((float *)fts_word_get_ptr(argv + 0));
  float * restrict out = (float *)fts_word_get_ptr(argv + 1);
  long int n = fts_word_get_long(argv + 2);
  int i;

  for (i = 0; i < n; i++)
    out[i] = f;
}

static void ftl_sig_64(fts_word_t *argv)
{
  float f = *((float *)fts_word_get_ptr(argv + 0));
  float * restrict out = (float *)fts_word_get_ptr(argv + 1);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = f;
}



static void
sig_put_dsp_function(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  if (fts_dsp_get_output_size(dsp, 0) == 64)
    {
      fts_set_ftl_data(argv, this->sig_ftl_data);
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      dsp_add_funcall(sig_64_dsp_function, 2, argv);
    }
  else
    {
      fts_set_ftl_data(argv, this->sig_ftl_data);
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_long  (argv+2, fts_dsp_get_output_size(dsp, 0));
      dsp_add_funcall(sig_dsp_function, 3, argv);
    }

}

/**********************************************************
 *
 *    user methods
 *
 */
 
static void
sig_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  float value = (float) fts_get_number_float(at);

  ftl_data_copy(float, this->sig_ftl_data, &value);
}

static void
sig_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* 0.26 compatibility ... */
}

/**********************************************************
 *
 *    class
 *
 */
 
static fts_status_t
sig_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigobj_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sig_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sig_delete, 0, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sig_put_dsp_function, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sig_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sig_number, 1, a);

  fts_method_define(cl, 0, fts_s_bang, sig_bang, 0, 0);


  /* definir les inlets et outlets sig */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  sig_dsp_function = fts_new_symbol("sig");
  dsp_declare_function(sig_dsp_function, ftl_sig);

  sig_64_dsp_function = fts_new_symbol("sig64");
  dsp_declare_function(sig_64_dsp_function, ftl_sig_64);

  return fts_Success;
}

void
sig_config(void)
{
  fts_class_install(fts_new_symbol(CLASS_NAME),sig_instantiate);
}
