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
  refdata_atom_refer(at + 1);
}

static void
fill_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;

  refdata_atom_release(&this->a);
}

/******************************************************
 *
 *  user methods
 *
 */

static void
fill_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;

  refdata_atom_release(&this->a);
  this->a = at[0];
  refdata_atom_refer(at);
}

static void
fill_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void
fill_ivec_by_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *ivec = int_vector_atom_get(&this->a);
  int size = int_vector_get_size(ivec);
  int n = fts_get_number_int(at);
  int i;

  for(i=0; i<size; i++)
    int_vector_set_element(ivec, i, n);
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->a);
}

static void
fill_ivec_by_ivec(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fill_t *this = (fill_t *)o;
  int_vector_t *this_ivec = int_vector_atom_get(&this->a);
  int_vector_t *in_ivec = int_vector_atom_get(at);
  int * restrict this_ptr = int_vector_get_ptr(this_ivec);
  int * restrict in_ptr = int_vector_get_ptr(in_ivec);
  int this_size = int_vector_get_size(this_ivec);
  int in_size = int_vector_get_size(in_ivec);
  int size = (this_size <= in_size)? this_size: in_size;
  int i;

  for(i=0; i<size; i++)
    this_ptr[i] = in_ptr[i];
  
  fts_outlet_send(o, 0, int_vector_symbol, 1, &this->a);
}

static void
fill_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static void
fill_matrix(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

/******************************************************
 *
 *  class
 *
 */

static fts_status_t
fill_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  if(ac == 2)
    {
      fts_class_init(cl, sizeof(fill_t), 2, 1, 0); 
      
      /* init/delete */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fill_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, fill_delete);
      
      if(vector_atom_is(at + 1))
	{
	}
      else if(int_vector_atom_is(at + 1))
	{
	  fts_method_define_varargs(cl, 0, fts_s_int, fill_ivec_by_number);
	  fts_method_define_varargs(cl, 0, fts_s_float, fill_ivec_by_number);
	  fts_method_define_varargs(cl, 0, int_vector_symbol, fill_ivec_by_ivec);
	  fts_method_define_varargs(cl, 1, int_vector_symbol, fill_set_reference);
	}
      else if(float_vector_atom_is(at + 1))
	{
	}
      else if(matrix_atom_is(at + 1))
	{
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
  fts_metaclass_install(fts_new_symbol("fill"), fill_instantiate, fts_arg_type_equiv);
}
