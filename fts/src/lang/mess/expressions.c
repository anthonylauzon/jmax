/*
 * The FTS expression evaluator
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

#define FTS_BINARY_OP_TYPE 0
#define FTS_UNARY_OP_TYPE  1
#define FTS_PAR_OP_TYPE    2

/* constants and limits */

#define FTS_OPS_NUMBER 6
#define FTS_EXPR_MAX_DEPTH 256

/* Operator priority */

static int op_priority[FTS_OPS_NUMBER];

#define fts_op_before(op1, op2)   (op_priority[op1] >= op_priority[op2]) 

/* Operator type */

#define fts_op_can_be_unary(op)  ((op == FTS_OP_PLUS) || (op == FTS_OP_MINUS))

/* Parentesys check */

#define fts_is_open_par(a)       (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_open_par))
#define fts_is_closed_par(a)     (fts_is_symbol((a)) && (fts_get_symbol(a) == fts_s_closed_par))

/* First prototype version; only int, not variables, only +, -, *, / */

void
fts_expressions_init(void)
{
  fts_symbol_set_operator(fts_s_plus,  FTS_OP_PLUS);
  fts_symbol_set_operator(fts_s_minus, FTS_OP_MINUS);
  fts_symbol_set_operator(fts_s_times, FTS_OP_TIMES);
  fts_symbol_set_operator(fts_s_div,   FTS_OP_DIV);

  op_priority[FTS_OP_PLUS]  = 1;
  op_priority[FTS_OP_MINUS] = 1;
  op_priority[FTS_OP_TIMES] = 2;
  op_priority[FTS_OP_DIV]   = 2;
}

/*
  expr is a list of atoms keeping a list of expression;
  each expression is computed, the result is put in the 
  fts_atom_t array result; the number of computed values
  is returned from this function.

  Variables/constants are interpreted in the context given 
  by the patcher argument.

  Return -1 in case of errors
  */



static void fts_op_eval(int op, int op_type, int *value_stack_p, fts_atom_t *value_stack);

int
fts_expression_eval(fts_object_t *patcher,
		    int expr_size, const fts_atom_t *expr,
		    int result_size, fts_atom_t *result)
{
  /* eval status:
     - value stack
     - operator stack, with  bin/unary co-stack

     Stacks grow up.
     */

  fts_atom_t value_stack[FTS_EXPR_MAX_DEPTH];
  int value_stack_p = -1;	/* point to the last actually used cell */

  int op_stack[FTS_EXPR_MAX_DEPTH];
  int op_type_stack[FTS_EXPR_MAX_DEPTH];
  int op_stack_p = -1;		/* point to the last actually used cell */

  int result_count = 0;		/* result counter */

  /* Automata state */

  enum {waiting_arg, waiting_op} status; 

  /* Expression pointer: no backtrack, so we just iterate once
     on the whole expression list */

  int i;

  status = waiting_arg;

  for (i = 0; i  < expr_size; i++)
    {
      if (status == waiting_arg)
	{
	  if (fts_is_open_par(&expr[i]))
	    {
	      op_stack_p++;
	      op_stack[op_stack_p] = FTS_OP_OPEN_PAR;
	      op_type_stack[op_stack_p] = FTS_PAR_OP_TYPE;

	      status = waiting_arg;
	    }
	  else if (fts_is_closed_par(&expr[i]))
	    {
	      /* ERROR !!: unbalanced closed parentesis */

	      return -1;
	    }
	  else if (fts_is_operator(&expr[i]))
	    {
	      if (fts_op_can_be_unary(fts_get_operator(&expr[i])))
		{
		  op_stack_p++;
		  op_stack[op_stack_p] = fts_get_operator(&expr[i]);
		  op_type_stack[op_stack_p] = FTS_UNARY_OP_TYPE;
		}
	      else
		{
		  /* ERROR !! : syntax error, expression start with operator */

		  return -1;
		}
	    }
	  else
	    {
	      /* Anything else is an argument */

	      value_stack_p++;
	      value_stack[value_stack_p] = expr[i];

	      while ((op_stack_p >= 0) && (op_type_stack[op_stack_p] == FTS_UNARY_OP_TYPE))
		{
		  fts_op_eval(op_stack[op_stack_p], op_type_stack[op_stack_p], &value_stack_p, value_stack);
		  op_stack_p--;
		}
	      status = waiting_op;
	    }
	}
      else
	{
	  if (fts_is_open_par(&expr[i]))
	    {
	      while ((op_stack_p >= 0) && (op_stack[op_stack_p] != FTS_OP_OPEN_PAR))
		{
		  fts_op_eval(op_stack[op_stack_p], op_type_stack[op_stack_p], &value_stack_p, value_stack);
		  op_stack_p--;
		}


	      if ((op_stack_p == -1) && (value_stack_p == 0))
		{
		  if (result_count > result_size)
		    return result_count;

		  result[result_count++] = value_stack[value_stack_p];
		}
	      else
		{
		  /* ERROR !!: Syntax error in expression */

		  return -1;
		}

	      /* Back track */

	      value_stack_p = -1;
	      op_stack_p = -1;
	      i--;
	      status = waiting_arg;
	    }
	  else if (fts_is_closed_par(&expr[i]))
	    {
	      while ((op_stack_p >= 0) && (op_stack[op_stack_p] != FTS_OP_OPEN_PAR))
		{
		  fts_op_eval(op_stack[op_stack_p], op_type_stack[op_stack_p], &value_stack_p, value_stack);
		  op_stack_p--;
		}

	      if ((op_stack_p == -1) || (op_type_stack[op_stack_p] != FTS_PAR_OP_TYPE))
		{
		  /* Error: unbalanced ')' parantesis */

		  return -1;
		}

	      op_stack_p --;	/* pop the parentesys */

	      status = waiting_op;
	    }
	  else if (fts_is_operator(&expr[i]))
	    {
	      /* Is this code correct for unary ops ??? */

	      int op;

	      op = fts_get_operator(&expr[i]);

	      while (op_stack_p >= 0 && fts_op_before(op_stack[op_stack_p], op))
		{
		  fts_op_eval(op_stack[op_stack_p], op_type_stack[op_stack_p], &value_stack_p, value_stack);
		  op_stack_p--;
		}

	      op_stack_p++;
	      op_stack[op_stack_p] = op;
	      op_type_stack[op_stack_p] = FTS_BINARY_OP_TYPE;

	      status = waiting_arg;
	    }
	  else
	    {
	      /* Anything else is an argument */

	      while ((op_stack_p >= 0) && (op_stack[op_stack_p] != FTS_OP_OPEN_PAR))
		{
		  fts_op_eval(op_stack[op_stack_p], op_type_stack[op_stack_p], &value_stack_p, value_stack);
		  op_stack_p--;
		}

	      if ((op_stack_p == -1) && (value_stack_p == 0))
		{
		  if (result_count > result_size)
		    return result_count;

		  result[result_count++] = value_stack[value_stack_p];
		}
	      else
		{
		  /* ERROR !!: Syntax error in expression */

		  return -1;
		}

	      /* Back track */

	      value_stack_p = -1;
	      op_stack_p = -1;
	      i--;
	      status = waiting_arg;
	    }
	}
    }

  while ((op_stack_p >= 0) && (op_stack[op_stack_p] != FTS_OP_OPEN_PAR))
    {
      fts_op_eval(op_stack[op_stack_p], op_type_stack[op_stack_p], &value_stack_p, value_stack);
      op_stack_p--;
    }

  if ((op_stack_p == -1) && (value_stack_p == 0))
    {
      if (result_count > result_size)
	return result_count;

      result[result_count++] = value_stack[value_stack_p];
    }
  else
    {
      /* ERROR !!: Syntax error in expression */

      return -1;
    }

  return result_count;
}

/* This thing work only for ints, for now !!!, no type promotion !! */

static void fts_op_eval(int op, int op_type, int *value_stack_p, fts_atom_t *value_stack)
{
  int tos = *value_stack_p;

  if (op_type == FTS_UNARY_OP_TYPE)
    {
      switch (op)
	{
	case FTS_OP_PLUS:
	  /* NOP, really */
	  return;
	case FTS_OP_MINUS:
	  fts_set_int(&value_stack[tos], (-1) * fts_get_int(&value_stack[tos]));
	  return;
	}
    }
  else
    {
      (*value_stack_p)--;

      if (fts_is_float(&value_stack[tos]) || fts_is_float(&value_stack[tos - 1]))
	{
	  float f1;
	  float f2;
      
	  f1 = fts_is_float(&value_stack[tos]) ? fts_get_float(&value_stack[tos]) : (float) fts_get_int(&value_stack[tos]);
	  f2 = fts_is_float(&value_stack[tos - 1]) ? fts_get_float(&value_stack[tos - 1]) : (float) fts_get_int(&value_stack[tos - 1]);

	  switch (op)
	    {
	    case FTS_OP_PLUS:
	      fts_set_float(&value_stack[tos-1], f2 + f1);
	      return;
	    case FTS_OP_MINUS:
	      fts_set_float(&value_stack[tos-1], f2 - f1);
	      return;
	    case FTS_OP_TIMES:
	      fts_set_float(&value_stack[tos-1], f2 * f1);
	      return;
	    case FTS_OP_DIV:
	      fts_set_float(&value_stack[tos-1], f2 / f1);
	      return;
	    }
	}
      else if (fts_is_int(&value_stack[tos]) && fts_is_int(&value_stack[tos - 1]))
	{
	  int i1;
	  int i2;

	  i1 = fts_get_int(&value_stack[tos]);
	  i2 = fts_get_int(&value_stack[tos - 1]);

	  switch (op)
	    {
	    case FTS_OP_PLUS:
	      fts_set_int(&value_stack[tos-1], i2 + i1);
	      return;
	    case FTS_OP_MINUS:
	      fts_set_int(&value_stack[tos-1], i2 - i1);
	      return;
	    case FTS_OP_TIMES:
	      fts_set_int(&value_stack[tos-1], i2 * i1);
	      return;
	    case FTS_OP_DIV:
	      fts_set_int(&value_stack[tos-1], i2 / i1);
	      return;
	    }
	}
    }
}


