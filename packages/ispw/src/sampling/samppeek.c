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
#include "fourpoint.h"
#include "sampbuf.h"
#include "sampunit.h"


/******************************************************************
 *
 *    object
 *
 */
 
typedef struct{
  fts_object_t _o;
  fts_symbol_t tab_name;
  fts_symbol_t unit;
  float conv;
} samppeek_t;

static void
samppeek_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samppeek_t *obj = (samppeek_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 2);

  obj->tab_name = name;

  if (!unit)
    unit = samples_unit_get_default();

  obj->conv = samples_unit_convert(unit, 1.0f, fts_dsp_get_sample_rate());
}

/******************************************************************
 *
 *    user methods
 *
 */
 
static void
samppeek_mess_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samppeek_t *obj = (samppeek_t *)o;

  obj->conv = samples_unit_convert(obj->unit, 1.0f, fts_dsp_get_sample_rate());
}

static void
samppeek_float(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  samppeek_t *obj = (samppeek_t *)o;
  float f = fts_get_float(at);
  sampbuf_t *buf = sampbuf_get(obj->tab_name);


  if (buf)
    {
      float onset = obj->conv * f;
      int i_onset = onset;
      float frac = onset - i_onset;

      if (onset < 0 || i_onset >= buf->size)
	fts_outlet_float(o, 0, 0.0f);
      else
	{
	  if(frac == 0.0f)
	    fts_outlet_float(o, 0, buf->samples[i_onset]);
	  else
	    {
	      float *samp = buf->samples + i_onset;
	      float f;
	      
	      if(i_onset > 0)
		fts_fourpoint_interpolate_frac(samp, frac, &f);
	      else
		fts_fourpoint_interpolate_first_frac(samp, frac, &f);
	      
	      fts_outlet_float(o, 0, f);
	    }
	}
    }
  else
    post("samppeek: can't find table~: %s\n", fts_symbol_name(obj->tab_name));
}

static void
samppeek_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a;
  fts_set_float(&a, (float)fts_get_int(at));
  samppeek_float(o, winlet, s, 1, &a);
}

static void
samppeek_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  samppeek_t *obj = (samppeek_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);

  if (sampbuf_get(tab_name))
    obj->tab_name = tab_name;
  else
    post("samppeek: can't find table~: %s\n", fts_symbol_name(tab_name));
}

static void
samppeek_set_by_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  char buf[64];
  fts_atom_t a;

  gensampname(buf, "sample", fts_get_int_arg(ac, at, 0, 0));
  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  samppeek_set(o, winlet, s, 1, &a);
}

/******************************************************************
 *
 *    class
 *
 */
 
static fts_status_t
class_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(samppeek_t), 2, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, samppeek_init, 3, a, 2);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_varargs(cl, 0,  fts_new_symbol("init"), samppeek_mess_init);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, samppeek_int, 1, a);
  
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, samppeek_float, 1, a);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_set, samppeek_set, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, samppeek_set_by_int, 1, a);
  
  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);

  return fts_Success;
}

void
samppeek_config(void)
{
  fts_class_install(fts_new_symbol("samppeek"), class_instantiate);
  fts_class_alias(fts_new_symbol("samppeek~"), fts_new_symbol("samppeek"));
}

