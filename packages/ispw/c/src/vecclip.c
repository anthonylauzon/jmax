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

#include <fts/fts.h>

typedef struct
{
  fts_object_t _o;

  ftl_data_t ftl_data_min;
  ftl_data_t ftl_data_max;
} sigclip_t;

static fts_symbol_t sym_clip = 0;

/************************************************
 *
 *    dsp
 *
 */

static void
ftl_clip(fts_word_t *argv)
{
  float min = *((float *)fts_word_get_ptr(argv + 0));
  float max = *((float *)fts_word_get_ptr(argv + 1));
  float *in = (float *)fts_word_get_ptr(argv + 2);
  float *out = (float *)fts_word_get_ptr(argv + 3);
  int n_tick = fts_word_get_int(argv + 4);
  int i;

  for(i=0; i<n_tick; i++)
    {
      float f = in[i];

      if(f <= min)
	out[i] = min;
      else if(f >= max)
	out[i] = max;
      else
	out[i] = f;
    }
}

static void
sigclip_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr(at);
  fts_atom_t argv[5];

  fts_set_ftl_data(argv + 0, this->ftl_data_min);
  fts_set_ftl_data(argv + 1, this->ftl_data_max);
  fts_set_symbol(argv + 2, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 3, fts_dsp_get_output_name(dsp, 0));
  fts_set_int(argv + 4, fts_dsp_get_input_size(dsp, 0));
  fts_dsp_add_function(sym_clip, 5, argv);
}

/************************************************
 *
 *    user methods
 *
 */
 
static void
sigclip_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  float *min = ftl_data_get_ptr(this->ftl_data_min);

  *min = fts_get_number_float(at);
}


static void
sigclip_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  float *max = ftl_data_get_ptr(this->ftl_data_max);

  *max = fts_get_number_float(at);
}

/************************************************
 *
 *    class
 *
 */

static void
sigclip_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;
  float *min;
  float *max;

  ac--;
  at++;

  this->ftl_data_min = ftl_data_alloc(sizeof(float));
  this->ftl_data_max = ftl_data_alloc(sizeof(float));

  min = (float *)ftl_data_get_ptr(this->ftl_data_min);
  max = (float *)ftl_data_get_ptr(this->ftl_data_max);

  if(ac > 0)
    {
      *min = fts_get_number_float(at);
      ac--;
      at++;
    }
  else
    *min = -1.0;

  if(ac > 0)
    *max = fts_get_number_float(at);
  else
    *max = 1.0;

  fts_dsp_add_object(o);
}


static void
sigclip_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigclip_t *this = (sigclip_t *)o;

  ftl_data_free(this->ftl_data_min);
  ftl_data_free(this->ftl_data_max);

  fts_dsp_remove_object(o);
}

static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigclip_t), 3, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sigclip_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sigclip_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigclip_put);
  
  fts_method_define_varargs(cl, 1, fts_s_int, sigclip_min);
  fts_method_define_varargs(cl, 1, fts_s_float, sigclip_min);

  fts_method_define_varargs(cl, 2, fts_s_int, sigclip_max);
  fts_method_define_varargs(cl, 2, fts_s_float, sigclip_max);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
vecclip_config(void)
{
  sym_clip = fts_new_symbol("clip~");

  fts_class_install(sym_clip, class_instantiate);
  fts_dsp_declare_function(sym_clip, ftl_clip);
}