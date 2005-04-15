/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ftsconfig.h>
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/parser.h>
#include <ftsprivate/message.h>
#include "parser.h"

#ifndef FTS_EXPRESSION_ARG_ONSET
#define FTS_EXPRESSION_ARG_ONSET 0
#endif

static fts_heap_t *expression_heap;

struct _fts_expression_t {
  fts_stack_t stack;            /* evaluation stack */
  fts_parsetree_t *tree;        /* parser abstract tree */
  int fp;                       /* frame pointer */
};

static fts_status_description_t invalid_expression_error_description = {
  "invalid expression"
};
static fts_status_t invalid_expression_error = &invalid_expression_error_description;

static fts_status_description_t operand_type_mismatch_error_description = {
  "operand type mismatch"
};
static fts_status_t operand_type_mismatch_error = &operand_type_mismatch_error_description;

static fts_status_description_t invalid_environment_variable_error_description = {
  "invalid environment variable"
};
static fts_status_t invalid_environment_variable_error = &invalid_environment_variable_error_description;

static fts_status_description_t invalid_parenthized_expression_error_description = {
  "invalid expression within parenthesis"
};
static fts_status_t invalid_parenthized_expression_error = &invalid_parenthized_expression_error_description;

/* **********************************************************************
 *
 * expression evaluation stack
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


static void
expression_stack_print( fts_expression_t *exp, const char *msg)
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

static void
expression_stack_push_frame( fts_expression_t *exp)
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

static void
expression_stack_pop_frame( fts_expression_t *exp)
{
  int old_fp, ac, i;
  fts_atom_t *at;

#ifdef STACK_DEBUG
  expression_stack_print( exp, "Stack before poping frame");
#endif

  /* Release all the objects that are in the frame */
  ac = expression_stack_frame_count( exp);
  at = expression_stack_frame( exp);
  for ( i = 0; i < ac; i++, at++)
    fts_atom_release( at);

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


/* **********************************************************************
 *
 * expression evaluation operators
 *
 */

/* Helper functions for symbol concatenation */
static fts_symbol_t 
concatenate_symbol( fts_atom_t *left, fts_atom_t *right)
{
  const char *lsym, *rsym;
  char *buffer;

  lsym = fts_symbol_name(fts_get_symbol( left));
  rsym = fts_symbol_name(fts_get_symbol( right));
  buffer = alloca( strlen( lsym) + strlen( rsym) + 1);

  strcpy( buffer, lsym);
  strcat( buffer, rsym);

  return fts_new_symbol( buffer);
}

static fts_symbol_t 
concatenate_symbol_int( fts_atom_t *left, fts_atom_t *right)
{
  const char *sym;
  int len;
  char *buffer;

  sym = fts_symbol_name(fts_get_symbol( left));
  len = strlen( sym);
  buffer = alloca( len + 256);

  strcpy( buffer, sym);
  sprintf( buffer + len, "%d", fts_get_int( right));

  return fts_new_symbol( buffer);
}

static fts_symbol_t 
concatenate_int_symbol( fts_atom_t *left, fts_atom_t *right)
{
  const char *sym;
  char *buffer;

  sym = fts_symbol_name(fts_get_symbol( right));
  buffer = alloca( strlen( sym) + 256);

  sprintf( buffer, "%d", fts_get_int( left));
  strcpy( buffer + strlen(buffer), sym);

  return fts_new_symbol( buffer);
}

/*
 * Operators
 */

#define UNOP_EVAL(OP)											 \
  if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											 \
  top = expression_stack_top( exp);									 \
  if (fts_is_int( top))											 \
    fts_set_int( top, OP fts_get_int( top));								 \
  else if (fts_is_float( top))										 \
    fts_set_float( top, OP fts_get_float( top));							 \
  else													 \
    return operand_type_mismatch_error;

#define IUNOP_EVAL(OP)											 \
  if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											 \
  top = expression_stack_top( exp);									 \
  if (fts_is_int( top))											 \
    fts_set_int( top, OP fts_get_int( top));								 \
  else													 \
    return operand_type_mismatch_error;

#define ABINOP_EVAL(OP)											  \
  if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);									  \
  if (fts_is_int( top-1) && fts_is_int( top))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top));					  \
  else if (fts_is_int( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_int( top-1) OP fts_get_float( top));					  \
  else if (fts_is_float( top-1) && fts_is_int( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) OP fts_get_int( top));					  \
  else if (fts_is_float( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) OP fts_get_float( top));				  \
  else													  \
    return operand_type_mismatch_error;									  \
  expression_stack_pop( exp, 1);

#define ABINOP_EVAL_PLUS										  \
  if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);									  \
  if (fts_is_int( top-1) && fts_is_int( top))								  \
    fts_set_int( top-1, fts_get_int( top-1) + fts_get_int( top));					  \
  else if (fts_is_int( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_int( top-1) + fts_get_float( top));					  \
  else if (fts_is_float( top-1) && fts_is_int( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) + fts_get_int( top));					  \
  else if (fts_is_float( top-1) && fts_is_float( top))							  \
    fts_set_float( top-1, fts_get_float( top-1) + fts_get_float( top));					  \
  else if (fts_is_symbol( top-1) && fts_is_symbol( top))						  \
    fts_set_symbol( top-1, concatenate_symbol( top-1, top));						  \
  else if (fts_is_symbol( top-1) && fts_is_int( top))							  \
    fts_set_symbol( top-1, concatenate_symbol_int( top-1, top));					  \
  else if (fts_is_int( top-1) && fts_is_symbol( top))							  \
    fts_set_symbol( top-1, concatenate_int_symbol( top-1, top));					  \
  else													  \
    return operand_type_mismatch_error;									  \
  expression_stack_pop( exp, 1);

#define IABINOP_EVAL(OP)										  \
  if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);									  \
  if (fts_is_int( top) && fts_is_int( top-1))								  \
    fts_set_int( top-1, fts_get_int( top-1) OP fts_get_int( top) );					  \
  else													  \
    return operand_type_mismatch_error;									  \
  expression_stack_pop( exp, 1);


/* macro generating code to evaluate binary comparison operator OP between 
   all combinations of data types: 
   (int|float) OP (int|float) and symbol OP symbol
*/
#define LBINOP_EVAL(OP)											  \
  if ((status = expression_eval_aux(tree->left,  exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)  \
    return status;											  \
  if ((status = expression_eval_aux(tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok) \
    return status;											  \
  top = expression_stack_top( exp);									  \
  if      (fts_is_int(top-1)    &&  fts_is_int(top))							  \
    fts_set_int(top-1, fts_get_int(top-1)   OP fts_get_int(top));					  \
  else if (fts_is_int(top-1)    &&  fts_is_float(top))							  \
    fts_set_int(top-1, fts_get_int(top-1)   OP fts_get_float(top));					  \
  else if (fts_is_float(top-1)  &&  fts_is_int(top))							  \
    fts_set_int(top-1, fts_get_float(top-1) OP fts_get_int(top));					  \
  else if (fts_is_float(top-1)  &&  fts_is_float(top))							  \
    fts_set_int(top-1, fts_get_float(top-1) OP fts_get_float(top));					  \
  else if (fts_is_symbol(top-1)  &&  fts_is_symbol(top))	/* string comparison */			  \
      fts_set_int(top-1, strcmp(fts_symbol_name(fts_get_symbol(top-1)),					  \
				fts_symbol_name(fts_get_symbol(top)))  OP  0); \
  else													  \
    return operand_type_mismatch_error;									  \
  expression_stack_pop( exp, 1);


/***********************************************************************
 *
 * expression evaluator
 *
 */

static fts_status_t 
expression_eval_aux( fts_parsetree_t *tree, fts_expression_t *exp, fts_hashtable_t *locals, fts_hashtable_t *globals, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data)
{
  int ac;
  fts_atom_t *at, *top, ret[1];
  fts_status_t status = fts_ok;
  fts_object_t *obj;

  if (!tree)
    return fts_ok;
  
  switch( tree->token) {

    case TK_COMMA:
      expression_stack_push_frame( exp);
      
      if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
        return status;
        
      ac = expression_stack_frame_count( exp);
      at = expression_stack_frame( exp);
      
      if ((status = (*callback)( ac, at, data)) != fts_ok)
        return status;
        
      expression_stack_pop_frame( exp);    
      
      if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
        return status;
        
        break;
      
    case TK_SEMI:
      expression_stack_push_frame( exp);
      
      if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
        return status;
        
      expression_stack_pop_frame( exp);    

      if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
        return status;
        
        break;
      
  case TK_TUPLE:
    expression_stack_push_frame( exp);

    if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);

    obj = fts_object_create( fts_tuple_class, ac, at);
    fts_object_refer( obj);
    fts_set_object( ret, obj);

    expression_stack_pop_frame( exp);
    expression_stack_push( exp, ret);

    break;

  case TK_ELEMENT:
    expression_stack_push_frame( exp);

    if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);

    if (!fts_is_object( at))
      return operand_type_mismatch_error;

    fts_set_void( fts_get_return_value());

    if(fts_send_message(fts_get_object( at), fts_s_get_element, ac - 1, at + 1) == NULL || fts_is_void( fts_get_return_value()))
      return fts_ignore;
      
    fts_atom_refer(fts_get_return_value());

    expression_stack_pop_frame( exp);
    expression_stack_push( exp, fts_get_return_value());

    break;

  case TK_SPACE:
    if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    break;

  case TK_PAR:
    expression_stack_push_frame( exp);

    if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;

    ac = expression_stack_frame_count( exp);
    at = expression_stack_frame( exp);

    if (ac > 0 && fts_is_symbol( at))
    {
      /* it is a function call */
      fts_symbol_t fun_name = fts_get_symbol( at);
      fts_fun_t fun;
      
      fun = fts_get_function_by_name(fun_name);
      
      if (!fun)
        return fts_status_format("undefined function %s", fts_symbol_name(fun_name));
      
      fts_set_void(ret);
      
      status = (*fun)(ac - 1, at + 1, ret);
      
      expression_stack_pop_frame( exp);
      
      if(!fts_is_void(ret))
      {
        expression_stack_push( exp, ret);
        fts_atom_refer(ret);
      }
      
      return status;
    }
    else if (ac > 1 && fts_is_object( at) && fts_is_symbol( at+1))
    {      
      /* it is a method invocation */
      fts_object_t *obj = fts_get_object(at);
      fts_symbol_t selector = fts_get_symbol(at + 1);
      
      fts_object_refer(obj);
      
      fts_set_void( fts_get_return_value());
      
      if(fts_send_message(obj, selector, ac - 2, at + 2) == NULL)
      {
        fts_symbol_t clname = fts_object_get_class_name(obj);
        
        if(clname != NULL)
          return fts_status_format("method %s not defined with given arguments for %s object", fts_symbol_name(selector), fts_symbol_name(clname));
        else
          return fts_status_format("method %s not defined with given arguments", fts_symbol_name(selector));
      }
      
      expression_stack_pop_frame( exp);
      
      if(!fts_is_void( fts_get_return_value()))
      {
        /* push return value */
        expression_stack_push( exp, fts_get_return_value());
        fts_atom_refer(fts_get_return_value());
      }
      
      fts_object_release(obj);
    }
    else if (ac == 1)
    {
      /* it is a plain parenthized term */
      ret[0] = at[0];
      
      fts_atom_refer(ret);
      
      expression_stack_pop_frame( exp);
      
      expression_stack_push( exp, ret);
    }
    else
      return invalid_parenthized_expression_error;

    break;

  case TK_INT:
  case TK_FLOAT:
  case TK_SYMBOL:
    expression_stack_push( exp, &tree->value);
    break;

  case TK_DOLLAR:
    if (fts_is_int( &tree->value))
    {
      int index = fts_get_int( &tree->value) - FTS_EXPRESSION_ARG_ONSET;
      
      if (index >= 0 && index < env_ac)
        expression_stack_push( exp, env_at + index);
      else
        return invalid_environment_variable_error;
      
      fts_atom_refer( env_at+index);
    }
    else if (fts_is_symbol( &tree->value))
    {
      fts_atom_t *name = &tree->value;
      fts_symbol_t sym = fts_get_symbol(name);
      
      if(sym == fts_s_times)
      {
        int i;
        
        for(i=0; i<env_ac; i++)
        {
          if(!fts_is_void(env_at + i))
          {
            expression_stack_push(exp, env_at + i);
            fts_atom_refer(env_at + i);
          }
        }
      }
      else
      {
        fts_atom_t value;
        
        fts_set_void(&value);
        
        if(locals != NULL && fts_hashtable_get(locals, name, &value))
        {
          if(fts_is_pointer(&value))
            value = *(fts_definition_get_value((fts_definition_t *)fts_get_pointer(&value)));
          
          if(!fts_is_void(&value))
          {
            expression_stack_push( exp, &value);
            fts_atom_refer(&value);
            break;
          }
        }
        
        if(globals != NULL && fts_hashtable_get(globals, name, &value))
        {
          if(fts_is_pointer(&value))
            value = *(fts_definition_get_value((fts_definition_t *)fts_get_pointer(&value)));
          
          if(!fts_is_void(&value))
          {
            expression_stack_push( exp, &value);
            fts_atom_refer(&value);
            break;
          }
        }
        
        return fts_status_format("undefined variable %s", fts_symbol_name(fts_get_symbol(name)));
      }
    }
    else
      return invalid_environment_variable_error;

    break;

  case TK_POWER:
    if ((status = expression_eval_aux( tree->left, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
      return status;
    if ((status = expression_eval_aux( tree->right, exp, locals, globals, env_ac, env_at, callback, data)) != fts_ok)
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
    ABINOP_EVAL_PLUS;
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
  }

  return fts_ok;
}


fts_status_t
fts_expression_evaluate( fts_expression_t *exp, fts_hashtable_t *locals, fts_hashtable_t *globals, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data)
{
#ifdef EXPRESSION_DEBUG
  fts_expression_print( exp);
#endif
  
  if (exp->tree == NULL)
    return invalid_expression_error;
  
  expression_stack_init( exp);
  
  return expression_eval_aux( exp->tree, exp, locals, globals, env_ac, env_at, callback, data);
}

fts_status_t 
fts_expression_reduce( fts_expression_t *exp, fts_patcher_t *scope, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data)
{
  fts_hashtable_t *locals = NULL;
  fts_hashtable_t *globals = fts_patcher_get_definitions(fts_get_root_patcher());
  
#ifdef EXPRESSION_DEBUG
  fts_expression_print( exp);
#endif

  if(scope != NULL)
    locals = fts_patcher_get_definitions(scope);
     
  if (exp->tree == NULL)
    return invalid_expression_error;
  
  expression_stack_init( exp);
  
  return expression_eval_aux( exp->tree, exp, locals, globals, env_ac, env_at, callback, data);
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
      int n = fts_get_int( &tree->value) - FTS_EXPRESSION_ARG_ONSET + 1;
      
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
  {
    fts_symbol_t name = fts_get_symbol( &tree->value);

    fts_name_add_listener(scope, name, obj);
    fts_object_add_binding(obj, scope, name);
  }
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
  case TK_SPACE: fprintf( stderr, "SPACE\n"); break;
  case TK_INT: fprintf( stderr, "INT %d\n", fts_get_int( &tree->value)); break;
  case TK_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( &tree->value)); break;
  case TK_SYMBOL: fprintf( stderr, "SYMBOL %s\n", fts_get_symbol( &tree->value)); break;
  case TK_PAR: fprintf( stderr, "()\n"); break;
  case TK_TUPLE: fprintf( stderr, "{}\n"); break;
  case TK_ELEMENT: fprintf( stderr, "[]\n"); break;
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
#if 0
  case TK_DOT: fprintf( stderr, ".\n"); break;
#endif
  default: fprintf( stderr, "UNKNOWN %d\n", tree->token); 
  }

  expression_print_aux( tree->left, indent+1);
  expression_print_aux( tree->right, indent+1);
}

static void fts_expression_print( fts_expression_t *exp)
{
  expression_print_aux( exp->tree, 0);
}

/***********************************************************************
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

void 
fts_expression_clear( fts_expression_t *exp)
{
  if (exp->tree)
    fts_parsetree_delete( exp->tree);
  
  exp->tree = NULL;
}

void 
fts_expression_delete( fts_expression_t *exp)
{
  if (exp->tree)
    fts_parsetree_delete( exp->tree);

  fts_stack_destroy( &exp->stack);

  fts_heap_free( exp, expression_heap);
}

int
fts_expression_is_valid( fts_expression_t *exp)
{
  return exp->tree != NULL;
}

/***********************************************************************
 *
 * Kernel initialization
 *
 */
void
fts_kernel_expression_init( void)
{
  expression_heap = fts_heap_new( sizeof( fts_expression_t));
}

