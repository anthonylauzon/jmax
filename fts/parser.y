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

%{
#include <stdio.h>
#include <stdlib.h>
#include <fts/fts.h>
#include <ftsprivate/parser.h>

/* #define this to 1 if you want a lot of debug printout of the parser */
#define YYDEBUG 0

static fts_parsetree_t *fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right);

static fts_status_description_t syntax_error_status_description = {
  "expression syntax error"
};
fts_status_t syntax_error_status = &syntax_error_status_description;

typedef struct _parser_data {
  int ac;
  const fts_atom_t *at;
  int par_level;
  enum {mode_infix, mode_prefix} mode[1024];
  fts_parsetree_t *tree;
} parser_data_t;

#define YYPARSE_PARAM parm
#define YYLEX_PARAM (parser_data_t *)parm

static int yylex ();

static int yyerror( const char *msg);

static fts_atom_t a_times;

%}

%pure_parser

%union {
  fts_atom_t a;
  fts_parsetree_t *n;
}


/* Tokens */
%token <a> TK_INT
%token <a> TK_FLOAT
%token <a> TK_SYMBOL
%token TK_OPEN_PAR
%token TK_CLOSED_PAR
%token TK_OPEN_CPAR
%token TK_CLOSED_CPAR
%token TK_OPEN_SQPAR
%token TK_CLOSED_SQPAR

/* Tokens that are used only to label nodes in the parse tree */
%token TK_TUPLE
%token TK_PREFIX
%token TK_ELEMENT

/* Operators */
%left TK_COMMA
%left TK_SEMI
%left TK_SPACE
%left TK_EQUAL
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
%right TK_DOLLAR


/*
 * Non-terminal types
 */
%type <n> comma_expression_list
%type <n> term_list
%type <n> term
%type <n> primitive
%type <n> tuple
%type <n> element
%type <n> reference
%type <n> expr
%type <n> simple_term
%type <n> infix_term
%type <n> binary

%%
/***********************************************************************
 *
 *  rules
 *
 */

toplevel: comma_expression_list { ((parser_data_t *)parm)->tree = fts_parsetree_new( TK_COMMA, 0, $1, 0); }
  | comma_expression_list TK_SEMI { ((parser_data_t *)parm)->tree = fts_parsetree_new( TK_SEMI, 0, $1, 0); }
  | /* nix */ { ((parser_data_t *)parm)->tree = NULL; }
;

comma_expression_list: comma_expression_list TK_COMMA term_list { $$ = fts_parsetree_new( TK_COMMA, 0, $1, $3); }
  | comma_expression_list TK_SEMI term_list { $$ = fts_parsetree_new( TK_SEMI, 0, $1, $3); }
  | term_list { $$ = $1; }
;

term_list: term_list { ((parser_data_t *)parm)->mode[((parser_data_t *)parm)->par_level] = mode_prefix; } term /*%prec TK_SPACE*/ { $$ = fts_parsetree_new( TK_SPACE, 0, $1, $3); }
  | term
;

term: simple_term
  | tuple
;

simple_term: primitive
  | reference
  | element
  | { ((parser_data_t *)parm)->mode[++(((parser_data_t *)parm)->par_level)] = mode_infix; } expr { ((parser_data_t *)parm)->par_level--; $$ = $2; }
;

primitive: TK_INT { $$ = fts_parsetree_new( TK_INT, &($1), 0, 0); }
  | TK_FLOAT { $$ = fts_parsetree_new( TK_FLOAT, &($1), 0, 0); }
  | TK_SYMBOL { $$ = fts_parsetree_new( TK_SYMBOL, &($1), 0, 0); }
;

tuple: TK_OPEN_CPAR { ((parser_data_t *)parm)->mode[++(((parser_data_t *)parm)->par_level)] = mode_prefix; } term_list { ((parser_data_t *)parm)->par_level--; } TK_CLOSED_CPAR { $$ = fts_parsetree_new( TK_TUPLE, 0, $3, 0); }
;

reference: TK_DOLLAR TK_SYMBOL { $$ = fts_parsetree_new( TK_DOLLAR, &($2), 0, 0); }
  | TK_DOLLAR TK_INT { $$ = fts_parsetree_new( TK_DOLLAR, &($2), 0, 0); }
  | TK_DOLLAR TK_TIMES { $$ = fts_parsetree_new( TK_DOLLAR, &a_times, 0, 0); }
;

element: term TK_OPEN_SQPAR { ((parser_data_t *)parm)->mode[++(((parser_data_t *)parm)->par_level)] = mode_prefix; } term_list { ((parser_data_t *)parm)->par_level--; } TK_CLOSED_SQPAR { $$ = fts_parsetree_new( TK_ELEMENT, 0, $1, $4); }
;

expr: TK_OPEN_PAR binary TK_CLOSED_PAR { $$ = $2; }
  | TK_OPEN_PAR term_list TK_CLOSED_PAR { $$ = fts_parsetree_new( TK_PREFIX, 0, $2, 0); }
;

infix_term: term
  | binary
;

binary: infix_term TK_EQUAL infix_term { $$ = fts_parsetree_new( TK_EQUAL, 0, $1, $3); }
  | infix_term TK_PLUS infix_term { $$ = fts_parsetree_new( TK_PLUS, 0, $1, $3); }
  | infix_term TK_MINUS infix_term { $$ = fts_parsetree_new( TK_MINUS, 0, $1, $3); }
	| infix_term TK_TIMES infix_term { $$ = fts_parsetree_new( TK_TIMES, 0, $1, $3); }
	| infix_term TK_DIV infix_term { $$ = fts_parsetree_new( TK_DIV, 0, $1, $3); }
	| infix_term TK_POWER infix_term { $$ = fts_parsetree_new( TK_POWER, 0, $1, $3); }
	| infix_term TK_PERCENT infix_term { $$ = fts_parsetree_new( TK_PERCENT, 0, $1, $3); }
	| infix_term TK_SHIFT_LEFT infix_term { $$ = fts_parsetree_new( TK_SHIFT_LEFT, 0, $1, $3); }
	| infix_term TK_SHIFT_RIGHT infix_term { $$ = fts_parsetree_new( TK_SHIFT_RIGHT, 0, $1, $3); }
	| infix_term TK_LOGICAL_AND infix_term { $$ = fts_parsetree_new( TK_LOGICAL_AND, 0, $1, $3); }
	| infix_term TK_LOGICAL_OR infix_term { $$ = fts_parsetree_new( TK_LOGICAL_OR, 0, $1, $3); }
	| infix_term TK_EQUAL_EQUAL infix_term { $$ = fts_parsetree_new( TK_EQUAL_EQUAL, 0, $1, $3); }
	| infix_term TK_NOT_EQUAL infix_term { $$ = fts_parsetree_new( TK_NOT_EQUAL, 0, $1, $3); }
	| infix_term TK_GREATER infix_term { $$ = fts_parsetree_new( TK_GREATER, 0, $1, $3); }
	| infix_term TK_GREATER_EQUAL infix_term { $$ = fts_parsetree_new( TK_GREATER_EQUAL, 0, $1, $3); }
	| infix_term TK_SMALLER infix_term { $$ = fts_parsetree_new( TK_SMALLER, 0, $1, $3); }
	| infix_term TK_SMALLER_EQUAL infix_term { $$ = fts_parsetree_new( TK_SMALLER_EQUAL, 0, $1, $3); }
;

%%


static int yyerror( const char *msg)
{
  return 0;
}

/***********************************************************************
 *
 *  lexical analyser
 *
 */

static fts_hashtable_t token_table;

struct token_definition {
  int token;
  int operator;
};

static int
yylex( YYSTYPE *lvalp, parser_data_t *data)
{
  int token = -1;
  
  if (data->ac <= 0)
    return 0; /* end of file */
  
  if ( fts_is_symbol(data->at))
  {
    fts_atom_t v;
    struct token_definition *def;
    
    /* Is it a token symbol? */
    if (fts_hashtable_get( &token_table, data->at, &v))
    {
      def = (struct token_definition *)fts_get_pointer( &v);
      
      if (def->operator)
	    {
	      /* if operator, return it as token in infix mode */
	      if (data->par_level > 0 && data->mode[data->par_level] == mode_infix)
          token = def->token;
	      else
        {
          token = TK_SYMBOL;
          lvalp->a = *data->at;
        }
	    }
      else 
        token = def->token;  /* not an operator: always return it as token */
    }
    else
    {
      token = TK_SYMBOL;
      lvalp->a = *data->at;
    }
  }
  else if (fts_is_int( data->at))
  {
    token = TK_INT;
    lvalp->a = *data->at;
  }
  else if (fts_is_float( data->at))
  {
    token = TK_FLOAT;
    lvalp->a = *data->at;
  }
  
  data->at++;
  data->ac--;
  
  return token;
}

static void
token_table_put_entry( fts_symbol_t s, int token, int operator)
{
  fts_atom_t k, v;
  struct token_definition *def;

  def = (struct token_definition *)fts_malloc( sizeof(struct token_definition));
  def->token = token;
  def->operator = operator;

  fts_set_symbol( &k, s);
  fts_set_pointer( &v, def);
  fts_hashtable_put( &token_table, &k, &v);
}

static void
token_table_init( void)
{
  fts_hashtable_init( &token_table, FTS_HASHTABLE_MEDIUM);

  /* always recognized as operators */
  token_table_put_entry( fts_s_dollar, TK_DOLLAR, 0);
  token_table_put_entry( fts_s_semi, TK_SEMI, 0);
  token_table_put_entry( fts_s_comma, TK_COMMA, 0);
  token_table_put_entry( fts_s_open_par, TK_OPEN_PAR, 0);
  token_table_put_entry( fts_s_closed_par, TK_CLOSED_PAR, 0);
  token_table_put_entry( fts_s_open_cpar, TK_OPEN_CPAR, 0);
  token_table_put_entry( fts_s_closed_cpar, TK_CLOSED_CPAR, 0);
  token_table_put_entry( fts_s_open_sqpar, TK_OPEN_SQPAR, 0);
  token_table_put_entry( fts_s_closed_sqpar, TK_CLOSED_SQPAR, 0);

  /* recognized as operators only if inside parenthesis */
  token_table_put_entry( fts_s_equal, TK_EQUAL, 1);
  token_table_put_entry( fts_s_plus, TK_PLUS, 1);
  token_table_put_entry( fts_s_minus, TK_MINUS, 1);
  token_table_put_entry( fts_s_times, TK_TIMES, 1);
  token_table_put_entry( fts_s_div, TK_DIV, 1);
  token_table_put_entry( fts_s_power, TK_POWER, 1);
  token_table_put_entry( fts_s_percent, TK_PERCENT, 1);
  token_table_put_entry( fts_s_shift_left, TK_SHIFT_LEFT, 1);
  token_table_put_entry( fts_s_shift_right, TK_SHIFT_RIGHT, 1);
  token_table_put_entry( fts_s_logical_and, TK_LOGICAL_AND, 1);
  token_table_put_entry( fts_s_logical_or, TK_LOGICAL_OR, 1);
  token_table_put_entry( fts_s_logical_not, TK_LOGICAL_NOT, 1);
  token_table_put_entry( fts_s_equal_equal, TK_EQUAL_EQUAL, 1);
  token_table_put_entry( fts_s_not_equal, TK_NOT_EQUAL, 1);
  token_table_put_entry( fts_s_greater, TK_GREATER, 1);
  token_table_put_entry( fts_s_greater_equal, TK_GREATER_EQUAL, 1);
  token_table_put_entry( fts_s_smaller, TK_SMALLER, 1);
  token_table_put_entry( fts_s_smaller_equal, TK_SMALLER_EQUAL, 1);
}

/***********************************************************************
 *
 *  parser abstract tree construction/destruction
 *
 */

static fts_heap_t *parsetree_heap;

static fts_parsetree_t *
fts_parsetree_new( int token, fts_atom_t *value, fts_parsetree_t *left, fts_parsetree_t *right)
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

fts_status_t 
fts_parsetree_parse( int ac, const fts_atom_t *at, fts_parsetree_t **ptree)
{
  parser_data_t data;
  int i;

  data.ac = ac;
  data.at = at;
  data.par_level = 0;
  
  for(i=0; i<1024; i++)
    data.mode[i] = mode_infix;

  if (yyparse( &data))
    {
      *ptree = NULL;
      return syntax_error_status;
    }

  *ptree = data.tree;
  return fts_ok;
}

void
fts_parsetree_delete( fts_parsetree_t *tree)
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

void
fts_kernel_parser_init( void)
{
  parsetree_heap = fts_heap_new( sizeof( fts_parsetree_t));

  token_table_init();
  
  fts_set_symbol(&a_times, fts_s_times);

#if YYDEBUG
  yydebug = 1;
#endif
}

