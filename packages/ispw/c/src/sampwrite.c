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
#include "sampwrite.h"


/******************************************************************
 *
 *    object
 *
 */
 
typedef struct
{
  fts_object_t _o;
  fts_symbol_t tab_name;
  ftl_data_t sampwrite_data;
} sampwrite_t;

static void
sampwrite_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;

  if(ac > 0 && fts_is_symbol(at))
    {
      fts_symbol_t tab_name = fts_get_symbol(at);
      sampbuf_t *null = 0;
      long l;
      
      this->tab_name = tab_name;
      this->sampwrite_data = ftl_data_new(sampwrite_ctl_t);
      
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &null);
      
      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);
      
      fts_dsp_add_object(o);
    }
  else
    fts_object_set_error(o, "Name argument required");
}

static void
sampwrite_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;

  ftl_data_free(this->sampwrite_data);
  fts_dsp_remove_object(o);
}

/******************************************************************
 *
 *    dsp
 *
 */
 
static fts_symbol_t dsp_symbol = 0;

static void
sampwrite_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_pointer(at);
  sampbuf_t *buf = sampbuf_get(this->tab_name);

  if (buf)
    {
      long l;

      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &buf);

      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);

      fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
      fts_set_ftl_data(argv + 1, this->sampwrite_data);
      fts_set_int(argv + 2, fts_dsp_get_input_size(dsp, 0));
      fts_dsp_add_function(dsp_symbol, 3, argv);
    }
  else
    post("sampwrite~: %s: can't find table~\n", this->tab_name);
}

/******************************************************************
 *
 *    user methods
 *
 */

static void
sampwrite_bang(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  const long zero = 0;

  ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &zero);
}

static void
sampwrite_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);
  sampbuf_t *buf = sampbuf_get(tab_name);
  
  if (buf)
    {
      long l;

      this->tab_name = tab_name;

      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &buf);

      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);
    }
  else
    post("sampwrite~: %s: can't find table~\n", tab_name);
}

static void
sampwrite_set_by_int(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sampwrite_t *this = (sampwrite_t *)o;
  char name_str[64];
  fts_symbol_t tab_name;
  sampbuf_t *buf;
 
  gensampname(name_str, "sample", fts_get_int(at));
  tab_name = fts_new_symbol_copy(name_str);  
  buf = sampbuf_get(tab_name);
  
  if (buf)
    {
      long l;

      this->tab_name = tab_name;

      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, buf, &buf);

      l = 0x7fffffff;
      ftl_data_set(sampwrite_ctl_t, this->sampwrite_data, onset, &l);
    }
  else
    post("sampwrite~: %s: can't find table~\n", tab_name);
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sampwrite_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sampwrite_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sampwrite_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sampwrite_put);
  
  fts_method_define_varargs(cl, 0, fts_s_set, sampwrite_set);  
  fts_method_define_varargs(cl, 0, fts_s_bang, sampwrite_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, sampwrite_set_by_int);
  
  fts_dsp_declare_inlet(cl, 0);

  return fts_Success;
}

void
sampwrite_config(void)
{
  fts_class_install(fts_new_symbol("sampwrite~"), class_instantiate);
  
  dsp_symbol = fts_new_symbol("sampwrite");
  fts_dsp_declare_function(dsp_symbol, ftl_sampwrite);
}
