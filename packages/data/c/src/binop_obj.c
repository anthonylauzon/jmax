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
#include "binops.h"

/***************************************************
 *
 *  generic binary operator class
 *
 *  one inlet
 *    <binop> const <constant right operand> ... right operand is constant data type
 *    <binop> static <static right operand> ... right operand can be static reference to non constant data
 *
 *  two inlets
 *    <binop> store [initial right operand] ... right operand is set (stored) via right inlet
 *    <binop> trigger [initial right operand] ... right aswell as left inlet set operand and triggers computation
 *    <binop> clear [initial right operand] ... right operand is cleared (released) after computation triggered by left operand
 *
 *  (the keyword which is set as default keyword - see binop_obj_config - is optional)
 *
 */

/* object argument keywords */
static fts_symbol_t sym_const = 0;
static fts_symbol_t sym_static = 0;
static fts_symbol_t sym_store = 0;
static fts_symbol_t sym_trigger = 0;
static fts_symbol_t sym_clear = 0;
static fts_symbol_t sym_inplace = 0;
static fts_symbol_t default_keyword = 0;

/******************************************************
 *
 *  binop operand
 *
 */

#define LEFT(op) ((op)[0])
#define RIGHT(op) ((op)[1])
#define RESULT(op) ((op)[2])

/******************************************************
 *
 *  object
 *
 */

typedef struct _binop_obj
{
  fts_object_t head;
  binop_t *binop;
  op_t op[3]; /* left, right, result */
} binop_obj_t;

int binop_obj_is_keyword(const fts_atom_t *atom)
{
  if(fts_is_symbol(atom))
    {
      fts_symbol_t sym = fts_get_symbol(atom);

      return (sym == sym_const || 
	      sym == sym_static || 
	      sym == sym_store || 
	      sym == sym_trigger || 
	      sym == sym_clear ||
	      sym == sym_inplace
	      );
    }
  else
    return 0;
}

static binop_t *
binop_obj_get_args(int ac, const fts_atom_t *at, fts_symbol_t *keyword, fts_atom_t *right)
{
  binop_t *binop = 0;

  if(ac > 1 && fts_is_symbol(at + 1))
    binop = binop_get(fts_get_symbol(at + 1));
      
  if(binop) 
    {
      if(ac == 4 && binop_obj_is_keyword(at + 2))
	{
	  *keyword = fts_get_symbol(at + 2);
	  *right = at[3];
	}
      else if(ac == 3 && binop_obj_is_keyword(at + 2))
	{
	  fts_symbol_t keyword_arg = fts_get_symbol(at + 2);
	  
	  if(keyword_arg == sym_const || keyword_arg == sym_static)
	    return 0; /* right operand required */
	  else
	    {
	      *keyword = keyword_arg;
	      fts_set_void(right);
	    }
	}
      else if(ac == 3)
	{
	  *keyword = default_keyword;
	  *right = at[2];
	}
      else if(ac == 2)
	{
	  *keyword = default_keyword;
	  fts_set_void(right);
	}

      return binop;
    }
  else
    {
      *keyword = 0;
      fts_set_void(right);
      return 0; /* wrong args */
    }
}

static void
binop_obj_error_const_in_inplace(binop_t *binop)
{
  const char *binop_str = fts_symbol_name(binop->name);

  post("binop: %s: left operand must not be constant for inplace calculation\n", binop_str);
}

static void
binop_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;
  fts_symbol_t name = fts_get_symbol(at + 1);
  fts_symbol_t keyword = 0;
  fts_atom_t right_init;  

  this->binop = binop_obj_get_args(ac, at, &keyword, &right_init);

  if(this->binop)
    {
      if(keyword == sym_const && !data_atom_is_const(right_init))
	{
	  op_init_void(&RIGHT(op));
	  post("binop %s const: constant initializer required\n", fts_symbol_name(name));
	}
      else
	op_init(&RIGHT(op), right_init);

      op_init_void(&LEFT(op));
      op_init_void(&RESULT(op));
    }
  else
    post("binop: unknown operator: %s\n", fts_symbol_name(name));

}

static void
binop_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;

  op_void(&LEFT(op));
  op_void(&RIGHT(op));
  op_void(&RESULT(op));
}

/******************************************************
 *
 *  user methods
 *
 */

static void
binop_obj_left_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;
  
  if(!op_is_void(RIGHT(op)))
    {
      op_set_with_type(&LEFT(op), at[0], s);

      if(binop_call_fun_recycle(this->binop, op))
	op_outlet(o, winlet, RESULT(op));
      
      op_release(&LEFT(op));
    }
}

static void
binop_obj_left_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;  
  
  if(data_atom_is_const(at[0]))
    {
      binop_obj_error_const_in_inplace(this->binop);
      return;
    }

  if(!op_is_void(RIGHT(op)))
    {
      op_set_with_type(&LEFT(op), at[0], s);

      if(binop_call_fun_inplace(this->binop, op))
	op_outlet(o, winlet, LEFT(op));

      op_release(&LEFT(op));
    }
}

static void
binop_obj_left_store_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;

  op_set_with_type(&LEFT(op), at[0], s);

  if(!op_is_void(RIGHT(op)) && binop_call_fun_recycle(this->binop, op))
    op_outlet(o, winlet, RESULT(op));
}

static void
binop_obj_left_trigger_and_clear_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;

  if(!op_is_void(RIGHT(op)))
    {
      op_set_with_type(&LEFT(op), at[0], s);
      
      if(binop_call_fun_recycle(this->binop, op))
	op_outlet(o, winlet, RESULT(op));

      op_release(&LEFT(op));
      op_void(&RIGHT(op));
    }
}

static void
binop_obj_right_store_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;

  op_set_with_type(&RIGHT(op), at[0], s);

  if(!op_is_void(LEFT(op)) && binop_call_fun_recycle(this->binop, op))
    op_outlet(o, winlet, RESULT(op));
}

static void
binop_obj_right_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  op_t *op = this->op;
  
  op_set_with_type(&RIGHT(op), at[0], s);
}

/******************************************************
 *
 *  class
 *
 */

int
binop_obj_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  fts_atom_t right;
  fts_symbol_t keyword0 = 0;
  fts_symbol_t keyword1 = 0;

  binop_obj_get_args(ac0, at0, &keyword0, &right);
  binop_obj_get_args(ac1, at1, &keyword1, &right);
  
  return (keyword0 == keyword1);
}

static void
binop_obj_const_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_obj_t), 1, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_obj_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, binop_obj_delete, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, binop_obj_left_trigger);
}

static void 
binop_obj_static_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_obj_t), 1, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_obj_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, binop_obj_delete, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, binop_obj_left_trigger);
}

static void
binop_obj_store_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_obj_t), 2, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_obj_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, binop_obj_delete, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, binop_obj_left_trigger);
  fts_method_define_varargs(cl, 1, fts_s_anything, binop_obj_right_store);
}

static void 
binop_obj_trigger_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_obj_t), 2, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_obj_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, binop_obj_delete, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, binop_obj_left_store_and_trigger);
  fts_method_define_varargs(cl, 1, fts_s_anything, binop_obj_right_store_and_trigger);
}

static void
binop_obj_clear_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_obj_t), 2, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_obj_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, binop_obj_delete, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, binop_obj_left_trigger_and_clear_right);
  fts_method_define_varargs(cl, 1, fts_s_anything, binop_obj_right_store);
}

static void
binop_obj_inplace_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(binop_obj_t), 2, 1, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, binop_obj_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, binop_obj_delete, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, binop_obj_left_inplace);
  fts_method_define_varargs(cl, 1, fts_s_anything, binop_obj_right_store);
}

static fts_status_t
binop_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t keyword = 0;
  fts_atom_t right_init;

  if(binop_obj_get_args(ac, at, &keyword, &right_init))
    {
      /* initialize the class */
      if(keyword == sym_const)
	binop_obj_const_instantiate(cl, ac, at);
      else if(keyword == sym_static)
	binop_obj_static_instantiate(cl, ac, at);
      else if(keyword == sym_store)
	binop_obj_store_instantiate(cl, ac, at);
      else if(keyword == sym_trigger)
	binop_obj_trigger_instantiate(cl, ac, at);
      else if(keyword == sym_clear)
	binop_obj_clear_instantiate(cl, ac, at);
      else if(keyword == sym_inplace)
	binop_obj_inplace_instantiate(cl, ac, at);
      else    
	return &fts_CannotInstantiate;

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

void
binop_obj_config(void)
{
  fts_metaclass_create(fts_new_symbol("binop"), binop_obj_instantiate, binop_obj_equiv);
  fts_metaclass_alias(fts_new_symbol("bo"), fts_new_symbol("binop"));

  sym_const = fts_new_symbol("const");
  sym_static = fts_new_symbol("static");
  sym_store = fts_new_symbol("store");
  sym_trigger = fts_new_symbol("trigger");
  sym_clear = fts_new_symbol("clear");
  sym_inplace = fts_new_symbol("inplace");
  default_keyword = sym_clear;
}
