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
#include <fts/fts.h>
#include <ftsprivate/parser.h>

#define free fts_free
#define malloc fts_malloc
#define YYPARSE_PARAM data
#define YYLEX_PARAM data

static int yylex();
static int yyerror( const char *msg);

static fts_parsetree_t *fts_parsetree_new_node( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right);

struct _parser_data {
  int ac;
  const fts_atom_t *at;
  fts_parsetree_t *tree;
};

%}

%pure_parser

%union {
  fts_atom_t a;
  fts_parsetree_t *n;
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

%token FTS_TOKEN_HIDDEN_MESSAGE
%token FTS_TOKEN_HIDDEN_OBJECT

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


%type <n> jmax_expression
%type <n> message
/* %type <n> object */
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

/* jmax_expression: FTS_TOKEN_HIDDEN_MESSAGE message */
/* 		{ ((struct _parser_data *)data)->tree = $2; } */
/* 	| FTS_TOKEN_HIDDEN_OBJECT object */
/* 		{ ((struct _parser_data *)data)->tree = $2; } */
/* ; */

/* object: FTS_TOKEN_SYMBOL FTS_TOKEN_COLON tuple */
/* 		{ $$ = 0; } */
/* 	| tuple */
/* 		{ $$ = 0; } */
/* ; */

jmax_expression: message
		{ ((struct _parser_data *)data)->tree = $1; }
;

message: message FTS_TOKEN_SEMI tuple
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SEMI, 0, $1, $3); }
	| tuple
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SEMI, 0, 0, $1); }
	| 
		{ $$ = 0; }
;

tuple: tuple term   %prec FTS_TOKEN_TUPLE   
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_TUPLE, 0, $1, $2); }
	| term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_TUPLE, 0, 0, $1); }
;

term: primitive
	| par_op
	| unary_op
	| binary_op
	| ref
;

primitive: FTS_TOKEN_INT
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_INT, &($1), 0, 0); }
	| FTS_TOKEN_FLOAT
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_FLOAT, &($1), 0, 0); }
	| FTS_TOKEN_SYMBOL
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SYMBOL, &($1), 0, 0); }
;

par_op: FTS_TOKEN_OPEN_PAR tuple FTS_TOKEN_CLOSED_PAR
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_PAR, 0, 0, $2); }
;

unary_op: FTS_TOKEN_PLUS term %prec FTS_TOKEN_UPLUS
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_UPLUS, 0, $2, 0); }
	| FTS_TOKEN_MINUS term %prec FTS_TOKEN_UMINUS
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_UMINUS, 0, $2, 0); }
	| FTS_TOKEN_LOGICAL_NOT term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_LOGICAL_NOT, 0, $2, 0); }
;

binary_op: term FTS_TOKEN_PLUS term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_PLUS, 0, $1, $3); }
	| term FTS_TOKEN_MINUS term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_MINUS, 0, $1, $3); }
	| term FTS_TOKEN_TIMES term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_TIMES, 0, $1, $3); }
	| term FTS_TOKEN_DIV term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_DIV, 0, $1, $3); }
	| term FTS_TOKEN_POWER term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_POWER, 0, $1, $3); }
	| term FTS_TOKEN_PERCENT term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_PERCENT, 0, $1, $3); }
	| term FTS_TOKEN_SHIFT_LEFT term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SHIFT_LEFT, 0, $1, $3); }
	| term FTS_TOKEN_SHIFT_RIGHT term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SHIFT_RIGHT, 0, $1, $3); }
	| term FTS_TOKEN_LOGICAL_AND term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_LOGICAL_AND, 0, $1, $3); }
	| term FTS_TOKEN_LOGICAL_OR term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_LOGICAL_OR, 0, $1, $3); }
	| term FTS_TOKEN_EQUAL_EQUAL term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_EQUAL_EQUAL, 0, $1, $3); }
	| term FTS_TOKEN_NOT_EQUAL term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_NOT_EQUAL, 0, $1, $3); }
	| term FTS_TOKEN_GREATER term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_GREATER, 0, $1, $3); }
	| term FTS_TOKEN_GREATER_EQUAL term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_GREATER_EQUAL, 0, $1, $3); }
	| term FTS_TOKEN_SMALLER term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SMALLER, 0, $1, $3); }
	| term FTS_TOKEN_SMALLER_EQUAL term
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_SMALLER_EQUAL, 0, $1, $3); }
;

ref: variable
	| variable FTS_TOKEN_OPEN_SQPAR tuple FTS_TOKEN_CLOSED_SQPAR %prec FTS_TOKEN_ARRAY_INDEX
;

variable: FTS_TOKEN_DOLLAR FTS_TOKEN_SYMBOL
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_DOLLAR, &($2), 0, 0); }
	| FTS_TOKEN_DOLLAR FTS_TOKEN_INT
		{ $$ = fts_parsetree_new_node( FTS_TOKEN_DOLLAR, &($2), 0, 0); }
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
  struct _parser_data *parser_data = (struct _parser_data *)data;
  int token = -1;

  if (parser_data->ac <= 0)
    return 0; /* end of file */

  if ( fts_is_symbol( parser_data->at))
    {
      fts_atom_t k, v;

      k = *parser_data->at;
      if (fts_hashtable_get( &token_table, &k, &v))
	token = fts_get_int( &v);
      else
	token = FTS_TOKEN_SYMBOL;
    }
  else if (fts_is_int( parser_data->at))
    token = FTS_TOKEN_INT;
  else if (fts_is_float( parser_data->at))
    token = FTS_TOKEN_FLOAT;

  lvalp->a = *parser_data->at;

  parser_data->at++;
  parser_data->ac--;

  return token;
}

/* **********************************************************************
 *
 * Parser abstract tree construction/destruction
 *
 */

static fts_heap_t *parsetree_heap;

static fts_parsetree_t *fts_parsetree_new_node( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right)
{
  fts_parsetree_t *tree;

  tree = (fts_parsetree_t *)fts_heap_alloc( parsetree_heap);

  tree->token = token;

  if (value)
    tree->value = *value;
  else
    fts_set_void( &tree->value);

  tree->left = left;
  tree->right = right;

  return tree;
}

fts_parsetree_t *fts_parsetree_new( int ac, const fts_atom_t *at)
{
  struct _parser_data parser_data;

  parser_data.ac = ac;
  parser_data.at = at;

  yyparse( &parser_data);

  return parser_data.tree;
}

void fts_parsetree_delete( fts_parsetree_t *tree)
{
  if (!tree)
    return;

  fts_parsetree_delete( tree->left);
  fts_parsetree_delete( tree->right);
  fts_heap_free( tree, parsetree_heap);
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  parsetree_heap = fts_heap_new( sizeof( fts_parsetree_t));

  fts_hashtable_init( &token_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

#define PUT(S,T) 				\
 {						\
   fts_atom_t k, v;				\
						\
   fts_set_symbol( &k, S);			\
   fts_set_int( &v, T);				\
   fts_hashtable_put( &token_table, &k, &v);	\
 }

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
