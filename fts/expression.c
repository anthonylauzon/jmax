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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/parser.h>
#include <ftsprivate/message.h>
#include "parser.h"

static fts_heap_t *expression_heap;

struct _fts_expression_t {
  fts_stack_t stack;            /* evaluation stack */
  fts_parsetree_t *tree;        /* parser abstract tree */
  int fp;                       /* frame pointer */
};

static fts_status_description_t empty_expression_error_description = {
  "Empty expression"
};
static fts_status_t empty_expression_error = &empty_expression_error_description;

static fts_status_description_t undefined_variable_error_description = {
  "Undefined variable"
};
static fts_status_t undefined_variable_error = &undefined_variable_error_description;

static fts_status_description_t operand_type_mismatch_error_description = {
  "Operand type mismatch"
};
static fts_status_t operand_type_mismatch_error = &operand_type_mismatch_error_description;

static fts_status_description_t array_access_error_description = {
  "Array access error"
};
static fts_status_t array_access_error = &array_access_error_description;

static fts_status_description_t invalid_environment_variable_error_description = {
  "Invalid environment variable"
};
static fts_status_t invalid_environment_variable_error = &invalid_environment_variable_error_description;

static fts_status_description_t undefined_class_error_description = {
  "undefined class"
};
static fts_status_t undefined_class_error = &undefined_class_error_description;

static fts_status_description_t object_creation_failed_error_description = {
  "object_creation_failed"
};
static fts_status_t object_creation_failed_error = &object_creation_failed_error_description;

#undef EXPRESSION_DEBUG

static void fts_expression_print( fts_expression_t *exp);

/* **********************************************************************
 *
 * Functions
 *
 */

#if 0
void fts_expression_declare_function( fts_symbol_t name, fts_function_t function)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_pointer( &v, function);
  fts_hashtable_put( &fts_token_table, &k, &v);
}

static void unique_function(int ac, const fts_atom_t *at)
{
  static int seed = 1;
  fts_atom_t ret[1];						\

  fts_set_int( ret, seed++);
  
  fts_return( ret);
}

#define DEFINE_FUN(FUN)						\
static void FUN##_function( int ac, const fts_atom_t *at)	\
{								\
  fts_atom_t ret[1];						\
								\
  if (ac == 1 && fts_is_number( at))				\
    fts_set_float( ret, FUN( fts_get_number_float( at)));	\
								\
  fts_return( ret);						\
}

#define FUN DEFINE_FUN
FUN(sin);
FUN(cos)
FUN(tan)
FUN(asin)
FUN(acos)
FUN(atan)
FUN(sinh)
FUN(cosh)
FUN(tanh)
FUN(asinh)
FUN(acosh)
FUN(atanh)

static void declare_functions( void)
{
#define DECLARE_FUN(FUN)							\
  fts_expression_declare_function( fts_new_symbol( #FUN), FUN##_function);

#define FUN DECLARE_FUN
  FUN(unique);
  FUN(sin);
  FUN(cos);
  FUN(tan);
  FUN(asin);
  FUN(acos);
  FUN(atan);
  FUN(sinh);
  FUN(cosh);
  FUN(tanh);
  FUN(asinh);
  FUN(acosh);
  FUN(atanh);
}
#endif


/* **********************************************************************
 *
 * Expression evaluator
 *
 */

/*
 * The evaluation stack is a stack of atoms, organized into frames.
 *
 * The frame pointer points to the first atom of the frame.
 *
 *
 * top ->        arg2       argument 2              ----------------
 *               arg1       argument 1              
 * fp ->         arg0       argument 0               current frame
 *               savedfp    saved frame pointer     ----------------
 *               arg1'      argument 1              ----------------
 * savedfp ->    arg0'      argument 0               previous frame
 *               savedfp'   saved frame pointer     ----------------
 *
 */

#undef STACK_DEBUG

#define FRAME_OFFSET 1  /* offset of first argument from frame base */

#define expression_stack_push(E,P) fts_stack_push( &(E)->stack, fts_atom_t, *(P))
#define expression_stack_pop(E,N) fts_stack_pop( &(E)->stack, (N))
#define expression_stack_frame(E) ((fts_atom_t *)fts_stack_base(&(E)->stack) + (E)->fp)
#define expression_stack_frame_count(E) (fts_stack_top(&(E)->stack) - (E)->fp + 1)
#define expression_stack_top(E) ((fts_atom_t *)fts_stack_base(&(E)->stack) + fts_stack_top(&(E)->stack))


#ifdef STACK_DEBUG
static void expression_stack_print( fts_expression_t *exp, const char *msg)
{
  int i, fp;
  fts_atom_t *p = (fts_atom_t *)fts_stack_base(&exp->stack);

  fprintf( stderr, "%s:\n", msg);

  fp = exp->fp;

  fprintf( stderr, "fp = %d\n", fp);

  for ( i = fts_stack_top( &exp->stack); i >= 0; i--)
    {
      fprintf( stderr, "[%2d] ", i);
      if ( i == fp - FRAME_OFFSET)
	{
	  fprintf( stderr, "%-7s %d\n", "FP", fts_get_int( p+i));
	  fp = fts_get_int( p+i);
	}
      else if (fts_is_int( p+i))
	fprintf( stderr, "%-7s %d\n", "INT", fts_get_int(p+i));
      else if (fts_is_float( p+i))
	fprintf( stderr, "%-7s %g\n", "FLOAT", fts_get_float(p+i));
      else if (fts_is_void( p+i))
	fprintf( stderr, "%-7s\n", "VOID");
      else if (fts_is_symbol( p+i))
	fprintf( stderr, "%-7s %s\n", "SYMBOL", fts_get_symbol(p+i));
      else if (fts_is_object( p+i))
	fprintf( stderr, "%-7s %s\n", "OBJECT", fts_object_get_class_name( fts_get_object(p+i)));
      else if (fts_is_pointer( p+i))
	fprintf( stderr, "%-7s %p\n", "POINTER", fts_get_pointer(p+i));
    }
}
#endif

static void expression_stack_push_frame( fts_expression_t *exp)
{
  fts_atom_t a;

  /* saved frame pointer */
  fts_set_int( &a, exp->fp);
  expression_stack_push( exp, &a);

  /* get new frame pointer */
  exp->fp = fts_stack_top( &exp->stack) + FRAME_OFFSET;

#ifdef STACK_DEBUG
  expression_stack_print( exp, "Stack after pushing frame");
#endif
}

static void expression_stack_pop_frame( fts_expression_t *exp)
{
  int old_fp;

#ifdef STACK_DEBUG
  expression_stack_print( exp, "Stack before poping frame");
#endif

  old_fp = exp->fp;
  exp->fp = fts_get_int( (fts_atom_t *)fts_stack_base(&exp->stack) + exp->fp - FRAME_OFFSET);
  expression_stack_pop( exp, fts_stack_top( &exp->stack) - old_fp + 1 + FRAME_OFFSET);

#ifdef STACK_DEBUG
  expression_stack_print( exp, "Stack after poping frame");
#endif
}

static void expression_stack_init( fts_expression_t *exp)
{
  fts_stack_clear( &exp->stack);

  exp->fp = -1;

  expression_stack_push_frame( exp);
}


/*
 * Operators
 */

#define UNOP_EVAL(OP)											 \
  if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok) \
    return status;											 \
  top = expression_stack_top( exp);								 \
  if (fts_is_int( top))											 \
    fts_set_int( top, OP fts_get_int( top));								 \
  else if (fts_is_float( top))										 \
    fts_set_float( top, OP fts_get_float( top));							 \
  else													 \
    return operand_type_mismatch_error;

#define IUNOP_EVAL(OP)											 \
  if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok) \
    return status;											 \
  top = expression_stack_top( exp);								 \
  if (fts_is_int( top))											 \
    fts_set_int( top, OP fts_get_int( top));								 \
  else													 \
    return operand_type_mismatch_error;

#define ABINOP_EVAL(OP)											  \
  if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);								  \
  if (fts_is_int( top-1) && fts_is_int( top))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top));					  \
  else if (fts_is_int( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_int( top-1) OP fts_get_float( top));					  \
  else if (fts_is_float( top-1) && fts_is_int( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) OP fts_get_int( top));					  \
  else if (fts_is_float( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) OP fts_get_float( top));				  \
  else													  \
    return operand_type_mismatch_error;								  \
  expression_stack_pop( exp, 1);

#define IABINOP_EVAL(OP)										  \
  if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);								  \
  if (fts_is_int( top) && fts_is_int( top-1))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top) );					  \
  else													  \
    return operand_type_mismatch_error;								  \
  expression_stack_pop( exp, 1);

#define LBINOP_EVAL(OP)											  \
  if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);								  \
  if (fts_is_int( top) && fts_is_int( top-1))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top));					  \
  else if (fts_is_int( top) && fts_is_float( top-1))							  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_float( top));					  \
  else if (fts_is_float( top) && fts_is_int( top-1))							  \
    fts_set_int( top-1, fts_get_float( top-1) OP fts_get_int( top));					  \
  else if (fts_is_float( top) && fts_is_float( top-1))							  \
    fts_set_int( top-1, fts_get_float( top-1) OP fts_get_float( top));					  \
  else													  \
    return operand_type_mismatch_error;								  \
  expression_stack_pop( exp, 1);


fts_status_t expression_eval_aux( fts_parsetree_t *tree, fts_expression_t *exp, fts_patcher_t *scope, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data, int toplevel)
{
  int ac;
  fts_atom_t *at, *top, ret[1];
  fts_status_t status;
  fts_object_t *obj;

  if (!tree)
    return fts_ok;
  
  switch( tree->token) {

  case TK_COMMA:
    if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, toplevel)) != fts_ok)
      return status;

    expression_stack_push_frame( exp);

    if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, toplevel)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);
    expression_stack_pop_frame( exp);

    if ((status = (*callback)( ac, at, data)) != fts_ok)
      return status;

    break;

  case TK_PAR:
    expression_stack_push_frame( exp);

    if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);

    /* FIXME */
    /* When creating an object (tuple or not tuple), when is this object released ? 
       Should we go through the stack when popping a frame and release the objects ? */

    /* Is first term a class ? */
    if (fts_get_class( at) == fts_class_class)
      {
	obj = fts_object_create( (fts_class_t *)fts_get_object( at), NULL, ac-1, at+1);
	fts_object_refer( obj);
	fts_set_object( ret, obj);
      }
    /* Is there is more than one term or are we at toplevel ? If yes, we always create a tuple */
    else if (ac > 1 || toplevel)
      {
	obj = fts_object_create( fts_tuple_class, NULL, ac, at);
	fts_object_refer( obj);
	fts_set_object( ret, obj);
      }
    else
      {
	ret[0] = *at;
      }

    expression_stack_pop_frame( exp);
    expression_stack_push( exp, ret);

    break;

  case TK_SQPAR:
    expression_stack_push_frame( exp);

    if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);

    if (!fts_is_object( at))
      return operand_type_mismatch_error;

    {
      fts_method_t mth = fts_class_get_method( fts_get_class( at), fts_s_get_element);
      if (mth)
	(*mth)( fts_get_object( at), fts_system_inlet, fts_s_get_element, ac-1, at+1);
      else
	return array_access_error;
    }

    expression_stack_pop_frame( exp);
    expression_stack_push( exp, fts_get_return_value());

    break;

  case TK_TUPLE:
    if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;
    break;

  case TK_INT:
  case TK_FLOAT:
  case TK_SYMBOL:
    expression_stack_push( exp, &tree->value);
    break;

  case TK_COLON:
    /* is class cached ? */
    if (fts_is_object( &tree->value))
      expression_stack_push( exp, &tree->value);
    else
      {
	fts_symbol_t package_name = (tree->left) ? fts_get_symbol( &tree->left->value) : NULL;
	fts_symbol_t class_name = fts_get_symbol( &tree->value);
	fts_class_t *cl = fts_class_get_by_name( package_name, class_name);

	if ( !cl)
	  return undefined_class_error;

	fts_set_object( &tree->value, cl);
	
	expression_stack_push( exp, &tree->value);
      }

    break;

  case TK_DOLLAR:
    if (fts_is_int( &tree->value))
      {
	int index = fts_get_int( &tree->value);

	if ( index < env_ac)
	  expression_stack_push( exp, env_at + index);
	else
	  return invalid_environment_variable_error;
      }
    else if (fts_is_symbol( &tree->value))
      {
	fts_atom_t *value;

	value = fts_name_get_value(scope, fts_get_symbol( &tree->value));

	if (fts_is_void(value))
	  return undefined_variable_error;
	else
	  expression_stack_push( exp, value);
      }
    else
      return invalid_environment_variable_error;

    break;

  case TK_POWER:
    if ((status = expression_eval_aux( tree->left, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data, 0)) != fts_ok)
      return status;
    top = expression_stack_top( exp);
    if (fts_is_int( top-1) && fts_is_int( top))
      fts_set_int( top-1, (int)pow( (double)fts_get_int( top-1), (double)fts_get_int( top)));
    else if (fts_is_float( top-1) && fts_is_int( top))
      fts_set_float( top-1, pow( fts_get_float( top-1), (double)fts_get_int( top)));
    else if (fts_is_int( top-1) && fts_is_float( top))
      fts_set_float( top-1, pow( (double)fts_get_int( top-1), fts_get_float( top)));
    else if (fts_is_float( top-1) && fts_is_float( top))
      fts_set_float( top-1, pow( fts_get_float( top-1), fts_get_float( top)));
    else
      return operand_type_mismatch_error;
    expression_stack_pop( exp, 1);
    break;

  case TK_UPLUS:
    UNOP_EVAL(+)
    break;

  case TK_UMINUS:
    UNOP_EVAL(-)
    break;

  case TK_LOGICAL_NOT:
    IUNOP_EVAL(!)
    break;

  case TK_PLUS:
    ABINOP_EVAL(+);
    break;

  case TK_MINUS:
    ABINOP_EVAL(-);
    break;

  case TK_TIMES:
    ABINOP_EVAL(*);
    break;

  case TK_DIV:
    ABINOP_EVAL(/);
    break;

  case TK_PERCENT:
    IABINOP_EVAL(%);
    break;

  case TK_SHIFT_LEFT:
    IABINOP_EVAL(<<);
    break;

  case TK_SHIFT_RIGHT:
    IABINOP_EVAL(>>);
    break;

  case TK_LOGICAL_AND:
    IABINOP_EVAL(&&);
    break;

  case TK_LOGICAL_OR:
    IABINOP_EVAL(||);
    break;

  case TK_EQUAL_EQUAL:
    LBINOP_EVAL(==);
    break;

  case TK_NOT_EQUAL:
    LBINOP_EVAL(!=);
    break;

  case TK_GREATER:
    LBINOP_EVAL(>);
    break;

  case TK_GREATER_EQUAL:
    LBINOP_EVAL(>=);
    break;

  case TK_SMALLER:
    LBINOP_EVAL(<);
    break;

  case TK_SMALLER_EQUAL:
    LBINOP_EVAL(<=);
    break;

#if 0
  case TK_FUNCALL:
    expression_stack_push_frame( exp);

    expression_eval_aux( tree->right, exp, scope, env_ac, env_at, callback, data);

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);

    expression_stack_pop_frame( exp);

    fts_set_void( fts_get_return_value());

    (*(fts_function_t)fts_get_pointer( &tree->value))( ac, at);

    expression_stack_push( exp, fts_get_return_value());

    break;
#endif
  }

  return fts_ok;
}


fts_status_t fts_expression_reduce( fts_expression_t *exp, fts_patcher_t *scope, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data)
{
#ifdef EXPRESSION_DEBUG
  fts_expression_print( exp);
#endif

  if (exp->tree == NULL)
    return empty_expression_error;

  expression_stack_init( exp);

  return expression_eval_aux( exp->tree, exp, scope, env_ac, env_at, callback, data, 1);
}


/* **********************************************************************
 *
 * Expression environment count (maximum value of i in $i terms)
 *
 */

static void get_env_count_aux( fts_parsetree_t *tree, int *count_p)
{
  if (!tree)
    return;

  get_env_count_aux( tree->left, count_p);
  get_env_count_aux( tree->right, count_p);

  if ( tree->token == TK_DOLLAR && fts_is_int( &tree->value))
    {
      int n = fts_get_int( &tree->value) + 1;
      
      if (n > *count_p)
	*count_p = n;
    }
}

int fts_expression_get_env_count( fts_expression_t *exp)
{
  int count = 0;

  get_env_count_aux( exp->tree, &count);
  
  return count;
}

/* **********************************************************************
 *
 * Expression variables user
 *
 */

static void add_variables_user_aux( fts_parsetree_t *tree, fts_patcher_t *scope, fts_object_t *obj)
{
  if (!tree)
    return;

  add_variables_user_aux( tree->left, scope, obj);
  add_variables_user_aux( tree->right, scope, obj);

  if ( tree->token == TK_DOLLAR && fts_is_symbol( &tree->value) )
    fts_name_add_listener(scope, fts_get_symbol( &tree->value), obj);
}

void fts_expression_add_variables_user( fts_expression_t *exp, fts_patcher_t *scope, fts_object_t *obj)
{
  add_variables_user_aux( exp->tree, scope, obj);
}

/* **********************************************************************
 *
 * Debug code
 *
 */

static void expression_print_aux( fts_parsetree_t *tree, int indent)
{
  int i;

  if (!tree)
    return;

  fprintf( stderr, "%d:", indent);

  for ( i = 0; i < indent; i++)
    fprintf( stderr, "   ");

  switch( tree->token) {
  case TK_COMMA: fprintf( stderr, ",\n"); break;
  case TK_TUPLE: fprintf( stderr, "TUPLE\n"); break;
  case TK_INT: fprintf( stderr, "INT %d\n", fts_get_int( &tree->value)); break;
  case TK_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( &tree->value)); break;
  case TK_SYMBOL: fprintf( stderr, "SYMBOL %s\n", fts_get_symbol( &tree->value)); break;
  case TK_PAR: fprintf( stderr, "()\n"); break;
  case TK_CPAR: fprintf( stderr, "{}\n"); break;
  case TK_SQPAR: fprintf( stderr, "[]\n"); break;
  case TK_DOLLAR: 
    if (fts_is_int( &tree->value))
      fprintf( stderr, "$%d\n", fts_get_int( &tree->value)); 
    else if (fts_is_symbol( &tree->value))
      fprintf( stderr, "$%s\n", fts_get_symbol( &tree->value)); 
    break;
  case TK_UPLUS: fprintf( stderr, "+u\n"); break;
  case TK_UMINUS: fprintf( stderr, "-u\n"); break;
  case TK_LOGICAL_NOT: fprintf( stderr, "!\n"); break;
  case TK_PLUS: fprintf( stderr, "+\n"); break;
  case TK_MINUS: fprintf( stderr, "-\n"); break;
  case TK_TIMES: fprintf( stderr, "*\n"); break;
  case TK_DIV: fprintf( stderr, "/\n"); break;
  case TK_POWER: fprintf( stderr, "**\n"); break;
  case TK_PERCENT: fprintf( stderr, "%%\n"); break;
  case TK_SHIFT_LEFT: fprintf( stderr, "<<\n"); break;
  case TK_SHIFT_RIGHT: fprintf( stderr, ">>\n"); break;
  case TK_LOGICAL_AND: fprintf( stderr, "&&\n"); break;
  case TK_LOGICAL_OR: fprintf( stderr, "||\n"); break;
  case TK_EQUAL_EQUAL: fprintf( stderr, "==\n"); break;
  case TK_NOT_EQUAL: fprintf( stderr, "!=\n"); break;
  case TK_GREATER: fprintf( stderr, ">\n"); break;
  case TK_GREATER_EQUAL: fprintf( stderr, ">=\n"); break;
  case TK_SMALLER: fprintf( stderr, "<\n"); break;
  case TK_SMALLER_EQUAL: fprintf( stderr, "<=\n"); break;
  case TK_COLON: fprintf( stderr, ":\n"); break;
  default: fprintf( stderr, "UNKNOWN %d\n", tree->token); 
  }

  expression_print_aux( tree->left, indent+1);
  expression_print_aux( tree->right, indent+1);
}

static void fts_expression_print( fts_expression_t *exp)
{
  expression_print_aux( exp->tree, 0);
}

/* **********************************************************************
 *
 * Expression construction & destruction
 *
 */

fts_status_t fts_expression_new( int ac, const fts_atom_t *at, fts_expression_t **pexp)
{
  *pexp = (fts_expression_t *)fts_heap_zalloc( expression_heap);

  fts_stack_init( &(*pexp)->stack, fts_atom_t);

  return fts_expression_set( *pexp, ac, at);
}

fts_status_t fts_expression_set( fts_expression_t *exp, int ac, const fts_atom_t *at)
{
  fts_status_t status;

  if (exp->tree)
    fts_parsetree_delete( exp->tree);

  if ((status = fts_parsetree_parse( ac, at, &exp->tree)) != fts_ok)
    return status;

  return fts_ok;
}

void fts_expression_delete( fts_expression_t *exp)
{
  if (exp->tree)
    fts_parsetree_delete( exp->tree);

  fts_stack_destroy( &exp->stack);

  fts_heap_free( exp, expression_heap);
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_expression_init( void)
{
  expression_heap = fts_heap_new( sizeof( fts_expression_t));

#if 0
  declare_functions();
#endif
}

