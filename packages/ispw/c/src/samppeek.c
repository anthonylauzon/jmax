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
samppeek_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  samppeek_t *obj = (samppeek_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);
  fts_symbol_t unit = samples_unit_get_arg(ac, at, 1);

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
samppeek_mess_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  samppeek_t *obj = (samppeek_t *)o;

  obj->conv = samples_unit_convert(obj->unit, 1.0f, fts_dsp_get_sample_rate());
}

static void
samppeek_float(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
    fts_post("samppeek: can't find table~: %s\n", obj->tab_name);
}

static void
samppeek_int(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fts_atom_t a;
  fts_set_float(&a, (float)fts_get_int(at));
  samppeek_float(o, s, 1, &a, fts_nix);
}

static void
samppeek_set(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  samppeek_t *obj = (samppeek_t *)o;
  fts_symbol_t tab_name = fts_get_symbol_arg(ac, at, 0, 0);

  if (sampbuf_get(tab_name))
    obj->tab_name = tab_name;
  else
    fts_post("samppeek: can't find table~: %s\n", tab_name);
}

static void
samppeek_set_by_int(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  char buf[64];
  fts_atom_t a;

  gensampname(buf, "sample", fts_get_int_arg(ac, at, 0, 0));
  fts_set_symbol(&a, fts_new_symbol(buf));
  samppeek_set(o, s, 1, &a, fts_nix);
}

/******************************************************************
 *
 *    class
 *
 */
 
static void
samppeek_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(samppeek_t), samppeek_init, 0);

  fts_class_message_varargs(cl, fts_s_init, samppeek_mess_init);
  fts_class_message_varargs(cl, fts_s_set, samppeek_set);

  fts_class_inlet_int(cl, 0, samppeek_int);
  fts_class_inlet_float(cl, 0, samppeek_float);

  fts_class_inlet_int(cl, 1, samppeek_set_by_int);

  fts_class_outlet_float(cl, 0);
}

void
samppeek_config(void)
{
  fts_class_t *cl = fts_class_install(fts_new_symbol("samppeek"), samppeek_instantiate);
  fts_class_alias(cl, fts_new_symbol("samppeek~"));
}

