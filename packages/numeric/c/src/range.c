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

#include <math.h>
#include <fts/fts.h>

/************************************************************
 *
 *  object
 *
 */

typedef struct {
  fts_object_t o;
  int *i_points;
  float *f_points;
  int n;
} range_t;

static void
range_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  range_t *this = (range_t *)o;
  int i, n;

  if(ac == 0)
    n = 1;
  else 
    n = ac;

  this->i_points = fts_malloc(sizeof(int) * n);
  this->f_points = fts_malloc(sizeof(float) * n);

  if(ac == 0)
    {
      this->i_points[0] = 0;
      this->f_points[0] = 0.0f;
    }
  else
    {
      for(i=0; i<n; i++)
	{
	  const fts_atom_t *ap = at + i;
	  
	  if(fts_is_int(ap))
	    {
	      int v = fts_get_int(ap);

	      this->i_points[i] = v;
	      this->f_points[i] = (float)v;
	    }
	  else if(fts_is_float(ap))
	    {
	      float v = fts_get_float(ap);

	      this->i_points[i] = (int)ceil(v);
	      this->f_points[i] = v;
	    }
	  else
	    {
	      this->i_points[i] = 0;
	      this->f_points[i] = 0.0;
	    }
	}
    }

  this->n = n;

  fts_object_set_inlets_number(o, n + 1);
  fts_object_set_outlets_number(o, n + 1);
}

static void
range_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  range_t *this = (range_t *)o;

  fts_free(this->i_points);
  fts_free(this->f_points);
}

/************************************************************
 *
 *  user methods
 *
 */

static void
range_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  range_t *this = (range_t *)o;
  int v = fts_get_int(at);
  int *points = this->i_points;
  int n = this->n;
  int i;

  if(v < points[0])
    {
      fts_outlet_int(o, 0, v);
      return;
    }

  for(i=1; i<n; i++)
    {
      if(v < points[i])
	{
	  fts_outlet_int(o, i, v);
	  return;
	}
    }

  if(v >= points[n-1])
    fts_outlet_int(o, n, v);
}

static void
range_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  range_t *this = (range_t *)o;
  float v = fts_get_float(at);
  float *points = this->f_points;
  int n = this->n;
  int i;

  if(v < points[0])
    {
      fts_outlet_float(o, 0, v);
      return;
    }

  for(i=1; i<n; i++)
    {
      if(v < points[i] || v == points[i - 1])
	{
	  fts_outlet_float(o, i, v);
	  return;
	}
    }

  if(v >= points[n-1])
    fts_outlet_float(o, n, v);
}

static void
range_point(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  range_t *this = (range_t *)o;
  int i = winlet - 1;

  if(fts_is_int(at))
    {
      int v = fts_get_int(at);
      
      this->i_points[i] = v;
      this->f_points[i] = (float)v;
    }
  else /* if(fts_is_float(at)) */
    {
      float v = fts_get_float(at);
      
      this->i_points[i] = (int)ceil(v);
      this->f_points[i] = v;
    }
}

/************************************************************
 *
 *  class
 *
 */
static void
range_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(range_t), range_init, range_delete);

  fts_class_inlet_number(cl, 0, range_int);
  fts_class_inlet_number(cl, 1, range_point);

  fts_class_outlet_number(cl, 0);
}

void
range_config(void)
{
  fts_class_install(fts_new_symbol("range"), range_instantiate);
}
