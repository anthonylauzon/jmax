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


static fts_symbol_t sigsnapshot_function = 0;

typedef struct
{
  fts_object_t _o;
  float val;
} sigsnapshot_t;


static void
sigsnapshot_dsp(fts_word_t *argv)
{
  float *in = (float *) fts_word_get_ptr(argv + 0);
  float *x  = (float *) fts_word_get_ptr(argv + 1);
  long int n =  fts_word_get_long(argv+2);

  *x = in[n - 1];
}


static void
sigsnapshot_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsnapshot_t *this = (sigsnapshot_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr   (argv+1, &(this->val));
  fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(sigsnapshot_function, 3, argv);
}

static void
sigsnapshot_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsnapshot_t *this = (sigsnapshot_t *)o;

  this->val = 0;

  dsp_list_insert(o);
}

static void
sigsnapshot_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsnapshot_t *this = (sigsnapshot_t *)o;

  this->val = fts_get_number_float(at);
}

static void
sigsnapshot_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigsnapshot_t *this = (sigsnapshot_t *)o;

  fts_outlet_float(o, 0, this->val);
}


static void
sigsnapshot_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

static fts_status_t
sigsnapshot_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(sigsnapshot_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigsnapshot_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigsnapshot_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigsnapshot_put, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigsnapshot_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sigsnapshot_number, 1, a);

  fts_method_define(cl, 0, fts_s_bang, sigsnapshot_bang, 0, a);
  
  dsp_sig_inlet(cl, 0);

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);

  sigsnapshot_function = fts_new_symbol("snapshot");
  dsp_declare_function(sigsnapshot_function, sigsnapshot_dsp);

  return fts_Success;
}

void
sigsnapshot_config(void)
{
  fts_class_install(fts_new_symbol("snapshot~"),sigsnapshot_instantiate);
}

