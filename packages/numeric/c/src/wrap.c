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
} wrap_t;

static void
wrap_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  wrap_t *this = (wrap_t *)o;
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
wrap_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

/************************************************************
 *
 *  user methods
 *
 */

static void
wrap_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wrap_t *this = (wrap_t *)o;
  float f = fts_get_number_float(at);
  float min = this->min;
  float max = this->max;
  float range = max - min;

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

      fts_outlet_int(o, 1, -(int)n);
    }
  else if (f >= max)
    {
      float diff = f - max;
      float n = floor(diff / range) + 1.0f;

      f -= n * range;

      fts_outlet_int(o, 1, (int)n);
    }      
  else
    fts_outlet_int(o, 1, 0.0f);

  fts_outlet_float(o, 0, f);
}

static void
wrap_a(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wrap_t *this = (wrap_t *)o;
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
wrap_b(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  wrap_t *this = (wrap_t *)o;
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
wrap_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(wrap_t), wrap_init, wrap_delete);

  fts_class_inlet_number(cl, 0, wrap_number);
  fts_class_inlet_number(cl, 1, wrap_a);
  fts_class_inlet_number(cl, 2, wrap_b);

  fts_class_outlet_float(cl, 0);
  fts_class_outlet_int(cl, 1);
}

void
wrap_config(void)
{
  fts_class_install(fts_new_symbol("wrap"), wrap_instantiate);
}
