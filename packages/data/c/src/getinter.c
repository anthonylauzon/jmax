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

/******************************************************
 *
 *  getinter <ivec>
 *  getinter <fvec>
 *  getinter <bpf>
 *
 */

typedef struct 
{
  fts_object_t o;
  fts_object_t *obj;
} getinter_t;

typedef struct 
{
  fts_object_t o;
  fts_object_t *obj;
  int index;
} getinter_bpf_t;

static void
getinter_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;

  this->obj = fts_get_object(at);
  fts_object_refer(this->obj);
}

static void
getinter_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;

  fts_object_release(this->obj);
}

/******************************************************
 *
 *  set indices and reference
 *
 */

static void
getinter_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_t *this = (getinter_t *)o;
  fts_object_t *obj = fts_get_object(at);

  fts_object_release(this->obj);
  this->obj = obj;
  fts_object_refer(obj);
}

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
  getinter_bpf_t *this = (getinter_bpf_t *)o;
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
  
/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getinter_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 1 && ivec_atom_is(at))
    {
      fts_class_init(cl, sizeof(getinter_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getinter_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getinter_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_int, getinter_ivec);
      fts_method_define_varargs(cl, 0, fts_s_float, getinter_ivec);

      fts_method_define_varargs(cl, 1, ivec_symbol, getinter_set_reference);
    }
  else if(ac == 1 && fvec_atom_is(at))
    {
      fts_class_init(cl, sizeof(getinter_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getinter_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getinter_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_int, getinter_fvec);
      fts_method_define_varargs(cl, 0, fts_s_float, getinter_fvec);

      fts_method_define_varargs(cl, 1, fvec_symbol, getinter_set_reference);
    }
  else if(ac == 1 && bpf_atom_is(at))
    {
      fts_class_init(cl, sizeof(getinter_bpf_t), 2, 1, 0); 
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, getinter_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, getinter_delete);
      
      fts_method_define_varargs(cl, 0, fts_s_int, getinter_bpf);
      fts_method_define_varargs(cl, 0, fts_s_float, getinter_bpf);

      fts_method_define_varargs(cl, 1, bpf_symbol, getinter_set_reference);
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

void
getinter_config(void)
{
  fts_metaclass_install(fts_new_symbol("getinter"), getinter_instantiate, fts_arg_type_equiv);
}
