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

%{
#include <math.h>
#include <stdio.h>
#include <fts/fts.h>

#define free fts_free
#define malloc fts_malloc
#define YYPARSE_PARAM data
#define YYLEX_PARAM data

static int yylex();
static int yyerror( const char *msg);

typedef struct _pnode_t pnode_t;
static pnode_t *pnode_new( int token, fts_atom_t *value, pnode_t *left, pnode_t *right);

%}

%pure_parser

%union {
  fts_atom_t a;
  pnode_t *n;
}

/*
 * Tokens
 */

%token <a> FTS_TOKEN_INT
%token <a> FTS_TOKEN_FLOAT
%token <a> FTS_TOKEN_SYMBOL

%token FTS_TOKEN_COLON
%token FTS_TOKEN_PAR
%token FTS_TOKEN_OPEN_PAR
%token FTS_TOKEN_CLOSED_PAR
%token FTS_TOKEN_CPAR
%token FTS_TOKEN_OPEN_CPAR
%token FTS_TOKEN_CLOSED_CPAR
%token FTS_TOKEN_SQPAR
%token FTS_TOKEN_OPEN_SQPAR
%token FTS_TOKEN_CLOSED_SQPAR

/*
 * Operators
 */


%left FTS_TOKEN_SEMI
%left FTS_TOKEN_TUPLE
%right FTS_TOKEN_EQUAL
%left FTS_TOKEN_LOGICAL_OR
%left FTS_TOKEN_LOGICAL_AND
%left FTS_TOKEN_EQUAL_EQUAL FTS_TOKEN_NOT_EQUAL
%left FTS_TOKEN_GREATER	FTS_TOKEN_GREATER_EQUAL FTS_TOKEN_SMALLER FTS_TOKEN_SMALLER_EQUAL
%right FTS_TOKEN_UMINUS, FTS_TOKEN_UPLUS
%left FTS_TOKEN_SHIFT_LEFT FTS_TOKEN_SHIFT_RIGHT
%left FTS_TOKEN_PLUS FTS_TOKEN_MINUS
%left FTS_TOKEN_TIMES FTS_TOKEN_DIV FTS_TOKEN_PERCENT
%right FTS_TOKEN_LOGICAL_NOT
%left FTS_TOKEN_POWER
%left FTS_TOKEN_ARRAY_INDEX
%left FTS_TOKEN_DOT
%right FTS_TOKEN_DOLLAR


%type <n> program
%type <n> list
%type <n> tuple
%type <n> term
%type <n> primitive
%type <n> par_op
%type <n> unary_op
%type <n> binary_op
%type <n> ref
%type <n> variable

%%
/* **********************************************************************
 *
 * Rules
 *
 */

program: list
		{ ((fts_parser_t *)data)->tree = $1; }
;

list: list FTS_TOKEN_SEMI tuple
		{ $$ = pnode_new( FTS_TOKEN_SEMI, 0, $1, $3); }
	| tuple
		{ $$ = pnode_new( FTS_TOKEN_SEMI, 0, 0, $1); }
	| 
		{ $$ = 0; }
;

tuple: tuple term   %prec FTS_TOKEN_TUPLE   
		{ $$ = pnode_new( FTS_TOKEN_TUPLE, 0, $1, $2); }
	| term
		{ $$ = pnode_new( FTS_TOKEN_TUPLE, 0, 0, $1); }
;

term: primitive
	| par_op
	| unary_op
	| binary_op
	| ref
;

primitive: FTS_TOKEN_INT
		{ $$ = pnode_new( FTS_TOKEN_INT, &($1), 0, 0); }
	| FTS_TOKEN_FLOAT
		{ $$ = pnode_new( FTS_TOKEN_FLOAT, &($1), 0, 0); }
	| FTS_TOKEN_SYMBOL
		{ $$ = pnode_new( FTS_TOKEN_SYMBOL, &($1), 0, 0); }
;

par_op: FTS_TOKEN_OPEN_PAR tuple FTS_TOKEN_CLOSED_PAR
		{ $$ = pnode_new( FTS_TOKEN_PAR, 0, 0, $2); }
;

unary_op: FTS_TOKEN_PLUS term %prec FTS_TOKEN_UPLUS
		{ $$ = pnode_new( FTS_TOKEN_UPLUS, 0, $2, 0); }
	| FTS_TOKEN_MINUS term %prec FTS_TOKEN_UMINUS
		{ $$ = pnode_new( FTS_TOKEN_UMINUS, 0, $2, 0); }
	| FTS_TOKEN_LOGICAL_NOT term
		{ $$ = pnode_new( FTS_TOKEN_LOGICAL_NOT, 0, $2, 0); }
;

binary_op: term FTS_TOKEN_PLUS term
		{ $$ = pnode_new( FTS_TOKEN_PLUS, 0, $1, $3); }
	| term FTS_TOKEN_MINUS term
		{ $$ = pnode_new( FTS_TOKEN_MINUS, 0, $1, $3); }
	| term FTS_TOKEN_TIMES term
		{ $$ = pnode_new( FTS_TOKEN_TIMES, 0, $1, $3); }
	| term FTS_TOKEN_DIV term
		{ $$ = pnode_new( FTS_TOKEN_DIV, 0, $1, $3); }
	| term FTS_TOKEN_POWER term
		{ $$ = pnode_new( FTS_TOKEN_POWER, 0, $1, $3); }
	| term FTS_TOKEN_PERCENT term
		{ $$ = pnode_new( FTS_TOKEN_PERCENT, 0, $1, $3); }
	| term FTS_TOKEN_SHIFT_LEFT term
		{ $$ = pnode_new( FTS_TOKEN_SHIFT_LEFT, 0, $1, $3); }
	| term FTS_TOKEN_SHIFT_RIGHT term
		{ $$ = pnode_new( FTS_TOKEN_SHIFT_RIGHT, 0, $1, $3); }
	| term FTS_TOKEN_LOGICAL_AND term
		{ $$ = pnode_new( FTS_TOKEN_LOGICAL_AND, 0, $1, $3); }
	| term FTS_TOKEN_LOGICAL_OR term
		{ $$ = pnode_new( FTS_TOKEN_LOGICAL_OR, 0, $1, $3); }
	| term FTS_TOKEN_EQUAL_EQUAL term
		{ $$ = pnode_new( FTS_TOKEN_EQUAL_EQUAL, 0, $1, $3); }
	| term FTS_TOKEN_NOT_EQUAL term
		{ $$ = pnode_new( FTS_TOKEN_NOT_EQUAL, 0, $1, $3); }
	| term FTS_TOKEN_GREATER term
		{ $$ = pnode_new( FTS_TOKEN_GREATER, 0, $1, $3); }
	| term FTS_TOKEN_GREATER_EQUAL term
		{ $$ = pnode_new( FTS_TOKEN_GREATER_EQUAL, 0, $1, $3); }
	| term FTS_TOKEN_SMALLER term
		{ $$ = pnode_new( FTS_TOKEN_SMALLER, 0, $1, $3); }
	| term FTS_TOKEN_SMALLER_EQUAL term
		{ $$ = pnode_new( FTS_TOKEN_SMALLER_EQUAL, 0, $1, $3); }
;

ref: variable
	| variable FTS_TOKEN_OPEN_SQPAR tuple FTS_TOKEN_CLOSED_SQPAR %prec FTS_TOKEN_ARRAY_INDEX
;

variable: FTS_TOKEN_DOLLAR FTS_TOKEN_SYMBOL
		{ $$ = pnode_new( FTS_TOKEN_DOLLAR, &($2), 0, 0); }
	| FTS_TOKEN_DOLLAR FTS_TOKEN_INT
		{ $$ = pnode_new( FTS_TOKEN_DOLLAR, &($2), 0, 0); }
;

%%

static fts_hashtable_t token_table;

static int yyerror( const char *msg)
{
  fprintf( stderr, "%s\n", msg);

  return 0;
}

static int yylex( YYSTYPE *lvalp, void *data)
{
  fts_parser_t *parser = (fts_parser_t *)data;
  int token = -1;

  if (parser->ac <= 0)
    return 0; /* end of file */

  if ( fts_is_symbol( parser->at))
    {
      fts_atom_t k, v;

      k = *parser->at;
      if (fts_hashtable_get( &token_table, &k, &v))
	token = fts_get_int( &v);
      else
	token = FTS_TOKEN_SYMBOL;
    }
  else if (fts_is_int( parser->at))
    token = FTS_TOKEN_INT;
  else if (fts_is_float( parser->at))
    token = FTS_TOKEN_FLOAT;

  lvalp->a = *parser->at;

  parser->at++;
  parser->ac--;

  return token;
}

/* **********************************************************************
 * 
 * Abstract tree 
 *
 */

struct _pnode_t {
  int token;
  fts_atom_t value;
  struct _pnode_t *left, *right;
};

static fts_heap_t *pnode_heap;

static pnode_t *pnode_new( int token, fts_atom_t *value, pnode_t *left, pnode_t *right)
{
  pnode_t *node;

  node = fts_heap_alloc( pnode_heap);

  node->token = token;

  if (value)
    node->value = *value;
  else
    fts_set_void( &node->value);

  node->left = left;
  node->right = right;

  return node;
}

static void pnode_walk( pnode_t *node, fts_parser_callback_table_t *callbacks, void *data)
{
  fts_parser_callback_t prefix, infix, postfix;

  if (!node)
    return;

#define CASE( TOKEN, CALLBACK)			\
case TOKEN:					\
  prefix = callbacks->CALLBACK.prefix;		\
  infix = callbacks->CALLBACK.infix;		\
  postfix = callbacks->CALLBACK.postfix;	\
break;

  switch( node->token) {
    CASE( FTS_TOKEN_SEMI, semi);
    CASE( FTS_TOKEN_TUPLE, tuple);
    CASE( FTS_TOKEN_INT, c_int);
    CASE( FTS_TOKEN_FLOAT, c_float);
    CASE( FTS_TOKEN_SYMBOL, symbol);
    CASE( FTS_TOKEN_DOLLAR, dollar);
    CASE( FTS_TOKEN_UPLUS, uplus);
    CASE( FTS_TOKEN_UMINUS, uminus);
    CASE( FTS_TOKEN_LOGICAL_NOT, logical_not);
    CASE( FTS_TOKEN_PLUS, plus);
    CASE( FTS_TOKEN_MINUS, minus);
    CASE( FTS_TOKEN_TIMES, times);
    CASE( FTS_TOKEN_DIV, div);
    CASE( FTS_TOKEN_POWER, power);
    CASE( FTS_TOKEN_PERCENT, percent);
    CASE( FTS_TOKEN_SHIFT_LEFT, shift_left);
    CASE( FTS_TOKEN_SHIFT_RIGHT, shift_right);
    CASE( FTS_TOKEN_LOGICAL_AND, logical_and);
    CASE( FTS_TOKEN_LOGICAL_OR, logical_or);
    CASE( FTS_TOKEN_EQUAL_EQUAL, equal_equal);
    CASE( FTS_TOKEN_NOT_EQUAL, not_equal);
    CASE( FTS_TOKEN_GREATER, greater);
    CASE( FTS_TOKEN_GREATER_EQUAL, greater_equal);
    CASE( FTS_TOKEN_SMALLER, smaller);
    CASE( FTS_TOKEN_SMALLER_EQUAL, smaller_equal);
  default:
    prefix = 0;
    infix = 0;
    postfix = 0;
    break;
  }

  if (prefix)
    (*prefix)( node->token, &node->value, data);

  pnode_walk( node->left, callbacks, data);

  if (infix)
    (*infix)( node->token, &node->value, data);

  pnode_walk( node->right, callbacks, data);

  if (postfix)
    (*postfix)( node->token, &node->value, data);
}

/* **********************************************************************
 *
 * Evaluator
 *
 */

/*
 * The evaluation stack is a stack of atoms, organized into frames.
 *
 * The frame pointer points to the first atom of the frame.
 * The returned value is located at (fp-2)
 *
 *
 * top ->        arg2       argument 2              ----------------
 *               arg1       argument 1              
 * fp ->         arg0       argument 2               current frame
 *               savedfp    saved frame pointer     
 *               retval     return value            ----------------
 *               arg1'      argument 1              ----------------
 * savedfp ->    arg0'      argument 0               previous frame
 *               savedfp'   saved frame pointer     
 *               retval'    return value            ----------------
 *
 */

/* #define EVALSTACK_DEBUG */
#undef EVALSTACK_DEBUG

typedef struct {
  fts_stack_t stack;
  int fp;
} evalstack_t;

#define evalstack_fp(EV) ((EV)->fp)
#define evalstack_push(EV,P) fts_stack_push( &(EV)->stack, fts_atom_t, *(P))
#define evalstack_pop(EV,N) fts_stack_pop( &(EV)->stack, (N))
#define evalstack_base(EV) ((fts_atom_t *)fts_stack_base( &(EV)->stack))
#define evalstack_top(EV) fts_stack_top(&(EV)->stack)

#ifdef EVALSTACK_DEBUG
static void evalstack_print( evalstack_t *ev, const char *msg)
{
  int i, current_fp;
  fts_atom_t *p = evalstack_base( ev);

  fprintf( stderr, "%s:\n", msg);

  current_fp = ev->fp;

  for ( i = fts_stack_top( &ev->stack); i >= 0; i--)
    {
      fprintf( stderr, "[%2d]", i);
      if ( i == current_fp - 1)
	{
	  fprintf( stderr, "* ");
	  current_fp = fts_get_int( p+i);
	}
      else
	fts_log( "  ");

      if (fts_is_int( p+i))
	fprintf( stderr, "INT %d\n", fts_get_int(p+i));
      else if (fts_is_float( p+i))
	fprintf( stderr, "FLOAT %g\n", fts_get_float(p+i));
      else if (fts_is_void( p+i))
	fprintf( stderr, "VOID\n");
      else if (fts_is_symbol( p+i))
	fprintf( stderr, "SYMBOL %s\n", fts_get_symbol(p+i));
    }
}
#endif

static void evalstack_init( evalstack_t *ev)
{
  fts_stack_init( &ev->stack, fts_atom_t);
  ev->fp = 0;
}

static void evalstack_push_frame( evalstack_t *ev)
{
  fts_atom_t a;

  /* return value */
  fts_set_void( &a);
  evalstack_push( ev, &a);

  /* saved frame pointer */
  fts_set_int( &a, ev->fp);
  evalstack_push( ev, &a);
  ev->fp = fts_stack_top( &ev->stack) + 1;

#ifdef EVALSTACK_DEBUG
  evalstack_print( ev, "Stack after pushing frame");
#endif
}

static void evalstack_pop_frame( evalstack_t *ev)
{
  int old_fp;

#ifdef EVALSTACK_DEBUG
  evalstack_print( ev, "Stack before poping frame");
#endif

  old_fp = ev->fp;
  ev->fp = fts_get_int( evalstack_base( ev) + ev->fp - 1);
  evalstack_pop( ev, fts_stack_top( &ev->stack) - old_fp + 2);

#ifdef EVALSTACK_DEBUG
  evalstack_print( ev, "Stack after poping frame");
#endif
}

void evalstack_return( evalstack_t *ev, const fts_atom_t *p)
{
  evalstack_base(ev)[ evalstack_fp(ev) - 2] = *p;
  evalstack_pop_frame( ev);
}

/*
 * Evaluator stack
 */

static evalstack_t parser_eval_stack;

struct _eval_data_t {
  fts_object_t *obj;
  int ac;
  const fts_atom_t *at;
};

/*
 * Evaluator callbacks
 */

static void semi_infix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push_frame( &parser_eval_stack);
}

static void semi_postfix_eval( int token, fts_atom_t *value, void *data)
{
  fts_object_t *obj;
  fts_symbol_t selector;
  fts_atom_t *at;
  int ac;

  obj = ((struct _eval_data_t *)data)->obj;

  ac = evalstack_top( &parser_eval_stack) - evalstack_fp( &parser_eval_stack) + 1;
  at = evalstack_base( &parser_eval_stack) + evalstack_fp( &parser_eval_stack);

  if ( fts_is_symbol( at))
    {
      selector = fts_get_symbol( at);
      ac--;
      at++;
    }
  else if (ac == 1)
    selector = fts_get_selector( at);
  else
    selector = fts_s_list;

  fts_outlet_send( obj, 0, selector, ac, at);

  evalstack_pop_frame( &parser_eval_stack);

  /* Pop return value */
  evalstack_pop( &parser_eval_stack, 1);
}

static void par_prefix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push_frame( &parser_eval_stack);
}

static void par_postfix_eval( int token, fts_atom_t *value, void *data)
{
  fts_atom_t *at;
  int ac;

  ac = evalstack_top( &parser_eval_stack) - evalstack_fp( &parser_eval_stack) + 1;
  at = evalstack_base( &parser_eval_stack) + evalstack_fp( &parser_eval_stack);

  if (ac == 1)
    evalstack_return( &parser_eval_stack, at);
  else
    {
      fts_tuple_t *tuple;
      fts_atom_t ret[1];

      tuple = (fts_tuple_t *)fts_object_create( fts_tuple_metaclass, ac, at);
      fts_object_refer( tuple);

      fts_set_tuple( ret, tuple);
      
      evalstack_return( &parser_eval_stack, ret);
    }
}

static void value_prefix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push( &parser_eval_stack, value);
}

static void dollar_postfix_eval( int token, fts_atom_t *value, void *data)
{
  int ac = ((struct _eval_data_t *)data)->ac;
  const fts_atom_t *at = ((struct _eval_data_t *)data)->at;

  if (fts_is_int( value))
    {
      int index = fts_get_int( value);

      if ( index < ac)
	evalstack_push( &parser_eval_stack, at+index);
    }
  else if (fts_is_symbol( value))
    {
      fts_atom_t *p;
      fts_object_t *obj;

      obj = ((struct _eval_data_t *)data)->obj;

      if ((p = fts_variable_get_value( fts_object_get_patcher( obj), fts_get_symbol( value))))
	evalstack_push( &parser_eval_stack, p);
      else
	{
	  fts_atom_t a[1];

	  fts_set_void( a);
	  evalstack_push( &parser_eval_stack, a);
	}
    }
  else
    {
      fts_atom_t a[1];

      fts_set_void( a);
      evalstack_push( &parser_eval_stack, a);
    }
}

static void op_prefix_eval( int token, fts_atom_t *value, void *data)
{
  evalstack_push_frame( &parser_eval_stack);
}

#define UNOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p))								\
    fts_set_int( &ret, OP fts_get_int( p));					\
  else if (fts_is_float( p))							\
    fts_set_float( &ret, OP fts_get_float( p));					\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define IUNOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p))								\
    fts_set_int( &ret, OP fts_get_int( p));					\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define ABINOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p-1) && fts_is_int( p))					\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_int( p));			\
  else if (fts_is_int( p-1) && fts_is_float( p))				\
    fts_set_float( &ret, fts_get_int( p-1) OP fts_get_float( p));		\
  else if (fts_is_float( p-1) && fts_is_int( p))				\
    fts_set_float( &ret, fts_get_float( p-1) OP fts_get_int( p));		\
  else if (fts_is_float( p-1) && fts_is_float( p))				\
    fts_set_float( &ret, fts_get_float( p-1) OP fts_get_float( p));		\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define IABINOP_POSTFIX_EVAL(FUN,OP)						\
static void FUN( int token, fts_atom_t *value, void *data)			\
{										\
  fts_atom_t *p;								\
  fts_atom_t ret;								\
										\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);	\
  if (fts_is_int( p) && fts_is_int( p-1))					\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_int( p) );			\
  else										\
    fts_set_void( &ret);							\
										\
  evalstack_return( &parser_eval_stack, &ret);					\
}

#define LBINOP_POSTFIX_EVAL(FUN,OP)								\
static void FUN( int token, fts_atom_t *value, void *data)					\
{												\
  fts_atom_t *p;										\
  fts_atom_t ret;										\
												\
  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);			\
  if (fts_is_int( p) && fts_is_int( p-1))						       	\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_int( p));					\
  else if (fts_is_int( p) && fts_is_float( p-1))						\
    fts_set_int( &ret, fts_get_int( p-1) OP fts_get_float( p));					\
  else if (fts_is_float( p) && fts_is_int( p-1))						\
    fts_set_int( &ret, fts_get_float( p-1) OP fts_get_int( p));					\
  else if (fts_is_float( p) && fts_is_float( p-1))						\
    fts_set_int( &ret, fts_get_float( p-1) OP fts_get_float( p));				\
  else												\
    fts_set_void( &ret);									\
												\
  evalstack_return( &parser_eval_stack, &ret);							\
}

static void power_postfix_eval( int token, fts_atom_t *value, void *data)
{
  fts_atom_t *p;
  fts_atom_t ret;

  p = evalstack_base( &parser_eval_stack) + evalstack_top( &parser_eval_stack);

  if (fts_is_int( p-1) && fts_is_int( p))
    fts_set_int( &ret, (int)pow( (double)fts_get_int( p-1), (double)fts_get_int( p)));
  else if (fts_is_float( p-1) && fts_is_int( p))
    fts_set_float( &ret, pow( fts_get_float( p-1), (double)fts_get_int( p)));
  else if (fts_is_int( p-1) && fts_is_float( p))
    fts_set_float( &ret, pow( (double)fts_get_int( p-1), fts_get_float( p)));
  else if (fts_is_float( p-1) && fts_is_float( p))
    fts_set_float( &ret, pow( fts_get_float( p-1), fts_get_float( p)));
  else
    fts_set_void( &ret);

  evalstack_return( &parser_eval_stack, &ret);
}

UNOP_POSTFIX_EVAL( uplus_postfix_eval,+)
UNOP_POSTFIX_EVAL( uminus_postfix_eval,+)
IUNOP_POSTFIX_EVAL( logical_not_postfix_eval,!)
ABINOP_POSTFIX_EVAL( plus_postfix_eval,+)
ABINOP_POSTFIX_EVAL( minus_postfix_eval,-)
ABINOP_POSTFIX_EVAL( times_postfix_eval,*)
ABINOP_POSTFIX_EVAL( div_postfix_eval,/)
IABINOP_POSTFIX_EVAL( percent_postfix_eval,%)
IABINOP_POSTFIX_EVAL( shift_left_postfix_eval,<<)
IABINOP_POSTFIX_EVAL( shift_right_postfix_eval,>>)
IABINOP_POSTFIX_EVAL( logical_and_postfix_eval,&&)
IABINOP_POSTFIX_EVAL( logical_or_postfix_eval,||)
LBINOP_POSTFIX_EVAL( equal_equal_postfix_eval,==)
LBINOP_POSTFIX_EVAL( not_equal_postfix_eval,!=)
LBINOP_POSTFIX_EVAL( greater_postfix_eval,>)
LBINOP_POSTFIX_EVAL( greater_equal_postfix_eval,>=)
LBINOP_POSTFIX_EVAL( smaller_postfix_eval,<)
LBINOP_POSTFIX_EVAL( smaller_equal_postfix_eval,<=)

static fts_parser_callback_table_t eval_callbacks = {
  /* semi             */   { 0, semi_infix_eval, semi_postfix_eval},
  /* tuple            */   { 0, 0, 0},
  /* c_int            */   { value_prefix_eval, 0, 0},
  /* c_float          */   { value_prefix_eval, 0, 0},
  /* symbol           */   { value_prefix_eval, 0, 0},
  /* par              */   { par_prefix_eval, 0, par_postfix_eval},
  /* cpar             */   { 0, 0, 0},
  /* sqpar            */   { 0, 0, 0},
  /* dollar           */   { 0, 0, dollar_postfix_eval},
  /* uplus            */   { op_prefix_eval, 0, uplus_postfix_eval},
  /* uminus           */   { op_prefix_eval, 0, uminus_postfix_eval},
  /* logical_not      */   { op_prefix_eval, 0, logical_not_postfix_eval},
  /* plus             */   { op_prefix_eval, 0, plus_postfix_eval},
  /* minus            */   { op_prefix_eval, 0, minus_postfix_eval},
  /* times            */   { op_prefix_eval, 0, times_postfix_eval},
  /* div              */   { op_prefix_eval, 0, div_postfix_eval},
  /* power            */   { op_prefix_eval, 0, power_postfix_eval},
  /* percent          */   { op_prefix_eval, 0, percent_postfix_eval},
  /* shift_left       */   { op_prefix_eval, 0, shift_left_postfix_eval},
  /* shift_right      */   { op_prefix_eval, 0, shift_right_postfix_eval},
  /* logical_and      */   { op_prefix_eval, 0, logical_and_postfix_eval},
  /* logical_or       */   { op_prefix_eval, 0, logical_or_postfix_eval},
  /* equal_equal      */   { op_prefix_eval, 0, equal_equal_postfix_eval},
  /* not_equal        */   { op_prefix_eval, 0, not_equal_postfix_eval},
  /* greater          */   { op_prefix_eval, 0, greater_postfix_eval},
  /* greater_equal    */   { op_prefix_eval, 0, greater_equal_postfix_eval},
  /* smaller          */   { op_prefix_eval, 0, smaller_postfix_eval},
  /* smaller_equal    */   { op_prefix_eval, 0, smaller_equal_postfix_eval}
};

void fts_parser_eval( fts_parser_t *parser, fts_object_t *obj, int ac, const fts_atom_t *at)
{
  struct _eval_data_t data;

  data.obj = obj;
  data.ac = ac;
  data.at = at;

  fts_parser_apply( parser, &eval_callbacks, &data);
}


/* **********************************************************************
 *
 * Parser API
 *
 */

void fts_parser_init( fts_parser_t *parser, int ac, const fts_atom_t *at)
{
  parser->ac = ac;
  parser->at = at;

  yyparse( parser);

  parser->ac = 0;
  parser->at = 0;
}

static void pnode_destroy( pnode_t *node)
{
  if (!node)
    return;

  pnode_destroy( node->left);
  pnode_destroy( node->right);
  fts_heap_free( node, pnode_heap);
}

void fts_parser_destroy( fts_parser_t *parser)
{
  pnode_destroy( parser->tree);
}

void fts_parser_apply( fts_parser_t *parser, fts_parser_callback_table_t *callbacks, void *data)
{
  pnode_walk( parser->tree, callbacks, data);
}

static void print_prefix_callback( int token, fts_atom_t *value, void *data)
{
  int *indentp = (int *)data;
  int i;

  fprintf( stderr, "%d:", *indentp);

  for ( i = 0; i < *indentp; i++)
    fprintf( stderr, "   ");

  switch( token) {
  case FTS_TOKEN_SEMI: fprintf( stderr, ";\n"); break;
  case FTS_TOKEN_TUPLE: fprintf( stderr, "TUPLE\n"); break;
  case FTS_TOKEN_INT: fprintf( stderr, "INT %d\n", fts_get_int( value)); break;
  case FTS_TOKEN_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( value)); break;
  case FTS_TOKEN_SYMBOL: fprintf( stderr, "SYMBOL %s\n", fts_get_symbol( value)); break;
  case FTS_TOKEN_PAR: fprintf( stderr, "()\n"); break;
  case FTS_TOKEN_CPAR: fprintf( stderr, "{}\n"); break;
  case FTS_TOKEN_SQPAR: fprintf( stderr, "[]\n"); break;
  case FTS_TOKEN_DOLLAR: 
    if (fts_is_int( value))
      fprintf( stderr, "$%d\n", fts_get_int( value)); 
    else if (fts_is_symbol( value))
      fprintf( stderr, "$%s\n", fts_get_symbol( value)); 
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

  *indentp += 1;
}

static void print_postfix_callback( int token, fts_atom_t *value, void *data)
{
  int *indentp = (int *)data;

  *indentp -= 1;
}

static fts_parser_callback_table_t print_callbacks = {
  /* semi             */   { print_prefix_callback, 0, print_postfix_callback},
  /* tuple            */   { print_prefix_callback, 0, print_postfix_callback},
  /* c_int            */   { print_prefix_callback, 0, print_postfix_callback},
  /* c_float          */   { print_prefix_callback, 0, print_postfix_callback},
  /* symbol           */   { print_prefix_callback, 0, print_postfix_callback},
  /* par              */   { print_prefix_callback, 0, print_postfix_callback},
  /* cpar             */   { print_prefix_callback, 0, print_postfix_callback},
  /* sqpar            */   { print_prefix_callback, 0, print_postfix_callback},
  /* dollar           */   { print_prefix_callback, 0, print_postfix_callback},
  /* uplus            */   { print_prefix_callback, 0, print_postfix_callback},
  /* uminus           */   { print_prefix_callback, 0, print_postfix_callback},
  /* logical_not      */   { print_prefix_callback, 0, print_postfix_callback},
  /* plus             */   { print_prefix_callback, 0, print_postfix_callback},
  /* minus            */   { print_prefix_callback, 0, print_postfix_callback},
  /* times            */   { print_prefix_callback, 0, print_postfix_callback},
  /* div              */   { print_prefix_callback, 0, print_postfix_callback},
  /* power            */   { print_prefix_callback, 0, print_postfix_callback},
  /* percent          */   { print_prefix_callback, 0, print_postfix_callback},
  /* shift_left       */   { print_prefix_callback, 0, print_postfix_callback},
  /* shift_right      */   { print_prefix_callback, 0, print_postfix_callback},
  /* logical_and      */   { print_prefix_callback, 0, print_postfix_callback},
  /* logical_or       */   { print_prefix_callback, 0, print_postfix_callback},
  /* equal_equal      */   { print_prefix_callback, 0, print_postfix_callback},
  /* not_equal        */   { print_prefix_callback, 0, print_postfix_callback},
  /* greater          */   { print_prefix_callback, 0, print_postfix_callback},
  /* greater_equal    */   { print_prefix_callback, 0, print_postfix_callback},
  /* smaller          */   { print_prefix_callback, 0, print_postfix_callback},
  /* smaller_equal    */   { print_prefix_callback, 0, print_postfix_callback}
};

void fts_parser_print( fts_parser_t *parser)
{
  int indent = 0;

  fts_parser_apply( parser, &print_callbacks, &indent);
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  fts_atom_t k, v;

  evalstack_init( &parser_eval_stack);

  pnode_heap = fts_heap_new( sizeof( pnode_t));

  fts_hashtable_init( &token_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

#define PUT(S,T) fts_set_symbol( &k, S); fts_set_int( &v, T); fts_hashtable_put( &token_table, &k, &v);
  PUT( fts_s_dollar, FTS_TOKEN_DOLLAR);
  PUT( fts_s_semi, FTS_TOKEN_SEMI);
  PUT( fts_s_plus, FTS_TOKEN_PLUS);
  PUT( fts_s_minus, FTS_TOKEN_MINUS);
  PUT( fts_s_times, FTS_TOKEN_TIMES);
  PUT( fts_s_div, FTS_TOKEN_DIV);
  PUT( fts_s_power, FTS_TOKEN_POWER);
  PUT( fts_s_open_par, FTS_TOKEN_OPEN_PAR);
  PUT( fts_s_closed_par, FTS_TOKEN_CLOSED_PAR);
  PUT( fts_s_open_sqpar, FTS_TOKEN_OPEN_SQPAR);
  PUT( fts_s_closed_sqpar, FTS_TOKEN_CLOSED_SQPAR);
  PUT( fts_s_open_cpar, FTS_TOKEN_OPEN_CPAR);
  PUT( fts_s_closed_cpar, FTS_TOKEN_CLOSED_CPAR);
  PUT( fts_s_dot, FTS_TOKEN_DOT);
  PUT( fts_s_percent, FTS_TOKEN_PERCENT);
  PUT( fts_s_shift_left, FTS_TOKEN_SHIFT_LEFT);
  PUT( fts_s_shift_right, FTS_TOKEN_SHIFT_RIGHT);
  PUT( fts_s_logical_and, FTS_TOKEN_LOGICAL_AND);
  PUT( fts_s_logical_or, FTS_TOKEN_LOGICAL_OR);
  PUT( fts_s_logical_not, FTS_TOKEN_LOGICAL_NOT);
  PUT( fts_s_equal_equal, FTS_TOKEN_EQUAL_EQUAL);
  PUT( fts_s_not_equal, FTS_TOKEN_NOT_EQUAL);
  PUT( fts_s_greater, FTS_TOKEN_GREATER);
  PUT( fts_s_greater_equal, FTS_TOKEN_GREATER_EQUAL);
  PUT( fts_s_smaller, FTS_TOKEN_SMALLER);
  PUT( fts_s_smaller_equal, FTS_TOKEN_SMALLER_EQUAL);
  PUT( fts_s_colon, FTS_TOKEN_COLON);
  PUT( fts_s_equal, FTS_TOKEN_EQUAL);
}
