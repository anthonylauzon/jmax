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
  float *points;
  int n;
} fit_t;

static void
fit_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  fit_t *this = (fit_t *)o;
  int i, n;

  ac--;
  at++;

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
}

static void
fit_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fit_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
fit_point(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fit_t *this = (fit_t *)o;
  int i = winlet - 1;
  float v = fts_get_number_float(at);
      
  this->points[i] = v;
}

/************************************************************
 *
 *  class
 *
 */
static fts_status_t
fit_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  int i, n;

  if(ac > 0)
    n = ac;
  else 
    n = 1;

  fts_class_init(cl, sizeof(fit_t), n + 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fit_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fit_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, fit_input);
  fts_method_define_varargs(cl, 0, fts_s_float, fit_input);

  for(i=0; i<n; i++)
    {
      fts_method_define_varargs(cl, i + 1, fts_s_int, fit_point);
      fts_method_define_varargs(cl, i + 1, fts_s_float, fit_point);
    }

  return fts_Success;
}

void
fit_config(void)
{
  fts_metaclass_install(fts_new_symbol("fit"), fit_instantiate, fts_narg_equiv);
}
