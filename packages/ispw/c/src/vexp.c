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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


/*
 * vexp.c -- a variable expression evaluator
 *
 * This modules implements an expression evaluator using the the 
 * operator-precedence parsing.  It transforms an infix expression
 * to a prefix stack ready to be evaluated.  The expression sysntax
 * is close to that of C.  There are a few operators that are not
 * supported and functions are also recognized.  Strings can be
 * passed to functions when they are quoted in '"'s. "[]" are implememted
 * as an easy way of accessing the content of tables, and the syntax
 * table_name[index].
 * Variables (inlets) are specified with the following syntax: $x#,
 * where x is either i(integers), f(floats), and s(strings); and #
 * is a digit that coresponds to the inlet number.  The string variables
 * can be used as strings when they are quoted and can also be used as
 * table names when they are followed by "[]".
 *
 */


#include <string.h>

#include <fts/fts.h>
#include "vexp.h" 
#include "vexp_util.h" 


char *atoif(char *s, long int *value, long int *type);

static struct ex_ex *ex_lex(struct expr *exp, long int *n);
struct ex_ex *ex_match(struct ex_ex *eptr, long int op);
struct ex_ex *ex_parse(struct ex_ex *iptr, struct ex_ex *optr, long int *argc);
struct ex_ex *ex_eval(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr);
struct ex_ex *eval_func(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr);
struct ex_ex *eval_tab(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr);
t_ex_func *find_func(char *s);

#define	MAX_ARGS	10
extern t_ex_func ex_funcs[];

struct ex_ex nullex;

void set_tokens (char *s);
int getoken (struct expr *exp, struct ex_ex *eptr);
void ex_print (struct ex_ex *eptr);

/*
 * ex_new -- translate the expression string in exp_string to a prefix stack
 *	     for the expr.
 *	     if successfull it will return 0 otherwise 1
 */

int ex_new(struct expr *expr, char *exp_string)
{
  struct ex_ex *list;
  struct ex_ex *ret;
  long max_node = 0;		/* maximum number of nodes needed */
  
  if (!exp_string)
    return(1);
  set_tokens(exp_string);
  list = ex_lex(expr, &max_node);
  set_tokens((char *)0);
  if (!list) {		/* syntax error */
    return (1);
  }
  expr->exp_stack = (struct ex_ex *)fts_malloc(max_node * sizeof (struct ex_ex));
  ret = ex_match(list, (long)0);
  if (!ret)		/* syntax error */
    goto error;
  ret = ex_parse(list, expr->exp_stack, (long *)0);
  if (ret) {
    *ret = nullex;
    /* print the stack that been built */
    return (0);
  }
 error:
  fts_free(expr->exp_stack);
  expr->exp_stack = 0;
  fts_free(list);
  return (1);
}

/*
 * ex_lex -- This routine is a bit more than a lexical parser since it will
 *	     also do some syntax checking.  It reads the string s and will
 *	     return a linked list of struct ex_ex. 
 *	     It will also put the number of the nodes in *n.
 */
struct ex_ex *
  ex_lex(struct expr *exp, long int *n)
{
  struct ex_ex *list_arr;
  struct ex_ex *exptr;
  long non = 0;		/* number of nodes */
  long maxnode = 0;
  
  list_arr = (struct ex_ex *)fts_malloc(sizeof (struct ex_ex) * MINODES);
  if (! list_arr) {
    post("ex_lex: no mem\n");
    return ((struct ex_ex *)0);
  }
  exptr = list_arr;
  maxnode = MINODES;
  
  while (8)
    {
      if (non >= maxnode)
	{
	  maxnode += MINODES;
      
	  list_arr = fts_realloc((void *)list_arr, sizeof (struct ex_ex) * maxnode);
	  if (! list_arr)
	    {
	      post("ex_lex: no mem\n");
	      return ((struct ex_ex *)0);
	    }
	  exptr = &(list_arr)[non];
	}

      if (getoken(exp, exptr))
	{
	  fts_free(list_arr);
	  return ((struct ex_ex *)0);
	}
      non++;

      if (!exptr->ex_type)
	break;

      exptr++;
    }
  *n = non;

  return list_arr;
}

/*
 * ex_match -- this routine walks through the eptr and matches the
 *             pranthesis and brackets, it also converts the function
 *	       names to a pointer to the describing structure of the 
 *	       specified function
 */
struct ex_ex *
  ex_match(struct ex_ex *eptr, long int op)

/* operator to match */
{
  int firstone = 1;
  struct ex_ex *ret;
  t_ex_func *fun;
  
  for (; 8; eptr++, firstone = 0) {
    switch (eptr->ex_type) {
   case 0:
      if (!op)
	return (eptr);
      post("expr syntax error: an open %s not matched\n",
	      op == OP_RP ? "parenthesis" : "bracket");
      return (exNULL);
   case ET_INT:
   case ET_FLT:
   case ET_II:
   case ET_FI:
   case ET_SI:
   case ET_SYM:
   case ET_VSYM:
      continue;
   case ET_TBL:
   case ET_FUNC:
   case ET_LP:
      /* CHANGE
	 case ET_RP:
	 */
   case ET_LB:
      /* CHANGE
	 case ET_RB:
	 */
      post("ex_match: unexpected type, %ld\n", eptr->ex_type);
      return (exNULL);
   case ET_OP:
      if (op == eptr->ex_op)
	return (eptr);
      /*
       * if we are looking for a right peranthesis
       * or a right bracket and find the other kind,
       * it has to be a syntax error
       */
      if ((eptr->ex_op == OP_RP && op == OP_RB) ||
	  (eptr->ex_op == OP_RB && op == OP_RP)) {
	post("expr syntax error: prenthesis or brackets not matched\n");
	return (exNULL);
      }
      /*
       * Up to now we have marked the unary minuses as
       * subrtacts.  Any minus that is the first one in
       * chain or is preceeded by anything except ')' and
       * ']' is a unary minus.
       */
      if (eptr->ex_op == OP_SUB) {
	ret = eptr - 1;
	if (firstone ||  (ret->ex_type == ET_OP &&
			  ret->ex_op != OP_RB && ret->ex_op != OP_RP))
	  eptr->ex_op = OP_UMINUS;
      } else if (eptr->ex_op == OP_LP) {
	ret = ex_match(eptr + 1, OP_RP);
	if (!ret)
	  return (ret);
	eptr->ex_type = ET_LP;
	eptr->ex_ptr = (char *) ret;
	eptr = ret;
      } else if (eptr->ex_op == OP_LB) {
	ret = ex_match(eptr + 1, OP_RB);
	if (!ret)
	  return (ret);
	eptr->ex_type = ET_LB;
	eptr->ex_ptr = (char *) ret;
	eptr = ret;
      }
      continue;
   case ET_STR:
      if (eptr[1].ex_type != ET_OP) {
	post("expr: syntax error: bad string '%s'\n", eptr->ex_ptr);
	return (exNULL);
      }
      if (eptr[1].ex_op == OP_LB) {
	char *tmp;

	eptr->ex_type = ET_TBL;
	tmp = eptr->ex_ptr;
	if (ex_getsym(tmp, &(eptr->ex_ptr))) {
	  post("expr: syntax error: problms with ex_getsym\n");
	  return (exNULL);
	}
	fts_free((void *)tmp);
      } else if (eptr[1].ex_op == OP_LP) 
	{
	  fun = find_func(eptr->ex_ptr);
	  if (!fun)
	    {
	      post("expr: syntax error: function %s not found\n", eptr->ex_ptr);
	      return (exNULL);
	    }
	  eptr->ex_type = ET_FUNC;
	  eptr->ex_ptr = (char *) fun;
      } else {
	post("expr: syntax error: bad string '%s'\n", eptr->ex_ptr);
	return (exNULL);
      }
      continue;
   default:
      post("ex_match: bad type\n");
      return (exNULL);
    }
  }
  /* NOTREACHED */
}

/*
 * ex_parse -- This function if called when we have already done some
 *	       parsing on the expression, and we have already matched
 *	       our brackets and parenthesis.  The main job of this
 *	       function is to convert the infix expression to the
 *	       prefix form. 
 *	       First we find the operator with the lowest precedence and
 *	       put it on the stack ('optr', it is really just an array), then
 *	       we call ourself (ex_parse()), on its arguments (unary operators
 *	       only have one operator.)  
 *	       When "argc" is set it means that we are parsing the arguments
 *	       of a function and we will increment *argc anytime we find
 *	       a a segment that can qualify as an argument (counting commas).
 *
 * 	       returns 0 on syntax error
 */
struct ex_ex *
  ex_parse(struct ex_ex *iptr, struct ex_ex *optr, long int *argc)

/* number of argument separated by comma */
{
  struct ex_ex *eptr;
  struct ex_ex *lowpre;	/* pointer to the lowest precedence */
  struct ex_ex savex;
  long pre = HI_PRE;
  long count;
  
  if (!iptr) {
    post("ex_parse: input is null, iptr = 0x%lx\n", iptr);
    return (exNULL);
  }
  if (!iptr->ex_type)
    return (exNULL);
  
  /*
   * the following loop finds the lowest precedence operator in the
   * the input token list, comma is explicitly checked here since
   * that is a special operator and is only legal in functions
   */
  for (eptr = iptr, count = 0; eptr->ex_type; eptr++, count++)
    switch (eptr->ex_type) {
   case ET_SYM:
   case ET_VSYM:
      if (!argc) {
	post("expr: syntax error: symbols allowed for functions only\n");
	ex_print(eptr);
	return (exNULL);
      }
   case ET_INT:
   case ET_FLT:
   case ET_II:
   case ET_FI:
      if (!count && !eptr[1].ex_type) {
	*optr++ = *eptr;
	return (optr);
      }
      break;
   case ET_OP:
      if (eptr->ex_op == OP_COMMA) {
	if (!argc || !count || !eptr[1].ex_type) {
	  post("expr: syntax error: illegal comma\n");
	  ex_print(eptr[1].ex_type ? eptr : iptr);
	  return (exNULL);
	}
      }
      if (!eptr[1].ex_type) {
	post("expr: syntax error: missing operand\n");
	ex_print(iptr);
	return (exNULL);
      }
      if ((eptr->ex_op & PRE_MASK) <= pre) {
	pre = eptr->ex_op & PRE_MASK;
	lowpre = eptr;
      }
      break;
   case ET_SI:
   case ET_TBL:
      if (eptr[1].ex_type != ET_LB) {
	post("expr: syntax error: tables need left bracket\n");
	ex_print(eptr);
	return (exNULL);
      }
      /* if this table is the only token, parse the table */
      if (!count &&
	  !((struct ex_ex *) eptr[1].ex_ptr)[1].ex_type) {
	savex = *((struct ex_ex *) eptr[1].ex_ptr);
	*((struct ex_ex *) eptr[1].ex_ptr) = nullex;
	*optr++ = *eptr;
	lowpre = ex_parse(&eptr[2], optr, (long *)0);
	*((struct ex_ex *) eptr[1].ex_ptr) = savex;
	return(lowpre);
      }
      eptr = (struct ex_ex *) eptr[1].ex_ptr;
      break;
   case ET_FUNC:
      if (eptr[1].ex_type != ET_LP) {
	post("expr: ex_parse: no parenthesis\n");
	return (exNULL);
      }
      /* if this function is the only token, parse it */
      if (!count &&
	  !((struct ex_ex *) eptr[1].ex_ptr)[1].ex_type) {
	long ac; 
	
	if (eptr[1].ex_ptr == (char *) &eptr[2]) {
	  post("expr: syntax error: missing argument\n");
	  ex_print(eptr);
	  return (exNULL);
	}
	ac = 0;
	savex = *((struct ex_ex *) eptr[1].ex_ptr);
	*((struct ex_ex *) eptr[1].ex_ptr) = nullex;
	*optr++ = *eptr;
	lowpre = ex_parse(&eptr[2], optr, &ac);
	if (!lowpre)
	  return (exNULL);
	ac++;
	if (ac !=
	    ((t_ex_func *)eptr->ex_ptr)->f_argc){
	  post("expr: syntax error: function '%s' needs %ld arguments\n",
		  ((t_ex_func *)eptr->ex_ptr)->f_name,
		  ((t_ex_func *)eptr->ex_ptr)->f_argc);
	  return (exNULL);
	}
	*((struct ex_ex *) eptr[1].ex_ptr) = savex;
	return (lowpre);
      }
      eptr = (struct ex_ex *) eptr[1].ex_ptr;
      break;
   case ET_LP:
   case ET_LB:
      if (!count &&
	  !((struct ex_ex *) eptr->ex_ptr)[1].ex_type) {
	if (eptr->ex_ptr == (char *)(&eptr[1])) {
	  post("expr: syntax error: empty '%s'\n",
		  eptr->ex_type==ET_LP?"()":"[]");
	  ex_print(eptr);
	  return (exNULL);
	}
	savex = *((struct ex_ex *) eptr->ex_ptr);
	*((struct ex_ex *) eptr->ex_ptr) = nullex;
	lowpre = ex_parse(&eptr[1], optr, (long *)0);
	*((struct ex_ex *) eptr->ex_ptr) = savex;
	return (lowpre);
      }
      eptr = (struct ex_ex *)eptr->ex_ptr;
      break;
   case ET_STR:
   default:
      ex_print(eptr);
      post("expr: ex_parse: type = 0x%lx\n", eptr->ex_type);
      return (exNULL);
    }
  
  if (pre == HI_PRE) {
    post("expr: syntax error: missing operation\n");
    ex_print(iptr);
    return (exNULL);
  }
  if (count < 2) {
    post("expr: syntax error: mission operand\n");
    ex_print(iptr);
    return (exNULL);
  }
  if (count == 2) {
    if (lowpre != iptr) {
      post("expr: ex_parse: unary operator should be first\n");
      return (exNULL);
    }
    if (!unary_op(lowpre->ex_op)) {
      post("expr: syntax error: not a uniary operator\n");
      ex_print(iptr);
      return (exNULL);
    }
    *optr++ = *lowpre;
    eptr = ex_parse(&lowpre[1], optr, argc);
    return (eptr);
  }
  if (lowpre == iptr) {
    post("expr: syntax error: mission operand\n");
    ex_print(iptr);
    return (exNULL);
  }
  savex = *lowpre;
  *lowpre = nullex;
  if (savex.ex_op != OP_COMMA)
    *optr++ = savex;
  else
    (*argc)++;
  eptr = ex_parse(iptr, optr, argc);
  if (eptr) {
    eptr = ex_parse(&lowpre[1], eptr, argc);
    *lowpre = savex;
  }
  return (eptr);
}

#define	EVAL(op); 	\
  eptr = ex_eval(exp, ex_eval(exp, eptr, &left), &right);		\
  if (left.ex_type == ET_INT || left.ex_type == ET_SYM) {		\
	  if (right.ex_type == ET_INT || right.ex_type == ET_SYM) { \
	      optr->ex_type = ET_INT;	\
		optr->ex_int = left.ex_int op right.ex_int;	\
	} else if (right.ex_type == ET_FLT) {			\
			  optr->ex_type = ET_FLT;	\
			    optr->ex_flt = (float)left.ex_int op right.ex_flt;\
    } else {			\
	     fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",	\
	__LINE__, right.ex_type);		\
	  return (exNULL);	\
	  }	\
  } else if (left.ex_type == ET_FLT) {	\
		  if (right.ex_type == ET_INT || right.ex_type == ET_SYM) { \
	      optr->ex_type = ET_FLT;	\
			optr->ex_flt=left.ex_flt op (float)right.ex_int;\
			} else if (right.ex_type == ET_FLT) {			\
				  optr->ex_type = ET_FLT;	\
				    optr->ex_flt = left.ex_flt op right.ex_flt; 	\
				    } else {			\
					  fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",	\
						__LINE__, right.ex_type);		\
						  return (exNULL);	\
						  }	\
						  } else { 	\
					  fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad left type %ld\n",__LINE__,left.ex_type);\
					    return (exNULL);		\
					    }		\
  return (eptr);

#define	EVAL_INT(op); 	\
  eptr = ex_eval(exp, ex_eval(exp, eptr, &left), &right);		\
  if (left.ex_type == ET_INT || left.ex_type == ET_SYM) {		\
			  if (right.ex_type == ET_INT || right.ex_type == ET_SYM) { \
	      optr->ex_type = ET_INT;	\
		optr->ex_int = left.ex_int op right.ex_int;	\
		} else if (right.ex_type == ET_FLT) {			\
			  optr->ex_type = ET_INT;	\
			    optr->ex_int=left.ex_int op (long)right.ex_flt;	\
			    } else {			\
				  fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",	\
		__LINE__, right.ex_type);		\
		  return (exNULL);	\
		  }	\
		  } else if (left.ex_type == ET_FLT) {	\
				  if (right.ex_type == ET_INT || right.ex_type == ET_SYM) { \
					      optr->ex_type = ET_INT;	\
			optr->ex_int = (long)left.ex_flt op right.ex_int;\
			} else if (right.ex_type == ET_FLT) {			\
				  optr->ex_type = ET_INT;	\
				    optr->ex_int = (long)left.ex_flt op (long)right.ex_flt;\
				    } else {			\
					  fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",	\
						__LINE__, right.ex_type);		\
						  return (exNULL);	\
						  }	\
						  } else { 	\
					  fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad left type %ld\n",__LINE__,left.ex_type);\
					    return (exNULL);		\
					    }		\
  return (eptr);

/*
 * ex_eval -- evaluate the array of prefix expression
 *	      ex_eval returns the pointer to the first unevaluated node
 *	      in the array.  This is a recursive routine.
 */
struct ex_ex *
  ex_eval(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr)
/* the expr object data pointer */
/* the operation stack */
/* the result pointer */
{
  struct ex_ex left, right;	/* left and right operands */
  
  if (!eptr)
    return (exNULL);
  switch (eptr->ex_type) {
 case ET_INT:
 case ET_FLT:
 case ET_SYM:
    *optr = *eptr++;
    return (eptr);
 case ET_II:
    optr->ex_type = ET_INT;
    if (eptr->ex_int == -1) {
      fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval: inlet number not set\n");
      return (exNULL);
    }
    optr->ex_int = exp->exp_var[eptr->ex_int].ex_int;
    return (++eptr);
 case ET_FI:
    optr->ex_type = ET_FLT;
    if (eptr->ex_int == -1) {
      fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval: inlet number not set\n");
      return (exNULL);
    }
    optr->ex_flt = exp->exp_var[eptr->ex_int].ex_flt;
    return (++eptr);
 case ET_VSYM:
    optr->ex_type = ET_SYM;
    if (eptr->ex_int == -1) {
      fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval: inlet number not set\n");
      return (exNULL);
    }
    optr->ex_ptr = exp->exp_var[eptr->ex_int].ex_ptr;
    return(++eptr);
 case ET_OP:
    break;
 case ET_TBL:
 case ET_SI:
    return (eval_tab(exp, eptr, optr));
 case ET_FUNC:
    return (eval_func(exp, eptr, optr));
 case ET_STR:
 case ET_LP:
 case ET_LB:
 default:
    fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval: unexpected type %d\n", eptr->ex_type);
    return (exNULL);
  }
  if (!eptr[1].ex_type) {
    fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval: not enough nodes 1\n");
    return (exNULL);
  }
  if (!unary_op(eptr->ex_op) && !eptr[2].ex_type) {
    fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval: not enough nodes 2\n");
    return (exNULL);
  }
  
  switch((eptr++)->ex_op) {
 case OP_NOT:
    eptr = ex_eval(exp, eptr, &left);
    optr->ex_type = ET_INT;
    if (left.ex_type == ET_FLT)
      optr->ex_int = !left.ex_flt;
    else
      optr->ex_int = !left.ex_int;
    return (eptr);
 case OP_NEG:
    eptr = ex_eval(exp, eptr, &left);
    optr->ex_type = ET_INT;
    if (left.ex_type == ET_FLT)
      optr->ex_int = ~((long)left.ex_flt);
    else
      optr->ex_int = ~left.ex_int;
    return (eptr);
 case OP_UMINUS:
    eptr = ex_eval(exp, eptr, &left);
    if (left.ex_type == ET_FLT) {
      optr->ex_type = ET_FLT;
      optr->ex_flt = -left.ex_flt;
    } else {
      optr->ex_type = ET_INT;
      optr->ex_int = -left.ex_int;
    }
    return (eptr);
 case OP_MUL:
    EVAL(*);
 case OP_DIV:
    eptr = ex_eval(exp, ex_eval(exp, eptr, &left), &right);
    if (left.ex_type == ET_INT || left.ex_type == ET_SYM) {
      if (right.ex_type==ET_INT || right.ex_type==ET_SYM) {
	optr->ex_type = ET_INT;
	if (!right.ex_int) {
	  right.ex_int = 1;
	}
	optr->ex_int = left.ex_int / right.ex_int;
      } else if (right.ex_type == ET_FLT) {
	optr->ex_type = ET_FLT;
	if (!right.ex_flt) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_flt = 1;
	}
	optr->ex_flt = (float)left.ex_int/right.ex_flt;
      } else {
	fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",
	      __LINE__, right.ex_type);
	return (exNULL);
      }
    } else if (left.ex_type == ET_FLT) {
      if (right.ex_type==ET_INT || right.ex_type==ET_SYM) {
	optr->ex_type = ET_FLT;
	if (!right.ex_int) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_int = 1;
	}
	optr->ex_flt=left.ex_flt / (float)right.ex_int;
      } else if (right.ex_type == ET_FLT) {
	optr->ex_type = ET_FLT;
	if (!right.ex_flt) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_flt = 1;
	}
	optr->ex_flt = left.ex_flt / right.ex_flt;
      } else {
	fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",
	      __LINE__, right.ex_type);
	return (exNULL);
      }
    } else {
	fts_object_signal_runtime_error((fts_object_t *) exp, 
		   "expr: ex_eval(%d): bad left type %ld\n",
		   __LINE__,left.ex_type);
      return (exNULL);
    }
    return (eptr);
    
 case OP_MOD:
    eptr = ex_eval(exp, ex_eval(exp, eptr, &left), &right);
    if (left.ex_type == ET_INT || left.ex_type == ET_SYM) {
      if (right.ex_type==ET_INT || right.ex_type==ET_SYM) {
	if (!right.ex_int) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_int = 1;
	}
	optr->ex_type = ET_INT;	
	optr->ex_int = left.ex_int % right.ex_int;
      } else if (right.ex_type == ET_FLT) {
	optr->ex_type = ET_INT;	
	if (!right.ex_flt) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_flt = 1;
	}
	optr->ex_int=left.ex_int % (long)right.ex_flt;
      } else {
	fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",
	      __LINE__, right.ex_type);
	return (exNULL);
      }
    } else if (left.ex_type == ET_FLT) {
      if (right.ex_type==ET_INT || right.ex_type==ET_SYM) { 
	optr->ex_type = ET_INT;	
	if (!right.ex_int) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_int = 1;
	}
	optr->ex_int=(long)left.ex_flt % right.ex_int;
      } else if (right.ex_type == ET_FLT) {
	optr->ex_type = ET_INT;	
	if (!right.ex_flt) {
	  /* post("expr: divide by zero detected\n"); */
	  right.ex_flt = 1;
	}
	optr->ex_int =
	  (long)left.ex_flt % (long)right.ex_flt;
      } else {
	fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_eval(%d): bad right type %ld\n",
	      __LINE__, right.ex_type);
	return (exNULL);
      }
    } else { 
	fts_object_signal_runtime_error((fts_object_t *) exp, 
		   "expr: ex_eval(%d): bad left type %ld\n",
	    __LINE__,left.ex_type);
      return (exNULL);
    }
    return (eptr);
    
    /* Was here after the return :-< ??*/
    /* EVAL_INT(%); */
 case OP_ADD:
    EVAL(+);
 case OP_SUB:
    EVAL(-);
 case OP_SL:
    EVAL_INT(<<);
 case OP_SR:
    EVAL_INT(>>);
 case OP_LT:
    EVAL(<);
 case OP_LE:
    EVAL(<=);
 case OP_GT:
    EVAL(>);
 case OP_GE:
    EVAL(>=);
 case OP_EQ:
    EVAL(==);
 case OP_NE:
    EVAL(!=);
 case OP_AND:
    EVAL_INT(&);
 case OP_XOR:
    EVAL_INT(^);
 case OP_OR:
    EVAL_INT(|);
 case OP_LAND:
    EVAL_INT(&&);
 case OP_LOR:
    EVAL_INT(||);
 case OP_LP:
 case OP_RP:
 case OP_LB:
 case OP_RB:
 case OP_COMMA:
 case OP_SEMI:
 default:
   fts_object_signal_runtime_error((fts_object_t *) exp, "expr: ex_print: bad op 0x%x\n", eptr->ex_op);
    return (exNULL);
  }
  /* NOTREACHED */
}

/*
 * eval_func --  evaluate a function, call ex_eval() on all the arguments
 *		 so that all of them are terminal nodes. The call the
 *		 appropriate function
 */
struct ex_ex *
  eval_func(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr)
/* the expr object data pointer */
/* the operation stack */
/* the result pointer */
{
  int i;
  struct ex_ex args[MAX_ARGS];
  t_ex_func *f;
  
  f = (t_ex_func *)(eptr++)->ex_ptr;
  if (!f || !f->f_name) {
    return (exNULL);
  }
  if (f->f_argc > MAX_ARGS) {
    fts_object_signal_runtime_error((fts_object_t *) exp, "expr: eval_func: asking too many arguments\n");
    return (exNULL);
  }
  
  for (i = 0; i < f->f_argc; i++)
    eptr = ex_eval(exp, eptr, &args[i]);
  (*f->f_func)(f->f_argc, args, optr);
  return (eptr);
}

/*
 * eval_tab --
 */
struct ex_ex *eval_tab(struct expr *exp, struct ex_ex *eptr, struct ex_ex *optr)
/* the expr object data pointer */
/* the operation stack */
/* the result pointer */
{
  struct ex_ex arg;
  char *tbl;
  
  if (eptr->ex_type == ET_SI) {
    if (!exp->exp_var[eptr->ex_int].ex_ptr) {
	fts_object_signal_runtime_error((fts_object_t *) exp, "expr: syntax error: no string for inlet %ld\n", eptr->ex_int);
      return (exNULL);
    }
    tbl = (char *) exp->exp_var[eptr->ex_int].ex_ptr;
  } else if (eptr->ex_type == ET_TBL)
    tbl = (char *) eptr->ex_ptr;
  else {
    fts_object_signal_runtime_error((fts_object_t *) exp, "expr: eval_tbl: bad type %ld\n", eptr->ex_type);
    return (exNULL);
  }
  eptr = ex_eval(exp, ++eptr, &arg);

  if (max_ex_tab(exp, tbl, &arg, optr))
    return (exNULL);
  return (eptr);
}

static char *exp_str;
/*
 * set_tokens -- set a new string for reading tokens
 */

void
set_tokens(char *s)
{
  exp_str = s;
}
/*
 * getoken -- return 1 on syntax error otherwise 0
 */
int
getoken(struct expr *exp, struct ex_ex *eptr)
{
  char *p;
  long i;
  
  if (!exp_str) {
    post("expr: getoken: expression string not set\n");
    return (0);
  }
 retry:
  if (!*exp_str) {
    eptr->ex_type = 0;
    eptr->ex_int = 0;
    return (0);
  }
  eptr->ex_type = ET_OP;
  switch (*exp_str++) {
 case ' ':
 case '\t':
    goto retry;
 case ';':
   post("expr: syntax error: ';' not implemented\n");
   return (1);
 case ',':
    eptr->ex_op = OP_COMMA;
    break;
 case '(':
    eptr->ex_op = OP_LP;
    break;
 case ')':
    eptr->ex_op = OP_RP;
    break;
 case ']':
    eptr->ex_op = OP_RB;
    break;
 case '~':
    eptr->ex_op = OP_NEG;
    break;
    /* we will take care of unary minus later */
 case '*':
    eptr->ex_op = OP_MUL;
    break;
 case '/':
    eptr->ex_op = OP_DIV;
    break;
 case '%':
    eptr->ex_op = OP_MOD;
    break;
 case '+':
    eptr->ex_op = OP_ADD;
    break;
 case '-':
    eptr->ex_op = OP_SUB;
    break;
 case '^':
    eptr->ex_op = OP_XOR;
    break;
 case '[':
    eptr->ex_op = OP_LB;
    break;
 case '!':
    if (*exp_str == '=') {
      eptr->ex_op = OP_NE;
      exp_str++;
    } else
      eptr->ex_op = OP_NOT;
    break;
 case '<':
    switch (*exp_str) {
   case '<':
      eptr->ex_op = OP_SL;
      exp_str++;
      break;
   case '=':
      eptr->ex_op = OP_LE;
      exp_str++;
      break;
   default:
      eptr->ex_op = OP_LT;
      break;
    }
    break;
 case '>':
    switch (*exp_str) {
   case '>':
      eptr->ex_op = OP_SR;
      exp_str++;
      break;
   case '=':
      eptr->ex_op = OP_GE;
      exp_str++;
      break;
   default:
      eptr->ex_op = OP_GT;
      break;
    }
    break;
 case '=':
    if (*exp_str++ != '=') {
      post("expr: syntax error: =\n");
      return (1);
    }
    eptr->ex_op = OP_EQ;
    break;
    
 case '&':
    if (*exp_str == '&') {
      exp_str++;
      eptr->ex_op = OP_LAND;
    } else
      eptr->ex_op = OP_AND;
    break;
    
 case '|':
    if ((*exp_str == '|')) {
      exp_str++;
      eptr->ex_op = OP_LOR;
    } else
      eptr->ex_op = OP_OR;
    break;
 case '$':
    switch (*exp_str++) {
   case 'I':
   case 'i':
      eptr->ex_type = ET_II;
      break;
   case 'F':
   case 'f':
      eptr->ex_type = ET_FI;
      break;
   case 'S':
   case 's':
      eptr->ex_type = ET_SI;
      break;
   default:
      post("expr: syntax error: %s\n", &exp_str[-2]);
      return (1);
    }
    p = atoif(exp_str, &eptr->ex_op, &i);
    if (!p) {
      post("expr: syntax error: %s\n", &exp_str[-2]);
      return (1);
    }
    if (i != ET_INT) {
      post("expr: syntax error: %s\n", exp_str);
      return (1);
    }
    /*
     * make the inlets one based rather than zero based
     */
    if (!eptr->ex_op || (eptr->ex_op)-- > MAX_VARS) {
      post("expr: syntax error: inlet out of range: %s\n", exp_str);
      return (1);
    }
    /* record the inlet type and check for consistency */
    if (!exp->exp_var[eptr->ex_op].ex_type)
      exp->exp_var[eptr->ex_op].ex_type = eptr->ex_type;
    else if (exp->exp_var[eptr->ex_op].ex_type != eptr->ex_type) {
      post("expr: syntax error: inlets can only have one type: %s\n", exp_str);
      return (1);
    }
    exp_str = p;
    break;
 case '"':
    {
      struct ex_ex ex;
      
      p = exp_str;
      if (!*exp_str || *exp_str == '"') {
	post("expr: syntax error: empty symbol: %s\n", --exp_str);
	return (1);
      }
      if (getoken(exp, &ex))
	return (1);
      switch (ex.ex_type) {
     case ET_STR:
	if (ex_getsym(ex.ex_ptr, &(eptr->ex_ptr))) {
	  post("expr: syntax error: getoken: problms with ex_getsym\n");
	  return (1);
	}
	eptr->ex_type = ET_SYM;
	break;
     case ET_SI:
	*eptr = ex;
	eptr->ex_type = ET_VSYM;
	break;
     default:
	post("expr: syntax error: bad symbol name: %s\n", p);
	return (1);
      }
      if (*exp_str++ != '"') {
	post("expr: syntax error: missing '\"'\n");
	return (1);
      }
      break;
    }
 case '.':
 case '0':
 case '1':
 case '2':
 case '3':
 case '4':
 case '5':
 case '6':
 case '7':
 case '8':
  case '9':
    p = atoif(--exp_str, &eptr->ex_int, &eptr->ex_type);
    if (!p)
      return (1);
    exp_str = p;
    break;
    
 default:
    /*
     * has to be a string, it should either be a 
     * function or a table 
     */
    p = --exp_str;
    for (i = 0; name_ok(*p); i++)
      p++;
    if (!i) {
      post("expr: syntax error: %s\n", exp_str);
      return (1);
    }
    eptr->ex_ptr = (char *)fts_malloc(i + 1);
    strncpy(eptr->ex_ptr, exp_str, (int) i);
    (eptr->ex_ptr)[i] = 0;
    exp_str = p;
    /*
     * we mark this as a string and later we will change this
     * to either a function or a table
     */
    eptr->ex_type = ET_STR;
    break;
  }
  return (0);
}

/*
 * atoif -- ascii to float or integer (understands hex numbers also)
 */
char *
  atoif(char *s, long int *value, long int *type)
{
  char *p;
  long int_val = 0;
  int flt = 0;
  float pos;
  float flt_val = 0;
  int base = 10;
  
  p = s;
  if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
    base = 16;
    p += 2;
  }
  while (8) {
    switch (*p) {
   case '.':
      if (flt || base != 10) {
	post("expr: syntax error: %s\n", s);
	return ((char *) 0);
      }
      flt++;
      pos = 10;
      flt_val = int_val;
      break;
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      if (flt) {
	flt_val += (*p - '0') / pos;
	pos *= 10;
      } else {
	int_val *= base;
	int_val += (*p - '0');
      }
      break;
   case 'a':
   case 'b':
   case 'c':
   case 'd':
   case 'e':
   case 'f':
      if (base != 16 || flt) {
	post("expr: syntax error: %s\n", s);
	return ((char *) 0);
      }
      int_val *= base;
      int_val += (*p - 'a' + 10);
      break;
   case 'A':
   case 'B':
   case 'C':
   case 'D':
   case 'E':
   case 'F':
      if (base != 16 || flt) {
	post("expr: syntax error: %s\n", s);
	return ((char *) 0);
      }
      int_val *= base;
      int_val += (*p - 'A' + 10);
      break;
   default:
      if (flt) {
	*type = ET_FLT;
	*((float *) value) = flt_val;
      } else {
	*type = ET_INT;
	*value = int_val;
      }
      return (p);
    }
    p++;
  }
}

/*
 * find_func -- returns a pointer to the found function structure
 *		otherwise it returns 0
 */
t_ex_func *
  find_func(char *s)
{
  t_ex_func *f;
  
  for (f = ex_funcs; f->f_name; f++)
    if (!strcmp(f->f_name, s))
      return (f);
  return ((t_ex_func *) 0);
}


/*
 * ex_print -- print an expression array
 */

void
ex_print(struct ex_ex *eptr)
{
  
  while (eptr->ex_type) {
    switch (eptr->ex_type) {
   case ET_INT:
      post("%ld ", eptr->ex_int);
      break;
   case ET_FLT:
      post("%f ", eptr->ex_flt);
      break;
   case ET_STR:
      post("%s ", eptr->ex_ptr);
      break;
   case ET_TBL:
      post("%s ", ex_symname((fts_symbol_t )eptr->ex_ptr));
   case ET_SYM:
      post("\"%s\" ", ex_symname((fts_symbol_t )eptr->ex_ptr));
      break;
   case ET_VSYM:
      post("\"$s%ld\" ", eptr->ex_int);
      break;
   case ET_FUNC:
      post("%s ",
	     ((t_ex_func *)eptr->ex_ptr)->f_name);
      break;
   case ET_LP:
      post("%c", '(');
      break;
      /* CHANGE
	 case ET_RP:
	 post("%c ", ')');
	 break;
	 */
   case ET_LB:
      post("%c", '[');
      break;
      /* CHANGE
	 case ET_RB:
	 post("%c ", ']');
	 break;
	 */
   case ET_II:
      post("$i%ld ", eptr->ex_int);
      break;
   case ET_FI:
      post("$f%ld ", eptr->ex_int);
      break;
   case ET_SI:
      post("$s%ld ", eptr->ex_int);
      break;
      
   case ET_OP:
      switch (eptr->ex_op) {
     case OP_LP:
	post("%c", '(');
	break;
     case OP_RP:
	post("%c ", ')');
	break;
     case OP_LB:
	post("%c", '[');
	break;
     case OP_RB:
	post("%c ", ']');
	break;
     case OP_NOT:
	post("%c", '!');
	break;
     case OP_NEG:
	post("%c", '~');
	break;
     case OP_UMINUS:
	post("%c", '-');
	break;
     case OP_MUL:
	post("%c", '*');
	break;
     case OP_DIV:
	post("%c", '/');
	break;
     case OP_MOD:
	post("%c", '%');
	break;
     case OP_ADD:
	post("%c", '+');
	break;
     case OP_SUB:
	post("%c", '-');
	break;
     case OP_SL:
	post("%s", "<<");
	break;
     case OP_SR:
	post("%s", ">>");
	break;
     case OP_LT:
	post("%c", '<');
	break;
     case OP_LE:
	post("%s", "<=");
	break;
     case OP_GT:
	post("%c", '>');
	break;
     case OP_GE:
	post("%s", ">=");
	break;
     case OP_EQ:
	post("%s", "==");
	break;
     case OP_NE:
	post("%s", "!=");
	break;
     case OP_AND:
	post("%c", '&');
	break;
     case OP_XOR:
	post("%c", '^');
	break;
     case OP_OR:
	post("%c", '|');
	break;
     case OP_LAND:
	post("%s", "&&");
	break;
     case OP_LOR:
	post("%s", "||");
	break;
     case OP_COMMA:
	post("%c", ',');
	break;
     case OP_SEMI:
	post("%c", ';');
	break;
     default:
	post("expr: ex_print: bad op 0x%lx\n", eptr->ex_op);
      }
      break;
   default:
      post("expr: ex_print: bad type 0x%lx\n", eptr->ex_type);
    }
    eptr++;
  }
  post("\n");
}
