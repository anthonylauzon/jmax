/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

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
} nsplit_t;

static void
nsplit_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  nsplit_t *this = (nsplit_t *)o;
  int i, n;

  /* test args are numbers */
  for(i=1; i<ac; i++)
    {
      if(!fts_is_number(at + i))
	{
	  post("nsplit: all arguments must be numbers\n");
	  return;
	}
    }

  if(ac < 2)
    n = 1;
  else 
    n = ac - 1;

  this->i_points = fts_malloc(sizeof(int) * n);
  this->f_points = fts_malloc(sizeof(float) * n);

  if(ac <= 1)
    {
      this->i_points[0] = 0;
      this->f_points[0] = 0.0f;
    }
  else
    {
      for(i=0; i<n; i++)
	{
	  const fts_atom_t *ap = at + 1 + i;
	  
	  if(fts_is_int(ap))
	    {
	      int v = fts_get_int(ap);

	      this->i_points[i] = v;
	      this->f_points[i] = (float)v;
	    }
	  else /* if(fts_is_float(ap)) */
	    {
	      float v = fts_get_float(ap);

	      this->i_points[i] = (int)ceil(v);
	      this->f_points[i] = v;
	    }
	}
    }

  this->n = n;
}

static void
nsplit_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nsplit_t *this = (nsplit_t *)o;

  fts_free(this->i_points);
  fts_free(this->f_points);
}

/************************************************************
 *
 *  user methods
 *
 */

static void
nsplit_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nsplit_t *this = (nsplit_t *)o;
  int v = fts_get_int(at);
  int *points = this->i_points;
  int n = this->n;
  int i;

  if(v >= points[n-1])
    fts_outlet_send(o, 0, fts_s_int, 1, at);

  for(i=n-1; i>0; i++)
    {
      if(v >= points[i - 1] && v < points[i])
	fts_outlet_send(o, i, fts_s_int, 1, at);
    }

  if(v < points[0])
    fts_outlet_send(o, 0, fts_s_int, 1, at);
}

static void
nsplit_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nsplit_t *this = (nsplit_t *)o;
  float v = fts_get_float(at);
  float *points = this->f_points;
  int n = this->n;
  int i;

  if(v >= points[n-1])
    fts_outlet_send(o, 0, fts_s_float, 1, at);

  for(i=n-1; i>0; i++)
    {
      if(v >= points[i - 1] && v < points[i])
	fts_outlet_send(o, i, fts_s_float, 1, at);
    }

  if(v < points[0])
    fts_outlet_send(o, 0, fts_s_float, 1, at);
}

static void
nsplit_point(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nsplit_t *this = (nsplit_t *)o;
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
static fts_status_t
nsplit_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  int i, n;

  if(ac == 1)
    n = 1;
  else 
    n = ac - 1;

  fts_class_init(cl, sizeof(nsplit_t), n + 1, n + 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, nsplit_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, nsplit_delete, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, nsplit_int, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, nsplit_float, 1, a);

  for(i=0; i<n; i++)
    {
      a[0] = fts_s_number;
      fts_method_define(cl, i + 1, fts_s_int, nsplit_point, 1, a);
      fts_method_define(cl, i + 1, fts_s_float, nsplit_point, 1, a);
    }

  return fts_Success;
}

void
nsplit_config(void)
{
  fts_metaclass_install(fts_new_symbol("nsplit"), nsplit_instantiate, fts_narg_equiv);
}
