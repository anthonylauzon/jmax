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
#include <fts/fts.h>
#include <float.h>
#include <data/c/include/bpf.h>
#include <data/c/include/ivec.h>
#include <data/c/include/fvec.h>

typedef struct 
{
  fts_object_t o;
  fts_object_t *obj;
  fts_method_t meth;
  int index;
} getinter_t;

/******************************************************
 *
 *  ivec
 *
 */

static void
getinter_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;
  int size = ivec_get_size((ivec_t *)this->obj);
  
  if(size)
    {
      double f = fts_get_number_float(at);

      if(size < 2 || f <= 0.0)
	fts_outlet_float(o, 0, ivec_get_element((ivec_t *)this->obj, 0));    
      else if(f >= size - 1)
	fts_outlet_float(o, 0, ivec_get_element((ivec_t *)this->obj, size - 1));
      else
	{
	  int i = (int)f;
	  double y0 = ivec_get_element((ivec_t *)this->obj, i);
	  double y1 = ivec_get_element((ivec_t *)this->obj, i + 1);
  
	  fts_outlet_float(o, 0, y0 + (f - i) * (y1 - y0));
	}
    }
}

/******************************************************
 *
 *  fvec
 *
 */

static void
getinter_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;
  int size = fvec_get_size((fvec_t *)this->obj);
  
  if(size)
    {
      double f = fts_get_number_float(at);

      if(size < 2 || f <= 0.0)
	fts_outlet_float(o, 0, fvec_get_element((fvec_t *)this->obj, 0));    
      else if(f >= size - 1)
	fts_outlet_float(o, 0, fvec_get_element((fvec_t *)this->obj, size - 1));
      else
	{
	  int i = (int)f;
	  double y0 = fvec_get_element((fvec_t *)this->obj, i);
	  double y1 = fvec_get_element((fvec_t *)this->obj, i + 1);
  
	  fts_outlet_float(o, 0, y0 + (f - i) * (y1 - y0));
	}
    }
}

/******************************************************
 *
 *  bpf
 *
 */

static void
getinter_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;
  bpf_t *bpf = (bpf_t *)this->obj;
  int size = bpf_get_size(bpf);

  if(size > 0)
    {
      double time = fts_get_number_float(at);
      
      if(time < bpf_get_time(bpf, 0))
	{
	  this->index = 0;
	  fts_outlet_float(o, 0, bpf_get_value(bpf, 0));
	}
      else if(time >= bpf_get_duration(bpf))
	{
	  this->index = size - 1;
	  fts_outlet_float(o, 0, bpf_get_target(bpf));
	}
      else
	{
	  int index = this->index;

	  if(index > size - 2)
	    index = size - 2;
	  
	  /* search index */
	  if(time >= bpf_get_time(bpf, index + 1))
	    {
	      index++;
	      
	      while(time >= bpf_get_time(bpf, index + 1))
		index++;
	    }
	  else if(time < bpf_get_time(bpf, index))
	    {
	      index--;
	      
	      while(time < bpf_get_time(bpf, index))
		index--;
	    }
	  else if(bpf_get_slope(bpf, index) == DBL_MAX)
	    {
	      index++;

	      while(bpf_get_slope(bpf, index) == DBL_MAX)
		index++;
	    }
	  
	  /* remember new index */
	  this->index = index;
	  
	  /* output interpolated value */
	  fts_outlet_float(o, 0, bpf_get_value(bpf, index) + 
			   (time - bpf_get_time(bpf, index)) * bpf_get_slope(bpf, index));
	}
    }
  else
    fts_outlet_float(o, 0, 0.0);    
}

static void
getinter_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;
  fts_object_t *obj = fts_get_object(at);

  if(fts_is_a(at, ivec_type))
    this->meth = getinter_ivec;
  else if(fts_is_a(at, fvec_type))
    this->meth = getinter_fvec;
  else if(fts_is_a(at, bpf_type))
    this->meth = getinter_bpf;
  else
    {
      if(winlet < 0)
	fts_object_set_error(o, "not implemented for %s", fts_get_class_name(at));
      else
	fts_object_signal_runtime_error(o, "not implemented for %s", fts_get_class_name(at));

      return;
    }

  if(this->obj != NULL)
    fts_object_release(this->obj);

  this->obj = obj;
  fts_object_refer(obj);
}

static void
getinter_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;

  this->meth(o, 0, 0, ac, at);
}
  
/******************************************************
 *
 *  class
 *
 */

static void
getinter_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;

  this->obj = NULL;
  this->meth = NULL;

  if(ac > 0 && fts_is_object(at))
    getinter_set_reference(o, 0, 0, 1, at);
  else
    fts_object_set_error(o, "argument missing");
}

static void
getinter_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;

  if(this->obj)
    fts_object_release(this->obj);
}

static void
getinter_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(getinter_t), getinter_init, getinter_delete);
  
  fts_class_inlet_int(cl, 0, getinter_input);
  fts_class_inlet_float(cl, 0, getinter_input);
  
  fts_class_inlet(cl, 1, ivec_type, getinter_set_reference);
  fts_class_inlet(cl, 1, fvec_type, getinter_set_reference);
  fts_class_inlet(cl, 1, bpf_type, getinter_set_reference);

  fts_class_outlet_float(cl, 0);
}

void
getinter_config(void)
{
  fts_class_install(fts_new_symbol("getinter"), getinter_instantiate);
}
