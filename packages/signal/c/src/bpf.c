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

#include "fts.h"

typedef struct _bp_
{
  fts_object_t o;
  double time;
  double value;
  struct _bp_ *next; /* list of break points */
} bp_t;

typedef struct _bpf_
{
  fts_object_t o;
  ftl_data_t data;
  bp_t *first;
  bp_t *last;
  float sr;
} bpf_t;

static fts_symbol_t sym_bpf = 0;

static void
bpf_append_point(bpt_t *this, )
{

}

/************************************************************
 *
 *  methods
 *
 */

void
bpf_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  fts_ramp_t *ramp = (fts_ramp_t *)ftl_data_get_ptr(this->data);
  float value = fts_get_number_float(at);

  fts_ramp_set_target(ramp, value, this->time, this->sr);
}

void
bpf_set_function(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  float *c = (float *)ftl_data_get_ptr(this->data);

  *c = fts_get_number_float(at);
}

void
bpf_set_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
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
bpf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  int n_tick = fts_dsp_get_output_size(dsp, 0);
  float sr = fts_dsp_get_output_srate(dsp, 0);
  fts_atom_t a[3];

  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  
  dsp_add_funcall(sym_bpf, 3, a);
}

static void
bpf_ftl(fts_word_t *argv)
{
  fts_ramp_t *ramp = (fts_ramp_t *) fts_word_get_ptr(argv + 0);
  float *out = (float *) fts_word_get_ptr(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int i;
  
  for(i=0; i<n_tick; i++)
    {
      fts_ramp_incr(ramp);
      out[i] = fts_ramp_get_value(ramp);
    }
}

/************************************************************
 *
 *  class
 *
 */

static void
bpf_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  dsp_list_insert(o);

  this->data = ftl_data_alloc(sizeof(float)); /* just a constant */

  this->time = 0.0;
  this->sr = 1.0;

  if(ac == 2)
    bpf_set_const(o, 0, 0, 1, at + 1);  
  else
    {
      fts_atom_t a[1];
      
      fts_set_int(a, 0);
      bpf_set_const(o, 0, 0, 1, a);
    }
}

static void
bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  dsp_list_remove(o);

  ftl_data_free(this->data);
}

static fts_status_t
bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1 || (ac == 2 && fts_is_number(at + 1)))
    {
      /* constant */
      fts_class_init(cl, sizeof(bpf_t), 1, 1, 0); 

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bpf_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bpf_delete);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, bpf_put);

      fts_method_define_varargs(cl, 0, fts_s_int, bpf_set);
      fts_method_define_varargs(cl, 0, fts_s_float, bpf_set);

      dsp_sig_outlet(cl, 0);
      
      return fts_Success;
   }
  else
    return &fts_CannotInstantiate;
}

void
signal_bpf_config(void)
{
  fts_metaclass_install(fts_new_symbol("bpf~"), bpf_instantiate, fts_arg_type_equiv);

  sym_bpf = fts_new_symbol("bpf");
  dsp_declare_function(sym_bpf, bpf_ftl_const);
}
