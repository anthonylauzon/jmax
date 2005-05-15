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


/* class scale */

#include <math.h>
#include <fts/fts.h>

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
scale_set_ilow(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scale_t *x = (scale_t *)o;

  x->ilow = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}


static void
scale_set_ihi(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scale_t *x = (scale_t *)o;

  x->ihi = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}


static void
scale_set_olow(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scale_t *x = (scale_t *)o;

  x->olow = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}


static void
scale_set_ohi(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scale_t *x = (scale_t *)o;

  x->ohi = (double) fts_get_float_arg(ac, at, 0, 0.0f);
  do_rescale(x);
}

static void
scale_set_logincr(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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
scale_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
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

static void
scale_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  scale_t *x = (scale_t *)o;
  int  i;

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
scale_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(scale_t), scale_init, 0);

  fts_class_inlet_number(cl, 0, scale_input);
  fts_class_inlet_number(cl, 1, scale_set_ilow);
  fts_class_inlet_number(cl, 2, scale_set_ihi);
  fts_class_inlet_number(cl, 3, scale_set_olow);
  fts_class_inlet_number(cl, 4, scale_set_ohi);
  fts_class_inlet_number(cl, 5, scale_set_logincr);

  fts_class_outlet_number(cl, 0);
}

void
scale_config(void)
{
  fts_class_install(fts_new_symbol("scale"), scale_instantiate);
}
