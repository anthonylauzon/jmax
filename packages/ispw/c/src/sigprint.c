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


typedef struct sigprint_t {
  fts_object_t _o;
  int n_print;
  fts_symbol_t sym;
  int size;
  int init;
  int alloc;
  int index;
  float *buf;
} sigprint_t;

static void
sigprint_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = (sigprint_t *)o;
  int i;
  
  if( x->sym)
    post("%s:\n", fts_symbol_name(x->sym));

  for (i = 0; i < x->size; i + =4)
    post("%4d: %f %f %f %f\n", i, x->buf[i], x->buf[i+1], x->buf[i+2], x->buf[i+3]);

  post("\n");

  x->n_print--;
}


static void
sigprint_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  x->sym = fts_get_symbol_arg(ac, at, 1, fts_new_symbol(""));
  x->init = fts_get_int_arg(ac, at, 2, 0);
  x->buf = 0; /* will be allocated in _put */
  x->size = 0;
  x->alloc = 0;

  dsp_list_insert(o);
}

static void
sigprint_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  if(x->buf) fts_free(x->buf);

  dsp_list_remove(o);
}

static void
sigprint_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  x->n_print = 1;
}

static void
sigprint_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);

  x->n_print = fts_get_int(at);
}

static void ftl_sigprint(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_pointer(argv);
  sigprint_t *x = (sigprint_t *)fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  int index = x->index;
  int i;

  for(i=0; i<n_tick; i++)
    x->buf[index + i] = in[i];

  index = index + n_tick;

  if(index >= x->size)
    {
      x->index = 0;

      if(x->n_print)
	fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)x, sigprint_post, 0, 0.0);
    }
  else
    x->index = index;
}

static void
sigprint_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigprint_t *x = ((sigprint_t *)o);
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer_arg(ac, at, 0, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  int size = x->init;

  if(size < n_tick)
    size = n_tick;
  
  size += (-size) & (n_tick - 1);
  
  if(size > x->alloc)
    {
      if(x->buf) 
	fts_free(x->buf); /* REalloc */
      x->buf = (float *)fts_zalloc(sizeof(float) * size);
      x->alloc = size;
    }

  x->size = size;
  x->index = 0;
  
  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_pointer(argv + 1, o);
  fts_set_int(argv + 2, n_tick);
  dsp_add_funcall(fts_s_print, 3, argv);
}

static fts_status_t
sigprint_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigprint_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigprint_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigprint_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigprint_put);

  fts_method_define_varargs(cl, 0, fts_s_bang, sigprint_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, sigprint_int);

  dsp_declare_function(fts_s_print, ftl_sigprint);

  dsp_sig_inlet(cl, 0);

  return fts_Success;
}

void
sigprint_config(void)
{
  fts_class_install(fts_new_symbol("print~"),sigprint_instantiate);
}
