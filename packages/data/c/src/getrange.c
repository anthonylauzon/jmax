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

#include <limits.h>
#include <float.h>
#include <fts/fts.h>

#include <fts/packages/data/data.h>

/******************************************************
 *
 *  user methods
 *
 */

static void
getrange_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ivec_t *vec = (ivec_t *)fts_get_object(at);
  int min = INT_MAX;
  int max = INT_MIN;
  int size = ivec_get_size(vec);
  int *ptr = ivec_get_ptr(vec);
  int i;

  for(i=0; i<size; i++)
    {
      int value = ptr[i];

      if(value > max)
	max = value;

      if(value < min)
	min = value;
    }

  fts_outlet_int(o, 1, max);
  fts_outlet_int(o, 0, min);
}

static void
getrange_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fvec_t *vec = (fvec_t *)fts_get_object(at);
  float min = FLT_MAX;
  float max = FLT_MIN;
  int size = fvec_get_size(vec);
  float *ptr = fvec_get_ptr(vec);
  int i;

  for(i=0; i<size; i++)
    {
      float value = ptr[i];

      if(value > max)
	max = value;

      if(value < min)
	min = value;
    }

  fts_outlet_float(o, 1, max);
  fts_outlet_float(o, 0, min);
}

/******************************************************
 *
 *  class
 *
 */

static void
getrange_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), NULL, NULL); 
  
  fts_class_inlet(cl, 0, ivec_type, getrange_ivec);
  fts_class_inlet(cl, 0, fvec_type, getrange_fvec);

  fts_class_outlet_number(cl, 0);
  fts_class_outlet_number(cl, 1);
}

void
getrange_config(void)
{
  fts_class_install(fts_new_symbol("getrange"), getrange_instantiate);
}
