/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
  fts_dsp_object_t _o;
  fts_symbol_t tab_name; /* name of table */
  float          max_speed; /* maximum up-shift of pitch */
  fts_symbol_t   unit;
  float          conv; /* cached locally, but stored also in sampread_data */
  ftl_data_t     sampread_data;
} sampread_t;


static void
sampread_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sampread_t *this = (sampread_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 1);
  float max_speed = (unit ? fts_get_float_arg(ac, at, 2, 0.0f): fts_get_float_arg(ac, at, 1, 0.0f));
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

  fts_dsp_object_init((fts_dsp_object_t *)o);
}

static void
sampread_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sampread_t *this = (sampread_t *)o;

  ftl_data_free(this->sampread_data);
  fts_dsp_object_delete((fts_dsp_object_t *)o);
}

/******************************************************************
 *
 *    dsp
 *
 */
 
static fts_symbol_t dsp_symbol = 0;

static void
sampread_put(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sampread_t *this = (sampread_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
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
      fts_dsp_add_function(dsp_symbol, 4, argv);
    }
  else
    fts_post("sampread~: %s: can't find table~\n", this->tab_name);
}

/******************************************************************
 *
 *    user methods
 *
 */
 
static void
sampread_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
    fts_post("sampread~: %s: can't find table~\n", tab_name);
}

static void
sampread_set_by_int(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  sampread_t *this = (sampread_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  sampbuf_t *buf;
 
  gensampname(name_str, "sample", fts_get_int(at));
  tab_name = fts_new_symbol(name_str);  
  buf = sampbuf_get(tab_name);
  
  if(buf)
    {
      this->tab_name = tab_name;
      ftl_data_set(sampread_ctl_t, this->sampread_data, buf, &buf);
    }
  else
    fts_post("sampread~: %s: can't find table~\n", tab_name);
}

static void
sampread_jump(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
 
static void
class_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(sampread_t), sampread_init, sampread_delete);

  fts_class_message_varargs(cl, fts_s_put, sampread_put);
  
  fts_class_inlet_int(cl, 0, sampread_set_by_int);
  fts_class_message_varargs(cl, fts_s_set, sampread_set);
  fts_class_message_varargs(cl, fts_s_jump, sampread_jump);
  
  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_outlet(cl, 0);
  }

void
sampread_config(void)
{
  fts_s_jump = fts_new_symbol("jump");
  
  fts_class_install(fts_new_symbol("sampread~"), class_instantiate);
  
  dsp_symbol = fts_new_symbol("sampread");
  fts_dsp_declare_function(dsp_symbol, ftl_sampread);

}


