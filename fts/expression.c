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
#include <ftsprivate/parser.h>
#include <ftsprivate/message.h>
#include "parser.h"

static fts_heap_t *expression_heap;

struct _fts_expression_t {
  fts_parsetree_t *tree;        /* parser abstract tree */
  fts_patcher_t *scope;         /* expression scope for variables */
  fts_stack_t stack;            /* evaluation stack */
  int fp;                       /* frame pointer */
};

static fts_status_description_t syntax_error_status_description = {
  "Syntax error"
};
fts_status_t syntax_error_status = &syntax_error_status_description;

static fts_status_description_t undefined_variable_status_description = {
  "Undefined variable"
};
fts_status_t undefined_variable_status = &undefined_variable_status_description;

static fts_status_description_t operand_type_mismatch_status_description = {
  "Operand type mismatch"
};
fts_status_t operand_type_mismatch_status = &operand_type_mismatch_status_description;

static fts_status_description_t array_access_error_status_description = {
  "Array access error"
};
fts_status_t array_access_error_status = &array_access_error_status_description;

static fts_status_description_t invalid_environment_variable_status_description = {
  "Invalid environment variable"
};
fts_status_t invalid_environment_variable_status = &invalid_environment_variable_status_description;

/* #define EXPRESSION_DEBUG */
#undef EXPRESSION_DEBUG

static void fts_expression_print( fts_expression_t *expression);

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
 * fp ->         arg0       argument 2               current frame
 *               savedfp    saved frame pointer     ----------------
 *               arg1'      argument 1              ----------------
 * savedfp ->    arg0'      argument 0               previous frame
 *               savedfp'   saved frame pointer     ----------------
 *
 */

/* #define STACK_DEBUG */
#undef STACK_DEBUG

#define expression_stack_push(E,P) fts_stack_push( &(E)->stack, fts_atom_t, *(P))
#define expression_stack_pop(E,N) fts_stack_pop( &(E)->stack, (N))
#define expression_stack_frame(E) ((fts_atom_t *)fts_stack_base(&(E)->stack) + (E)->fp)
#define expression_stack_frame_count(E) (fts_stack_top(&(E)->stack) - (E)->fp + 1)
#define expression_stack_top(E) ((fts_atom_t *)fts_stack_base(&(E)->stack) + fts_stack_top(&(E)->stack))


#ifdef STACK_DEBUG
static void expression_stack_print( fts_expression_t *expression, const char *msg)
{
  int i, current_fp;
  fts_atom_t *p = (fts_atom_t *)fts_stack_base(&expression->stack);

  fprintf( stderr, "%s:\n", msg);

  current_fp = expression->fp;

  for ( i = fts_stack_top( &expression->stack); i >= 0; i--)
    {
      fprintf( stderr, "[%2d]", i);
      if ( i == current_fp - 1)
	{
	  fprintf( stderr, "* ");
	  current_fp = fts_get_int( p+i);
	}
      else
	fprintf( stderr, "  ");

      if (fts_is_int( p+i))
	fprintf( stderr, "INT %d\n", fts_get_int(p+i));
      else if (fts_is_float( p+i))
	fprintf( stderr, "FLOAT %g\n", fts_get_float(p+i));
      else if (fts_is_void( p+i))
	fprintf( stderr, "VOID\n");
      else if (fts_is_symbol( p+i))
	fprintf( stderr, "SYMBOL %s\n", fts_get_symbol(p+i));
      else if (fts_is_object( p+i))
	fprintf( stderr, "OBJECT %s\n", fts_object_get_class_name( fts_get_object(p+i)));
    }
}
#endif

static void expression_stack_push_frame( fts_expression_t *expression)
{
  fts_atom_t a;

  /* saved frame pointer */
  fts_set_int( &a, expression->fp);
  expression_stack_push( expression, &a);

  /* get new frame pointer */
  expression->fp = fts_stack_top( &expression->stack) + 1;

#ifdef STACK_DEBUG
  expression_stack_print( expression, "Stack after pushing frame");
#endif
}

static void expression_stack_pop_frame( fts_expression_t *expression)
{
  int old_fp;

#ifdef STACK_DEBUG
  expression_stack_print( expression, "Stack before poping frame");
#endif

  old_fp = expression->fp;
  expression->fp = fts_get_int( (fts_atom_t *)fts_stack_base(&expression->stack) + expression->fp - 1);
  expression_stack_pop( expression, fts_stack_top( &expression->stack) - old_fp + 2);

#ifdef STACK_DEBUG
  expression_stack_print( expression, "Stack after poping frame");
#endif
}


/*
 * Operators
 */

#define UNOP_EVAL(OP)											 \
  if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											 \
  top = expression_stack_top( expression);								 \
  if (fts_is_int( top))											 \
    fts_set_int( top, OP fts_get_int( top));								 \
  else if (fts_is_float( top))										 \
    fts_set_float( top, OP fts_get_float( top));							 \
  else													 \
    return operand_type_mismatch_status;

#define IUNOP_EVAL(OP)											 \
  if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											 \
  top = expression_stack_top( expression);								 \
  if (fts_is_int( top))											 \
    fts_set_int( top, OP fts_get_int( top));								 \
  else													 \
    return operand_type_mismatch_status;

#define ABINOP_EVAL(OP)											  \
  if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( expression);								  \
  if (fts_is_int( top-1) && fts_is_int( top))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top));					  \
  else if (fts_is_int( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_int( top-1) OP fts_get_float( top));					  \
  else if (fts_is_float( top-1) && fts_is_int( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) OP fts_get_int( top));					  \
  else if (fts_is_float( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) OP fts_get_float( top));				  \
  else													  \
    return operand_type_mismatch_status;								  \
  expression_stack_pop( expression, 1);

#define IABINOP_EVAL(OP)										  \
  if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( expression);								  \
  if (fts_is_int( top) && fts_is_int( top-1))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top) );					  \
  else													  \
    return operand_type_mismatch_status;								  \
  expression_stack_pop( expression, 1);

#define LBINOP_EVAL(OP)											  \
  if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( expression);								  \
  if (fts_is_int( top) && fts_is_int( top-1))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top));					  \
  else if (fts_is_int( top) && fts_is_float( top-1))							  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_float( top));					  \
  else if (fts_is_float( top) && fts_is_int( top-1))							  \
    fts_set_int( top-1, fts_get_float( top-1) OP fts_get_int( top));					  \
  else if (fts_is_float( top) && fts_is_float( top-1))							  \
    fts_set_int( top-1, fts_get_float( top-1) OP fts_get_float( top));					  \
  else													  \
    return operand_type_mismatch_status;								  \
  expression_stack_pop( expression, 1);


fts_status_t expression_eval_aux( fts_parsetree_t *tree, fts_expression_t *expression, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data)
{
  int ac;
  fts_atom_t *at;
  fts_atom_t *top;
  fts_status_t status;

  if (!tree)
    return fts_ok;
  
  switch( tree->token) {

  case FTS_TOKEN_COMMA:
    if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    expression_stack_push_frame( expression);

    if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( expression);
    at = expression_stack_frame( expression);
    expression_stack_pop_frame( expression);

    (*callback)( ac, at, data);
    break;

  case FTS_TOKEN_TOPLEVEL_PAR:
    expression_stack_push_frame( expression);

    if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( expression);
    at = expression_stack_frame( expression);
    expression_stack_pop_frame( expression);

    {
      fts_atom_t ret[1];
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create( fts_tuple_metaclass, ac, at);

      /* FIXME */
      /* When is this tuple released ? Should we go through the stack when
	 popping a frame and release the tuples ? */
      fts_object_refer( tuple);
      
      fts_set_object( ret, (fts_object_t *)tuple);
      
      expression_stack_push( expression, ret);
    }
    break;

  case FTS_TOKEN_PAR:
    expression_stack_push_frame( expression);

    if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( expression);
    at = expression_stack_frame( expression);
    expression_stack_pop_frame( expression);

    /* we don't create a tuple if there is only one atom in the frame */
    if ( ac == 1)
      expression_stack_push( expression, at);
    else
      {
	fts_atom_t ret[1];
	fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create( fts_tuple_metaclass, ac, at);

	/* FIXME */
	/* When is this tuple released ? Should we go through the stack when
	   popping a frame and release the tuples ? */
	fts_object_refer( tuple);
	
	fts_set_object( ret, (fts_object_t *)tuple);

	expression_stack_push( expression, ret);
      }
    break;

  case FTS_TOKEN_TUPLE:
    if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    break;

  case FTS_TOKEN_INT:
  case FTS_TOKEN_FLOAT:
  case FTS_TOKEN_SYMBOL:
    expression_stack_push( expression, &tree->value);
    break;

  case FTS_TOKEN_DOLLAR:
    if (fts_is_int( &tree->value))
      {
	int index = fts_get_int( &tree->value);

	if ( index < env_ac)
	  expression_stack_push( expression, env_at + index);
	else
	  return invalid_environment_variable_status;
      }
    else if (fts_is_symbol( &tree->value))
      {
	fts_atom_t *p;

	if ((p = fts_variable_get_value( expression->scope, fts_get_symbol( &tree->value))))
	  expression_stack_push( expression, p);
	else
	  return undefined_variable_status;
      }
    break;

  case FTS_TOKEN_POWER:
    if ((status = expression_eval_aux( tree->left, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    top = expression_stack_top( expression);
    if (fts_is_int( top-1) && fts_is_int( top))
      fts_set_int( top-1, (int)pow( (double)fts_get_int( top-1), (double)fts_get_int( top)));
    else if (fts_is_float( top-1) && fts_is_int( top))
      fts_set_float( top-1, pow( fts_get_float( top-1), (double)fts_get_int( top)));
    else if (fts_is_int( top-1) && fts_is_float( top))
      fts_set_float( top-1, pow( (double)fts_get_int( top-1), fts_get_float( top)));
    else if (fts_is_float( top-1) && fts_is_float( top))
      fts_set_float( top-1, pow( fts_get_float( top-1), fts_get_float( top)));
    else
      return operand_type_mismatch_status;
    expression_stack_pop( expression, 1);
    break;

  case FTS_TOKEN_UPLUS:
    UNOP_EVAL(+)
    break;

  case FTS_TOKEN_UMINUS:
    UNOP_EVAL(-)
    break;

  case FTS_TOKEN_LOGICAL_NOT:
    IUNOP_EVAL(!)
    break;

  case FTS_TOKEN_PLUS:
    ABINOP_EVAL(+);
    break;

  case FTS_TOKEN_MINUS:
    ABINOP_EVAL(-);
    break;

  case FTS_TOKEN_TIMES:
    ABINOP_EVAL(*);
    break;

  case FTS_TOKEN_DIV:
    ABINOP_EVAL(/);
    break;

  case FTS_TOKEN_PERCENT:
    IABINOP_EVAL(%);
    break;

  case FTS_TOKEN_SHIFT_LEFT:
    IABINOP_EVAL(<<);
    break;

  case FTS_TOKEN_SHIFT_RIGHT:
    IABINOP_EVAL(>>);
    break;

  case FTS_TOKEN_LOGICAL_AND:
    IABINOP_EVAL(&&);
    break;

  case FTS_TOKEN_LOGICAL_OR:
    IABINOP_EVAL(||);
    break;

  case FTS_TOKEN_EQUAL_EQUAL:
    LBINOP_EVAL(==);
    break;

  case FTS_TOKEN_NOT_EQUAL:
    LBINOP_EVAL(!=);
    break;

  case FTS_TOKEN_GREATER:
    LBINOP_EVAL(>);
    break;

  case FTS_TOKEN_GREATER_EQUAL:
    LBINOP_EVAL(>=);
    break;

  case FTS_TOKEN_SMALLER:
    LBINOP_EVAL(<);
    break;

  case FTS_TOKEN_SMALLER_EQUAL:
    LBINOP_EVAL(<=);
    break;

#if 0
  case FTS_TOKEN_FUNCALL:
    expression_stack_push_frame( expression);

    expression_eval_aux( tree->right, expression, env_ac, env_at, callback, data);

    ac = expression_stack_frame_count( expression);
    at = expression_stack_frame( expression);

    expression_stack_pop_frame( expression);

    fts_set_void( fts_get_return_value());

    (*(fts_function_t)fts_get_pointer( &tree->value))( ac, at);

    expression_stack_push( expression, fts_get_return_value());

    break;
#endif
  }

  return fts_ok;
}


int fts_expression_reduce( fts_expression_t *expression, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data)
{
#ifdef EXPRESSION_DEBUG
  fts_expression_print( expression);
#endif

  expression_eval_aux( expression->tree, expression, env_ac, env_at, callback, data);

  return 1;
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

  switch( tree->token) {
  case FTS_TOKEN_DOLLAR: 
    if ( fts_is_int( &tree->value))
      {
	int n = fts_get_int( &tree->value) + 1;

	if (n > *count_p)
	  *count_p = n;
      }
    break;
  }
}

int fts_expression_get_env_count( fts_expression_t *expression)
{
  int count = 0;

  get_env_count_aux( expression->tree, &count);
  
  return count;
}

/* **********************************************************************
 *
 * Debug code
 *
 */

static void parsetree_print_aux( fts_parsetree_t *tree, int indent)
{
  int i;

  if (!tree)
    return;

  fprintf( stderr, "%d:", indent);

  for ( i = 0; i < indent; i++)
    fprintf( stderr, "   ");

  switch( tree->token) {
  case FTS_TOKEN_COMMA: fprintf( stderr, ",\n"); break;
  case FTS_TOKEN_TUPLE: fprintf( stderr, "TUPLE\n"); break;
  case FTS_TOKEN_INT: fprintf( stderr, "INT %d\n", fts_get_int( &tree->value)); break;
  case FTS_TOKEN_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( &tree->value)); break;
  case FTS_TOKEN_SYMBOL: fprintf( stderr, "SYMBOL %s\n", fts_get_symbol( &tree->value)); break;
  case FTS_TOKEN_PAR: fprintf( stderr, "()\n"); break;
  case FTS_TOKEN_CPAR: fprintf( stderr, "{}\n"); break;
  case FTS_TOKEN_SQPAR: fprintf( stderr, "[]\n"); break;
  case FTS_TOKEN_DOLLAR: 
    if (fts_is_int( &tree->value))
      fprintf( stderr, "$%d\n", fts_get_int( &tree->value)); 
    else if (fts_is_symbol( &tree->value))
      fprintf( stderr, "$%s\n", fts_get_symbol( &tree->value)); 
    break;
  case FTS_TOKEN_UPLUS: fprintf( stderr, "+\n"); break;
  case FTS_TOKEN_UMINUS: fprintf( stderr, "-\n"); break;
  case FTS_TOKEN_LOGICAL_NOT: fprintf( stderr, "!\n"); break;
  case FTS_TOKEN_PLUS: fprintf( stderr, "+\n"); break;
  case FTS_TOKEN_MINUS: fprintf( stderr, "-\n"); break;
  case FTS_TOKEN_TIMES: fprintf( stderr, "*\n"); break;
  case FTS_TOKEN_DIV: fprintf( stderr, "/\n"); break;
  case FTS_TOKEN_POWER: fprintf( stderr, "**\n"); break;
  case FTS_TOKEN_PERCENT: fprintf( stderr, "%%\n"); break;
  case FTS_TOKEN_SHIFT_LEFT: fprintf( stderr, "<<\n"); break;
  case FTS_TOKEN_SHIFT_RIGHT: fprintf( stderr, ">>\n"); break;
  case FTS_TOKEN_LOGICAL_AND: fprintf( stderr, "&&\n"); break;
  case FTS_TOKEN_LOGICAL_OR: fprintf( stderr, "||\n"); break;
  case FTS_TOKEN_EQUAL_EQUAL: fprintf( stderr, "==\n"); break;
  case FTS_TOKEN_NOT_EQUAL: fprintf( stderr, "!=\n"); break;
  case FTS_TOKEN_GREATER: fprintf( stderr, ">\n"); break;
  case FTS_TOKEN_GREATER_EQUAL: fprintf( stderr, ">=\n"); break;
  case FTS_TOKEN_SMALLER: fprintf( stderr, "<\n"); break;
  case FTS_TOKEN_SMALLER_EQUAL: fprintf( stderr, "<=\n"); break;
  }

  parsetree_print_aux( tree->left, indent+1);
  parsetree_print_aux( tree->right, indent+1);
}

static void fts_expression_print( fts_expression_t *expression)
{
  parsetree_print_aux( expression->tree, 0);
}

/* **********************************************************************
 *
 * Expression construction & destruction
 *
 */

fts_expression_t *fts_expression_new( int ac, const fts_atom_t *at, fts_patcher_t *scope)
{
  fts_expression_t *expression;

  expression = (fts_expression_t *)fts_heap_zalloc( expression_heap);

  fts_expression_set( expression, ac, at, scope);
  fts_stack_init( &expression->stack, fts_atom_t);
  expression->fp = 0;

  return expression;
}

void fts_expression_set( fts_expression_t *expression, int ac, const fts_atom_t *at, fts_patcher_t *scope)
{
  if (expression->scope)
    fts_object_release( expression->scope);

  if (expression->tree)
    fts_parsetree_delete( expression->tree);

  if (ac >= 1)
    expression->tree = fts_parsetree_new( ac, at);
  else
    expression->tree = 0;

  expression->scope = scope;
  
  fts_object_refer( expression->scope);
}

void fts_expression_delete( fts_expression_t *expression)
{
  if (expression->tree)
    fts_parsetree_delete( expression->tree);

  fts_object_release( expression->scope);

  fts_stack_destroy( &expression->stack);

  fts_heap_free( expression, expression_heap);
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

