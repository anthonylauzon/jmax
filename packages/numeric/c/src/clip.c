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
 */

#include <stdlib.h>
#include <fts/fts.h>

typedef struct 
{
  fts_object_t _o;
  int i_min;
  int i_max;
  float f_min;
  float f_max;
} clip_t;


static void
clip_set_min(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *this = (clip_t *)o;

  if(fts_is_int(at))
    {
      this->i_min = fts_get_int(at);
      this->f_min = (float)this->i_min;
    }
  else if(fts_is_float(at))
    {
      this->f_min = fts_get_float(at);
      this->i_min = (int)this->f_min;
    }
}

static void
clip_set_max(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *this = (clip_t *)o;

  if(fts_is_int(at))
    {
      this->i_max = fts_get_int(at);
      this->f_max = (float)this->i_max;
    }
  else if(fts_is_float(at))
    {
      this->f_max = fts_get_float(at);
      this->i_max = (int)this->f_max;
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
  clip_t *this = (clip_t *)o;
  
  this->i_min = 0;
  this->i_max = 0;
  this->f_min = 0.;
  this->f_max = 0.;

  clip_set(o, winlet, s, ac, at);
}

static void
clip_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *this = (clip_t *)o;
  float f = fts_get_float(at);

  if (f > this->f_max)
    f = this->f_max;
  else if (f < this->f_min)
    f = this->f_min;

  fts_outlet_float(o, 0, f);
}

static void
clip_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int n = fts_get_int(at);
  clip_t *this = (clip_t *)o;

  if (n > this->i_max)
    n = this->i_max;
  else if (n < this->i_min)
    n = this->i_min;

  fts_outlet_int(o, 0, n);
}

static void
clip_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  clip_t *this = (clip_t *)o;
  fts_atom_t *out = alloca(ac * sizeof(fts_atom_t));
  int i;

  for(i=0; i<ac; i++)
    {
      if (fts_is_int(at + i))
	{
	  if (fts_get_int(at + i) > this->i_max)
	    fts_set_int(out + i, this->i_max);
	  else if (fts_get_int(at + i) < this->i_min)
	    fts_set_int(out + i, this->i_min);
	  else
	    out[i] = at[i];
	}
      else if(fts_is_float(at + i))
	{
	  if (fts_get_float(at + i) > this->i_max)
	    fts_set_float(out + i, this->f_max);
	  else if (fts_get_float(at + i) < this->i_min)
	    fts_set_float(out + i, this->f_min);
	  else
	    out[i] = at[i];
	}
      else
	out[i] = at[i];
    }

  fts_outlet_varargs(o, 0, ac, out);
}

static void
clip_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(clip_t), clip_init, 0);

  fts_class_message_varargs(cl, fts_s_set, clip_set);

  fts_class_inlet_varargs(cl, 0, clip_varargs);
  fts_class_inlet_int(cl, 0, clip_int);
  fts_class_inlet_float(cl, 0, clip_float);
  fts_class_inlet_number(cl, 1, clip_set_min);
  fts_class_inlet_number(cl, 2, clip_set_max);

  fts_class_outlet_varargs(cl, 0);
}

void
clip_config(void)
{
  fts_class_install(fts_new_symbol("clip"), clip_instantiate);
}
