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


/*
 * The FTS expression evaluator
 * 
 * the standard C binary arithmetic operators ( + - * / % ) 
 *    (+ and - unary also), floats and ints, '+' also as symbol concatenation
 * the standard C binary bitwise operators ( ~ & | ^  << >> )
 *   (~ unary only), ints only.
 * the standard C binary logical operators ( && || ! )
 *   (! unary only), ints only.
 *
 * the standard C binary comparison operators ( == != > >=  < <= )
 *  == and != on all types, others on ints and floats.
 *
 * The standard C conditional expression ( ? : )
 *  (the conditional must be an int, the arguments any type).
 *
 * The FTS '$' get variable operator (on a symbol only, for the moment)
 * 
 * The FTS '.' get property or nested variable of an object operator.
 *
 * The FTS ''' evaluate to the next atom in the expression, also if the atom
 * is a reserved word; note that this is not an operator, but a meta-linguistic
 * operartor, like parentesys.
 * 
 * A syntax for constant tuples definition { foo, bar, zoo }
 *
 * Warning: in the current implementation, "? :", and logical "&&" and "||"
 * eval all the arguments before giving the result; this prevent the use
 * of this expression evaluator for computing recursive functions !!
 * Since there are no side effects in expression evaluators, there should be no other
 * problems with this.
 */

/*
  EXPRESSION_TRACE_DEBUG, if defined, cause some debug printouts
  before, while and after evaluating an expression
  */
/*
  #define EXPRESSION_TRACE_DEBUG 
*/

#include <fts/fts.h>
#include <ftsprivate/OLDexpression.h>
#include <ftsprivate/variable.h>
#include <ftsprivate/patcher.h>

/* Operator codes: symbols can store a special operator code,
   to identify them as operators, and to do direct access to some
   system table; the constant are defined here, and are put in the
   symbol in the expression init function.
 */

#define FTS_OP_PLUS          1
#define FTS_OP_MINUS         2
#define FTS_OP_TIMES         3
#define FTS_OP_DIV           4
#define FTS_OP_DOLLAR        5
#define FTS_OP_DOT           6
#define FTS_OP_REMAINDER     7
#define FTS_OP_SHIFT_LEFT    8
#define FTS_OP_SHIFT_RIGHT   9
#define FTS_OP_BIT_AND      10
#define FTS_OP_BIT_OR       11
#define FTS_OP_BIT_XOR      12
#define FTS_OP_BIT_NOT      13
#define FTS_OP_LOGICAL_AND  14
#define FTS_OP_LOGICAL_OR   15
#define FTS_OP_LOGICAL_NOT  16
#define FTS_OP_EQUAL        17
#define FTS_OP_NOT_EQUAL    18
#define FTS_OP_GREATER      19 
#define FTS_OP_GREATER_EQUAL 20
#define FTS_OP_SMALLER       21
#define FTS_OP_SMALLER_EQUAL 22
#define FTS_OP_CONDITIONAL   23
#define FTS_OP_ELSE          24
#define FTS_OP_ASSIGN        25
#define FTS_OP_ARRAY_REF     26
#define FTS_OP_FIRST_UNUSED  28

/* Codes to identify unary and binary operators in the operator stack */

#define FTS_BINARY_OP_TYPE 0
#define FTS_UNARY_OP_TYPE  1

/* constants and limits */

#define FTS_EXPR_MAX_SYMBOL_LENGTH 2048

#define FTS_EXPR_MAX_DEPTH 32

/* Operator priority and type: direct access tables giving 
   priority and type of operators.
 */

static int op_priority[FTS_OP_FIRST_UNUSED];
static int op_unary[FTS_OP_FIRST_UNUSED];
static int op_binary[FTS_OP_FIRST_UNUSED];



/* Convenience macro for checking symbol atoms  */

#define fts_is_operator(a)        (fts_is_symbol((a)) && fts_symbol_is_operator(fts_get_symbol(a)))
#define fts_get_operator(a)        (fts_symbol_get_operator(fts_get_symbol(a)))

/* Operator type and priority */

#define fts_op_before(op1, op2)   (op_priority[(op1)] >= op_priority[(op2)]) 
#define fts_op_can_be_unary(op)  (op_unary[(op)])
#define fts_op_can_be_binary(op) (op_binary[(op)])

/* Utility macros */

#define fts_is_open_par(a)       (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_open_par))
#define fts_is_closed_par(a)     (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_closed_par))
#define fts_is_open_sqpar(a)     (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_open_sqpar))
#define fts_is_closed_sqpar(a)   (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_closed_sqpar))
#define fts_is_open_cpar(a)     (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_open_cpar))
#define fts_is_closed_cpar(a)   (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_closed_cpar))
#define fts_is_comma(a)          (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_comma))
#define fts_is_dollar(a)         (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_dollar))
#define fts_is_dot(a)            (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_dot)) 
#define fts_is_assign(a)         (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_equal))
#define fts_is_quote(a)         (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_quote))

/* Function hash table */

static fts_hashtable_t fts_expression_fun_table;

/* Structure used to store  assignements in expression states */

struct fts_expression_assignement
{
  fts_symbol_t name;
  fts_atom_t   value;
  struct fts_expression_assignement *next;
};

static fts_heap_t *expr_prop_heap;

/* Structure used to store  reference to variables in expression states */

typedef struct fts_expr_var_ref
{
  fts_symbol_t name;
  fts_patcher_t *scope;
  struct fts_expr_var_ref *next;
} fts_expr_var_ref_t;

static fts_heap_t *expr_var_ref_heap;

/* The espression type, with the relative macros/functions.
 *
 * This structure keep the state or the evaluation and side result of the expression
 * evaluation; used variables, property to store, and so on, and of course the 
 * evaluation stacks
 * Stacks grow up.
 */

struct fts_expression_state
{
  fts_atom_t value_stack[FTS_EXPR_MAX_DEPTH];
  int value_stack_p;	/* point to the last actually used cell */

  int op_stack[FTS_EXPR_MAX_DEPTH];
  int op_type_stack[FTS_EXPR_MAX_DEPTH];
  int op_stack_p;		/* point to the last actually used cell */

  int expr_size;		/* the expression list being parsed */
  const fts_atom_t *expr; 
  fts_patcher_t    *scope;	/* the scope where the expression should be evaluated */

  int in;			/* current input atom in expression list*/
  int count;			/* result counter */
  int ret;			/* return code */
  const char *err_arg;		/* argument for error messages */
  const char *msg;		/* error message */
  fts_expression_assignement_t *assignements;	/* properties expressed in the expression */

  fts_expr_var_ref_t *var_refs;	/* list of variable names referred  in the expression */
};

/* 
 * utility function to check nullity of an atom:
 * a null content can be a null pointer or a zero value 
 */
static int atom_is_null( const fts_atom_t *a)
{
  if (fts_is_void(a))
    return 1;
  else if (fts_is_symbol(a))
    return fts_get_symbol(a) == 0;
  else if (fts_is_string(a))
    return fts_get_string(a) == 0;
  else if (fts_is_pointer(a))
    return fts_get_pointer(a) == 0;
  else if (fts_is_int(a))
    return fts_get_int(a) == 0;
  else if (fts_is_float(a))
    return fts_get_float(a) == 0.0;
  else if (fts_is_object(a))
    return fts_get_object(a) == 0;
  else
    return 0;
}

/* Error utility */
static int expression_error(fts_expression_state_t *e, int err, const char *msg, const char *arg)
{
  e->msg = msg;
  e->err_arg = arg;
  e->ret = err;
  return err;
}

/* operator stack manipulation */

static void op_stack_push(fts_expression_state_t *e, int op, int op_type)
{
  e->op_stack_p++;
  e->op_stack[e->op_stack_p] = op;
  e->op_type_stack[e->op_stack_p] = op_type;
}

static void op_stack_pop(fts_expression_state_t *e)
{
  e->op_stack_p--;
}

static int op_stack_is_empty(fts_expression_state_t *e)
{
  return (e->op_stack_p == -1);
}

static int op_stack_deeper_than(fts_expression_state_t *e, int n)
{
  return (e->op_stack_p >= (n - 1));
}

static void op_stack_set_empty(fts_expression_state_t *e)
{
  e->op_stack_p = -1;
}

static int op_stack_top(fts_expression_state_t *e)
{
  return e->op_stack[e->op_stack_p];
}

static int op_stack_top_p(fts_expression_state_t *e)
{
  return e->op_stack_p;
}

static int op_stack_peek(fts_expression_state_t *e, int i)
{
  return e->op_stack[e->op_stack_p - i];
}

static int op_type_stack_top(fts_expression_state_t *e)
{
  return e->op_type_stack[e->op_stack_p];
}

/* Value stack manipulation */

static void value_stack_push(fts_expression_state_t *e, const fts_atom_t *value)
{
  e->value_stack_p++;
  e->value_stack[e->value_stack_p] = *value;
}

static void value_stack_pop(fts_expression_state_t *e, int pos)
{
  e->value_stack_p -= pos;
}

static int value_stack_is_empty(fts_expression_state_t *e)
{
  return (e->value_stack_p == -1);
}

static int value_stack_is_deep(fts_expression_state_t *e, int n)
{
  return (e->value_stack_p == (n - 1));
}

static int value_stack_deeper_than(fts_expression_state_t *e, int n)
{
  return (e->value_stack_p >= (n - 1));
}

static void value_stack_set_empty(fts_expression_state_t *e)
{
  e->value_stack_p = -1;
}

static fts_atom_t *value_stack_top(fts_expression_state_t *e)
{
  return &(e->value_stack[e->value_stack_p]);
}

static int value_stack_top_p(fts_expression_state_t *e)
{
  return e->value_stack_p;
}

static fts_atom_t *value_stack_peek(fts_expression_state_t *e, int i)
{
  return &(e->value_stack[e->value_stack_p - i]);
}

static const fts_atom_t *current_in(fts_expression_state_t *e)
{
  return &(e->expr[e->in]);
}

static int next_in(fts_expression_state_t *e)
{
  return e->in++; 
}

static int more_in(fts_expression_state_t *e)
{
  return (e->in < e->expr_size);
}

/* expression function related functions */

static fts_expression_fun_t get_expression_fun(fts_symbol_t name)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(&fts_expression_fun_table, &k, &data))
    return (fts_expression_fun_t) fts_get_pointer(&data);
  else
    return (fts_expression_fun_t) 0;
}

void fts_expression_declare_fun(fts_symbol_t name, fts_expression_fun_t f)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, name);
  fts_set_pointer(&v, f);
  fts_hashtable_put(&fts_expression_fun_table, &k, &v);
}


static int fts_op_eval(fts_expression_state_t *e);
static void fts_expression_add_assignement(fts_expression_state_t *e, fts_symbol_t name, fts_atom_t *value);
static void fts_expression_add_var_ref(fts_expression_state_t *e, fts_patcher_t *scope, fts_symbol_t name);

static fts_heap_t *expr_state_heap;


/* Create a new initiaized expression state */

static fts_expression_state_t *fts_expression_state_new(fts_patcher_t *scope,
							int expr_size, const fts_atom_t *expr)
{
  fts_expression_state_t *expr_state;

  expr_state = (fts_expression_state_t *) fts_heap_alloc(expr_state_heap);

  value_stack_set_empty(expr_state);
  op_stack_set_empty(expr_state);
  expr_state->expr         = expr; 
  expr_state->expr_size    = expr_size;
  expr_state->scope        = scope;
  expr_state->in           = 0;		
  expr_state->count        = 0;	
  expr_state->assignements = 0;	
  expr_state->var_refs     = 0;	
  expr_state->ret          = FTS_EXPRESSION_OK;	

  return expr_state;
}

/* Free the expression state */

void fts_expression_state_free(fts_expression_state_t *e)
{
  fts_expression_assignement_t *p;
  fts_expr_var_ref_t *v;

  /* Free the assignement list  */

  p = e->assignements;

  while (p)
    {
      fts_expression_assignement_t *p2;
      
      p2 = p;
      p = p->next;
      fts_heap_free((char *) p2, expr_prop_heap);
    }

  /* Free the used variable list */

  v = e->var_refs;

  while (v)
    {
      fts_expr_var_ref_t *v2;
      
      v2 = v;
      v = v->next;
      fts_heap_free((char *) v2, expr_var_ref_heap);
    }

  /* Free the expression state */

  fts_heap_free((char *) e, expr_state_heap);
}


/*
  Evaluate a single expression.
  Put the result in the top of the value stack
  (eat one value stack element, then).
  Return an error code, 0 if ok.

  For the moment, due the '=' operator, may not return a value.
  */


/* Macro to wrap around calls to fts_expression_eval_one and fts_op_eval *only* */

#define TRY(call)             {int ret; ret = (call); if (ret != FTS_EXPRESSION_OK)  return ret;}


static int fts_expression_eval_one(fts_expression_state_t *e)
{
  int op_stack_tos;
  int value_stack_tos;
  enum {waiting_arg, waiting_op, expression_end} status; 

  /* A small stack base finite state machine, using operator
     precedence for the evaluation, and recursive call for parsing
     the content of parenthesys, the indexes of tuples and the 
     argument of functions.
   */

  status = waiting_arg;

  op_stack_tos = op_stack_top_p(e);
  value_stack_tos = value_stack_top_p(e);

  while (status != expression_end)
    {
      if (status == waiting_arg)
	{
	  if (fts_is_closed_par(current_in(e)))
	    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Unbalanced closed parentesis", 0);	
	  else if (fts_is_operator(current_in(e)))
	    {
	      /* Unary operator, push it and still wait for an argument */
	      if (fts_op_can_be_unary(fts_get_operator(current_in(e))))
		op_stack_push(e, fts_get_operator(current_in(e)), FTS_UNARY_OP_TYPE);
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					"Syntax error, expression start with operator", 0);
	    }
	  else  if (fts_is_quote(current_in(e)))
	    {
	      /* Quoted value, eval to the value itself */
	      next_in(e);

	      if (more_in(e))
		{
		  value_stack_push(e, current_in(e));

		  /* next_in(e); */
		  status = waiting_op;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Syntax error, value missing after quote", 0);
	    }
	  else  if (fts_is_open_cpar(current_in(e)))
	    {

	      /* TUPLE constant: note that we don't handle freeing the tuple,
		 so somebody should free it !!!
	       */
	      fts_atom_t *tos;
	      int args;
	      fts_atom_t result;
	      fts_tuple_t *tuple;

	      tos = value_stack_top(e);
	      args = 0;
		  
	      next_in(e);

	      while (more_in(e) && (! fts_is_closed_cpar(current_in(e))))
		{
		  /* Evaluate the expression arguments, and push them in the value stack */
		  TRY(fts_expression_eval_one(e));

		  args++;
		}

	      /* Make the tuple */
	      tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, args, tos + 1);
	      fts_object_refer(tuple);

	      /* set result */
	      fts_set_tuple(&result, tuple);

	      /* Pop the stack, and push the result */
	      value_stack_pop(e, args);
	      value_stack_push(e, &result);

	      status = waiting_op;
	    }
	  else
	    {
	      if (fts_is_open_par(current_in(e)))
		{
		  /* Parentesized sub-expression;
		     recursively call this function, that push the result
		     value on the value stack */
		  e->in++;
		  TRY(fts_expression_eval_one(e));

		  if (! fts_is_closed_par(current_in(e)))
		    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Unbalanced parentesis", 0);	

		  status = waiting_arg;
		}
	      else
		{
		  /* Anything else is a constant argument */
		  value_stack_push(e, current_in(e));
		}

	      /* Compute all the pending unary operators  */
	      while ((! op_stack_is_empty(e)) && (op_type_stack_top(e) == FTS_UNARY_OP_TYPE))
		{
		  TRY(fts_op_eval(e));
		  op_stack_pop(e);
		}

	      status = waiting_op;
	    }
	}
      else
	{
	  /* We are waiting for an operator */

	  if (fts_is_open_sqpar(current_in(e)))
	    {
	      /* Array indexing operator */

	      next_in(e);

	      /* Compute the index, and leave it in the value stack */

	      TRY(fts_expression_eval_one(e));

	      if (! fts_is_closed_sqpar(current_in(e)))
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR,
					"Syntax error in Array access",
					0);

	      /* Push the array reference operator in the op stack */

	      op_stack_push(e, FTS_OP_ARRAY_REF, FTS_BINARY_OP_TYPE);
	    }
	  else if (fts_is_open_par(current_in(e)))
	    {
	      /* Function call */

	      if (value_stack_top(e) && fts_is_symbol(value_stack_top(e)))
		{
		  fts_symbol_t name;
		  fts_atom_t *tos;
		  int args;
		  fts_atom_t result;

		  tos = value_stack_top(e);
		  name = fts_get_symbol(value_stack_top(e));
		  args = 1;	/* count and pass also the function name */
		  
		  next_in(e);	/* Skip the parentesys */

		  while (more_in(e) && (! fts_is_closed_par(current_in(e))))
		    {

		      /* Evaluate the expression arguments, and push them in the value stack */

		      TRY(fts_expression_eval_one(e));

		      /* Skip the comma if any (little HACK !!) */
		      
		      if (fts_is_comma(current_in(e)))
			next_in(e);

		      args++;
		    }


		  /* call the function */
		  {
		    fts_expression_fun_t f;

		    f = get_expression_fun(name);

		    if (f)
		      {
			int ret;

			ret = (* f)(args, tos, &result);

			if (ret != FTS_EXPRESSION_OK)
			  return expression_error(e, ret, "Error in function %s",
						  name);
		      }
		    else
		      return expression_error(e, FTS_EXPRESSION_UNDEFINED_FUNCTION,
					      "Undefined function %s",
					      name);
		  }

		  /* Pop the stack, and push the result */

		  value_stack_pop(e, args);
		  value_stack_push(e, &result);

		  status = waiting_op;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR,
					"Syntax error in function call", 0);
	    }
	  else if (fts_is_operator(current_in(e)) && (! fts_is_assign(current_in(e))))
	    {
	      int op;

	      op = fts_get_operator(current_in(e));
	      
	      if (fts_op_can_be_binary(op))
		{
		  /* execute the pending binary ops with lower precedence  */

		  while (fts_op_before(op_stack_top(e), op) &&
			 (op_stack_top_p(e) != op_stack_tos))
		    {
		      TRY(fts_op_eval(e));
		      op_stack_pop(e);
		    }

		  /* Push the new op */

		  op_stack_push(e, op, FTS_BINARY_OP_TYPE);
		  status = waiting_arg;
		}
	      else
		status = expression_end;
	    }
	  else
	    status = expression_end;
	}

      /* Move to next token, if any */
      if (status != expression_end)
	next_in(e);

      if (! more_in(e))
	status = expression_end;

    }

  /* Compute all the pending operators */

  while (op_stack_top_p(e) != op_stack_tos)
    {
      TRY(fts_op_eval(e));
      op_stack_pop(e);
    }

  /* this should change by cutting the assignements out of expressions */

  if (value_stack_top_p(e) == value_stack_tos + 1)
    return FTS_EXPRESSION_OK;
  else
    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
			    "Syntax error in expression", 0);
}


/* 
 * Evaluate a simplified expression; to avoid introducing ambiguities 
 * in the FTS object language, and to keep the language backward compatibility
 * we accept only simplified expression for objects; to use a complex 
 * expression, you need to put it within parentesys.
 *
 * A simplified expression is a constant, a parentesized subexpression
 * or an expression including only the '$', the '.' and the '[]' operators.
 * 
 * It is implemented by a copy of the full parser; it can actually be 
 * substituted by one argument and some check more added to the basic parser.
 */

static int fts_expression_eval_simple(fts_expression_state_t *e)
{
  int op_stack_tos;
  int value_stack_tos;
  enum {waiting_arg, waiting_op, expression_end} status; 

  /* A small stack base finite state machine, using operator
     precedence for the evaluation, and recursive call for parsing
     the content of parenthesys, the indexes of arrays and the 
     argument of functions.
   */

  status = waiting_arg;

  op_stack_tos = op_stack_top_p(e);
  value_stack_tos = value_stack_top_p(e);

  while (status != expression_end)
    {
      if (status == waiting_arg)
	{
	  /* Dollars are the only unary operator accepted in simple expressions */
	  if (fts_is_closed_par(current_in(e)))
	    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Unbalanced closed parentesis", 0);	
	  else if (fts_is_dollar(current_in(e)))
	    {
	      /* Unary operator, push it and still wait for an argument */
	      op_stack_push(e, fts_get_operator(current_in(e)), FTS_UNARY_OP_TYPE);
	    }
	  else  if (fts_is_quote(current_in(e)))
	    {
	      next_in(e);

	      if (more_in(e))
		{
		  value_stack_push(e, current_in(e));

		  next_in(e);
		  status = waiting_op;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					"Syntax error, value missing after quote", 0);
	    }
	  else  if (fts_is_open_cpar(current_in(e)))
	    {
	      /* TUPLE constant: note that we don't handle freeing the tuple,
		 so somebody should free it !!!
	       */
	      fts_atom_t *tos;
	      int args;
	      fts_atom_t result;
	      fts_tuple_t *tuple;

	      tos = value_stack_top(e);
	      args = 0;
		  
	      next_in(e);
		  
	      while (more_in(e) && (! fts_is_closed_cpar(current_in(e))))
		{
		  if (! more_in(e))
		    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Syntax error in tuple constant", 0);

		  if (fts_is_closed_cpar(current_in(e)))
		    break;	

		  /* Evaluate the expression arguments, and push them in the value stack */
		  TRY(fts_expression_eval_one(e));

		  /* Skip the comma if any (little HACK !! for compatibility) */
		  if (fts_is_comma(current_in(e)))
		    next_in(e);

		  args++;
		}

	      /* Make the tuple */
	      tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, args, tos + 1);
	      fts_object_refer(tuple);

	      /* set result */
	      fts_set_tuple(&result, tuple);

	      /* Pop the stack, and push the result */
	      value_stack_pop(e, args);
	      value_stack_push(e, &result);

	      status = waiting_op;
	    }
	  else
	    {
	      if (fts_is_open_par(current_in(e)))
		{
		  /* Parentesized sub-expression;
		     recursively call this function, that push the result
		     value on the value stack */
		  e->in++;
		  TRY(fts_expression_eval_one(e));

		  if (! fts_is_closed_par(current_in(e)))
		    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Unbalanced parentesis", 0);	

		  status = waiting_arg;
		}
	      else
		{
		  /* Anything else is a constant argument */
		  value_stack_push(e, current_in(e));
		}

	      /* Compute all the pending unary operators  */
	      while ((! op_stack_is_empty(e)) && (op_type_stack_top(e) == FTS_UNARY_OP_TYPE))
		{
		  TRY(fts_op_eval(e));
		  op_stack_pop(e);
		}

	      status = waiting_op;
	    }
	}
      else
	{
	  /* We are waiting for an operator */

	  /* We accept only the '.' operator and the array access syntax */
	  if (fts_is_open_sqpar(current_in(e)))
	    {
	      /* Array access */
	      next_in(e);

	      /* Compute the index, and leave it in the value stack */
	      TRY(fts_expression_eval_one(e));

	      if (! fts_is_closed_sqpar(current_in(e)))
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Syntax error in Array access", 0);

	      /* Push the array reference operator in the op stack */
	      op_stack_push(e, FTS_OP_ARRAY_REF, FTS_BINARY_OP_TYPE);
	    }
	  else if (fts_is_operator(current_in(e)))
	    {
	      if (fts_is_dot(current_in(e)) || fts_is_assign(current_in(e)))
		{
		  int op;

		  op = fts_get_operator(current_in(e));
	      
		  /* execute the pending binary ops with lower precedence  */
		  while (fts_op_before(op_stack_top(e), op) &&
			 (op_stack_top_p(e) != op_stack_tos))
		    {
		      TRY(fts_op_eval(e));
		      op_stack_pop(e);
		    }

		  /* Push the new op */
		  op_stack_push(e, op, FTS_BINARY_OP_TYPE);
		  status = waiting_arg;
		}
	      else
		status = expression_end;
	    }
	  else
	    status = expression_end;
	}

      /* Move to next token, if any */
      if(status != expression_end)
	next_in(e);

      if(!more_in(e))
	status = expression_end;

    }

  /* Compute all the pending operators */

  while (op_stack_top_p(e) != op_stack_tos)
    {
      TRY(fts_op_eval(e));
      op_stack_pop(e);
    }

  /* this should change by cutting the assignements out of expressions */

  if ((value_stack_top_p(e) == value_stack_tos + 1) ||
      (value_stack_top_p(e) == value_stack_tos))
    return FTS_EXPRESSION_OK;
  else
    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
			    "Syntax error in expression", 0);
}


/* This thing work only for ints, for now !!!, no type promotion !! */

#define fts_get_float_num(a)   (fts_is_float((a)) ? fts_get_float((a)) : (float) fts_get_int((a)))

/* Apply the top of the stack operator to the top of the stacks values.
   Do type promotion. 

   Note that the interpretation of the operator is made using a switch;
   since the operator indexes are contiguous, is very likely that this code
   generate a jump table, and it is surely faster and more efficent than
   to call a function thru a pointer table 

   The negative part is that type promotion is done for each operator;
   code should be factorized here.
*/

static int fts_op_eval(fts_expression_state_t *e)
{
  int op = op_stack_top(e);
  int pop = op_stack_peek(e, 1);
  int op_type = op_type_stack_top(e);

#ifdef EXPRESSION_TRACE_DEBUG
  fts_log( "Evaluting op %d\n", op);
#endif

  if (op_type == FTS_UNARY_OP_TYPE)
    {
      fts_atom_t *tos = value_stack_top(e);

      switch (op)
	{
	case FTS_OP_DOLLAR:
	  if (fts_is_symbol(tos))
	    {
	      fts_atom_t *value;
	      fts_symbol_t varname;

	      varname = fts_get_symbol(tos);
	      value = fts_variable_get_value_or_void(e->scope, varname);

	      if (value)
		{
		  fts_expression_add_var_ref(e, e->scope, varname);

		  if (fts_is_void(value))
		    return expression_error(e, FTS_EXPRESSION_UNDEFINED_VARIABLE, "Variable %s is undefined",
					    varname);
		  else
		    *tos = *value;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_UNDEFINED_VARIABLE, "Variable %s is undefined",
					varname);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Syntax Error", 0);
	  break;

	case FTS_OP_PLUS:
	  /* NOP, really */
	  if (! (fts_is_int(tos) || fts_is_float(tos)))
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator +", 0);
	  break;

	case FTS_OP_MINUS:
	  if (fts_is_int(tos))
	    fts_set_int(tos, (-1) * fts_get_int(tos));
	  else if (fts_is_float(tos))
	    fts_set_float(tos, (-1) * fts_get_float(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for  operator -", 0);
	  break;

	case FTS_OP_BIT_NOT:
	  if (fts_is_int(tos))
	    fts_set_int(tos, (~ fts_get_int(tos)));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator ~", 0);

	case FTS_OP_LOGICAL_NOT:
	  if (fts_is_int(tos))
	    fts_set_int(tos, (! fts_get_int(tos)));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator !", 0);

	default:
	  return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Syntax error", 0);
	}

      return FTS_EXPRESSION_OK;
    }
  else
    {
      /* Warning: assume that value_stack_pop do not overwrite the stack;
	 of course is dirty, but is faster than copying the atom around */

      fts_atom_t *tos;
      fts_atom_t *ptos;

      tos = value_stack_top(e);
      value_stack_pop(e, 1);
      ptos = value_stack_top(e);

      switch (op)
	{
	case FTS_OP_PLUS:
	  if (fts_is_symbol(tos) || fts_is_symbol(ptos))
	    {
	      char buf[FTS_EXPR_MAX_SYMBOL_LENGTH];

	      if (fts_is_symbol(tos))
		{
		  if (fts_is_symbol(ptos))
		    sprintf(buf, "%s%s",
			    fts_get_symbol(ptos),
			    fts_get_symbol(tos));
		  else if (fts_is_int(ptos))
		    sprintf(buf, "%d%s",
			    fts_get_int(ptos),
			    fts_get_symbol(tos));
		  else if (fts_is_float(ptos))
		    sprintf(buf, "%f%s",
			    fts_get_float(ptos),
			    fts_get_symbol(tos));
		}
	      else if (fts_is_int(tos))
		sprintf(buf, "%s%d",
			fts_get_symbol(ptos),
			fts_get_int(tos));
	      else if (fts_is_float(tos))
		sprintf(buf, "%s%f",
			fts_get_symbol(ptos),
			fts_get_float(tos));

	      fts_set_symbol(ptos, fts_new_symbol_copy(buf));
	    }
	  else
	    {
	      if (fts_is_float(tos))
		{
		  if (fts_is_float(ptos))
		    fts_set_float(ptos, fts_get_float(ptos) + fts_get_float(tos));
		  else if (fts_is_int(ptos))
		    fts_set_float(ptos, ((float) fts_get_int(ptos)) + fts_get_float(tos));
		  else
		    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator +", 0);
		}
	      else if (fts_is_int(tos))
		{
		  if (fts_is_float(ptos))
		    fts_set_float(ptos, fts_get_float(ptos) + ((float) fts_get_int(tos)));
		  else if (fts_is_int(ptos))
		    fts_set_int(ptos, fts_get_int(ptos) + fts_get_int(tos));
		  else
		    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator +", 0);
		}
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator +", 0);
	    }
	  break;

	case FTS_OP_MINUS:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_float(ptos, fts_get_float(ptos) - fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_float(ptos, ((float) fts_get_int(ptos)) - fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator -", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_float(ptos, fts_get_float(ptos) - ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) - fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator -", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator -", 0);
	  break;

	case FTS_OP_TIMES:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_float(ptos, fts_get_float(ptos) * fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_float(ptos, ((float) fts_get_int(ptos)) * fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator *", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_float(ptos, fts_get_float(ptos) * ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) * fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator *", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator *", 0);
	  break;

	case FTS_OP_DIV:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_float(ptos, fts_get_float(ptos) / fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_float(ptos, ((float) fts_get_int(ptos)) / fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator /", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_float(ptos, fts_get_float(ptos) / ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) / fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator /", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator /", 0);
	  break;

	case FTS_OP_DOT:
	  if (fts_is_symbol(tos) && fts_is_object(ptos) && fts_object_is_patcher(fts_get_object(ptos)))
	    {
	      fts_object_t *obj = fts_get_object(ptos);
	      fts_symbol_t varname = fts_get_symbol(tos);
	      fts_atom_t *value;

	      value = fts_variable_get_value_or_void((fts_patcher_t *)obj, varname);

	      if (value)
		{
		  fts_expression_add_var_ref(e, (fts_patcher_t *)obj, varname);

		  if (fts_is_void(value))
		    return expression_error(e, FTS_EXPRESSION_UNDEFINED_VARIABLE, "Variable %s is undefined",
					    varname);
		  else
		    *ptos = *value;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_UNDEFINED_VARIABLE, "Variable %s is not defined ",
					varname);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator .", 0);
	  break;

	case FTS_OP_REMAINDER:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) % fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator %", 0);
	  break;

	case FTS_OP_SHIFT_LEFT:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) << fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <<", 0);
	  break;

	case FTS_OP_SHIFT_RIGHT:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) >> fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >>", 0);
	  break;

	case FTS_OP_BIT_AND:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) & fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator &", 0);
	  break;

	case FTS_OP_BIT_OR:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) | fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator |", 0);
	  break;

	case FTS_OP_BIT_XOR:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) ^ fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator ^", 0);
	  break;

	case FTS_OP_LOGICAL_AND:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) && fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator &&", 0);
	  break;

	case FTS_OP_LOGICAL_OR:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) || fts_get_int(tos));
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator ||", 0);
	  break;

	case FTS_OP_EQUAL:
	  if (fts_atom_equals(tos, ptos))
	    fts_set_int(ptos, 1);
	  else
	    fts_set_int(ptos, 0);

	  break;

	case FTS_OP_NOT_EQUAL:
	  if (fts_atom_equals(tos, ptos))
	    fts_set_int(ptos, 0);
	  else
	    fts_set_int(ptos, 1);

	  break;

	case FTS_OP_GREATER:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) > fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, ((float) fts_get_int(ptos)) > fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) > ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) > fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >", 0);
	  break;

	case FTS_OP_GREATER_EQUAL:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) >= fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, ((float) fts_get_int(ptos)) >= fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >=", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) >= ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) >= fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >=", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator >=", 0);
	  break;

	case FTS_OP_SMALLER:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) < fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, ((float) fts_get_int(ptos)) < fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) < ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) < fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <", 0);
	  break;

	case FTS_OP_SMALLER_EQUAL:
	  if (fts_is_float(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) <= fts_get_float(tos));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, ((float) fts_get_int(ptos)) <= fts_get_float(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <=", 0);
	    }
	  else if (fts_is_int(tos))
	    {
	      if (fts_is_float(ptos))
		fts_set_int(ptos, fts_get_float(ptos) <= ((float) fts_get_int(tos)));
	      else if (fts_is_int(ptos))
		fts_set_int(ptos, fts_get_int(ptos) <= fts_get_int(tos));
	      else
		return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <=", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator <=", 0);
	  break;

	case FTS_OP_ELSE:
	  if ((value_stack_deeper_than(e, 2)) && (pop == FTS_OP_CONDITIONAL))
	    {
	      value_stack_pop(e, 1);
	      op_stack_pop(e); /* Pop one operator more, the conditional 
				  eat two operators */

	      if (atom_is_null(value_stack_top(e)))
		*value_stack_top(e) = *tos;
	      else
		*value_stack_top(e) = *ptos;
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator ?:", 0);
	  break;

	case FTS_OP_ASSIGN:
	  if (fts_is_symbol(ptos))
	    {
	      fts_expression_add_assignement(e, fts_get_symbol(ptos), tos);
	      value_stack_pop(e, 1); /* forget the value */
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for operator =", 0);
	  break;

	case FTS_OP_ARRAY_REF:
	  if (fts_is_int(tos) && fts_is_tuple(ptos))
	    {
	      fts_tuple_t *tup = fts_get_tuple(ptos);
	      int idx = fts_get_int(tos);
	      
	      if(idx >= 0 && fts_tuple_get_size(tup))
		*value_stack_top(e) = *fts_tuple_get_element(tup, idx);
	      else
		return expression_error(e, FTS_EXPRESSION_ARRAY_ACCESS_ERROR, "Index out of bounds", 0);
	    }
	  else
	    return expression_error(e, FTS_EXPRESSION_OP_TYPE_ERROR, "Type error for array access", 0);

	  break;

	default:
	  return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Syntax Error", 0);
	}

      return FTS_EXPRESSION_OK;
    }
}

/* Post-evaluation Functions */

int fts_expression_get_result_count(fts_expression_state_t *e)
{
  return e->count;
}

int fts_expression_get_status(fts_expression_state_t *e)
{
  return e->ret;
}

const char *fts_expression_get_msg(fts_expression_state_t *e)
{
  return e->msg;
}

const char *fts_expression_get_err_arg(fts_expression_state_t *e)
{
  return e->err_arg;
}



/*
 *
 * expression property  handling support
 *
 */

static void fts_expression_add_assignement(fts_expression_state_t *e, fts_symbol_t name, fts_atom_t *value)
{
  fts_expression_assignement_t *p;

  p = (fts_expression_assignement_t *) fts_heap_alloc(expr_prop_heap);

  p->name  = name;
  p->value = *value;
  p->next  = e->assignements;
  e->assignements = p;
}


/* return the number of assignements found */

int fts_expression_map_to_assignements(fts_expression_state_t *e, 
				       void (* f)(fts_symbol_t name, fts_atom_t *value, void *data),
				       void *data)
{
  fts_expression_assignement_t *p;
  int i;

  p = e->assignements;

  i = 0;
  while (p)
    {
      (* f)(p->name, &(p->value), data);

      p = p->next;
      i++;
    }

  return i;
}


/* Variable reference handling */

static void fts_expression_add_var_ref(fts_expression_state_t *e, fts_patcher_t *scope, fts_symbol_t name)
{
  fts_expr_var_ref_t *p;

  p = (fts_expr_var_ref_t *) fts_heap_alloc(expr_var_ref_heap);

  p->name  = name;
  p->scope = scope;
  p->next  = e->var_refs;
  e->var_refs = p;
}


void fts_expression_add_variables_user(fts_expression_state_t *e, fts_object_t *obj)
{
  fts_expr_var_ref_t *p;

  p = e->var_refs;

  while (p)
    {
      fts_expr_var_ref_t *p2;
      
      p2 = p;

      fts_variable_add_user(p->scope, p->name, obj);

      p = p->next;
    }
}


/* DEBUG CODE */

#ifdef EXPRESSION_TRACE_DEBUG
void fts_expression_printf_assignements(fts_expression_state_t *e)
{
  fts_expression_assignement_t *p;

  p = e->assignements;

  while (p)
    {
      fts_log( "\tGot name %s value ", p->name);
      fts_log_atoms( 1, &(p->value));
      fts_log( "\n");
      p = p->next;
    }
}
#endif


/* Eval all the expressions found in the expr list of arguments,
   put the result is result */

fts_expression_state_t *fts_expression_eval(fts_patcher_t *scope,
					    int expr_size, const fts_atom_t *expr,
					    int result_size, fts_atom_t *result)
{
  fts_expression_state_t *e;

#ifdef EXPRESSION_TRACE_DEBUG
  fts_log( "Expression: Evaluting: ");
  fts_log_atoms( expr_size, expr);
  fts_log( "\n");
#endif

  /* Calles of fts_expression_eval should free the resulting structure */

  e = fts_expression_state_new(scope, expr_size, expr);

  while ((e->count < result_size) && (e->in < expr_size))
    {
      e->ret = fts_expression_eval_simple(e);

      if (e->ret == FTS_EXPRESSION_OK)
	{
	  if (! value_stack_is_empty(e))
	    {
	      /* Got a result */
	      result[e->count] = * value_stack_top(e);
	      value_stack_pop(e, 1);
	      e->count++;
	    }
	}
      else
	{
#ifdef EXPRESSION_TRACE_DEBUG
	  fts_log( "Error in Expression: %s\n", e->msg);	/* @@@ */
#endif
	  return e;
	}
    }

#ifdef EXPRESSION_TRACE_DEBUG
  fts_log( "\tgot result: ");
  fts_log_atoms( e->count, result);
  fts_log( "\n");
  fts_expression_printf_assignements(e);
#endif

  return e;
}

/* Function library; external libraries can add new functions */


static int unique(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  static int seed = 1;

  fts_set_int(result, seed++);
  
  return FTS_EXPRESSION_OK;
}

static int get_array_element(int ac, const fts_atom_t *at, fts_atom_t *result)
{
  if ((ac == 4) && fts_is_int(at + 2) && fts_is_tuple(at + 1))
    {
      fts_tuple_t *tuple = fts_get_tuple(at + 1);
      int idx = fts_get_int(&at[2]);

      if (idx > 0 && idx < fts_tuple_get_size(tuple))
	*result = *fts_tuple_get_element(tuple, idx);
      else
	*result = at[3];
  
      return FTS_EXPRESSION_OK;
    }
  else
    return FTS_EXPRESSION_SYNTAX_ERROR;
}



/***********************************************************************
 * Compatibility:
 * fts_symbol_set_operator  
 * fts_symbol_get_operator  
 * fts_symbol_is_operator
 */
static fts_hashtable_t operator_table;

int fts_symbol_get_operator( fts_symbol_t s)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, s);
  if (fts_hashtable_get( &operator_table, &k, &v) && fts_is_int( &v))
    return fts_get_int( &v);

  return 0;
}

void fts_symbol_set_operator( fts_symbol_t s, int op)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, s);
  fts_set_int( &v, op);
  fts_hashtable_put( &operator_table, &k, &v);
}

int fts_symbol_is_operator( fts_symbol_t s)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, s);
  return fts_hashtable_get( &operator_table, &k, &v);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_expression_init(void)
{
  expr_prop_heap = fts_heap_new(sizeof(fts_expression_assignement_t));
  expr_var_ref_heap = fts_heap_new(sizeof(fts_expr_var_ref_t));
  expr_state_heap = fts_heap_new(sizeof(fts_expression_state_t));

  fts_hashtable_init(&fts_expression_fun_table, 0, FTS_HASHTABLE_MEDIUM);

  /* function installation */
  fts_expression_declare_fun(fts_new_symbol("unique"), unique);

  /* operator declarations  */
  fts_hashtable_init( &operator_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);
  fts_symbol_set_operator(fts_s_plus,  FTS_OP_PLUS);
  fts_symbol_set_operator(fts_s_minus, FTS_OP_MINUS);
  fts_symbol_set_operator(fts_s_times, FTS_OP_TIMES);
  fts_symbol_set_operator(fts_s_div,   FTS_OP_DIV);
  fts_symbol_set_operator(fts_s_dollar, FTS_OP_DOLLAR);
  /* fts_symbol_set_operator(fts_s_dot,    FTS_OP_DOT); */
  fts_symbol_set_operator(fts_s_percent,     FTS_OP_REMAINDER);
  fts_symbol_set_operator(fts_s_shift_left,  FTS_OP_SHIFT_LEFT);
  fts_symbol_set_operator(fts_s_shift_right, FTS_OP_SHIFT_RIGHT);
  fts_symbol_set_operator(fts_s_bit_and,     FTS_OP_BIT_AND);
  fts_symbol_set_operator(fts_s_bit_or,      FTS_OP_BIT_OR);
  fts_symbol_set_operator(fts_s_bit_xor,     FTS_OP_BIT_XOR);
  fts_symbol_set_operator(fts_s_bit_not,  FTS_OP_BIT_NOT);
  fts_symbol_set_operator(fts_s_logical_and, FTS_OP_LOGICAL_AND);
  fts_symbol_set_operator(fts_s_logical_or,  FTS_OP_LOGICAL_OR);
  fts_symbol_set_operator(fts_s_logical_not, FTS_OP_LOGICAL_NOT);
  fts_symbol_set_operator(fts_s_equal_equal, FTS_OP_EQUAL);
  fts_symbol_set_operator(fts_s_not_equal,   FTS_OP_NOT_EQUAL);
  fts_symbol_set_operator(fts_s_greater,     FTS_OP_GREATER);
  fts_symbol_set_operator(fts_s_greater_equal, FTS_OP_GREATER_EQUAL);
  fts_symbol_set_operator(fts_s_smaller, FTS_OP_SMALLER);
  fts_symbol_set_operator(fts_s_smaller_equal, FTS_OP_SMALLER_EQUAL);
  fts_symbol_set_operator(fts_s_conditional, FTS_OP_CONDITIONAL);
  fts_symbol_set_operator(fts_s_colon, FTS_OP_ELSE);
  fts_symbol_set_operator(fts_s_equal, FTS_OP_ASSIGN);


  op_priority[FTS_OP_PLUS] = 3;
  op_priority[FTS_OP_MINUS] = 3;
  op_priority[FTS_OP_TIMES] = 4;
  op_priority[FTS_OP_DIV] = 4;
  op_priority[FTS_OP_DOLLAR] = 6;
  op_priority[FTS_OP_DOT] = 5;
  op_priority[FTS_OP_REMAINDER] = 4;
  op_priority[FTS_OP_SHIFT_LEFT] = 4;
  op_priority[FTS_OP_SHIFT_RIGHT] = 4;
  op_priority[FTS_OP_BIT_AND] = 4;
  op_priority[FTS_OP_BIT_OR] = 3;
  op_priority[FTS_OP_BIT_XOR] = 3;
  op_priority[FTS_OP_BIT_NOT] = 0;
  op_priority[FTS_OP_LOGICAL_AND] = 4;
  op_priority[FTS_OP_LOGICAL_OR] = 2;
  op_priority[FTS_OP_LOGICAL_NOT] = 0;
  op_priority[FTS_OP_EQUAL] = 5;
  op_priority[FTS_OP_NOT_EQUAL] = 5;
  op_priority[FTS_OP_GREATER] = 5;
  op_priority[FTS_OP_GREATER_EQUAL] = 5;
  op_priority[FTS_OP_SMALLER] = 5;
  op_priority[FTS_OP_SMALLER_EQUAL] = 5;
  op_priority[FTS_OP_CONDITIONAL] = 1;
  op_priority[FTS_OP_ELSE] = 2;
  op_priority[FTS_OP_ASSIGN] = 0;
  op_priority[FTS_OP_ARRAY_REF] = 6; /* or 5 or 7 ?? */

  op_unary[FTS_OP_PLUS] = 1;
  op_unary[FTS_OP_MINUS] = 1;
  op_unary[FTS_OP_TIMES] = 0;
  op_unary[FTS_OP_DIV] = 0;
  op_unary[FTS_OP_DOLLAR] = 1;
  op_unary[FTS_OP_DOT] = 0;
  op_unary[FTS_OP_REMAINDER] = 0;
  op_unary[FTS_OP_SHIFT_LEFT] = 0;
  op_unary[FTS_OP_SHIFT_RIGHT] = 0;
  op_unary[FTS_OP_BIT_AND] = 0;
  op_unary[FTS_OP_BIT_OR] = 0;
  op_unary[FTS_OP_BIT_XOR] = 0;
  op_unary[FTS_OP_BIT_NOT] = 1;
  op_unary[FTS_OP_LOGICAL_AND] = 0;
  op_unary[FTS_OP_LOGICAL_OR] = 0;
  op_unary[FTS_OP_LOGICAL_NOT] = 1;
  op_unary[FTS_OP_EQUAL] = 0;
  op_unary[FTS_OP_NOT_EQUAL] = 0;
  op_unary[FTS_OP_GREATER] = 0;
  op_unary[FTS_OP_GREATER_EQUAL] = 0;
  op_unary[FTS_OP_SMALLER] = 0;
  op_unary[FTS_OP_SMALLER_EQUAL] = 0;
  op_unary[FTS_OP_CONDITIONAL] = 0;
  op_unary[FTS_OP_ELSE] = 0;
  op_unary[FTS_OP_ASSIGN] = 0;
  op_unary[FTS_OP_ARRAY_REF] = 0;

  op_binary[FTS_OP_PLUS] = 1;
  op_binary[FTS_OP_MINUS] = 1;
  op_binary[FTS_OP_TIMES] = 1;
  op_binary[FTS_OP_DIV] = 1;
  op_binary[FTS_OP_DOLLAR] = 0;
  op_binary[FTS_OP_DOT] = 1;
  op_binary[FTS_OP_REMAINDER] = 1;
  op_binary[FTS_OP_SHIFT_LEFT] = 1;
  op_binary[FTS_OP_SHIFT_RIGHT] = 1;
  op_binary[FTS_OP_BIT_AND] = 1;
  op_binary[FTS_OP_BIT_OR] = 1;
  op_binary[FTS_OP_BIT_XOR] = 1;
  op_binary[FTS_OP_BIT_NOT] = 0;
  op_binary[FTS_OP_LOGICAL_AND] = 1;
  op_binary[FTS_OP_LOGICAL_OR] = 1;
  op_binary[FTS_OP_LOGICAL_NOT] = 0;
  op_binary[FTS_OP_EQUAL] = 1;
  op_binary[FTS_OP_NOT_EQUAL] = 1;
  op_binary[FTS_OP_GREATER] = 1;
  op_binary[FTS_OP_GREATER_EQUAL] = 1;
  op_binary[FTS_OP_SMALLER] = 1;
  op_binary[FTS_OP_SMALLER_EQUAL] = 1;
  op_binary[FTS_OP_CONDITIONAL] = 1;
  op_binary[FTS_OP_ELSE] = 1;
  op_binary[FTS_OP_ASSIGN] = 1;
  op_binary[FTS_OP_ARRAY_REF] = 1;
}
