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
 */


/* sigsamplerate.c */

#include <fts/fts.h>


typedef struct
{
  fts_dsp_object_t _o;
  float c_rate;
  int c_vecsize;
  int c_isvecsize;
} sigparam_t;

static void
sigparam_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);

  x->c_rate = fts_dsp_get_input_srate(dsp, 0);
  x->c_vecsize = fts_dsp_get_input_size(dsp, 0);
}

static void
sigparam_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;

  if (x->c_vecsize <= 0)
    post("samplerate~/vecsize~: not available until DSP started\n");
  else if (x->c_isvecsize)
    fts_outlet_int(o, 0, x->c_vecsize);
  else
    fts_outlet_float(o, 0, x->c_rate);
}

static void
sigsamplerate_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;

  x->c_vecsize   = -1;
  x->c_isvecsize = 0;

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
sigvectorsize_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigparam_t *x = (sigparam_t *)o;

  x->c_vecsize   = -1;
  x->c_isvecsize = 1;

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
sigparam_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

static void
sigsamplerate_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigparam_t), sigsamplerate_init, sigparam_delete);

  fts_class_message_varargs(cl, fts_s_put, sigparam_put);
  
  fts_class_inlet_bang(cl, 0, sigparam_bang);

  fts_dsp_declare_inlet(cl, 0);  
  fts_class_outlet_number(cl, 0);
}

static void
sigvectorsize_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sigparam_t), sigvectorsize_init, sigparam_delete);

  fts_class_message_varargs(cl, fts_s_put, sigparam_put);

  fts_class_inlet_bang(cl, 0, sigparam_bang);

  fts_dsp_declare_inlet(cl, 0);
  fts_class_outlet_number(cl, 0);
}


void
sigparam_config(void)
{
  fts_class_install(fts_new_symbol("samplerate~"),sigsamplerate_instantiate);
  fts_class_install(fts_new_symbol("vectorsize~"),sigvectorsize_instantiate);
}
