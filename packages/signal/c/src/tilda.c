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
 * Authors: Norbert Schnell
 *
 */

#include <fts/fts.h>
#include <fts/packages/utils/utils.h>

typedef struct 
{
  fts_dsp_object_t o;
  ftl_data_t data;
  float time;
  float cr;
  int ramp;
} tilda_t;

static fts_symbol_t sym_tilda_scalar = 0;
static fts_symbol_t sym_tilda_ramp = 0;

/************************************************************
 *
 *  dsp
 *
 */

static void
tilda_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;
  fts_ramp_t *ramp = (fts_ramp_t *)ftl_data_get_ptr(this->data);  
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  float cr = fts_dsp_get_output_srate(dsp, 0) / n_tick;
  fts_atom_t a[3];

  if(this->ramp)
    {
      fts_ramp_jump(ramp);
      this->cr = cr;
      
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);
      fts_dsp_add_function(sym_tilda_ramp, 3, a);
    }
  else
    {
      fts_set_ftl_data(a + 0, this->data);
      fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_int(a + 2, n_tick);
      fts_dsp_add_function(sym_tilda_scalar, 3, a);
    }
}

static void
tilda_ftl_scalar(fts_word_t *argv)
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
 *  methods
 *
 */

static void
tilda_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  tilda_t *this = (tilda_t *)o;

  if(this->ramp)
    {
      fts_ramp_t *ramp = (fts_ramp_t *)ftl_data_get_ptr(this->data);
      float value = fts_get_number_float(at);
      
      fts_ramp_set_target(ramp, value, this->time, this->cr);
    }
  else
    {
      float *c = (float *)ftl_data_get_ptr(this->data);
      
      *c = fts_get_number_float(at);
    }
}

static void
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
 *  class
 *
 */

static void
tilda_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tilda_t *this = (tilda_t *)o;

  fts_dsp_object_init((fts_dsp_object_t *)o);

  this->time = 0.0;
  this->cr = 1.0;

  if(ac == 0 || (ac == 1 && fts_is_number(at)))
    {
      /* scalar version */
      this->ramp = 0;
      this->data = ftl_data_alloc(sizeof(float));

      if(ac == 0)
	{
	  fts_atom_t a;
	  
	  fts_set_int(&a, 0);
	  tilda_set_value(o, 0, 0, 1, &a);
	}
      else
	tilda_set_value(o, 0, 0, 1, at);  
      
      fts_object_set_inlets_number(o, 1);
    }
  else if(ac == 2 && fts_is_number(at) && fts_is_number(at + 1))
    {
      /* ramp version */
      this->ramp = 1;
      this->data = ftl_data_alloc(sizeof(fts_ramp_t));

      tilda_set_value(o, 0, 0, 1, at + 0);
      tilda_set_time(o, 0, 0, 1, at + 1);
    }
  else
    fts_object_error(o, "bad arguments");
}

static void
tilda_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  tilda_t *this = (tilda_t *)o;

  fts_dsp_object_delete((fts_dsp_object_t *)o);

  ftl_data_free(this->data);
}

static void
tilda_instantiate(fts_class_t *cl)
{
  /* slide with ramp */
  fts_class_init(cl, sizeof(tilda_t), tilda_init, tilda_delete);
  
  fts_class_message_varargs(cl, fts_s_put, tilda_put);
  
  fts_class_inlet_int(cl, 0, tilda_set_value);
  fts_class_inlet_float(cl, 0, tilda_set_value);
  
  fts_class_inlet_int(cl, 1, tilda_set_time);
  fts_class_inlet_float(cl, 1, tilda_set_time);

  fts_dsp_declare_outlet(cl, 0);
}

void
signal_tilda_config(void)
{
  sym_tilda_scalar = fts_new_symbol("~ scalar");
  sym_tilda_ramp = fts_new_symbol("~ ramp");

  fts_dsp_declare_function(sym_tilda_scalar, tilda_ftl_scalar);
  fts_dsp_declare_function(sym_tilda_ramp, tilda_ftl_ramp);

  fts_class_install(fts_new_symbol("~"), tilda_instantiate);

}
