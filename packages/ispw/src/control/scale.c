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


/* class scale */

#include <math.h>
#include "fts.h"

typedef struct scale_t
{
  fts_object_t _o;

  double ilow;
  double ihi;
  double olow;
  double ohi;
  double logincr;

  double scaler;
  double zval;
  double ioffset;
  double irange;

  int  logf;
  int  floatf;
} scale_t;

static void
do_rescale(scale_t *x)
{
  double inputrange = x->ihi - x->ilow;
  double outputrange = x->ohi - x->olow;
  
  if (inputrange == 0)		/* no 0s allowed - n/0  */
    x->scaler = x->zval = 0.0f;
  else
    {
      x->scaler = outputrange / inputrange;
      x->irange= inputrange;
      x->ioffset = (x->ilow<x->ihi) ? x->ilow:x->ihi;

      if (inputrange < 0)
	inputrange = inputrange * -1.0f;

      x->zval = outputrange * exp(- (inputrange * x->logincr));
    }
}

static void
scale_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;
  int  i;

  ac--; at++;			/* throw away class name argument */

  if (ac > 0)
    {
      for (i = 0; i < ac; i++)
	if (fts_is_float(&at[i]))
	  x->floatf = 1;
    }
  else
    x->floatf = 1;

  if (ac == 5)
    x->logf = 1;
  else
    x->logf = 0;

  x->ilow = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  x->ihi  = (double) fts_get_float_arg(ac, at, 1, 127.0f);
  x->olow = (double) fts_get_float_arg(ac, at, 2, 0.0f);
  x->ohi  = (double) fts_get_float_arg(ac, at, 3, 1.0f);

  if (x->logf)
    {
      double f;

      f = (double) fts_get_float_arg(ac, at, 4, 1.0f);
  
      if (f < (double)1.0)
	f = (double) 1.0;

      x->logincr = log(f);
    }

  do_rescale(x);
}


static void
scale_set_ilow(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;

  x->ilow = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}


static void
scale_set_ihi(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;

  x->ihi = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}


static void
scale_set_olow(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;

  x->olow = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}


static void
scale_set_ohi(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;

  x->ohi = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}

static void
scale_set_logincr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;
  double f;

  x->logf = 1;

  f = (double) fts_get_float_arg(ac, at, 0, 1.0f);
  
  if (f < (double)1.0)
    f = (double) 1.0;

  x->logincr = log(f);

  do_rescale(x);
}

static void
scale_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  scale_t *x = (scale_t *)o;
  float f = fts_get_float_arg(ac, at, 0, 0.0f);

  if (x->logf)
    {
      f = (x->ioffset<0) ? f - x->ioffset : f + x->ioffset;	/* inverted?? */

      if (x->irange<0)
	f = -x->irange - f;

      f = (f== 0) ? 0.0f : x->zval * exp(f * x->logincr);
    }
  else 
    {
      f -= x->ilow;
      f *= x->scaler;
    }

  f += x->olow;

  if (x->floatf)
    fts_outlet_float(o, 0, f);
  else
    fts_outlet_int(o, 0, (long)f);
}

static fts_status_t
scale_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[6];
  int i, z, floatf = 0;

  ac--;at++;			/* throw away the class name argument */
  
  fts_class_init(cl, sizeof(scale_t), 6, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  a[3] = fts_s_number;
  a[4] = fts_s_number;
  a[5] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, scale_init, 6, a, 1);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, scale_input, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, scale_input, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, scale_set_ilow, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, scale_set_ilow, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, scale_set_ihi, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, scale_set_ihi, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 3, fts_s_int, scale_set_olow, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 3, fts_s_float, scale_set_olow, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 4, fts_s_int, scale_set_ohi, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 4, fts_s_float, scale_set_ohi, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 5, fts_s_int, scale_set_logincr, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 5, fts_s_float, scale_set_logincr, 1, a);


  if (ac > 0)
    {
      for(z = 0; ac > 0; z++, ac--, at++)
	if (fts_is_float(at))
	  floatf = 1;
    }
  else
    floatf = 1;

	
  if (floatf)
    {
      a[0] = fts_s_float;
      fts_outlet_type_define(cl, 0, fts_s_float, 1, a);
    }
  else
    {
      a[0] = fts_s_int;
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
    }

  return fts_Success;
}

void
scale_config(void)
{
  fts_metaclass_install(fts_new_symbol("scale"), scale_instantiate, fts_arg_type_equiv);
}
