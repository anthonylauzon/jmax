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
#include "wave.h"

struct wave_ftl_symbols wave_ftl_symbols_ptr = {0, 0};
struct wave_ftl_symbols wave_ftl_symbols_fvec = {0, 0};

/***************************************************************************
 *
 *  FTL data
 *
 */

ftl_data_t 
wave_data_new(void)
{
  ftl_data_t ftl_data = ftl_data_new(wave_data_t);
  wave_data_t *data = (wave_data_t *)ftl_data_get_ptr(ftl_data);

  data->table.ptr = 0;

  return ftl_data;
}

void
wave_data_set_fvec(ftl_data_t ftl_data, fvec_t *fvec)
{
  wave_data_t *data = (wave_data_t *)ftl_data_get_ptr(ftl_data);

  if(data->table.fvec)
    fts_object_release((fts_object_t *)data->table.fvec);
  
  data->table.fvec = fvec;
  
  if(fvec)
    fts_object_refer((fts_object_t *)fvec);
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
wave_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wave_t *this = (wave_t *)o;
  fvec_t *fvec = fvec_atom_get(at);

  /* check float vector size */
  if(fvec_get_size(fvec) < WAVE_TABLE_SIZE + 1)
    fvec_set_size(fvec, WAVE_TABLE_SIZE + 1);

  wave_data_set_fvec(this->data, fvec);
}

/***************************************************************************************
 *
 *  put
 *
 */

static void
wave_put(wave_t *this, fts_dsp_descr_t *dsp, struct wave_ftl_symbols *sym)
{
  float sr = fts_dsp_get_output_srate(dsp, 0);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[3];
  
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

static void 
wave_put_cosine(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wave_t *this = (wave_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  wave_put(this, dsp, &wave_ftl_symbols_ptr);
}

static void 
wave_put_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wave_t *this = (wave_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  wave_put(this, dsp, &wave_ftl_symbols_fvec);
}

/***************************************************************************************
 *
 *  init
 *
 */

static void
wave_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wave_t *this = (wave_t *)o;

  fts_dsp_add_object((fts_object_t *)this);

  this->data = wave_data_new();
}

static void
wave_init_cosine(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wave_t *this = (wave_t *)o;

  wave_init(o, 0, 0, 0, 0);

  wave_data_set_ptr(this->data, fts_fftab_get_cosine(WAVE_TABLE_SIZE));
}

static void
wave_init_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wave_t *this = (wave_t *)o;

  wave_init(o, 0, 0, 0, 0);

  wave_set_fvec(o, 0, 0, 1, at + 1);
}

/***************************************************************************************
 *
 *  delete
 *
 */

static void
wave_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wave_t *this = (wave_t *)o;

  ftl_data_free(this->data);

  fts_dsp_remove_object(o);
}

static void
wave_delete_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wave_t *this = (wave_t *)o;

  /* release fvec */
  wave_data_set_fvec(this->data, 0);

  ftl_data_free(this->data);

  fts_dsp_remove_object(o);
}

/***************************************************************************************
 *
 *  instantiate
 *
 */

static fts_status_t
wave_instantiate_cosine(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(wave_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, wave_init_cosine);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, wave_delete);
      
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), wave_put_cosine);
  
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
    
  return fts_Success;
}

static fts_status_t
wave_instantiate_fvec(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(wave_t), 2, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, wave_init_fvec);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, wave_delete_fvec);
      
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("put"), wave_put_fvec);
  
  fts_method_define_varargs(cl, 1, fvec_symbol, wave_set_fvec);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
    
  return fts_Success;
}

static fts_status_t
wave_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 0)
    return wave_instantiate_cosine(cl, ac, at);
  else if (ac == 1 && fvec_atom_is(at))
    return wave_instantiate_fvec(cl, ac, at);
  else
    return &fts_CannotInstantiate;
  }

void
signal_wave_config(void)
{
  /* ftl wavections using a pointer to float as wave table */
  wave_ftl_symbols_ptr.outplace = fts_new_symbol("wave_ptr_outplace");
  wave_ftl_symbols_ptr.inplace = fts_new_symbol("wave_ptr_inplace");

  /* ftl wavections using a float vector as wave table */
  wave_ftl_symbols_fvec.outplace = fts_new_symbol("wave_fvec_outplace");
  wave_ftl_symbols_fvec.inplace = fts_new_symbol("wave_fvec_inplace");

  /* declare the waveillator related FTL wavections (platform dependent) */
  wave_declare_functions();

  fts_metaclass_install(fts_new_symbol("wave~"), wave_instantiate, fts_arg_type_equiv);
}
