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
#include <ftsconfig.h>

static fts_symbol_t sig_dsp_function = 0;
static fts_symbol_t sig_64_dsp_function = 0;

typedef struct 
{
  fts_dsp_object_t _o;
  ftl_data_t sig_ftl_data;
} sigobj_t;


static void
sig_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  int down = fts_get_int_arg(ac, at, 1, 0);
  float value = (float) fts_get_float_arg(ac, at, 0, 0.0f);

  this->sig_ftl_data = ftl_data_new(float);
  ftl_data_copy(float, this->sig_ftl_data, &value);

  if (down > 0)
    fts_dsp_object_set_resampling((fts_dsp_object_t *)o, -down);

  fts_dsp_object_init((fts_dsp_object_t *)o); /* just put object in list */
}


static void
sig_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;

  ftl_data_free(this->sig_ftl_data);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

/**********************************************************
 *
 *    dsp
 *
 */

static void ftl_sig(fts_word_t *argv)
{
  float f = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  long int n = fts_word_get_int(argv + 2);
  int i;

  for (i = 0; i < n; i++)
    out[i] = f;
}

static void ftl_sig_64(fts_word_t *argv)
{
  float f = *((float *)fts_word_get_pointer(argv + 0));
  float * restrict out = (float *)fts_word_get_pointer(argv + 1);
  int i;

  for (i = 0; i < 64; i++)
    out[i] = f;
}



static void
sig_put_dsp_function(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  if (fts_dsp_get_output_size(dsp, 0) == 64)
    {
      fts_set_ftl_data(argv, this->sig_ftl_data);
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_dsp_add_function(sig_64_dsp_function, 2, argv);
    }
  else
    {
      fts_set_ftl_data(argv, this->sig_ftl_data);
      fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int  (argv+2, fts_dsp_get_output_size(dsp, 0));
      fts_dsp_add_function(sig_dsp_function, 3, argv);
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

/**********************************************************
 *
 *    class
 *
 */
 
static void
sig_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigobj_t), sig_init, sig_delete);

  fts_class_message_varargs(cl, fts_s_put, sig_put_dsp_function);

  fts_class_inlet_number(cl, 0, sig_number);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);

  sig_dsp_function = fts_new_symbol("sig");
  fts_dsp_declare_function(sig_dsp_function, ftl_sig);

  sig_64_dsp_function = fts_new_symbol("sig64");
  fts_dsp_declare_function(sig_64_dsp_function, ftl_sig_64);
}

void
sig_config(void)
{
  fts_class_install(fts_new_symbol("sig~"),sig_instantiate);
}
