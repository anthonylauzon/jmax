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

typedef struct _binop_optype
{
  fts_symbol_t name; /* cash for last operand type */
  int id; /* id of cashed type */
} binop_optype_t;

#define LEFT 0
#define RIGHT 1
#define RESULT 2

#define LEFT_TYPE_ID(obj) ((obj)->type[LEFT].id)
#define RIGHT_TYPE_ID(obj) ((obj)->type[RIGHT].id)
#define LEFT_TYPE_NAME(obj) ((obj)->type[LEFT].name)
#define RIGHT_TYPE_NAME(obj) ((obj)->type[RIGHT].name)

#define LEFT_AT(obj) ((obj)->at[LEFT])
#define RIGHT_AT(obj) ((obj)->at[RIGHT])
#define RESULT_AT(obj) ((obj)->at[RESULT])

#define FUN_ARGS(obj) ((obj)->at)

#define binop_obj_set_left_type(obj, s) \
  (((s) != LEFT_TYPE_NAME(obj))? (LEFT_TYPE_ID(obj) = data_type_get_id(s), LEFT_TYPE_NAME(obj) = (s)): (s))
#define binop_obj_set_right_type(obj, s) \
  (((s) != RIGHT_TYPE_NAME(obj))? (RIGHT_TYPE_ID(obj) = data_type_get_id(s), RIGHT_TYPE_NAME(obj) = (s)): (s))

#define binop_obj_set_left(obj, s, at) {binop_obj_set_left_type((obj), (s)); data_atom_set(&LEFT_AT(obj), (at));}
#define binop_obj_set_right(obj, s, at) {binop_obj_set_right_type((obj), (s)); data_atom_set(&RIGHT_AT(obj), (at));}

#define binop_obj_void_left(obj) data_atom_void(&LEFT_AT(obj))
#define binop_obj_void_right(obj) data_atom_void(&RIGHT_AT(obj))
#define binop_obj_void_result(obj) data_atom_void(&RIGHT_AT(obj))

#define binop_obj_left_is_void(obj) (fts_is_void(&LEFT_AT(obj)))
#define binop_obj_right_is_void(obj) (fts_is_void(&RIGHT_AT(obj)))

#define binop_obj_get_fun(obj) ((obj)->funs[LEFT_TYPE_ID(obj)][RIGHT_TYPE_ID(obj)])

/******************************************************
 *
 *  object
 *
 */

typedef struct _binop_obj
{
  fts_object_t head;
  fts_symbol_t binop;
  operator_matrix_t funs;
  fts_atom_t at[3]; /* left, right, result */
  binop_optype_t type[2]; /* left, right */
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

static int
binop_obj_get_args(int ac, const fts_atom_t *at, fts_symbol_t *keyword, fts_atom_t *right)
{
  if(ac == 3 && binop_obj_is_keyword(at + 1))
    {
      *keyword = fts_get_symbol(at + 1);
      *right = at[2];
    }
  else if(ac == 2 && binop_obj_is_keyword(at + 1))
    {
      fts_symbol_t keyword_arg = fts_get_symbol(at + 1);
      
      if(keyword_arg == sym_const || keyword_arg == sym_static)
	return 0; /* right operand required */
      else
	{
	  *keyword = keyword_arg;
	  fts_set_void(right);
	}
    }
  else if(ac == 2)
    {
      *keyword = default_keyword;
      *right = at[1];
    }
  else if(ac == 1)
    {
      *keyword = default_keyword;
      fts_set_void(right);
    }
  else
    {
      *keyword = 0;
      fts_set_void(right);
      return 0; /* wrong args */
    }
  
  return 1;
}

static void
binop_obj_error_operand_type(binop_obj_t *this)
{
  const char *binop_str = fts_symbol_name(this->binop);
  const char *left_type_str = fts_symbol_name(LEFT_TYPE_NAME(this));
  const char *right_type_str = fts_symbol_name(RIGHT_TYPE_NAME(this));

  post("binop: %s: no function for operands of given types (%s and %s)\n", binop_str, left_type_str, right_type_str);
}

static void
binop_obj_error_const_in_inplace(binop_obj_t *this)
{
  const char *binop_str = fts_symbol_name(this->binop);

  post("binop: %s: left operand must not be constant for inplace calculation\n", binop_str);
}

static void
binop_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  fts_symbol_t binop = fts_get_symbol(at);
  fts_symbol_t keyword = 0;
  fts_atom_t left_init;
  fts_atom_t right_init;  
  operator_matrix_t funs = 0;

  fts_set_void(&left_init);
  binop_obj_get_args(ac, at, &keyword, &right_init);

  this->binop = binop;

  if(keyword == binops_s_inplace)
    funs = binop_get_matrix(binop, binops_s_inplace);
  else
    funs = binop_get_matrix(binop, binops_s_recycle);

  if(funs)
    {
      this->funs = funs;
      
      fts_set_void(&LEFT_AT(this));
      fts_set_void(&RIGHT_AT(this));
      fts_set_void(&RESULT_AT(this));

      binop_obj_set_left(this, fts_s_void, left_init);
      binop_obj_set_right(this, data_atom_get_type(right_init), right_init);
    }
  else
    post("binop: unkonown operator: %s\n", fts_symbol_name(binop));
}

static void
binop_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;

  binop_obj_void_left(this);
  binop_obj_void_right(this);
  binop_obj_void_result(this);
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
  
  if(!binop_obj_right_is_void(this))
    {
      operator_fun_t fun;

      binop_obj_set_left(this, s, at[0]);

      fun = binop_obj_get_fun(this);
      
      if(fun)
	{
	  fun(FUN_ARGS(this));
	  data_atom_outlet(o, 0, RESULT_AT(this));
	}
      else
	binop_obj_error_operand_type(this);

      binop_obj_void_left(this);
    }
}

static void
binop_obj_left_inplace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  
  
  if(data_atom_is_const(at[0]))
    {
      binop_obj_error_const_in_inplace(this);
      return;
    }

  if(!binop_obj_right_is_void(this))
    {
      operator_fun_t fun;

      data_atom_refer(at);
      binop_obj_set_left(this, s, at[0]);

      fun = binop_obj_get_fun(this);
      
      if(fun)
	{
	  fun(FUN_ARGS(this));
	  data_atom_outlet(o, 0, LEFT_AT(this));
	}
      else
	binop_obj_error_operand_type(this);

      binop_obj_void_left(this);
    }
}

static void
binop_obj_left_store_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;

  binop_obj_set_left(this, s, at[0]);

  if(!binop_obj_right_is_void(this))
    {
      operator_fun_t fun;
      
      fun = binop_obj_get_fun(this);

      if(fun)
	{
	  fun(FUN_ARGS(this));
	  data_atom_outlet(o, 0, RESULT_AT(this));
	}
      else
	binop_obj_error_operand_type(this);
    }
}

static void
binop_obj_left_trigger_and_clear_right(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;

  if(!binop_obj_right_is_void(this))
    {
      operator_fun_t fun;

      binop_obj_set_left(this, s, at[0]);
      
      fun = binop_obj_get_fun(this);

      if(fun)
	{
	  fun(FUN_ARGS(this));
	  data_atom_outlet(o, 0, RESULT_AT(this));
	}
      else
	binop_obj_error_operand_type(this);
      
      binop_obj_void_right(this);
      binop_obj_void_left(this);
    }
}

static void
binop_obj_right_store_and_trigger(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;

  binop_obj_set_right(this, s, at[0]);

  if(!binop_obj_left_is_void(this))
    {
      operator_fun_t fun;
      
      fun = binop_obj_get_fun(this);

      if(fun)
	{
	  fun(FUN_ARGS(this));
	  data_atom_outlet(o, 0, RESULT_AT(this));
	}
      else
	binop_obj_error_operand_type(this);
    }
}

static void
binop_obj_right_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  binop_obj_t *this = (binop_obj_t *)o;
  
  binop_obj_set_right(this, s, at[0]);
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
  fts_metaclass_alias(fts_new_symbol("add"), fts_new_symbol("binop"));
  fts_metaclass_alias(fts_new_symbol("sub"), fts_new_symbol("binop"));
  fts_metaclass_alias(fts_new_symbol("mul"), fts_new_symbol("binop"));
  fts_metaclass_alias(fts_new_symbol("div"), fts_new_symbol("binop"));
  fts_metaclass_alias(fts_new_symbol("bus"), fts_new_symbol("binop"));
  fts_metaclass_alias(fts_new_symbol("vid"), fts_new_symbol("binop"));

  sym_const = fts_new_symbol("const");
  sym_static = fts_new_symbol("static");
  sym_store = fts_new_symbol("store");
  sym_trigger = fts_new_symbol("trigger");
  sym_clear = fts_new_symbol("clear");
  sym_inplace = fts_new_symbol("inplace");
  default_keyword = sym_clear;
}
