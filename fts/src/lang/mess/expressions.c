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
 * and the FTS '.' get property or nested variable of an object operator.
 *
 * Warning: in the current implementation, "? :", and logical "&&" and "||"
 * eval all the arguments before giving the result; this prevent the use
 * of this expression evaluator for computing recursive functions !!
 * Since there are no side effects in expression evaluators, there should be no other
 * problems with this.
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

/* Operator codes */

#define FTS_OP_PLUS   1
#define FTS_OP_MINUS  2
#define FTS_OP_TIMES  3
#define FTS_OP_DIV    4
#define FTS_OP_OPEN_PAR    5
#define FTS_OP_CLOSED_PAR  6
#define FTS_OP_DOLLAR  7
#define FTS_OP_DOT     8
#define FTS_OP_REMAINDER     9
#define FTS_OP_SHIFT_LEFT   10
#define FTS_OP_SHIFT_RIGHT  11
#define FTS_OP_BIT_AND      12
#define FTS_OP_BIT_OR       13
#define FTS_OP_BIT_XOR      14
#define FTS_OP_BIT_NOT      15
#define FTS_OP_LOGICAL_AND  16
#define FTS_OP_LOGICAL_OR   17
#define FTS_OP_LOGICAL_NOT  18
#define FTS_OP_EQUAL        19
#define FTS_OP_NOT_EQUAL    20
#define FTS_OP_GREATER      21 
#define FTS_OP_GREATER_EQUAL 22
#define FTS_OP_SMALLER       23
#define FTS_OP_SMALLER_EQUAL 24
#define FTS_OP_CONDITIONAL   25
#define FTS_OP_ELSE          26
#define FTS_OP_ASSIGN        27
#define FTS_OP_FIRST_UNUSED  28

#define FTS_BINARY_OP_TYPE 0
#define FTS_UNARY_OP_TYPE  1
#define FTS_PAR_OP_TYPE    2

/* constants and limits */

#define FTS_EXPR_MAX_SYMBOL_LENGTH 2048

#define FTS_EXPR_MAX_DEPTH 256

/* Operator priority */

static int op_priority[FTS_OP_FIRST_UNUSED];
static int op_unary[FTS_OP_FIRST_UNUSED];
static int op_binary[FTS_OP_FIRST_UNUSED];

#define fts_op_before(op1, op2)   (op_priority[(op1)] >= op_priority[(op2)]) 

/* Operator type */

#define fts_op_can_be_unary(op)  (op_unary[(op)])
#define fts_op_can_be_binary(op) (op_binary[(op)])

/* Parentesys check */

#define fts_is_open_par(a)       (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_open_par))
#define fts_is_closed_par(a)     (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_closed_par))

/* Storing properties set in expressions for future created objects */

typedef struct fts_expr_prop
{
  fts_symbol_t name;
  fts_atom_t   value;
  struct fts_expr_prop *next;
} fts_expr_prop_t;

static fts_heap_t expr_prop_heap;



void
fts_expressions_init(void)
{

  fts_heap_init(&expr_prop_heap, sizeof(fts_expr_prop_t), 16);


  fts_symbol_set_operator(fts_s_plus,  FTS_OP_PLUS);
  fts_symbol_set_operator(fts_s_minus, FTS_OP_MINUS);
  fts_symbol_set_operator(fts_s_times, FTS_OP_TIMES);
  fts_symbol_set_operator(fts_s_div,   FTS_OP_DIV);
  fts_symbol_set_operator(fts_s_dollar, FTS_OP_DOLLAR);
  fts_symbol_set_operator(fts_s_dot,    FTS_OP_DOT);
  fts_symbol_set_operator(fts_s_remainder,   FTS_OP_REMAINDER);
  fts_symbol_set_operator(fts_s_shift_left,  FTS_OP_SHIFT_LEFT);
  fts_symbol_set_operator(fts_s_shift_right, FTS_OP_SHIFT_RIGHT);
  fts_symbol_set_operator(fts_s_bit_and, FTS_OP_BIT_AND);
  fts_symbol_set_operator(fts_s_bit_or,  FTS_OP_BIT_OR);
  fts_symbol_set_operator(fts_s_bit_xor,  FTS_OP_BIT_XOR);
  fts_symbol_set_operator(fts_s_bit_not, FTS_OP_BIT_NOT);
  fts_symbol_set_operator(fts_s_logical_and, FTS_OP_LOGICAL_AND);
  fts_symbol_set_operator(fts_s_logical_or,  FTS_OP_LOGICAL_OR);
  fts_symbol_set_operator(fts_s_logical_not, FTS_OP_LOGICAL_NOT);
  fts_symbol_set_operator(fts_s_equal,   FTS_OP_EQUAL);
  fts_symbol_set_operator(fts_s_not_equal,   FTS_OP_NOT_EQUAL);
  fts_symbol_set_operator(fts_s_greater, FTS_OP_GREATER);
  fts_symbol_set_operator(fts_s_greater_equal, FTS_OP_GREATER_EQUAL);
  fts_symbol_set_operator(fts_s_smaller, FTS_OP_SMALLER);
  fts_symbol_set_operator(fts_s_smaller_equal, FTS_OP_SMALLER_EQUAL);
  fts_symbol_set_operator(fts_s_conditional, FTS_OP_CONDITIONAL);
  fts_symbol_set_operator(fts_s_else, FTS_OP_ELSE);
  fts_symbol_set_operator(fts_s_assign, FTS_OP_ASSIGN);


  op_priority[FTS_OP_PLUS] = 3;
  op_priority[FTS_OP_MINUS] = 3;
  op_priority[FTS_OP_TIMES] = 4;
  op_priority[FTS_OP_DIV] = 4;
  op_priority[FTS_OP_OPEN_PAR] = 0;
  op_priority[FTS_OP_CLOSED_PAR] = 0;
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
  op_priority[FTS_OP_CONDITIONAL] = 2;
  op_priority[FTS_OP_ELSE] = 1;
  op_priority[FTS_OP_ASSIGN] = 0;

  op_unary[FTS_OP_PLUS] = 1;
  op_unary[FTS_OP_MINUS] = 1;
  op_unary[FTS_OP_TIMES] = 0;
  op_unary[FTS_OP_DIV] = 0;
  op_unary[FTS_OP_OPEN_PAR] = 0;
  op_unary[FTS_OP_CLOSED_PAR] = 0;
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

  op_binary[FTS_OP_PLUS] = 1;
  op_binary[FTS_OP_MINUS] = 1;
  op_binary[FTS_OP_TIMES] = 1;
  op_binary[FTS_OP_DIV] = 1;
  op_binary[FTS_OP_OPEN_PAR] = 0;
  op_binary[FTS_OP_CLOSED_PAR] = 0;
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
}

/*
  expr is a list of atoms keeping a list of expression;
  each expression is computed, the result is put in the 
  fts_atom_t array result; the number of computed values
  is returned from this function.

  Variables/constants are interpreted in the context given 
  by the patcher argument.

  Return an error code to get with fts_expression_get_status in case of errors,
  0 if ok.
  */


/*
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

  int count;			/* result counter */
  int ret;			/* return code */

  fts_expr_prop_t *props;	/* properties expressed in the expression */
};



/* Private Macros and functions */

/* Return 1 in case of error, 0 otherwise */

static int fts_op_eval(fts_object_t *object, fts_expression_state_t *e);
static void fts_expression_add_prop(fts_expression_state_t *e, fts_symbol_t name, fts_atom_t *value);



static fts_expression_state_t expr_state;

fts_expression_state_t *fts_expression_eval(fts_object_t *object,
					    int expr_size, const fts_atom_t *expr,
					    int result_size, fts_atom_t *result)
{
  int i;
  fts_expression_state_t *e = &expr_state;
  enum {waiting_arg, waiting_op} status; 

  e->value_stack_p = -1;	/* point to the last actually used cell */
  e->op_stack_p = -1;		/* point to the last actually used cell */
  e->count = 0;			/* result counter */
  e->props = 0;			/* init property assign list */
  e->ret  = FTS_EXPRESSION_OK;	/* init return status */

  /* Expression pointer: no backtrack, so we just iterate once
     on the whole expression list */

  status = waiting_arg;

  for (i = 0; i  < expr_size; i++)
    {
      if (status == waiting_arg)
	{
	  if (fts_is_open_par(&expr[i]))
	    {
	      e->op_stack_p++;
	      e->op_stack[e->op_stack_p] = FTS_OP_OPEN_PAR;
	      e->op_type_stack[e->op_stack_p] = FTS_PAR_OP_TYPE;

	      status = waiting_arg;
	    }
	  else if (fts_is_closed_par(&expr[i]))
	    {
	      /* ERROR !!: unbalanced closed parentesis */

	      e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
	      return e;
	    }
	  else if (fts_is_operator(&expr[i]))
	    {
	      if (fts_op_can_be_unary(fts_get_operator(&expr[i])))
		{
		  e->op_stack_p++;
		  e->op_stack[e->op_stack_p] = fts_get_operator(&expr[i]);
		  e->op_type_stack[e->op_stack_p] = FTS_UNARY_OP_TYPE;
		}
	      else
		{
		  /* ERROR !! : syntax error, expression start with operator */

		  e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
		  return e;
		}
	    }
	  else
	    {
	      /* Anything else is an argument */

	      e->value_stack_p++;
	      e->value_stack[e->value_stack_p] = expr[i];

	      while ((e->op_stack_p >= 0) && (e->op_type_stack[e->op_stack_p] == FTS_UNARY_OP_TYPE))
		{
		  e->ret = fts_op_eval(object, e);

		  if (e->ret != FTS_EXPRESSION_OK)
		    return e;

		  e->op_stack_p--;
		}
	      status = waiting_op;
	    }
	}
      else
	{
	  if (fts_is_open_par(&expr[i]))
	    {
	      while ((e->op_stack_p >= 0) && (e->op_stack[e->op_stack_p] != FTS_OP_OPEN_PAR))
		{
		  e->ret = fts_op_eval(object, e);

		  if (e->ret != FTS_EXPRESSION_OK)
		    return e;

		  e->op_stack_p--;
		}


	      if ((e->op_stack_p == -1) && (e->value_stack_p == 0))
		{
		  if (e->count > result_size)
		    return e;

		  result[e->count++] = e->value_stack[e->value_stack_p];
		}
	      else
		{
		  /* ERROR !!: Syntax error in expression */

		  e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
		  return e;
		}

	      /* Back track */

	      e->value_stack_p = -1;
	      e->op_stack_p = -1;
	      i--;
	      status = waiting_arg;
	    }
	  else if (fts_is_closed_par(&expr[i]))
	    {
	      while ((e->op_stack_p >= 0) && (e->op_stack[e->op_stack_p] != FTS_OP_OPEN_PAR))
		{
		  e->ret = fts_op_eval(object, e);

		  if (e->ret != FTS_EXPRESSION_OK)
		    return e;

		  e->op_stack_p--;
		}

	      if ((e->op_stack_p == -1) || (e->op_type_stack[e->op_stack_p] != FTS_PAR_OP_TYPE))
		{
		  /* Error: unbalanced ')' parantesis */

		  e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
		  return e;
		}

	      e->op_stack_p --;	/* pop the parentesys */

	      status = waiting_op;
	    }
	  else if (fts_is_operator(&expr[i]))
	    {
	      /* Is this code correct for unary ops ??? */

	      int op;

	      op = fts_get_operator(&expr[i]);

	      if (fts_op_can_be_binary(op))
		{
		  while (e->op_stack_p >= 0 && fts_op_before(e->op_stack[e->op_stack_p], op))
		    {
		      e->ret = fts_op_eval(object, e);

		      if (e->ret != FTS_EXPRESSION_OK)
			return e;

		      e->op_stack_p--;
		    }

		  e->op_stack_p++;
		  e->op_stack[e->op_stack_p] = op;
		  e->op_type_stack[e->op_stack_p] = FTS_BINARY_OP_TYPE;

		  status = waiting_arg;
		}
	      else
		{
		  /* Same thing as for oper par */

		  while ((e->op_stack_p >= 0) && (e->op_stack[e->op_stack_p] != FTS_OP_OPEN_PAR))
		    {
		      e->ret = fts_op_eval(object, e);

		      if (e->ret != FTS_EXPRESSION_OK)
			return e;

		      e->op_stack_p--;
		    }


		  if ((e->op_stack_p == -1) && (e->value_stack_p == 0))
		    {
		      if (e->count > result_size)
			return e;

		      result[e->count++] = e->value_stack[e->value_stack_p];
		    }
		  else
		    {
		      /* ERROR !!: Syntax error in expression */

		      e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
		      return e;
		    }

		  /* Back track */

		  e->value_stack_p = -1;
		  e->op_stack_p = -1;
		  i--;
		  status = waiting_arg;
		}
	    }
	  else
	    {
	      /* Anything else is an argument */

	      while ((e->op_stack_p >= 0) && (e->op_stack[e->op_stack_p] != FTS_OP_OPEN_PAR))
		{
		  e->ret = fts_op_eval(object, e);

		  if (e->ret != FTS_EXPRESSION_OK)
		    return e;

		  e->op_stack_p--;
		}

	      if ((e->op_stack_p == -1) && (e->value_stack_p == 0))
		{
		  if (e->count > result_size)
		    return e;

		  result[e->count++] = e->value_stack[e->value_stack_p];
		}
	      else
		{
		  /* ERROR !!: Syntax error in expression */

		  e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
		  return e;
		}

	      /* Back track */

	      e->value_stack_p = -1;
	      e->op_stack_p = -1;
	      i--;
	      status = waiting_arg;
	    }
	}
    }

  while ((e->op_stack_p >= 0) && (e->op_stack[e->op_stack_p] != FTS_OP_OPEN_PAR))
    {
      e->ret = fts_op_eval(object, e);

      if (e->ret != FTS_EXPRESSION_OK)
	return e;

      e->op_stack_p--;
    }

  if ((e->op_stack_p == -1) && (e->value_stack_p == 0))
    {
      if (e->count > result_size)
	return e;

      result[e->count++] = e->value_stack[e->value_stack_p];
    }
  else
    {
      /* ERROR !!: Syntax error in expression */

      e->ret = FTS_EXPRESSION_SYNTAX_ERROR;
      return e;
    }

  return e;
}

/* This thing work only for ints, for now !!!, no type promotion !! */

#define fts_get_float_num(a)   (fts_is_float((a)) ? fts_get_float((a)) : (float) fts_get_int((a)))
#define OP_ERROR               return(1)

/* pop is the previous operator; used only for ?: checking */

static int fts_op_eval(fts_object_t *object, fts_expression_state_t *e)
{
  int op = e->op_stack[e->op_stack_p];
  int pop = e->op_stack[e->op_stack_p - 1];
  int op_type = e->op_type_stack[e->op_stack_p];
  int tos = e->value_stack_p;

  if (op_type == FTS_UNARY_OP_TYPE)
    {
      switch (op)
	{
	case FTS_OP_DOLLAR:
	  if (fts_is_symbol(&e->value_stack[tos]))
	    {
	      fts_atom_t *value;

	      value = fts_variable_get_value(object, fts_get_symbol(&e->value_stack[tos]));

	      if (value)
		e->value_stack[tos] = *value;
	      else
		return FTS_EXPRESSION_UNDEFINED_VARIABLE;
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	  break;

	case FTS_OP_PLUS:
	  /* NOP, really */
	  if (! (fts_is_int(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos])))
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_MINUS:
	  if (fts_is_int(&e->value_stack[tos]))
	    fts_set_int(&e->value_stack[tos], (-1) * fts_get_int(&e->value_stack[tos]));
	  else if (fts_is_float(&e->value_stack[tos]))
	    fts_set_float(&e->value_stack[tos], (-1) * fts_get_float(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_NOT:
	  if (fts_is_int(&e->value_stack[tos]))
	    fts_set_int(&e->value_stack[tos], (~ fts_get_int(&e->value_stack[tos])));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;

	case FTS_OP_LOGICAL_NOT:
	  if (fts_is_int(&e->value_stack[tos]))
	    fts_set_int(&e->value_stack[tos], (! fts_get_int(&e->value_stack[tos])));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;

	default:
	  return FTS_EXPRESSION_SYNTAX_ERROR;
	}

      return FTS_EXPRESSION_OK;
    }
  else
    {
      (e->value_stack_p)--;

      switch (op)
	{
	case FTS_OP_PLUS:
	  if (fts_is_symbol(&e->value_stack[tos]) || fts_is_symbol(&e->value_stack[tos - 1]))
	    {
	      char buf[FTS_EXPR_MAX_SYMBOL_LENGTH];

	      if (fts_is_symbol(&e->value_stack[tos]))
		{
		  if (fts_is_symbol(&e->value_stack[tos - 1]))
		    sprintf(buf, "%s%s",
			    fts_symbol_name(fts_get_symbol(&e->value_stack[tos - 1])),
			    fts_symbol_name(fts_get_symbol(&e->value_stack[tos])));
		  else if (fts_is_int(&e->value_stack[tos - 1]))
		    sprintf(buf, "%d%s",
			    fts_get_int(&e->value_stack[tos - 1]),
			    fts_symbol_name(fts_get_symbol(&e->value_stack[tos])));
		  else if (fts_is_float(&e->value_stack[tos - 1]))
		    sprintf(buf, "%f%s",
			    fts_get_float(&e->value_stack[tos - 1]),
			    fts_symbol_name(fts_get_symbol(&e->value_stack[tos])));
		}
	      else if (fts_is_int(&e->value_stack[tos]))
		sprintf(buf, "%s%d",
			fts_symbol_name(fts_get_symbol(&e->value_stack[tos - 1])),
			fts_get_int(&e->value_stack[tos]));
	      else if (fts_is_float(&e->value_stack[tos]))
		sprintf(buf, "%s%f",
			fts_symbol_name(fts_get_symbol(&e->value_stack[tos - 1])),
			fts_get_float(&e->value_stack[tos]));

	      fts_set_symbol(&e->value_stack[tos-1], fts_new_symbol_copy(buf));
	    }
	  else if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    {
	      fts_set_float(&e->value_stack[tos-1],
			    fts_get_float_num(&e->value_stack[tos-1]) + fts_get_float_num(&e->value_stack[tos]));
	    }
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    {
	      fts_set_int(&e->value_stack[tos-1],
			  fts_get_int(&e->value_stack[tos - 1]) + fts_get_int(&e->value_stack[tos]));
	    }
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_MINUS:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_float(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) - fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) - fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_TIMES:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_float(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) * fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) * fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_DIV:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_float(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) / fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) / fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_DOT:
	  if (fts_is_symbol(&e->value_stack[tos]) || fts_is_object(&e->value_stack[tos - 1]))
	    {
	      fts_object_t *obj = fts_get_object(&e->value_stack[tos - 1]);
	      fts_symbol_t prop = fts_get_symbol(&e->value_stack[tos]);

	      if (fts_object_is_patcher(obj))
		{
		  fts_atom_t *value;

		  value = fts_variable_get_value(object, fts_get_symbol(&e->value_stack[tos]));

		  if (value)
		    e->value_stack[tos] = *value;
		  else
		    OP_ERROR;
		}
	      else
		{
		  fts_object_get_prop(obj, prop, &e->value_stack[tos - 1]);

		  if (fts_is_void(&e->value_stack[tos - 1]))
		    OP_ERROR;
		}
	    }
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_REMAINDER:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) % fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SHIFT_LEFT:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) << fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SHIFT_RIGHT:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) >> fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_AND:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) & fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_OR:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) | fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_XOR:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) ^ fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_LOGICAL_AND:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) && fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_LOGICAL_OR:
	  if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) || fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_EQUAL:
	  if (fts_atom_equal(&e->value_stack[tos], &e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1], 1);
	  else
	    fts_set_int(&e->value_stack[tos-1], 0);

	  break;

	case FTS_OP_NOT_EQUAL:
	  if (fts_atom_equal(&e->value_stack[tos], &e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1], 0);
	  else
	    fts_set_int(&e->value_stack[tos-1], 1);

	  break;

	case FTS_OP_GREATER:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) > fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) > fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_GREATER_EQUAL:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) >= fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) >= fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SMALLER:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) < fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) < fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SMALLER_EQUAL:
	  if (fts_is_float(&e->value_stack[tos]) || fts_is_float(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			  fts_get_float_num(&e->value_stack[tos-1]) <= fts_get_float_num(&e->value_stack[tos]));
	  else if (fts_is_int(&e->value_stack[tos]) && fts_is_int(&e->value_stack[tos - 1]))
	    fts_set_int(&e->value_stack[tos-1],
			fts_get_int(&e->value_stack[tos - 1]) <= fts_get_int(&e->value_stack[tos]));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_ELSE:
	  if ((e->value_stack_p > 1) && (pop == FTS_OP_CONDITIONAL))
	    {
	      (e->value_stack_p)--;

	      if (fts_atom_is_null(&e->value_stack[tos-2]))
		e->value_stack[tos-2] = e->value_stack[tos];
	      else
		e->value_stack[tos-2] = e->value_stack[tos-1];
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	  break;

	case FTS_OP_ASSIGN:
	  if (fts_is_symbol(&e->value_stack[tos - 1]))
	    {
	      fts_expression_add_prop(e, fts_get_symbol(&e->value_stack[tos - 1]), &e->value_stack[tos]);
	      (e->value_stack_p)--; /* forget the value */
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	  break;

	default:
	  return FTS_EXPRESSION_SYNTAX_ERROR;
	}

      return FTS_EXPRESSION_OK;
    }
}

/* Post-evaluation Functions */

int fts_expression_get_count(fts_expression_state_t *e)
{
  return e->count;
}

int fts_expression_get_status(fts_expression_state_t *e)
{
  return e->ret;
}


/*
 *
 * expression property  handling support
 *
 */

static void fts_expression_add_prop(fts_expression_state_t *e, fts_symbol_t name, fts_atom_t *value)
{
  fts_expr_prop_t *p;

  p = (fts_expr_prop_t *) fts_heap_alloc(&expr_prop_heap);

  p->name  = name;
  p->value = *value;
  p->next  = e->props;
  e->props = p;
}


void fts_expression_assign_properties(fts_expression_state_t *e, fts_object_t *obj)
{
  fts_expr_prop_t *p;

  p = e->props;

  while (p)
    {
      fts_expr_prop_t *p2;
      
      p2 = p;

      if (fts_object_is_patcher(obj))
	fts_patcher_variable_define((fts_patcher_t *) obj, p->name, &(p->value));
      else
	fts_object_put_prop(obj, p->name, &(p->value));

      p = p->next;
      fts_heap_free((char *) p2, &expr_prop_heap);
    }
}


