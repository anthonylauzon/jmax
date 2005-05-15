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
#include <fts/packages/utils/utils.h>
#include "wave.h"

struct wave_ftl_symbols wave_ftl_symbols_ptr = {0, 0};
struct wave_ftl_symbols wave_ftl_symbols_fmat = {0, 0};

ftl_data_t 
wave_data_new(void)
{
  ftl_data_t ftl_data = ftl_data_new(wave_data_t);
  wave_data_t *data = (wave_data_t *)ftl_data_get_ptr(ftl_data);

  data->table.ptr = 0;

  return ftl_data;
}

void
wave_data_set_fmat(ftl_data_t ftl_data, fmat_t *fmat)
{
  wave_data_t *data = (wave_data_t *)ftl_data_get_ptr(ftl_data);

  if(data->table.fmat)
    fts_object_release((fts_object_t *)data->table.fmat);
  
  data->table.fmat = fmat;
  
  if(fmat)
    fts_object_refer((fts_object_t *)fmat);
}

void
wave_data_set_ptr(ftl_data_t ftl_data, float *ptr)
{
  wave_data_t *data = (wave_data_t *)ftl_data_get_ptr(ftl_data);

  data->table.ptr = ptr;
}

/***************************************************************************************
 *
 *  user methods
 *
 */

static void 
wave_set_fmat(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  wave_t *this = (wave_t *)o;
  fmat_t *fmat = (fmat_t *)fts_get_object(at);

  /* check float vector size */
  if(fmat_get_m(fmat) < WAVE_TABLE_SIZE + 1)
    fmat_set_m(fmat, WAVE_TABLE_SIZE + 1);

  wave_data_set_fmat(this->data, fmat);
}

/***************************************************************************************
 *
 *  put
 *
 */

static void
wave_put(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  wave_t *this = (wave_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  struct wave_ftl_symbols *sym;  

  if(this->fmat)
    sym = &wave_ftl_symbols_fmat;
  else
    sym = &wave_ftl_symbols_ptr;
  
  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    {
      fts_atom_t a[4];
      
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(a + 2, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 3, n_tick);
      fts_dsp_add_function(sym->outplace, 4, a);
    }
  else /* inplace */
    {
      fts_atom_t a[3];
      
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);
      fts_dsp_add_function(sym->inplace, 3, a);
    }
}

/***************************************************************************************
 *
 *  class
 *
 */

static void
wave_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  wave_t *this = (wave_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->data = wave_data_new();

  if(ac == 0)
    {
      this->fmat = 0;
      wave_data_set_ptr(this->data, fts_fftab_get_cosine(WAVE_TABLE_SIZE));
      fts_object_set_inlets_number(o, 1);
    }
  else if(ac == 1 && fts_is_a(at, fmat_class))
    {
      this->fmat = 1;
      wave_set_fmat(o, 0, 1, at, fts_nix);
    }
  else
    fts_object_error(o, "bad arguments");
}

static void
wave_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  wave_t *this = (wave_t *)o;

  /* release fmat */
  if(this->fmat)
    wave_data_set_fmat(this->data, 0);

  ftl_data_free(this->data);

  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
wave_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(wave_t), wave_init, wave_delete);
      
  fts_class_message_varargs(cl, fts_new_symbol("put"), wave_put);
  
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
    }

void
signal_wave_config(void)
{
  /* ftl wavections using a pointer to float as wave table */
  wave_ftl_symbols_ptr.outplace = fts_new_symbol("wave_ptr_outplace");
  wave_ftl_symbols_ptr.inplace = fts_new_symbol("wave_ptr_inplace");

  /* ftl wavections using a float vector as wave table */
  wave_ftl_symbols_fmat.outplace = fts_new_symbol("wave_fmat_outplace");
  wave_ftl_symbols_fmat.inplace = fts_new_symbol("wave_fmat_inplace");

  /* declare the waveillator related FTL wavections (platform dependent) */
  wave_declare_functions();

  fts_class_install(fts_new_symbol("wave~"), wave_instantiate);
}
