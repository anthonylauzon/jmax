/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "data.h"
#include "operators.h"

fts_symbol_t
atom_obj_get_type(int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at + 1))
    return fts_get_symbol(at + 1);
  else
    return fts_s_anything;
}

const fts_atom_t *
atom_obj_get_initializer(int ac, const fts_atom_t *at)
{
  if(ac > 2)
    return (at + 2);
  else if(ac == 2)
    return (at + 1);
  else
    return 0;
}

static fts_symbol_t
atom_obj_get_initializer_type(int ac, const fts_atom_t *at)
{
  const fts_atom_t *init = atom_obj_get_initializer(ac, at);

  if(init)
    return (fts_is_data(init)? fts_data_get_class_name(fts_get_data(init)): fts_get_type(init));
  else
    return 0;
}

/********************************************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t o;
  op_t op;
} atom_obj_t;

static void
atom_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;
  fts_symbol_t type = atom_obj_get_type(ac, at);
  const fts_atom_t *init = atom_obj_get_initializer(ac, at);
  fts_symbol_t init_type = atom_obj_get_initializer_type(ac, at);

  if(init)
    fts_send_message(o, 0, init_type, 1, init);
  else
    op_init_void(&this->op);
}

static void
atom_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *) o;

  op_release(&this->op);
}

/*********************************************************************
 *
 *  methods
 *
 */

static void
atom_obj_set_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  op_set_int(&this->op, fts_get_int(at));
}

static void
atom_obj_set_int_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  op_set_int(&this->op, fts_get_int(at));
  op_outlet(o, 0, this->op);
}

static void
atom_obj_set_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  op_set_float(&this->op, fts_get_float(at));
}

static void
atom_obj_set_float_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  op_set_float(&this->op, fts_get_float(at));
  op_outlet(o, 0, this->op);  
}

static void
atom_obj_set_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  op_set_symbol(&this->op, fts_get_symbol(at));
}

static void
atom_obj_set_symbol_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  op_set_symbol(&this->op, fts_get_symbol(at));
  op_outlet(o, 0, this->op);  
}

static void
atom_obj_set_integer_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;
  op_t *op = &this->op;
  fts_integer_vector_t *incomming = (fts_integer_vector_t *)fts_get_data(at);  
  int size = fts_integer_vector_get_size(incomming);

  if(op_is_integer_vector(*op) && op_ref_can_be_reused(*op))
    {
      /* reuse integer vector */
      fts_integer_vector_t *vector = op_get_integer_vector(*op);
      fts_integer_vector_copy(incomming, vector);
    }
  else
    {
      /* make new vector */
      fts_integer_vector_t *vector = fts_integer_vector_new(size);
      fts_integer_vector_copy(incomming, vector);
      op_set_integer_vector(op, vector);
    }

  if(winlet == 0)
    op_outlet(o, 0, *op);      
}

static void
atom_obj_set_float_vector(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;
  op_t *op = &this->op;
  fts_float_vector_t *incomming = (fts_float_vector_t *)fts_get_data(at);  
  int size = fts_float_vector_get_size(incomming);

  if(op_is_float_vector(*op) && op_ref_can_be_reused(*op))
    {
      /* reuse float vector */
      fts_float_vector_t *vector = op_get_float_vector(*op);
      fts_float_vector_copy(incomming, vector);
    }
  else
    {
      /* make new vector */
      fts_float_vector_t *vector = fts_float_vector_new(size);
      fts_float_vector_copy(incomming, vector);
      op_set_float_vector(op, vector);
    }

  if(winlet == 0)
    op_outlet(o, 0, *op);
}

static void
atom_obj_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  atom_obj_t *this = (atom_obj_t *)o;

  if(!op_is_void(this->op))
    op_outlet(o, 0, this->op);      
}

/********************************************************************
 *
 *   class
 *
 */

static int
atom_obj_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (atom_obj_get_type(ac0, at0) == atom_obj_get_type(ac1, at1) &&
	  atom_obj_get_initializer_type(ac0, at0) == atom_obj_get_initializer_type(ac1, at1));
}

static fts_status_t
atom_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t s[4];
  fts_symbol_t type = atom_obj_get_type(ac, at);
  const fts_atom_t *init = atom_obj_get_initializer(ac, at);
  fts_symbol_t init_type = atom_obj_get_initializer_type(ac, at);
  
  if(type != fts_s_anything && type != init_type && fts_get_symbol(init) != fts_s_void)
    {
      /* no conversion yet :-( */
      post("atom: can not initialize %s with %s\n", fts_symbol_name(type), fts_symbol_name(init_type));
      return &fts_CannotInstantiate;
    }

  fts_class_init(cl, sizeof(atom_obj_t), 2, 1, 0);

  s[0] = fts_s_symbol;
  s[1] = fts_s_anything;
  s[2] = fts_s_anything;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, atom_obj_init, 3, s, 1);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, atom_obj_delete, 0, 0);

  if(type == fts_s_int)
    {
      fts_method_define(cl, 0, fts_s_int, atom_obj_set_int_and_trigger, 1, &fts_s_int);
      fts_method_define(cl, 1, fts_s_int, atom_obj_set_int, 1, &fts_s_int); 
      fts_method_define_bang(cl, 0, atom_obj_output);
    }
  else if(type == fts_s_float)
    {
      fts_method_define(cl, 0, fts_s_float, atom_obj_set_float_and_trigger, 1, &fts_s_float);
      fts_method_define(cl, 1, fts_s_float, atom_obj_set_float, 1, &fts_s_float);
      fts_method_define_bang(cl, 0, atom_obj_output);
    }
  else if(type == fts_s_symbol)
    {
      fts_method_define(cl, 0, fts_s_symbol, atom_obj_set_symbol_and_trigger, 1, &fts_s_symbol);
      fts_method_define(cl, 1, fts_s_symbol, atom_obj_set_symbol, 1, &fts_s_symbol);
      fts_method_define_bang(cl, 0, atom_obj_output);
    }
  else if(type == fts_s_integer_vector)
    {
      fts_method_define(cl, 0, fts_s_integer_vector, atom_obj_set_integer_vector, 1, &fts_s_data);
      fts_method_define(cl, 1, fts_s_integer_vector, atom_obj_set_integer_vector, 1, &fts_s_data);
      fts_method_define_bang(cl, 0, atom_obj_output);
    }
  else if(type == fts_s_float_vector)
    {
      fts_method_define(cl, 0, fts_s_float_vector, atom_obj_set_float_vector, 1, &fts_s_data);
      fts_method_define(cl, 1, fts_s_float_vector, atom_obj_set_float_vector, 1, &fts_s_data);
      fts_method_define_bang(cl, 0, atom_obj_output);
    }
  else if(type == fts_s_anything)
    {
      fts_method_define(cl, 0, fts_s_int, atom_obj_set_int_and_trigger, 1, &fts_s_int);
      fts_method_define(cl, 1, fts_s_int, atom_obj_set_int, 1, &fts_s_int); 
      fts_method_define(cl, 0, fts_s_float, atom_obj_set_float_and_trigger, 1, &fts_s_float);
      fts_method_define(cl, 1, fts_s_float, atom_obj_set_float, 1, &fts_s_float);
      fts_method_define(cl, 0, fts_s_symbol, atom_obj_set_symbol_and_trigger, 1, &fts_s_symbol);
      fts_method_define(cl, 1, fts_s_symbol, atom_obj_set_symbol, 1, &fts_s_symbol);

      fts_method_define(cl, 0, fts_s_integer_vector, atom_obj_set_integer_vector, 1, &fts_s_data);
      fts_method_define(cl, 1, fts_s_integer_vector, atom_obj_set_integer_vector, 1, &fts_s_data);
      fts_method_define(cl, 0, fts_s_float_vector, atom_obj_set_float_vector, 1, &fts_s_data);
      fts_method_define(cl, 1, fts_s_float_vector, atom_obj_set_float_vector, 1, &fts_s_data);

      fts_method_define_bang(cl, 0, atom_obj_output);
    }
  else
    {
      post("atom: %s is not a supported atom type for variables\n", fts_symbol_name(type));
      return &fts_CannotInstantiate;

    }

  return fts_Success;
}

void
atom_obj_config(void)
{
  fts_metaclass_create(fts_new_symbol("atom"), atom_obj_instantiate, atom_obj_equiv);
}
