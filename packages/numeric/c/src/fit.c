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

#include <math.h>
#include <fts/fts.h>

/************************************************************
 *
 *  object
 *
 */

typedef struct {
  fts_object_t o;
  float *points;
  int n;
} fit_t;

static void
fit_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  fit_t *this = (fit_t *)o;
  int i, n;

  if(ac)
    n = ac;
  else
    n = 1;

  this->points = fts_malloc(sizeof(float) * n);

  if(ac == 0)
    this->points[0] = 0.0f;
  else
    {
      for(i=0; i<n; i++)
	{
	  const fts_atom_t *ap = at + i;
	  
	  if(fts_is_number(ap))
	    this->points[i] = fts_get_number_float(ap);
	  else
	    this->points[i] = 0.0;
	}
    }

  this->n = n;

  fts_object_set_inlets_number(o, n + 1);
}

static void
fit_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fit_t *this = (fit_t *)o;

  fts_free(this->points);
}

/************************************************************
 *
 *  user methods
 *
 */

static void
fit_input(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fit_t *this = (fit_t *)o;
  float v = fts_get_number_float(at);
  float *points = this->points;
  float diff = v - points[0];
  int i;

  if(diff <= 0.0)
    fts_outlet_float(o, 0, points[0]);
  else
    {
      float next = diff;

      for(i=1; i<this->n; i++)
	{
	  next = v - points[i];

	  if(next < 0.0)
	    next = -next;

	  if(diff < next)
	    break;
	  
	  diff = next;
	}
      
      fts_outlet_float(o, 0, points[i - 1]);
    }
}

static void
fit_point(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  fit_t *this = (fit_t *)o;
  int winlet = fts_object_get_message_inlet(o);
  int i = winlet - 1;
  float v = fts_get_number_float(at);
      
  this->points[i] = v;
}

/************************************************************
 *
 *  class
 *
 */
static void
fit_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fit_t), fit_init, fit_delete);

  fts_class_inlet_number(cl, 0, fit_input);
  fts_class_inlet_number(cl, 1, fit_point);

  fts_class_outlet_float(cl, 0);
}

void
fit_config(void)
{
  fts_class_install(fts_new_symbol("fit"), fit_instantiate);
}
