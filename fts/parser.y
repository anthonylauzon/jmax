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
#include <fts/fts.h>
#include <ftsprivate/parser.h>

  /* #define this to 1 if you want a lot of debug printout of the parser */
#define YYDEBUG 0

#ifndef STANDALONE
#define free fts_free
#define malloc fts_malloc
static int yylex();
#else
extern int yylex();
extern void tokenizer_init( const char *s);
#endif

static int yyerror( const char *msg);

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right);

static struct _parser_data {
  int ac;
  const fts_atom_t *at;
  fts_parsetree_t *tree;
} parser_data;

static fts_status_description_t syntax_error_status_description = {
  "Syntax error"
};
fts_status_t syntax_error_status = &syntax_error_status_description;

%}

%union {
  fts_atom_t a;
  fts_parsetree_t *n;
}


/*
 * Tokens
 */

%token <a> TK_INT
%token <a> TK_FLOAT
%token <a> TK_SYMBOL
%token TK_SEMI
%token TK_PAR
%token TK_OPEN_PAR
%token TK_CLOSED_PAR
%token TK_CPAR
%token TK_OPEN_CPAR
%token TK_CLOSED_CPAR
%token TK_OPEN_SQPAR
%token TK_CLOSED_SQPAR


/*
 * Operators
 */

%left TK_COMMA
%left TK_SPACE
%left TK_LOGICAL_OR
%left TK_LOGICAL_AND
%left TK_EQUAL_EQUAL TK_NOT_EQUAL
%left TK_GREATER TK_GREATER_EQUAL TK_SMALLER TK_SMALLER_EQUAL
%left TK_SHIFT_LEFT TK_SHIFT_RIGHT
%left TK_PLUS TK_MINUS
%left TK_TIMES TK_DIV TK_PERCENT
%right TK_UMINUS, TK_UPLUS
%right TK_LOGICAL_NOT
%left TK_POWER
%left TK_SQPAR
%left TK_DOT
%right TK_DOLLAR
%nonassoc TK_COLON


/*
 * Non-terminal types
 */

%type <n> binary
%type <n> instance
%type <a> class_name
%type <n> comma_expression_list
%type <n> expression
%type <n> invocation
%type <n> function
%type <n> par
%type <n> primitive
%type <n> reference
%type <n> term
%type <n> term_list
%type <n> tuple
%type <n> unary
%type <n> variable

%%
/* **********************************************************************
 *
 * Rules
 *
 */

toplevel: comma_expression_list
		{ parser_data.tree = $1; }
;

comma_expression_list: comma_expression_list TK_COMMA expression
		{ $$ = fts_parsetree_new( TK_COMMA, 0, $1, $3); }
	| expression
		{ $$ = fts_parsetree_new( TK_COMMA, 0, 0, $1); }
	| 
		{ $$ = 0; }
;

expression: instance
	| tuple
	| invocation
	| function
;

instance: TK_SYMBOL TK_COLON class_name term_list
		{ $$ = fts_parsetree_new( TK_COLON, &($3), fts_parsetree_new( TK_SYMBOL, &($1), 0, 0), $4); }
	| TK_COLON class_name term_list
		{ $$ = fts_parsetree_new( TK_COLON, &($2), 0, $3); }
;

class_name: TK_SYMBOL
	| TK_PLUS
		{ fts_set_symbol( &($$), fts_s_plus); }
	| TK_MINUS
		{ fts_set_symbol( &($$), fts_s_minus); }
	| TK_TIMES
		{ fts_set_symbol( &($$), fts_s_times); }
	| TK_DIV
		{ fts_set_symbol( &($$), fts_s_div); }
	| TK_PERCENT
		{ fts_set_symbol( &($$), fts_s_percent); }
	| TK_SHIFT_LEFT
		{ fts_set_symbol( &($$), fts_s_shift_left); }
	| TK_SHIFT_RIGHT
		{ fts_set_symbol( &($$), fts_s_shift_right); }
	| TK_GREATER
		{ fts_set_symbol( &($$), fts_s_greater); }
	| TK_GREATER_EQUAL
		{ fts_set_symbol( &($$), fts_s_greater_equal); }
	| TK_SMALLER
		{ fts_set_symbol( &($$), fts_s_smaller); }
	| TK_SMALLER_EQUAL
		{ fts_set_symbol( &($$), fts_s_smaller_equal); }
	| TK_EQUAL_EQUAL
		{ fts_set_symbol( &($$), fts_s_equal_equal); }
	| TK_NOT_EQUAL
		{ fts_set_symbol( &($$), fts_s_not_equal); }
	| TK_LOGICAL_NOT
		{ fts_set_symbol( &($$), fts_s_logical_not); }
	| TK_LOGICAL_OR
		{ fts_set_symbol( &($$), fts_s_logical_or); }
	| TK_LOGICAL_AND
		{ fts_set_symbol( &($$), fts_s_logical_and); }
;

tuple: tuple term /*---*/ %prec TK_SPACE
		{ $$ = fts_parsetree_new( TK_SPACE, 0, $1, $2); }
	| term
;

invocation: reference TK_DOT TK_SYMBOL term_list
		{ $$ = fts_parsetree_new( TK_DOT, &($3), $1, $4); }
;

term_list: term_list term  /*---*/ %prec TK_SPACE
		{ $$ = fts_parsetree_new( TK_SPACE, 0, $1, $2); }
	|
		{ $$ = 0; }
;

term: primitive
	| par
	| unary
	| binary
	| reference
;

primitive: TK_INT
		{ $$ = fts_parsetree_new( TK_INT, &($1), 0, 0); }
	| TK_FLOAT
		{ $$ = fts_parsetree_new( TK_FLOAT, &($1), 0, 0); }
	| TK_SYMBOL
		{ $$ = fts_parsetree_new( TK_SYMBOL, &($1), 0, 0); }
;

par: TK_OPEN_PAR expression TK_CLOSED_PAR
		{ $$ = fts_parsetree_new( TK_PAR, 0, 0, $2); }
;

unary: TK_PLUS term %prec TK_UPLUS
		{ $$ = fts_parsetree_new( TK_UPLUS, 0, $2, 0); }
	| TK_MINUS term %prec TK_UMINUS
		{ $$ = fts_parsetree_new( TK_UMINUS, 0, $2, 0); }
	| TK_LOGICAL_NOT term
		{ $$ = fts_parsetree_new( TK_LOGICAL_NOT, 0, $2, 0); }
;

binary: term TK_PLUS term
		{ $$ = fts_parsetree_new( TK_PLUS, 0, $1, $3); }
	| term TK_MINUS term
		{ $$ = fts_parsetree_new( TK_MINUS, 0, $1, $3); }
	| term TK_TIMES term
		{ $$ = fts_parsetree_new( TK_TIMES, 0, $1, $3); }
	| term TK_DIV term
		{ $$ = fts_parsetree_new( TK_DIV, 0, $1, $3); }
	| term TK_POWER term
		{ $$ = fts_parsetree_new( TK_POWER, 0, $1, $3); }
	| term TK_PERCENT term
		{ $$ = fts_parsetree_new( TK_PERCENT, 0, $1, $3); }
	| term TK_SHIFT_LEFT term
		{ $$ = fts_parsetree_new( TK_SHIFT_LEFT, 0, $1, $3); }
	| term TK_SHIFT_RIGHT term
		{ $$ = fts_parsetree_new( TK_SHIFT_RIGHT, 0, $1, $3); }
	| term TK_LOGICAL_AND term
		{ $$ = fts_parsetree_new( TK_LOGICAL_AND, 0, $1, $3); }
	| term TK_LOGICAL_OR term
		{ $$ = fts_parsetree_new( TK_LOGICAL_OR, 0, $1, $3); }
	| term TK_EQUAL_EQUAL term
		{ $$ = fts_parsetree_new( TK_EQUAL_EQUAL, 0, $1, $3); }
	| term TK_NOT_EQUAL term
		{ $$ = fts_parsetree_new( TK_NOT_EQUAL, 0, $1, $3); }
	| term TK_GREATER term
		{ $$ = fts_parsetree_new( TK_GREATER, 0, $1, $3); }
	| term TK_GREATER_EQUAL term
		{ $$ = fts_parsetree_new( TK_GREATER_EQUAL, 0, $1, $3); }
	| term TK_SMALLER term
		{ $$ = fts_parsetree_new( TK_SMALLER, 0, $1, $3); }
	| term TK_SMALLER_EQUAL term
		{ $$ = fts_parsetree_new( TK_SMALLER_EQUAL, 0, $1, $3); }
;

reference: variable TK_OPEN_SQPAR expression TK_CLOSED_SQPAR /*---*/ %prec TK_SQPAR
		{ $$ = fts_parsetree_new( TK_SQPAR, 0, $1, $3); }
	| variable;

variable: TK_DOLLAR TK_SYMBOL
		{ $$ = fts_parsetree_new( TK_DOLLAR, &($2), 0, 0); }
	| TK_DOLLAR TK_INT
		{ $$ = fts_parsetree_new( TK_DOLLAR, &($2), 0, 0); }
;

function: TK_DOT TK_SYMBOL term_list
		{ $$ = 0; }
;
%%

#ifndef STANDALONE

/* **********************************************************************
 *
 * FTS code
 *
 */

static fts_hashtable_t fts_token_table;

static int yyerror( const char *msg)
{
  return 0;
}

static int yylex()
{
  int token = -1;
  if (parser_data.ac <= 0)
    return 0; /* end of file */

  if ( fts_is_symbol(parser_data.at))
    {
      fts_atom_t v;

      if (fts_hashtable_get( &fts_token_table, parser_data.at, &v))
	token = fts_get_int( &v);
      else
	{
	  token = TK_SYMBOL;
	  yylval.a = *parser_data.at;
	}
    }
  else if (fts_is_int( parser_data.at))
    {
      token = TK_INT;
      yylval.a = *parser_data.at;
    }
  else if (fts_is_float( parser_data.at))
    {
      token = TK_FLOAT;
      yylval.a = *parser_data.at;
    }

  parser_data.at++;
  parser_data.ac--;

  return token;
}

/* **********************************************************************
 *
 * Parser abstract tree construction/destruction
 *
 */

static fts_heap_t *parsetree_heap;

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right)
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

fts_status_t fts_parsetree_parse( int ac, const fts_atom_t *at, fts_parsetree_t **ptree)
{
  parser_data.ac = ac;
  parser_data.at = at;

  if (yyparse())
    {
      *ptree = NULL;
      return syntax_error_status;
    }

  *ptree = parser_data.tree;
  return fts_ok;
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

  fts_hashtable_init( &fts_token_table, fts_symbol_class, FTS_HASHTABLE_MEDIUM);

#define PUT_TOKEN(S,T)					\
 {							\
   fts_atom_t k, v;					\
							\
   fts_set_symbol( &k, S);				\
   fts_set_int( &v, T);					\
   fts_hashtable_put( &fts_token_table, &k, &v);	\
 }

  PUT_TOKEN( fts_s_dollar, TK_DOLLAR);
  PUT_TOKEN( fts_s_semi, TK_SEMI);
  PUT_TOKEN( fts_s_comma, TK_COMMA);
  PUT_TOKEN( fts_s_plus, TK_PLUS);
  PUT_TOKEN( fts_s_minus, TK_MINUS);
  PUT_TOKEN( fts_s_times, TK_TIMES);
  PUT_TOKEN( fts_s_div, TK_DIV);
  PUT_TOKEN( fts_s_power, TK_POWER);
  PUT_TOKEN( fts_s_open_par, TK_OPEN_PAR);
  PUT_TOKEN( fts_s_closed_par, TK_CLOSED_PAR);
  PUT_TOKEN( fts_s_open_sqpar, TK_OPEN_SQPAR);
  PUT_TOKEN( fts_s_closed_sqpar, TK_CLOSED_SQPAR);
  PUT_TOKEN( fts_s_open_cpar, TK_OPEN_CPAR);
  PUT_TOKEN( fts_s_closed_cpar, TK_CLOSED_CPAR);
  PUT_TOKEN( fts_s_dot, TK_DOT);
  PUT_TOKEN( fts_s_percent, TK_PERCENT);
  PUT_TOKEN( fts_s_shift_left, TK_SHIFT_LEFT);
  PUT_TOKEN( fts_s_shift_right, TK_SHIFT_RIGHT);
  PUT_TOKEN( fts_s_logical_and, TK_LOGICAL_AND);
  PUT_TOKEN( fts_s_logical_or, TK_LOGICAL_OR);
  PUT_TOKEN( fts_s_logical_not, TK_LOGICAL_NOT);
  PUT_TOKEN( fts_s_equal_equal, TK_EQUAL_EQUAL);
  PUT_TOKEN( fts_s_not_equal, TK_NOT_EQUAL);
  PUT_TOKEN( fts_s_greater, TK_GREATER);
  PUT_TOKEN( fts_s_greater_equal, TK_GREATER_EQUAL);
  PUT_TOKEN( fts_s_smaller, TK_SMALLER);
  PUT_TOKEN( fts_s_smaller_equal, TK_SMALLER_EQUAL);
  PUT_TOKEN( fts_s_colon, TK_COLON);

#if YYDEBUG
  yydebug = 1;
#endif
}

#else

/* **********************************************************************
 *
 * Standalone code
 *
 */

#define PREDEF_SYMBOL(V,S) fts_symbol_t V = S;
#include <fts/predefsymbols.h>

static int yyerror( const char *msg)
{
  fprintf( stderr, "***** %s\n", msg);
  return 0;
}

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right)
{
  fts_parsetree_t *tree;

  tree = (fts_parsetree_t *)malloc( sizeof( fts_parsetree_t));

  tree->token = token;

  if (value)
    tree->value = *value;
  else
    fts_set_void( &tree->value);

  tree->left = left;
  tree->right = right;

  return tree;
}

fts_status_t fts_parsetree_parse( int ac, const fts_atom_t *at, fts_parsetree_t **ptree)
{
  parser_data.ac = ac;
  parser_data.at = at;

  if (yyparse())
    {
      *ptree = NULL;
      return syntax_error_status;
    }

  *ptree = parser_data.tree;
  return fts_ok;
}

void fts_parsetree_delete( fts_parsetree_t *tree)
{
  if (!tree)
    return;

  fts_parsetree_delete( tree->left);
  fts_parsetree_delete( tree->right);
  free( tree);
}

static void parsetree_print_aux( fts_parsetree_t *tree, int indent)
{
  int i;

  fprintf( stderr, "%d:", indent);

  if (!tree)
    {
      fprintf( stderr, "\n");
      return;
    }

  for ( i = 0; i < indent; i++)
    fprintf( stderr, "   ");

  switch( tree->token) {
  case TK_DOT: fprintf( stderr, ".%s\n", fts_get_symbol( &tree->value)); break;
  case TK_COLON: fprintf( stderr, ": %s\n", fts_get_symbol( &tree->value)); break;
  case TK_COMMA: fprintf( stderr, ",\n"); break;
  case TK_SPACE: fprintf( stderr, "SPACE\n"); break;
  case TK_INT: fprintf( stderr, "INT %d\n", fts_get_int( &tree->value)); break;
  case TK_FLOAT: fprintf( stderr, "FLOAT %g\n", fts_get_float( &tree->value)); break;
  case TK_SYMBOL: 
    {
      fts_symbol_t s = fts_get_symbol( &tree->value);
      fprintf( stderr, "SYMBOL %s\n", (s != NULL) ? s : "null");
    }
    break;
  case TK_PAR: fprintf( stderr, "()\n"); break;
  case TK_CPAR: fprintf( stderr, "{}\n"); break;
  case TK_SQPAR: fprintf( stderr, "[]\n"); break;
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
  default: fprintf( stderr, "UNKNOWN %d\n", tree->token); 
  }

  parsetree_print_aux( tree->left, indent+1);
  parsetree_print_aux( tree->right, indent+1);
}

void fts_parsetree_print( fts_parsetree_t *tree)
{
  parsetree_print_aux( tree, 0);
}

main( int argc, char **argv)
{
  fts_parsetree_t *tree;

#if YYDEBUG
  yydebug = 1;
#endif

  tokenizer_init( argv[1]);

  if (fts_parsetree_parse( 0, 0, &tree) == fts_ok)
    fts_parsetree_print( tree);
  else
    fprintf( stderr, "Syntax error\n");
}

#endif
