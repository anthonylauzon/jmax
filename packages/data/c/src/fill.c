/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "vector.h"
#include "intvec.h"
#include "floatvec.h"
#include "matrix.h"

/******************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  fts_atom_t a;
} fill_t;

static void
fill_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;

  this->a = at[1];

  if(refdata_atom_is(&this->a))
     refdata_atom_refer(&this->a);
}

static void
fill_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;

  if(refdata_atom_is(&this->a))
     refdata_atom_release(&this->a);
}

/******************************************************
 *
 *  user methods
 *
 */

static void
fill_set_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;

  this->a = at[0];
}

static void
fill_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;

  refdata_atom_release(&this->a);
  this->a = at[0];
  refdata_atom_refer(at);
}

/* number */

static void
fillfrom_number_to_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *vec = int_vector_atom_get(at);
  int n = fts_get_number_int(&this->a);

  int_vector_set_const(vec, n);  
  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
fillfrom_number_to_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  float_vector_t *vec = float_vector_atom_get(at);
  float f = fts_get_number_float(&this->a);

  float_vector_set_const(vec, f);
  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

/* int vector */

static void
fillto_ivec_from_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *vec = int_vector_atom_get(&this->a);
  int size = int_vector_get_size(vec);
  int n = fts_get_number_int(at);
  int i;

  for(i=0; i<size; i++)
    int_vector_set_element(vec, i, n);
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->a);
}

static void
fillto_ivec_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *this_vec = int_vector_atom_get(&this->a);
  int_vector_t *in_vec = int_vector_atom_get(at);
  int * restrict this_ptr = int_vector_get_ptr(this_vec);
  int * restrict in_ptr = int_vector_get_ptr(in_vec);
  int this_size = int_vector_get_size(this_vec);
  int in_size = int_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->a);
}

static void
fillto_ivec_from_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *this_vec = int_vector_atom_get(&this->a);
  float_vector_t *in_vec = float_vector_atom_get(at);
  int * restrict this_ptr = int_vector_get_ptr(this_vec);
  float * restrict in_ptr = float_vector_get_ptr(in_vec);
  int this_size = int_vector_get_size(this_vec);
  int in_size = float_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = (int)in_ptr[i];
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->a);
}

static void
fillfrom_ivec_to_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *this_vec = int_vector_atom_get(&this->a);
  int_vector_t *in_vec = int_vector_atom_get(at);
  int * restrict this_ptr = int_vector_get_ptr(this_vec);
  int * restrict in_ptr = int_vector_get_ptr(in_vec);
  int this_size = int_vector_get_size(this_vec);
  int in_size = int_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = this_ptr[i];
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

static void
fillfrom_ivec_to_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *this_vec = int_vector_atom_get(&this->a);
  float_vector_t *in_vec = float_vector_atom_get(at);
  int * restrict this_ptr = int_vector_get_ptr(this_vec);
  float * restrict in_ptr = float_vector_get_ptr(in_vec);
  int this_size = int_vector_get_size(this_vec);
  int in_size = float_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = (float)this_ptr[i];
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, at);
}

/* float vector */

static void
fillto_fvec_from_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  float_vector_t *vec = float_vector_atom_get(&this->a);
  int size = float_vector_get_size(vec);
  float f = fts_get_number_int(at);
  int i;

  for(i=0; i<size; i++)
    float_vector_set_element(vec, i, f);
  
  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->a);
}

static void
fillto_fvec_from_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  float_vector_t *this_vec = float_vector_atom_get(&this->a);
  int_vector_t *in_vec = int_vector_atom_get(at);
  float * restrict this_ptr = float_vector_get_ptr(this_vec);
  int * restrict in_ptr = int_vector_get_ptr(in_vec);
  int this_size = float_vector_get_size(this_vec);
  int in_size = int_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = (float)in_ptr[i];
  
  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->a);
}

static void
fillto_fvec_from_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  float_vector_t *this_vec = float_vector_atom_get(&this->a);
  float_vector_t *in_vec = float_vector_atom_get(at);
  float * restrict this_ptr = float_vector_get_ptr(this_vec);
  float * restrict in_ptr = float_vector_get_ptr(in_vec);
  int this_size = float_vector_get_size(this_vec);
  int in_size = float_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, float_vector_symbol, 1, &this->a);
}

static void
fillfrom_fvec_to_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  float_vector_t *this_vec = float_vector_atom_get(&this->a);
  int_vector_t *in_vec = int_vector_atom_get(at);
  float * restrict this_ptr = float_vector_get_ptr(this_vec);
  int * restrict in_ptr = int_vector_get_ptr(in_vec);
  int this_size = float_vector_get_size(this_vec);
  int in_size = int_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = (int)this_ptr[i];
  
  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

static void
fillfrom_fvec_to_fvec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  float_vector_t *this_vec = float_vector_atom_get(&this->a);
  float_vector_t *in_vec = float_vector_atom_get(at);
  float * restrict this_ptr = float_vector_get_ptr(this_vec);
  float * restrict in_ptr = float_vector_get_ptr(in_vec);
  int this_size = float_vector_get_size(this_vec);
  int in_size = float_vector_get_size(in_vec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    in_ptr[i] = this_ptr[i];
  
  fts_outlet_send(o, 0, float_vector_symbol, 1, at);
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
fillto_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 2)
    {
      fts_class_init(cl, sizeof(fill_t), 2, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fill_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fill_delete);
      
      if(int_vector_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_int, fillto_ivec_from_number);
	  fts_method_define_varargs(cl, 0, fts_s_float, fillto_ivec_from_number);
	  fts_method_define_varargs(cl, 0, int_vector_symbol, fillto_ivec_from_ivec);
	  fts_method_define_varargs(cl, 0, float_vector_symbol, fillto_ivec_from_fvec);
	  fts_method_define_varargs(cl, 1, int_vector_symbol, fill_set_reference);
	}
      else if(float_vector_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_int, fillto_fvec_from_number);
	  fts_method_define_varargs(cl, 0, fts_s_float, fillto_fvec_from_number);
	  fts_method_define_varargs(cl, 0, int_vector_symbol, fillto_fvec_from_ivec);
	  fts_method_define_varargs(cl, 0, float_vector_symbol, fillto_fvec_from_fvec);
	  fts_method_define_varargs(cl, 1, float_vector_symbol, fill_set_reference);
	}
      else
	return &fts_CannotInstantiate;
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

static fts_status_t
fillfrom_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 2)
    {
      fts_class_init(cl, sizeof(fill_t), 2, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fill_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fill_delete);

      if(fts_is_number(at + 1))
	{
	  fts_method_define_varargs(cl, 0, int_vector_symbol, fillfrom_number_to_ivec);
	  fts_method_define_varargs(cl, 0, float_vector_symbol, fillfrom_number_to_fvec);
	  fts_method_define_varargs(cl, 1, fts_s_int, fill_set_number);
	  fts_method_define_varargs(cl, 1, fts_s_float, fill_set_number);
	}
      else if(int_vector_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, int_vector_symbol, fillfrom_ivec_to_ivec);
	  fts_method_define_varargs(cl, 0, float_vector_symbol, fillfrom_ivec_to_fvec);
	  fts_method_define_varargs(cl, 1, int_vector_symbol, fill_set_reference);
	}
      else if(float_vector_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, int_vector_symbol, fillfrom_fvec_to_ivec);
	  fts_method_define_varargs(cl, 0, float_vector_symbol, fillfrom_fvec_to_fvec);
	  fts_method_define_varargs(cl, 1, float_vector_symbol, fill_set_reference);
	}
      else
	return &fts_CannotInstantiate;
    }
  else
    return &fts_CannotInstantiate;
  
  return fts_Success;
}

void
fill_config(void)
{
  fts_metaclass_install(fts_new_symbol("fillto"), fillto_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("fillfrom"), fillfrom_instantiate, fts_arg_type_equiv);
}
