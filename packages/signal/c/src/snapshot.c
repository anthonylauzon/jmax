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

static fts_symbol_t sym_snapshot_tilda = 0;

typedef struct
{
  fts_dsp_object_t o;
  ftl_data_t val;
} snapshot_t;

static void
ftl_snapshot(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv + 0);
  float *x = (float *)fts_word_get_pointer(argv + 1);
  int n =fts_word_get_int(argv + 2);

  *x = in[n - 1];
}


static void
snapshot_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  snapshot_t *this = (snapshot_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  fts_atom_t argv[3];

  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data(argv + 1, this->val);
  fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
  fts_dsp_add_function(sym_snapshot_tilda, 3, argv);
}

static void
snapshot_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  snapshot_t *this = (snapshot_t *)o;
  float *val;

  this->val = ftl_data_new(float);
  val = (float *)ftl_data_get_ptr(this->val);

  *val = 0.0;

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
snapshot_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  snapshot_t *this = (snapshot_t *)o;
  float *val = (float *)ftl_data_get_ptr(this->val);

  *val = fts_get_number_float(at);
}

static void
snapshot_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  snapshot_t *this = (snapshot_t *)o;
  float *val = (float *)ftl_data_get_ptr(this->val);

  fts_outlet_float(o, 0, *val);
}

static void
snapshot_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
snapshot_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(snapshot_t), snapshot_init, snapshot_delete);

  fts_class_message_varargs(cl, fts_s_put, snapshot_put);
  
  fts_class_message_varargs(cl, fts_s_bang, snapshot_bang);

  fts_dsp_declare_inlet(cl, 0);

  fts_class_inlet_number(cl, 0, snapshot_number);
  
  fts_class_outlet_float(cl, 0);
}

void
signal_snapshot_config(void)
{
  sym_snapshot_tilda = fts_new_symbol("snapshot~");
  fts_dsp_declare_function(sym_snapshot_tilda, ftl_snapshot);

  fts_class_install(sym_snapshot_tilda, snapshot_instantiate);
}

