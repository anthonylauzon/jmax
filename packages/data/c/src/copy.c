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
#include <ftsconfig.h>
#include <fts/fts.h>
#include "vec.h"
#include "ivec.h"
#include "fvec.h"
#include "mat.h"

/******************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  fts_atom_t a;
} copy_t;

static void
copy_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;

  ac--;
  at++;

  fts_set_void(&this->a);
  fts_atom_assign(&this->a, at);
}

static void
copy_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;

  fts_atom_void(&this->a);
}

/******************************************************
 *
 *  user methods
 *
 */

static void
copy_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;

  fts_atom_assign(&this->a, at);
}

/* int vector */

static void
copyto_ivec_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;
  ivec_t *this_ivec = ivec_atom_get(&this->a);
  ivec_t *in_ivec = ivec_atom_get(at);
  int size = ivec_get_size(in_ivec);
  int * restrict this_ptr;
  int * restrict in_ptr;
  int i;

  ivec_set_size(this_ivec, size);
  this_ptr = ivec_get_ptr(this_ivec);
  in_ptr = ivec_get_ptr(in_ivec);

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, ivec_symbol, 1, &this->a);
}

static void
copyto_ivec_from_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;
  ivec_t *this_ivec = ivec_atom_get(&this->a);
  fvec_t *in_fvec = fvec_atom_get(at);
  int size = ivec_get_size(in_fvec);
  int * restrict this_ptr;
  float * restrict in_ptr;
  int i;

  ivec_set_size(this_ivec, size);
  this_ptr = ivec_get_ptr(this_ivec);
  in_ptr = ivec_get_ptr(in_fvec);

  for(i=0; i<size; i++)
    this_ptr[i] = (int)in_ptr[i];
  
  fts_outlet_send(o, 0, ivec_symbol, 1, &this->a);
}

/* float vector */

static void
copyto_fvec_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;
  fvec_t *this_fvec = fvec_atom_get(&this->a);
  ivec_t *in_ivec = ivec_atom_get(at);
  int size = ivec_get_size(in_ivec);
  float * restrict this_ptr;
  int * restrict in_ptr;
  int i;

  fvec_set_size(this_fvec, size);
  this_ptr = fvec_get_ptr(this_fvec);
  in_ptr = ivec_get_ptr(in_ivec);

  for(i=0; i<size; i++)
    this_ptr[i] = (float)in_ptr[i];
  
  fts_outlet_send(o, 0, fvec_symbol, 1, &this->a);
}

static void
copyto_fvec_from_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  copy_t *this = (copy_t *)o;
  fvec_t *this_fvec = fvec_atom_get(&this->a);
  fvec_t *in_fvec = fvec_atom_get(at);
  int size = fvec_get_size(in_fvec);
  float * restrict this_ptr;
  float * restrict in_ptr;
  int i;

  fvec_set_size(this_fvec, size);
  this_ptr = fvec_get_ptr(this_fvec);
  in_ptr = fvec_get_ptr(in_fvec);

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, fvec_symbol, 1, &this->a);
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
copyto_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(ac == 2)
    {
      fts_class_init(cl, sizeof(copy_t), 2, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, copy_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, copy_delete);
      
      if(ivec_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, ivec_symbol, copyto_ivec_from_ivec);
	  fts_method_define_varargs(cl, 0, fvec_symbol, copyto_ivec_from_fvec);
	  fts_method_define_varargs(cl, 1, ivec_symbol, copy_set_reference);
	}
      else if(fvec_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, ivec_symbol, copyto_fvec_from_ivec);
	  fts_method_define_varargs(cl, 0, fvec_symbol, copyto_fvec_from_fvec);
	  fts_method_define_varargs(cl, 1, fvec_symbol, copy_set_reference);
	}
      else
	return &fts_CannotInstantiate;
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

void
copy_config(void)
{
  fts_metaclass_install(fts_new_symbol("copyto"), copyto_instantiate, fts_arg_type_equiv);
}
