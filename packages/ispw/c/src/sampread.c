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
#include "sampbuf.h"
#include "sampread.h"
#include "sampunit.h"


static fts_symbol_t fts_s_jump;


/******************************************************************
 *
 *    object
 *
 */

typedef struct
{
  fts_object_t _o;

  fts_symbol_t tab_name; /* name of table */
  float          max_speed; /* maximum up-shift of pitch */
  fts_symbol_t   unit;
  float          conv; /* cached locally, but stored also in sampread_data */
  ftl_data_t     sampread_data;
} sampread_t;


static void
sampread_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampread_t *this = (sampread_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 2);
  float max_speed = (unit ? fts_get_float_arg(ac, at, 3, 0.0f): fts_get_float_arg(ac, at, 2, 0.0f));
  sampbuf_t *null = 0;
  float zero = 0.0f;

  this->tab_name = tab_name;

  if (! unit)
    unit = samples_unit_get_default();
  
  this->max_speed = (max_speed > 1.0f ? max_speed : 4.0f);
  this->unit = unit;

  this->sampread_data = ftl_data_new(sampread_ctl_t);

  ftl_data_set(sampread_ctl_t, this->sampread_data, buf, &null);
  ftl_data_set(sampread_ctl_t, this->sampread_data, last_in, &zero);
  ftl_data_set(sampread_ctl_t, this->sampread_data, conv,    &zero);

  dsp_list_insert(o);
}

static void
sampread_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampread_t *this = (sampread_t *)o;

  ftl_data_free(this->sampread_data);
  dsp_list_remove(o);
}

/******************************************************************
 *
 *    dsp
 *
 */
 
static fts_symbol_t dsp_symbol = 0;

static void
sampread_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampread_t *this = (sampread_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer_arg(ac, at, 0, 0);
  fts_atom_t argv[4];
  float sr = fts_dsp_get_input_srate(dsp, 0);
  long n_tick = fts_dsp_get_input_size(dsp, 0);
  sampbuf_t *buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      float f;

      f = this->max_speed * n_tick;
      ftl_data_set(sampread_ctl_t, this->sampread_data, max_extent, &f);

      f = 1.0f / n_tick;
      ftl_data_set(sampread_ctl_t, this->sampread_data, inv_n, &f);

      ftl_data_set(sampread_ctl_t, this->sampread_data, buf, &buf);

      f = samples_unit_convert(this->unit, 1.0f, sr);
      this->conv = f;
      ftl_data_set(sampread_ctl_t, this->sampread_data, conv, &f);

      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
      fts_set_ftl_data(argv + 2, this->sampread_data);
      fts_set_int(argv + 3, n_tick);
      dsp_add_funcall(dsp_symbol, 4, argv);
    }
  else
    post("sampread~: %s: can't find table~\n", fts_symbol_name(this->tab_name));
}

/******************************************************************
 *
 *    user methods
 *
 */
 
static void
sampread_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampread_t *this = (sampread_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(tab_name);

  if (buf)
    {
      this->tab_name = tab_name;
      ftl_data_set(sampread_ctl_t, this->sampread_data, buf, &buf);
    }
  else
    post("sampread~: %s: can't find table~\n", fts_symbol_name(tab_name));
}

static void
sampread_set_by_int(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampread_t *this = (sampread_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  sampbuf_t *buf;
 
  gensampname(name_str, "sample", fts_get_int(at));
  tab_name = fts_new_symbol_copy(name_str);  
  buf = sampbuf_get(tab_name);
  
  if(buf)
    {
      this->tab_name = tab_name;
      ftl_data_set(sampread_ctl_t, this->sampread_data, buf, &buf);
    }
  else
    post("sampread~: %s: can't find table~\n", fts_symbol_name(tab_name));
}

static void
sampread_jump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampread_t *this = (sampread_t *)o;
  float f;

  f = fts_get_float_arg(ac, at, 0, 0.0f) * this->conv;

  ftl_data_set(sampread_ctl_t, this->sampread_data, last_in, &f);
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sampread_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sampread_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sampread_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sampread_put);
  
  fts_method_define_varargs(cl, 0, fts_s_int, sampread_set_by_int);
  fts_method_define_varargs(cl, 0, fts_s_set, sampread_set);
  fts_method_define_varargs(cl, 0, fts_s_jump, sampread_jump);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  return fts_Success;
}

void
sampread_config(void)
{
  fts_s_jump = fts_new_symbol("jump");
  
  fts_class_install(fts_new_symbol("sampread~"), class_instantiate);
  
  dsp_symbol = fts_new_symbol("sampread");
  dsp_declare_function(dsp_symbol, ftl_sampread);

}


