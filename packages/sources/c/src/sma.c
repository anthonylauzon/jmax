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


#include "fts.h"

typedef struct
{
  fts_object_t obj;

  ftl_data_t data;
} sma_t;

static fts_symbol_t sma_function = 0;

static void sma_dsp_function( fts_word_t *args)
{
  float *in1 = (float *) fts_word_get_ptr(args);
  float *in2 = (float *) fts_word_get_ptr(args + 1);
  float *out = (float *) fts_word_get_ptr(args + 2);
  float *ps = (float *) fts_word_get_ptr(args + 3);
  long n = fts_word_get_int(args + 4);
  float s;
  long i;

  s = *ps;
  for ( i = 0; i < n; i++)
    out[i] = s * in1[i] + in2[i];
}

static void sma_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sma_t *this = (sma_t *)o;
  float *p;

  /* Allocate ftl data */

  this->data = ftl_data_new(float);

  /* initializing  ftl data */
  p = ftl_data_get_ptr( this->data);

  if (fts_is_int( &(at[1])))
    *p = (float) fts_get_int_arg( ac, at, 1, 0);
  else
    *p = fts_get_float_arg( ac, at, 1, 0.0);

  /* insert object in DSP objects list */

  dsp_list_insert(o);
}

static void sma_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sma_t *this = (sma_t *)o;

  /* Freeing ftl data */

  ftl_data_free( this->data);

  /* remove object from DSP objects list */

  dsp_list_remove(o);
}


static void sma_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t args[5];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg( ac, at, 0, 0);
  sma_t *this = (sma_t *)o;

  fts_set_symbol  (args,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol  (args+1, fts_dsp_get_input_name(dsp, 1));
  fts_set_symbol  (args+2, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data(args+3, this->data);
  fts_set_long    (args+4, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall( sma_function, 5, args);
}

static void sma_set( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sma_t *this = (sma_t *)o;
  float *p;

  p = ftl_data_get_ptr( this->data);

  /* changing ftl data */
  if (fts_is_int( &(at[0])))
    *p = (float) fts_get_int_arg( ac, at, 0, 0);
  else
    *p = fts_get_float_arg( ac, at, 0, 0.0);
}

static fts_status_t sma_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[2];

  post( "Instantiating class `.*+~' of package `sources'\n");

  /* Class initialization : 2 inlets, 1 outlet */

  fts_class_init( cl, sizeof(sma_t), 2, 1, 0);

  /* Definition of DSP specific methods */
  t[0] = fts_t_symbol;
  t[1] = fts_t_float;
  fts_method_define_optargs( cl, fts_SystemInlet, fts_s_init, sma_init, 2, t, 1);

  fts_method_define( cl, fts_SystemInlet, fts_s_delete, sma_delete, 0, 0);

  t[0] = fts_t_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sma_put, 1, t);

  /* Definition of other methods */

  t[0] = fts_t_int;
  fts_method_define(cl, 0, fts_type_get_selector(fts_t_int), sma_set, 1, t);

  t[0] = fts_t_float;
  fts_method_define(cl, 0, fts_type_get_selector(fts_t_float), sma_set, 1, t);


  /* Definition of DSP inlets and outlets */
  dsp_sig_inlet(cl, 0);
  dsp_sig_inlet(cl, 1);
  dsp_sig_outlet(cl, 0);

  /* Declare DSP function and keep the associated symbol */

  sma_function = fts_new_symbol("sma");
  dsp_declare_function( sma_function, sma_dsp_function);
  
  return fts_Success;
}

void sma_config( void)
{
  post( "Installing class `s*+~' of package `sources'\n");

  fts_class_install( fts_new_symbol( ".*+~"), sma_instantiate);
}

