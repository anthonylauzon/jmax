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

#include "fts.h"

typedef struct clip_t {
  fts_object_t _o;
  long i_min;
  long i_max;
  float f_min;
  float f_max;
} clip_t;


static void
clip_set_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *x = (clip_t *)o;

  if(fts_is_long(at))
    {
      x->i_min = fts_get_long(at);
      x->f_min = (float)x->i_min;
    }
  else if(fts_is_float(at))
    {
      x->f_min = fts_get_float(at);
      x->i_min = (long)x->f_min;
    }
}

static void
clip_set_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *x = (clip_t *)o;

  if(fts_is_long(at))
    {
      x->i_max = fts_get_long(at);
      x->f_max = (float)x->i_max;
    }
  else if(fts_is_float(at))
    {
      x->f_max = fts_get_float(at);
      x->i_max = (long)x->f_max;
    }
}

static void
clip_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0)
    clip_set_min(o, 0, 0, 1, at);
  if(ac > 1)
    clip_set_max(o, 0, 0, 1, at + 1);
}

static void
clip_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *x = (clip_t *)o;
  
  x->i_min = 0;
  x->i_max = 0;
  x->f_min = 0.;
  x->f_max = 0.;
  clip_set(o, winlet, s, ac-1, at+1);
}

static void
clip_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *x = (clip_t *)o;
  float f = fts_get_float(at);

  if (f > x->f_max)
    f = x->f_max;
  else if (f < x->f_min)
    f = x->f_min;

  fts_outlet_float(o, 0, f);
}

static void
clip_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  long n = fts_get_long(at);
  clip_t *x = (clip_t *)o;

  if (n > x->i_max)
    n = x->i_max;
  else if (n < x->i_min)
    n = x->i_min;

  fts_outlet_int(o, 0, n);
}

static void
clip_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t temp[256];		/* stack-crushing */
  register int i;
  register fts_atom_t *ap;
  clip_t *x = (clip_t *)o;

  if (ac > 256)
    ac = 256;
	
  ap = temp;

  for (i = 0; i < ac; i++, at++, ap++)
    {
      if (fts_is_long(at))
	{
	  if (fts_get_long(at) > x->i_max)
	    fts_set_long(ap, x->i_max);
	  else if (fts_get_long(at) < x->i_min)
	    fts_set_long(ap, x->i_min);
	  else
	    *ap = *at;
	}
      else if (fts_is_float(at))
	{
	  if (fts_get_float(at) > x->i_max)
	    fts_set_float(ap, x->f_max);
	  else if (fts_get_float(at) < x->i_min)
	    fts_set_float(ap, x->f_min);
	  else
	    *ap = *at;
	}
      else
	fts_set_long(ap, 0L);
    }

  fts_outlet_send(o, 0, s, ac, temp);
}

static fts_status_t
clip_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[12];

  fts_class_init(cl, sizeof(clip_t), 3, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, clip_init);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, clip_float, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, clip_int, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_set, clip_set);
  fts_method_define_varargs(cl, 0, fts_s_list, clip_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, clip_set_min, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, clip_set_min, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, clip_set_max, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, clip_set_max, 1, a);

  return fts_Success;
}

void
clip_config(void)
{
  fts_class_install(fts_new_symbol("clip"),clip_instantiate);
}
