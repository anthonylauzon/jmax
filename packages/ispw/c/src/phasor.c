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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include <fts/fts.h>
#include "phasor.h"

fts_symbol_t phasor_function = 0;
fts_symbol_t phasor_inplace_function = 0;

/******************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t _o;
  ftl_data_t state;
} phasor_t;

static void
phasor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  this->state = phasor_ftl_data_new();

  dsp_list_insert(o);
}

static void
phasor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  ftl_data_free(this->state);
  dsp_list_remove(o);
}

void
phasor_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  double phi = (double)fts_get_number_float(at);

  phasor_ftl_set_phase(this->state, phi);
}

void
phasor_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  phasor_ftl_t *state = ftl_data_get_ptr(this->state);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  fts_atom_t argv[4];
  double incr;

  phasor_ftl_data_init(this->state, sr);

  if (fts_dsp_get_input_name(dsp, 0) == fts_dsp_get_output_name(dsp, 0))
    {
      /* Use the inplace version */

      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data(argv+1, this->state);
      fts_set_int(argv+2, fts_dsp_get_input_size(dsp, 0));

      dsp_add_funcall(phasor_inplace_function, 3, argv);
    }
  else
    {
      /* standard code */
      fts_set_symbol( argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data( argv+2, this->state);
      fts_set_int( argv+3, fts_dsp_get_input_size(dsp, 0));

      dsp_add_funcall(phasor_function, 4, argv);
    }
}

/******************************************************************
 *
 *  class
 *
 */

static fts_status_t
phasor_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(phasor_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, phasor_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, phasor_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, phasor_put);

  fts_method_define_varargs(cl, 0, fts_s_int, phasor_set);
  fts_method_define_varargs(cl, 0, fts_s_float, phasor_set);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  phasor_function = fts_new_symbol("phasor");
  dsp_declare_function(phasor_function, phasor_ftl);

  phasor_inplace_function = fts_new_symbol("phasor_inplace");
  dsp_declare_function(phasor_inplace_function, phasor_ftl_inplace);

  return fts_Success;
}

void
phasor_config(void)
{
  fts_class_install(fts_new_symbol("phasor~"), phasor_instantiate);
}
