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

typedef struct _point_
{
  double value;
  double time;  
  struct _point_ *next; /* list of break points */
} point_t;

typedef struct 
{
  fts_object_t o;
  point_t *first; /* first break point */
  point_t *last; /* last break point */
  int ready;
} bpf_t;

static fts_symbol_t sym_bpf = 0;

void
bpf_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bpf_t *this = (bpf_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t a[2];
  
  fts_set_symbol(a + 0, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 1, fts_dsp_get_output_size(dsp, 0));
  
  dsp_add_funcall(sym_bpf, 2, a);
}

static void
bpf_ftl(fts_word_t *argv)
{
  float *out = (float *) fts_word_get_ptr(argv + 0);
  int n = fts_word_get_int(argv + 1);
  int i;
  
  for(i=0; i<n; i++)
    out[i] = fts_random_float();
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
}

static void
bpf_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  bpf_t *this = (bpf_t *)o;

  dsp_list_remove(o);
}

static fts_status_t
bpf_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(bpf_t), 0, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, bpf_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, bpf_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, bpf_put);

  dsp_sig_outlet(cl, 0);

  return fts_Success;
}

void
signal_bpf_config(void)
{
  fts_metaclass_install(fts_new_symbol("bpf~"), bpf_instantiate, fts_always_equiv);

  sym_bpf = fts_new_symbol("bpf");
  dsp_declare_function(sym_bpf, bpf_ftl);
}
