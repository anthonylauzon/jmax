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
 */

#include <math.h>
#include <fts/fts.h>

/************************************************************
 *
 *  object
 *
 */

typedef struct {
  fts_object_t o;
  float min;
  float max;
} rewrap_t;

static void
rewrap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  rewrap_t *this = (rewrap_t *)o;
  float a = fts_get_float_arg(ac, at, 0, 0.0f);
  float b = fts_get_float_arg(ac, at, 1, 0.0f);

  if(a <= b)
    {
      this->min = a;
      this->max = b;
    }
  else
    {
      this->min = b;
      this->max = a;
    }
}

static void
rewrap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

/************************************************************
 *
 *  user methods
 *
 */

static void
rewrap_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
  float f = fts_get_number_float(at);
  float min = this->min;
  float max = this->max;
  float range = 2.0f * (max - min);

  if(range == 0.0f)
    {
      f = min;
      fts_outlet_int(o, 1, 0);
    }
  else if(f < min)
    {
      float diff = min - f;
      float n = ceil(diff / range);

      f += n * range;

      if(f >= max)
	{
	  f = 2 * max - f;
	  n -= 0.5;
	}

      fts_outlet_int(o, 1, (int)(-2.0f * n));
    }
  else if (f >= max)
    {
      float diff = f - max;
      float n = floor(diff / range) + 1.0f;

      f -= n * range;

      if(f < min)
	{
	  f = 2 * min - f;
	  n -= 0.5;
	}

      fts_outlet_int(o, 1, (int)(2.0f * n));
    }
  else
    fts_outlet_int(o, 1, 0.0f);
    

  fts_outlet_float(o, 0, f);
}

static void
rewrap_a(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
  float a = fts_get_float_arg(ac, at, 0, 0.0f);
  float max = this->max;

  if(a <= max)
    this->min = a;
  else
    {
      this->min = max;
      this->max = a;
    }
}

static void
rewrap_b(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  rewrap_t *this = (rewrap_t *)o;
  float b = fts_get_float_arg(ac, at, 0, 0.0f);
  float min = this->min;

  if(b >= min)
    this->max = b;
  else
    {
      this->max = min;
      this->min = b;
    }
}

/************************************************************
 *
 *  class
 *
 */
static void
rewrap_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(rewrap_t), rewrap_init, rewrap_delete);

  fts_class_inlet_number(cl, 0, rewrap_number);
  fts_class_inlet_number(cl, 1, rewrap_a);
  fts_class_inlet_number(cl, 2, rewrap_b);

  fts_class_outlet_float(cl, 0);
  fts_class_outlet_int(cl, 1);
}

void
rewrap_config(void)
{
  fts_class_install(fts_new_symbol("rewrap"), rewrap_instantiate);
}
