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
#include "bpf.h"
#include "ivec.h"
#include "fvec.h"

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

  this->obj = fts_get_object(at + 1);
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

static int
bpf_advance(bpf_t *bpf, int index, double time)
{
  /* time must be > 0.0 */
  if(time >= bpf_get_time(bpf, index + 1))
    {
      index++;
      
      while(time > bpf_get_time(bpf, index + 1))
	index++;
    }
  else if(time < bpf_get_time(bpf, index))
    {
      index--;
      
      while(time < bpf_get_time(bpf, index))
	index--;
    }
  else if(bpf_get_time(bpf, index) == bpf_get_time(bpf, index + 1))
    index++;

  return index;
}

static void
getinter_bpf(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  getinter_bpf_t *this = (getinter_bpf_t *)o;
  bpf_t *bpf = (bpf_t *)this->obj;
  int size = bpf_get_size(bpf);
  double duration = bpf_get_duration(bpf);
  double time = fts_get_number_float(at);
  int index;
  
  if(size < 2 || time < 0.0)
    {
      this->index = 0;
      fts_outlet_float(o, 0, bpf_get_value(bpf, 0));
    }
  else
    {
      if(time >= duration)
	{
	  this->index = size - 2;
	  fts_outlet_float(o, 0, bpf_get_value(bpf, size - 1));
	}
      else
	{
	  double t0, t1, y0, y1;
      
	  if(this->index > size - 2)
	    index = bpf_advance(bpf, size - 2, time);
	  else
	    index = bpf_advance(bpf, this->index, time);
      
	  this->index = index;
	  
	  t0 = bpf_get_time(bpf, index + 0);
	  t1 = bpf_get_time(bpf, index + 1);
	  y0 = bpf_get_value(bpf, index + 0);
	  y1 = bpf_get_value(bpf, index + 1);
	  
	  fts_outlet_float(o, 0, y0 + (y1 - y0) * (time - t0) / (t1 - t0));
	}
    }
}
  
/******************************************************
 *
 *  class
 *
 */

static fts_status_t
getinter_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  ac--;
  at++;

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
