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
 * The FTS '.' get property or nested variable of an object operator.
 *
 * Warning: in the current implementation, "? :", and logical "&&" and "||"
 * eval all the arguments before giving the result; this prevent the use
 * of this expression evaluator for computing recursive functions !!
 * Since there are no side effects in expression evaluators, there should be no other
 * problems with this.
 */

/*
  EXPRESSION_TRACE_DEBUG, if defined, cause some debug printouts
  before while and after evalutating an expression
  */

/* #define EXPRESSION_TRACE_DEBUG */

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

/* Storing assignement set in expressions for future created objects */

typedef struct fts_expr_assignement
{
  fts_symbol_t name;
  fts_atom_t   value;
  struct fts_expr_assignement *next;
} fts_expr_assignement_t;

static fts_heap_t expr_prop_heap;

typedef struct fts_expr_var_ref
{
  fts_symbol_t name;
  struct fts_expr_var_ref *next;
} fts_expr_var_ref_t;

static fts_heap_t expr_var_ref_heap;

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

  int count;			/* result counter */
  int ret;			/* return code */

  fts_expr_assignement_t *assignements;	/* properties expressed in the expression */

  fts_expr_var_ref_t *var_refs;	/* list of variable names referred  in the expression */
};

/* Error utility */

static int expression_error(fts_expression_state_t *e, int err, const char *msg)
{
  post("ERROR !!! %s\n", msg);	/* @@@ */
  e->ret = err;
  return err;
}

/* OP stack manipulation */

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

static void value_stack_pop(fts_expression_state_t *e)
{
  e->value_stack_p--;
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

static fts_atom_t *value_stack_peek(fts_expression_state_t *e, int i)
{
  return &(e->value_stack[e->value_stack_p - i]);
}

/* Private Macros and functions */

/* Return 1 in case of error, 0 otherwise */

static int fts_op_eval(fts_object_t *object, fts_expression_state_t *e);
static void fts_expression_add_prop(fts_expression_state_t *e, fts_symbol_t name, fts_atom_t *value);
static void fts_expression_add_var_ref(fts_expression_state_t *e, fts_symbol_t name);


static fts_expression_state_t *expr_state = 0;

static fts_expression_state_t *fts_get_expression_state()
{
  if (expr_state == NULL)
    {
      expr_state = (fts_expression_state_t *) fts_malloc(sizeof(struct fts_expression_state));
    }
  else
    {
      /* Free the assignement list */

      fts_expr_assignement_t *p;

      p = expr_state->assignements;

      while (p)
	{
	  fts_expr_assignement_t *p2;
      
	  p2 = p;
	  p = p->next;
	  fts_heap_free((char *) p2, &expr_prop_heap);
	}
    }

  value_stack_set_empty(expr_state);
  op_stack_set_empty(expr_state);
  expr_state->count = 0;			/* result counter */
  expr_state->assignements = 0;			/* init assignement  list */
  expr_state->var_refs = 0;		/* init var ref list */
  expr_state->ret  = FTS_EXPRESSION_OK;	/* init return status */

  return expr_state;
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
fts_expression_state_t *fts_expression_eval(fts_object_t *object,
					    int expr_size, const fts_atom_t *expr,
					    int result_size, fts_atom_t *result)
					    */

#define TRY(call)             {int ret; ret = call; if (ret != FTS_EXPRESSION_OK) return ret;}

static int fts_do_expression_eval(fts_expression_state_t *e,
						      fts_object_t *object,
						      int expr_size, const fts_atom_t *expr,
						      fts_atom_t *result)
{
  int ret;
  int i;
  enum {waiting_arg, waiting_op} status; 

  /* Expression pointer: no backtrack, so we just iterate once
     on the whole expression list */

  value_stack_set_empty(e);
  op_stack_set_empty(e);
  status = waiting_arg;

  for (i = 0; i < expr_size; i++)
    {
      if (status == waiting_arg)
	{
	  if (fts_is_open_par(&expr[i]))
	    {
	      op_stack_push(e, FTS_OP_OPEN_PAR, FTS_PAR_OP_TYPE);

	      status = waiting_arg;
	    }
	  else if (fts_is_closed_par(&expr[i]))
	    {
	      return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, "Unbalanced closed parentesis\n");	
	    }
	  else if (fts_is_operator(&expr[i]))
	    {
	      if (fts_op_can_be_unary(fts_get_operator(&expr[i])))
		op_stack_push(e, fts_get_operator(&expr[i]), FTS_UNARY_OP_TYPE);
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					"Syntax error, expression start with operator");
	    }
	  else
	    {
	      /* Anything else is an argument */

	      value_stack_push(e, &expr[i]);

	      while ((! op_stack_is_empty(e)) && (op_type_stack_top(e) == FTS_UNARY_OP_TYPE))
		{
		  TRY(fts_op_eval(object, e));
		  op_stack_pop(e);
		}
	      status = waiting_op;
	    }
	}
      else
	{
	  if (fts_is_open_par(&expr[i]))
	    {
	      while ((! op_stack_is_empty(e)) && (op_stack_top(e) != FTS_OP_OPEN_PAR))
		{
		  TRY(fts_op_eval(object, e));
		  op_stack_pop(e);
		}


	      if (op_stack_is_empty(e) && value_stack_is_deep(e, 1))
		{
		  *result = * value_stack_top(e);
		  return i - 1;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					"Syntax error in expression (1)");
	    }
	  else if (fts_is_closed_par(&expr[i]))
	    {
	      while ((! op_stack_is_empty(e)) && (op_stack_top(e) != FTS_OP_OPEN_PAR))
		{
		  TRY(fts_op_eval(object, e));
		  op_stack_pop(e);
		}

	      if (op_stack_is_empty(e) || (op_type_stack_top(e) != FTS_PAR_OP_TYPE))
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					"Unbalanced ')' parantesis");

	      op_stack_pop(e); /* pop the parentesys */

	      status = waiting_op;
	    }
	  else if (fts_is_operator(&expr[i]))
	    {
	      /* Is this code correct for unary ops ??? */

	      int op;

	      op = fts_get_operator(&expr[i]);

	      if (fts_op_can_be_binary(op))
		{
		  while ((! op_stack_is_empty(e)) && fts_op_before(op_stack_top(e), op))
		    {
		      TRY(fts_op_eval(object, e));
		      op_stack_pop(e);
		    }

		  op_stack_push(e, op, FTS_BINARY_OP_TYPE);

		  status = waiting_arg;
		}
	      else
		{
		  /* Same thing as for oper par */

		  while ((! op_stack_is_empty(e)) && (op_stack_top(e) != FTS_OP_OPEN_PAR))
		    {
		      TRY(fts_op_eval(object, e))
		      op_stack_pop(e);
		    }

		  if (op_stack_is_empty(e) && value_stack_is_deep(e, 1))
		    {
		      *result = *value_stack_top(e);
		      return i - 1;
		    }
		  else
		    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					    "Syntax error in expression (2)");
		}
	    }
	  else
	    {
	      /* Anything else is an argument */

	      while ((! op_stack_is_empty(e)) && (op_stack_top(e) != FTS_OP_OPEN_PAR))
		{
		  TRY(fts_op_eval(object, e));
		  op_stack_pop(e);
		}

	      if (op_stack_is_empty(e) && value_stack_is_deep(e, 1))
		{
		  *result = *value_stack_top(e);
		  return i - 1;
		}
	      else
		return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
					"Syntax error in expression (3)");
	    }
	}
    }

  while ((! op_stack_is_empty(e)) && (op_stack_top(e) != FTS_OP_OPEN_PAR))
    {
      TRY(fts_op_eval(object, e))
      op_stack_pop(e);
    }


  if (op_stack_is_empty(e))
    {
      if (value_stack_is_empty(e))
	{
	  /* last expression did not produced a result (was an property/variable assignemnt) */
	  fts_set_void(result);	/* @@@ ??? */
	}
      else if (value_stack_is_deep(e, 1))
	{
	  *result  = * value_stack_top(e);
	}
      else
	return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
				"Syntax error in expression (4)");
    }
  else
    return expression_error(e, FTS_EXPRESSION_SYNTAX_ERROR, 
			    "Syntax error in expression (4)");

  return i;
}

/* This thing work only for ints, for now !!!, no type promotion !! */

#define fts_get_float_num(a)   (fts_is_float((a)) ? fts_get_float((a)) : (float) fts_get_int((a)))
#define OP_ERROR               return(1)

/* pop is the previous operator; used only for ?: checking */

static int fts_op_eval(fts_object_t *object, fts_expression_state_t *e)
{
  int op = op_stack_top(e);
  int pop = op_stack_peek(e, 1);
  int op_type = op_type_stack_top(e);

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
	      value = fts_variable_get_value(object, varname);

	      if (value)
		{
		  fts_expression_add_var_ref(e, varname);

		  if (fts_is_void(value))
		    return FTS_EXPRESSION_UNDEFINED_VARIABLE;		    
		  else if (fts_is_object(value) && fts_object_is_error(fts_get_object(value)))
		    return FTS_EXPRESSION_ERROR_OBJECT_REFERENCE;
		  else
		    *tos = *value;
		}
	      else
		return FTS_EXPRESSION_UNDEFINED_VARIABLE;
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	  break;

	case FTS_OP_PLUS:
	  /* NOP, really */
	  if (! (fts_is_int(tos) || fts_is_float(tos)))
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_MINUS:
	  if (fts_is_int(tos))
	    fts_set_int(tos, (-1) * fts_get_int(tos));
	  else if (fts_is_float(tos))
	    fts_set_float(tos, (-1) * fts_get_float(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_NOT:
	  if (fts_is_int(tos))
	    fts_set_int(tos, (~ fts_get_int(tos)));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;

	case FTS_OP_LOGICAL_NOT:
	  if (fts_is_int(tos))
	    fts_set_int(tos, (! fts_get_int(tos)));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;

	default:
	  return FTS_EXPRESSION_SYNTAX_ERROR;
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
      value_stack_pop(e);
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
			    fts_symbol_name(fts_get_symbol(ptos)),
			    fts_symbol_name(fts_get_symbol(tos)));
		  else if (fts_is_int(ptos))
		    sprintf(buf, "%d%s",
			    fts_get_int(ptos),
			    fts_symbol_name(fts_get_symbol(tos)));
		  else if (fts_is_float(ptos))
		    sprintf(buf, "%f%s",
			    fts_get_float(ptos),
			    fts_symbol_name(fts_get_symbol(tos)));
		}
	      else if (fts_is_int(tos))
		sprintf(buf, "%s%d",
			fts_symbol_name(fts_get_symbol(ptos)),
			fts_get_int(tos));
	      else if (fts_is_float(tos))
		sprintf(buf, "%s%f",
			fts_symbol_name(fts_get_symbol(ptos)),
			fts_get_float(tos));

	      fts_set_symbol(ptos, fts_new_symbol_copy(buf));
	    }
	  else if (fts_is_float(tos) || fts_is_float(ptos))
	    {
	      fts_set_float(ptos,
			    fts_get_float_num(ptos) + fts_get_float_num(tos));
	    }
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    {
	      fts_set_int(ptos,
			  fts_get_int(ptos) + fts_get_int(tos));
	    }
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_MINUS:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_float(ptos,
			  fts_get_float_num(ptos) - fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos,
			fts_get_int(ptos) - fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_TIMES:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_float(ptos,
			  fts_get_float_num(ptos) * fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos,
			fts_get_int(ptos) * fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_DIV:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_float(ptos,
			  fts_get_float_num(ptos) / fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos,
			fts_get_int(ptos) / fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_DOT:
	  if (fts_is_symbol(tos) || fts_is_object(ptos))
	    {
	      fts_object_t *obj = fts_get_object(ptos);
	      fts_symbol_t prop = fts_get_symbol(tos);

	      if (fts_object_is_patcher(obj))
		{
		  fts_atom_t *value;
		  fts_symbol_t varname;

		  varname = fts_get_symbol(tos);
		  value = fts_variable_get_value(object, varname);

		  if (value)
		    {
		      *tos = *value;
		      fts_expression_add_var_ref(e, varname);
		    }
		  else
		    OP_ERROR;
		}
	      else
		{
		  fts_object_get_prop(obj, prop, ptos);

		  if (fts_is_void(ptos))
		    OP_ERROR;
		}
	    }
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_REMAINDER:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) % fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SHIFT_LEFT:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) << fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SHIFT_RIGHT:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) >> fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_AND:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) & fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_OR:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) | fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_BIT_XOR:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) ^ fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_LOGICAL_AND:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) && fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_LOGICAL_OR:
	  if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) || fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_EQUAL:
	  if (fts_atom_equal(tos, ptos))
	    fts_set_int(ptos, 1);
	  else
	    fts_set_int(ptos, 0);

	  break;

	case FTS_OP_NOT_EQUAL:
	  if (fts_atom_equal(tos, ptos))
	    fts_set_int(ptos, 0);
	  else
	    fts_set_int(ptos, 1);

	  break;

	case FTS_OP_GREATER:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_int(ptos, fts_get_float_num(ptos) > fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) > fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_GREATER_EQUAL:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_int(ptos, fts_get_float_num(ptos) >= fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) >= fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SMALLER:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_int(ptos, fts_get_float_num(ptos) < fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) < fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_SMALLER_EQUAL:
	  if (fts_is_float(tos) || fts_is_float(ptos))
	    fts_set_int(ptos,
			fts_get_float_num(ptos) <= fts_get_float_num(tos));
	  else if (fts_is_int(tos) && fts_is_int(ptos))
	    fts_set_int(ptos, fts_get_int(ptos) <= fts_get_int(tos));
	  else
	    return FTS_EXPRESSION_OP_TYPE_ERROR;
	  break;

	case FTS_OP_ELSE:
	  if ((value_stack_deeper_than(e, 2)) && (pop == FTS_OP_CONDITIONAL))
	    {
	      value_stack_pop(e);
	      
	      if (fts_atom_is_null(value_stack_top(e)))
		*value_stack_top(e) = *tos;
	      else
		*value_stack_top(e) = *ptos;
	    }
	  else
	    return FTS_EXPRESSION_SYNTAX_ERROR;
	  break;

	case FTS_OP_ASSIGN:
	  if (fts_is_symbol(ptos))
	    {
	      fts_expression_add_prop(e, fts_get_symbol(ptos), tos);
	      value_stack_pop(e); /* forget the value */
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
  fts_expr_assignement_t *p;

  p = (fts_expr_assignement_t *) fts_heap_alloc(&expr_prop_heap);

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
  int i;
  fts_expr_assignement_t *p;

  p = e->assignements;
  i = 0;
  while (p)
    {
      (* f)(p->name, &(p->value), data);

      p = p->next;
      i++;
    }

  e->assignements = 0;

  return i;
}


/* Variable reference handling */

static void fts_expression_add_var_ref(fts_expression_state_t *e, fts_symbol_t name)
{
  fts_expr_var_ref_t *p;

  p = (fts_expr_var_ref_t *) fts_heap_alloc(&expr_var_ref_heap);

  p->name  = name;
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

      fts_variable_add_user(obj, p->name, obj);

      p = p->next;
      fts_heap_free((char *) p2, &expr_var_ref_heap);
    }

  e->var_refs = 0;
}


/* DEBUG CODE */

void fts_expression_printf_assignements(fts_expression_state_t *e)
{
  fts_expr_assignement_t *p;

  p = e->assignements;

  while (p)
    {
      fprintf(stderr, "\tGot name %s value ", fts_symbol_name(p->name));
      fprintf_atoms(stderr, 1, &(p->value));
      fprintf(stderr, "\n");
      p = p->next;
    }
}


fts_expression_state_t *fts_expression_eval(fts_object_t *object,
					    int expr_size, const fts_atom_t *expr,
					    int result_size, fts_atom_t *result)
{
  fts_expression_state_t *e;

  e = fts_get_expression_state();

  while ((e->count < result_size) && expr_size > 0)
    {
      int ret;

      ret = fts_do_expression_eval(e, object, expr_size, expr, &result[e->count]);

      if (ret > 0)
	{
	  expr_size =- ret;

	  if (!fts_is_void(&result[e->count]))
	    e->count++;

	  if (e->count == result_size)
	    break;
	}
      else
	{
	  e->ret = ret;		/* @@@ ???  */

	  switch (ret)
	    {
	    case FTS_EXPRESSION_SYNTAX_ERROR:
	      fprintf(stderr, "Syntax error\n");
	      break;
	    case FTS_EXPRESSION_UNDEFINED_VARIABLE:
	      fprintf(stderr, "Undefined Variable\n");
	      break;
	    case FTS_EXPRESSION_ERROR_OBJECT_REFERENCE:
	      fprintf(stderr, "Reference to an error object\n");
	      break;
	    case FTS_EXPRESSION_OP_TYPE_ERROR:
	      fprintf(stderr, "Op Type Error\n");
	      break;
	    }

	  return e;
	}
    }

#ifdef EXPRESSION_TRACE_DEBUG
      fprintf(stderr, "\n Got result result: ");
      fprintf_atoms(stderr, e->count, result);
      fprintf(stderr, "\n");
#endif

#ifdef EXPRESSION_TRACE_DEBUG
  fts_expression_printf_assignements(e);
#endif

  return e;
}


/* Init function  */

void
fts_expressions_init(void)
{
  fts_heap_init(&expr_prop_heap, sizeof(fts_expr_assignement_t), 16);
  fts_heap_init(&expr_var_ref_heap, sizeof(fts_expr_var_ref_t), 16);

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
