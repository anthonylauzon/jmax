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

typedef struct 
{
  fts_object_t o;
} white_t;

static fts_symbol_t white_ftl_sym = 0;

void
white_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  white_t *this = (white_t *)o;
  fts_dsp_descr_t* dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t a[2];
  
  fts_set_symbol(a + 0, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(a + 1, fts_dsp_get_output_size(dsp, 0));
  
  dsp_add_funcall(white_ftl_sym, 2, a);
}

static void
white_ftl(fts_word_t *argv)
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
white_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  white_t *this = (white_t *)o;

  dsp_list_insert(o);
}

static void
white_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  white_t *this = (white_t *)o;

  dsp_list_remove(o);
}

static fts_status_t
white_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(white_t), 0, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, white_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, white_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, white_put);

  white_ftl_sym = fts_new_symbol("white");
  dsp_declare_function(white_ftl_sym, white_ftl);

  dsp_sig_outlet(cl, 0);

  return fts_Success;
}

void
signal_white_config(void)
{
  fts_metaclass_install(fts_new_symbol("white~"), white_instantiate, fts_always_equiv);
}
