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
 */

#include <fts/fts.h>
#include <ftsconfig.h>
#include "fvec.h"

fts_symbol_t cut_symbol = 0;

typedef struct _cut_ftl_
{
  fts_object_t *object;
  fvec_t *fvec;
  int index;
} cut_ftl_t;


typedef struct _cut_
{
  fts_object_t o;
  ftl_data_t data;
} cut_t;

/***************************************************************************************
 *
 *  user methods
 *
 */

static void cut_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  cut_ftl_t *data = (cut_ftl_t *)ftl_data_get_ptr(this->data);

  fts_outlet_object((fts_object_t *)o, 0, (fts_object_t *)data->fvec);
}

static void 
cut_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  cut_ftl_t *data = (cut_ftl_t *)ftl_data_get_ptr(this->data);

  data->index = 0;
}

static void 
cut_set_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  cut_ftl_t *data = (cut_ftl_t *)ftl_data_get_ptr(this->data);
  fvec_t *fvec = fvec_atom_get(at);

  fts_object_release((fts_object_t *)data->fvec);
  data->fvec = fvec;
  fts_object_refer((fts_object_t *)fvec);

  data->index = fvec_get_size(fvec);
}

/***************************************************************************************
 *
 *  put
 *
 */

static void 
cut_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  float sr = fts_dsp_get_input_srate(dsp, 0);
  int n_tick = fts_dsp_get_input_size(dsp, 0);
  fts_atom_t a[3];
  
  fts_set_ftl_data(a + 0, this->data);
  fts_set_symbol(a + 1, fts_dsp_get_input_name(dsp, 0));
  fts_set_int(a + 2, n_tick);
  fts_dsp_add_function(cut_symbol, 3, a);
}

static void
cut_ftl(fts_word_t *argv)
{
  cut_ftl_t *data = (cut_ftl_t *)fts_word_get_pointer(argv + 0);
  float * restrict in = (float *) fts_word_get_pointer(argv + 1);
  int n_tick = fts_word_get_int(argv + 2);
  float *buf = fvec_get_ptr(data->fvec);
  int size = fvec_get_size(data->fvec);
  int index = data->index;
  int n = size - index;
  int i;

  if(n > n_tick)
    n = n_tick;
  else if(n)
    fts_timebase_add_call(fts_get_timebase(), data->object, cut_output, 0, 0.0);

  for(i=0; i<n; i++)
    buf[index + i] = in[i];

  data->index += n;
}

/***************************************************************************************
 *
 *  class
 *
 */

static void
cut_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  cut_t *this = (cut_t *)o;
  cut_ftl_t *data;
  fvec_t *fvec;

  ac--;
  at++;

  this->data = ftl_data_alloc(sizeof(cut_ftl_t));
  data = ftl_data_get_ptr(this->data);
  
  if(fts_is_int(at))
    data->fvec = (fvec_t *)fts_object_create(fvec_type, 1, at);
  else
    data->fvec = fvec_atom_get(at);

  fts_object_refer((fts_object_t *)data->fvec);

  data->object = o;
  data->index = fvec_get_size(data->fvec);

  fts_dsp_add_object((fts_object_t *)this);
}

static void
cut_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cut_t *this = (cut_t *)o;
  cut_ftl_t *data = ftl_data_get_ptr(this->data);

  if(data->fvec)
    fts_object_release((fts_object_t *)data->fvec);    

  ftl_data_free(this->data);

  fts_dsp_remove_object(o);
}

static fts_status_t
cut_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{  
  if(ac == 1 && fts_is_int(at))
    {
      fts_class_init(cl, sizeof(cut_t), 1, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, cut_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, cut_delete);      

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, cut_put);

      fts_method_define_varargs(cl, 0, fts_s_bang, cut_bang);
    }
  else if(ac == 1 && fvec_atom_is(at))
    {
      fts_class_init(cl, sizeof(cut_t), 2, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, cut_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, cut_delete);      

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, cut_put);

      fts_method_define_varargs(cl, 0, fts_s_bang, cut_bang);
      fts_method_define_varargs(cl, 1, fvec_symbol, cut_set_fvec);
    }
  else
    return &fts_CannotInstantiate;

  fts_dsp_declare_inlet(cl, 0);

  return fts_Success;
}

void
signal_cut_config(void)
{
  cut_symbol = fts_new_symbol("cut~");
  fts_dsp_declare_function(cut_symbol, cut_ftl);  

  fts_metaclass_install(cut_symbol, cut_instantiate, fts_arg_type_equiv);
}
