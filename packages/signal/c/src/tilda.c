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
#include <utils/c/include/utils.h>

typedef struct 
{
  fts_object_t o;
  ftl_data_t data;
  float time;
  float cr;
} tilda_t;

static fts_symbol_t sym_tilda_const = 0;
static fts_symbol_t sym_tilda_ramp = 0;

/************************************************************
 *
 *  methods
 *
 */

void
tilda_set_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;
  float *c = (float *)ftl_data_get_ptr(this->data);

  *c = fts_get_number_float(at);
}

void
tilda_set_target(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;
  fts_ramp_t *ramp = (fts_ramp_t *)ftl_data_get_ptr(this->data);
  float value = fts_get_number_float(at);

  fts_ramp_set_target(ramp, value, this->time, this->cr);
}

void
tilda_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;
  float value = fts_get_number_float(at);

  if(value >= 0)
    this->time = value;
  else
    this->time = 0.0;
}

/************************************************************
 *
 *  dsp
 *
 */

void
tilda_put_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  fts_atom_t a[3];

  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(sym_tilda_const, 3, a);
}

void
tilda_put_ramp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;
  fts_ramp_t *ramp = (fts_ramp_t *)ftl_data_get_ptr(this->data);  
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  float cr = fts_dsp_get_output_srate(dsp, 0) / n_tick;
  fts_atom_t a[3];

  fts_ramp_jump(ramp);
  this->cr = cr;
  
  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  fts_dsp_add_function(sym_tilda_ramp, 3, a);
}

static void
tilda_ftl_const(fts_word_t *argv)
{
  float *c = (float *)fts_word_get_pointer(argv + 0);
  float *out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int i;
  
  for(i=0; i<n_tick; i++)
    out[i] = *c;
}

static void
tilda_ftl_ramp(fts_word_t *argv)
{
  fts_ramp_t *ramp = (fts_ramp_t *) fts_word_get_pointer(argv + 0);
  float *out = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int i;
  
  if(fts_ramp_running(ramp))
    {
      double incr = fts_ramp_get_incr(ramp) / n_tick;
      double value = fts_ramp_get_value(ramp);

      for(i=0; i<n_tick; i++)
	{
	  out[i] = value;
	  value += incr;
	}

      fts_ramp_incr(ramp);
    }
  else
    {
      float c = fts_ramp_get_value(ramp);

      for(i=0; i<n_tick; i++)
	out[i] = c;
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
tilda_init_const(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tilda_t *this = (tilda_t *)o;

  fts_dsp_add_object(o);

  this->data = ftl_data_alloc(sizeof(float)); /* just a constant */

  this->time = 0.0;
  this->cr = 1.0;

  if(ac == 1)
    tilda_set_const(o, 0, 0, 1, at);  
  else
    {
      fts_atom_t a;
      
      fts_set_int(&a, 0);
      tilda_set_const(o, 0, 0, 1, &a);
    }
}

static void
tilda_init_ramp(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tilda_t *this = (tilda_t *)o;

  fts_dsp_add_object(o);

  this->data = ftl_data_alloc(sizeof(fts_ramp_t)); /* ramp */

  this->time = 0.0;
  this->cr = 1.0;

  tilda_set_time(o, 0, 0, 1, at + 1);
  tilda_set_target(o, 0, 0, 1, at + 0);
}

static void
tilda_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tilda_t *this = (tilda_t *)o;

  fts_dsp_remove_object(o);

  ftl_data_free(this->data);
}

static fts_status_t
tilda_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 0 || (ac == 1 && fts_is_number(at)))
    {
      /* constant */
      fts_class_init(cl, sizeof(tilda_t), 1, 1, 0); 

      fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, tilda_init_const);
      fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, tilda_delete);

      fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, tilda_put_const);

      fts_method_define_varargs(cl, 0, fts_s_int, tilda_set_const);
      fts_method_define_varargs(cl, 0, fts_s_float, tilda_set_const);
   }
  else  if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      /* slide with ramp */
      fts_class_init(cl, sizeof(tilda_t), 2, 1, 0);

      fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, tilda_init_ramp);
      fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, tilda_delete);

      fts_method_define_varargs(cl, fts_system_inlet, fts_s_put, tilda_put_ramp);

      fts_method_define_varargs(cl, 0, fts_s_int, tilda_set_target);
      fts_method_define_varargs(cl, 0, fts_s_float, tilda_set_target);

      fts_method_define_varargs(cl, 1, fts_s_int, tilda_set_time);
      fts_method_define_varargs(cl, 1, fts_s_float, tilda_set_time);
    }
  else
    return &fts_CannotInstantiate;

  fts_dsp_declare_outlet(cl, 0);

  return fts_ok;
}

void
signal_tilda_config(void)
{
  fts_metaclass_install(fts_new_symbol("~"), tilda_instantiate, fts_arg_type_equiv);

  sym_tilda_const = fts_new_symbol("tilda_const");
  fts_dsp_declare_function(sym_tilda_const, tilda_ftl_const);

  sym_tilda_ramp = fts_new_symbol("tilda_ramp");
  fts_dsp_declare_function(sym_tilda_ramp, tilda_ftl_ramp);
}
