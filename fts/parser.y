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
#include <stdio.h>
#include <unistd.h>
#include <fts/fts.h>

#define free fts_free

#define YYSTYPE fts_atom_t
#define YYPARSE_PARAM data
#define YYLEX_PARAM data

static int yylex( YYSTYPE *lvalp, void *data);
static int yyerror( const char *msg);

typedef struct {
  int ac;
  fts_atom_t *at;
  int *tokens;
  int *current_token;
} fts_parser_t;

#define ARITH_UNNOP( ret, a1, OP)			\
  if (fts_is_int( a1))					\
    fts_set_int( ret, OP fts_get_int( a1));		\
  else if (fts_is_double( a1))				\
    fts_set_double( ret, OP fts_get_double( a1));	\
  else							\
    fts_set_void( ret);

#define ARITH_IUNNOP( ret, a1, OP)		\
  if (fts_is_int( a1))				\
    fts_set_int( ret, OP fts_get_int( a1));	\
  else						\
    fts_set_void( ret);

#define ARITH_BINOP( ret, a1, a2, OP)							\
  if (fts_is_int( a1) && fts_is_int( a2))						\
    fts_set_int( ret, fts_get_int( a1) OP fts_get_int( a2));				\
  else if (fts_is_double( a1) && fts_is_double( a2))					\
    fts_set_double( ret, fts_get_number_double( a1) OP fts_get_number_double( a2));	\
  else											\
    fts_set_void( ret);

#define ARITH_IBINOP( ret, a1, a2, OP)							\
  if (fts_is_int( a1) && fts_is_int( a2))						\
    fts_set_int( ret, fts_get_int( a1) OP fts_get_int( a2));				\
  else											\
    fts_set_void( ret);

#define LOGIC_BINOP( ret, a1, a2, OP)					\
  if (fts_is_int( a1))							\
    {									\
      if (fts_is_int( a2))						\
	fts_set_int( ret, fts_get_int( a1) OP fts_get_int( a2)); 	\
      else if (fts_is_double( a2))					\
	fts_set_int( ret, fts_get_int( a1) OP fts_get_double( a2)); 	\
      else								\
	fts_set_void( ret);						\
    }									\
  else if (fts_is_double( a1))						\
    {									\
      if (fts_is_int( a2))						\
	fts_set_int( ret, fts_get_double( a1) OP fts_get_int( a2)); 	\
      else if (fts_is_double( a2))					\
	fts_set_int( ret, fts_get_double( a1) OP fts_get_double( a2)); 	\
      else								\
	fts_set_void( ret);						\
    }									\
  else									\
    fts_set_void( ret);

%}

%pure_parser

/*
 * Tokens
 */

%token FTS_TOKEN_INT
%token FTS_TOKEN_DOUBLE
%token FTS_TOKEN_SYMBOL

%token FTS_TOKEN_SEMI
%token FTS_TOKEN_COLON
%token FTS_TOKEN_OPEN_PAR
%token FTS_TOKEN_CLOSED_PAR
%token FTS_TOKEN_OPEN_CPAR
%token FTS_TOKEN_CLOSED_CPAR
%token FTS_TOKEN_OPEN_SQPAR
%token FTS_TOKEN_CLOSED_SQPAR

/*
 * Operators
 */


%left FTS_TOKEN_SPACE
%right FTS_TOKEN_EQUAL
%left FTS_TOKEN_LOGICAL_OR
%left FTS_TOKEN_LOGICAL_AND
%left FTS_TOKEN_EQUAL_EQUAL FTS_TOKEN_NOT_EQUAL
%left FTS_TOKEN_GREATER	FTS_TOKEN_GREATER_EQUAL FTS_TOKEN_SMALLER FTS_TOKEN_SMALLER_EQUAL
%left FTS_TOKEN_SHIFT_LEFT FTS_TOKEN_SHIFT_RIGHT
%left FTS_TOKEN_PLUS FTS_TOKEN_MINUS
%left FTS_TOKEN_TIMES FTS_TOKEN_DIV FTS_TOKEN_PERCENT
%right FTS_TOKEN_UNARY_MINUS, FTS_TOKEN_UNARY_PLUS
%right FTS_TOKEN_LOGICAL_NOT
%left FTS_TOKEN_ARRAY_INDEX
%left FTS_TOKEN_DOT
%right FTS_TOKEN_DOLLAR

%%
/* **********************************************************************
 *
 * Rules
 *
 */

program: list ;

list: list ";" tuple
	| tuple ;

tuple: tuple term %prec FTS_TOKEN_SPACE
	| /* empty */ ;

term: primitive
	| par_op
	| unary_op
	| binary_op
	| ref
;

primitive: FTS_TOKEN_INT
	| FTS_TOKEN_DOUBLE
	| FTS_TOKEN_SYMBOL
;

par_op: FTS_TOKEN_OPEN_PAR term FTS_TOKEN_CLOSED_PAR         { $$ = $2; }

unary_op: FTS_TOKEN_PLUS term %prec FTS_TOKEN_UNARY_PLUS     { ARITH_UNNOP( &($$), &($2), +); }
	| FTS_TOKEN_MINUS term %prec FTS_TOKEN_UNARY_MINUS   { ARITH_UNNOP( &($$), &($2), -); }
	| FTS_TOKEN_LOGICAL_NOT term                         { ARITH_IUNNOP( &($$), &($2), !); }
;

binary_op: term FTS_TOKEN_PLUS term             { ARITH_BINOP( &($$), &($1), &($3), +); }
	| term FTS_TOKEN_MINUS term             { ARITH_BINOP( &($$), &($1), &($3), -); }
	| term FTS_TOKEN_TIMES term             { ARITH_BINOP( &($$), &($1), &($3), *); }
	| term FTS_TOKEN_DIV term               { ARITH_BINOP( &($$), &($1), &($3), /); }
	| term FTS_TOKEN_PERCENT term           { ARITH_IBINOP( &($$), &($1), &($3), %); }
	| term FTS_TOKEN_SHIFT_LEFT term        { ARITH_IBINOP( &($$), &($1), &($3), <<); }
	| term FTS_TOKEN_SHIFT_RIGHT term       { ARITH_IBINOP( &($$), &($1), &($3), >>); }
	| term FTS_TOKEN_LOGICAL_AND term       { ARITH_IBINOP( &($$), &($1), &($3), &&); }
	| term FTS_TOKEN_LOGICAL_OR term        { ARITH_IBINOP( &($$), &($1), &($3), ||); }
	| term FTS_TOKEN_EQUAL_EQUAL term       { LOGIC_BINOP( &($$), &($1), &($3), ==); }
	| term FTS_TOKEN_NOT_EQUAL term         { LOGIC_BINOP( &($$), &($1), &($3), !=); }
	| term FTS_TOKEN_GREATER term           { LOGIC_BINOP( &($$), &($1), &($3), >); }
	| term FTS_TOKEN_GREATER_EQUAL term     { LOGIC_BINOP( &($$), &($1), &($3), >=); }
	| term FTS_TOKEN_SMALLER term           { LOGIC_BINOP( &($$), &($1), &($3), <); }
	| term FTS_TOKEN_SMALLER_EQUAL term     { LOGIC_BINOP( &($$), &($1), &($3), <=); }
;

ref: variable
	| variable FTS_TOKEN_OPEN_SQPAR term FTS_TOKEN_CLOSED_SQPAR %prec FTS_TOKEN_ARRAY_INDEX
;

variable: FTS_TOKEN_DOLLAR FTS_TOKEN_SYMBOL
;

%%

/* **********************************************************************
 *
 * Parser extra functions
 *
 */

static int yyerror( const char *msg)
{
  fprintf( stderr, "%s\n", msg);

  return 0;
}


/* **********************************************************************
 * 
 * Atom array parser + scanner
 *
 */

static fts_hashtable_t token_table;

void fts_parser_init( fts_parser_t *parser, int ac, fts_atom_t *at)
{
  int i;

  parser->ac = ac;
  parser->at = at;

  parser->tokens = (int *)fts_malloc( ac * sizeof( int));

  for ( i = 0; i < ac; i++)
    {
      if ( fts_is_symbol( at+i))
	{
	  fts_atom_t k, v;

	  k = *at;
	  if (fts_hashtable_get( &token_table, &k, &v))
	    parser->tokens[i] = fts_get_int( &v);
	  else
	    parser->tokens[i] = FTS_TOKEN_SYMBOL;
	}
      else if (fts_is_int( at+i))
	parser->tokens[i] = FTS_TOKEN_INT;
      else if (fts_is_double( at+i))
	parser->tokens[i] = FTS_TOKEN_DOUBLE;
    }
}

static int yylex( YYSTYPE *lvalp, void *data)
{
  fts_parser_t *parser = (fts_parser_t *)data;
  fts_atom_t *at = parser->at;
  int token = -1;

  if (parser->ac <= 0)
    return 0; /* end of file */

  token = *parser->current_token;
  parser->current_token++;

  *lvalp = *at;

  parser->at++;
  parser->ac--;

  return token;
}

int fts_parser_run( fts_parser_t *parser)
{
  parser->current_token = parser->tokens;

  return yyparse( parser);
}

void fts_parser_destroy( fts_parser_t *parser)
{
  fts_free( parser->tokens);
}


/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  fts_atom_t k, v;

  fts_hashtable_init( &token_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

#define PUT(S,T) fts_set_symbol( &k, S); fts_set_int( &v, T); fts_hashtable_put( &token_table, &k, &v);
  PUT( fts_s_dollar, FTS_TOKEN_DOLLAR);
  PUT( fts_s_semi, FTS_TOKEN_SEMI);
  PUT( fts_s_plus, FTS_TOKEN_PLUS);
  PUT( fts_s_minus, FTS_TOKEN_MINUS);
  PUT( fts_s_times, FTS_TOKEN_TIMES);
  PUT( fts_s_div, FTS_TOKEN_DIV);
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
