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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include <fts/fts.h>

#include "iir.h"

static fts_symbol_t iir_dsp_function[5] = {0, 0, 0, 0, 0};

/****************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t head;
  ftl_data_t state;
  ftl_data_t coefs;  
  long n_order;
} iir_t;

static void
iir_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;

  if(ac > 1 && ac <= 5)
    {
      float *coefs;
      long n_order;
      long i;
      
      n_order = ac - 1;
      
      this->n_order = n_order;
      this->coefs = ftl_data_alloc(sizeof(float) * n_order);
      this->state = ftl_data_alloc(sizeof(float) * n_order);
      
      coefs = (float *)ftl_data_get_ptr(this->coefs);

      for(i=0; i<n_order; i++)
	{
	  if(fts_is_number(at + i))
	    coefs[i] = fts_get_float_arg(ac, at, i + 1, 0.0f);
	  else
	    coefs[i] = 0.0;
	}
  
      dsp_list_insert(o);
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}


static void
iir_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;

  if(this->n_order > 0)
    {
      ftl_data_free(this->coefs);
      ftl_data_free(this->state);
    }

  dsp_list_remove(o);
}

static void
iir_clear(fts_object_t *o, int inlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  float *state = (float *)ftl_data_get_ptr(this->state);
  int i;

  for(i=0; i<this->n_order; i++)
    state[i] = 0.0;
}

/****************************************
 *
 *   dsp
 *
 */

static void
iir_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  fts_atom_t argv[6];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer_arg(ac, at, 0, 0);

  iir_clear(o, 0, 0, 0, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(argv + 2, this->state);
  fts_set_ftl_data(argv + 3, this->coefs);
  fts_set_int(argv + 4, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(iir_dsp_function[this->n_order], 5, argv);
}


/****************************************
 *
 *   user methods
 *
 */

static void
iir_set_coef(fts_object_t *o, int i, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  iir_t *this = (iir_t *)o;
  float *coefs = (float *)ftl_data_get_ptr(this->coefs);

  coefs[i-1] = fts_get_float_arg(ac, at, 0, 0.0f);
}

/****************************************
 *
 *   class
 *
 */

static fts_status_t
iir_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int i;
 
  fts_class_init(cl, sizeof(iir_t), ac + 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, iir_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, iir_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, iir_put);
  
  for(i=1; i<ac+1; i++)
    {
      fts_method_define_varargs(cl, i, fts_s_int, iir_set_coef);
      fts_method_define_varargs(cl, i, fts_s_float, iir_set_coef);
    }

  fts_method_define_varargs(cl, 0, fts_s_clear, iir_clear);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
iir_config(void)
{ 
  iir_dsp_function[1] = fts_new_symbol("iir_1");
  iir_dsp_function[2] = fts_new_symbol("iir_2");
  iir_dsp_function[3] = fts_new_symbol("iir_3");
  iir_dsp_function[4] = fts_new_symbol("iir_4");

  dsp_declare_function(iir_dsp_function[1], ftl_iir_1);
  dsp_declare_function(iir_dsp_function[2], ftl_iir_2);
  dsp_declare_function(iir_dsp_function[3], ftl_iir_3);
  dsp_declare_function(iir_dsp_function[4], ftl_iir_4);

  fts_metaclass_install(fts_new_symbol("iir~"), iir_instantiate, fts_narg_equiv);
}
