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

#define YYSTYPE fts_atom_t
#define YYPARSE_PARAM data
#define YYLEX_PARAM data
#define free fts_free

static int yylex( YYSTYPE *lvalp, void *data);
static int yyerror( const char *msg);

/* Actions */
static void push_frame( void);
static void pop_frame( void);
static void push_value( const fts_atom_t *yylval);

%}

%pure_parser

/*
 * Tokens
 */

%token FTS_TOKEN_INT
%token FTS_TOKEN_FLOAT
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

/*
  Idea
  add the +=, -= etc operators so that you can do something like
  x : fvec
  $x += 1
  (:fvec 1 2 3) += 3
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

primitive: FTS_TOKEN_INT { push_value( &($1)); }
	| FTS_TOKEN_FLOAT { push_value( &($1)); }
	| FTS_TOKEN_SYMBOL { push_value( &($1)); }
;

par_op: FTS_TOKEN_OPEN_PAR term FTS_TOKEN_CLOSED_PAR

unary_op: FTS_TOKEN_PLUS term %prec FTS_TOKEN_UNARY_PLUS
	| FTS_TOKEN_MINUS term %prec FTS_TOKEN_UNARY_MINUS
	| FTS_TOKEN_LOGICAL_NOT term
;

binary_op: term FTS_TOKEN_PLUS term
	| term FTS_TOKEN_MINUS term
	| term FTS_TOKEN_TIMES term
	| term FTS_TOKEN_DIV term
	| term FTS_TOKEN_PERCENT term
	| term FTS_TOKEN_SHIFT_LEFT term
	| term FTS_TOKEN_SHIFT_RIGHT term
	| term FTS_TOKEN_LOGICAL_AND term
	| term FTS_TOKEN_LOGICAL_OR term
	| term FTS_TOKEN_EQUAL_EQUAL term
	| term FTS_TOKEN_NOT_EQUAL term
	| term FTS_TOKEN_GREATER term
	| term FTS_TOKEN_GREATER_EQUAL term
	| term FTS_TOKEN_SMALLER term
	| term FTS_TOKEN_SMALLER_EQUAL term
;

ref: FTS_TOKEN_DOLLAR FTS_TOKEN_SYMBOL
	| ref FTS_TOKEN_OPEN_SQPAR term FTS_TOKEN_CLOSED_SQPAR %prec FTS_TOKEN_ARRAY_INDEX
	| ref FTS_TOKEN_DOT FTS_TOKEN_SYMBOL
;

%%

/* **********************************************************************
 *
 * Parser extra functions
 *
 */

static fts_stack_t interpreter_stack;
static int fp = 0;

#define PUSH(V) fts_stack_push( &interpreter_stack, fts_atom_t, (V))
#define POP(N) fts_stack_pop( &interpreter_stack, (N))

#define TOP fts_stack_get_top( &interpreter_stack)
#define BASE ((fts_atom_t *)fts_stack_get_base( &interpreter_stack))

static void print_stack( const char *msg);

/*

Description of stack organization:

Before calling a method or outputing a message:
           <- top
arg3  
arg2
arg1       <- fp
savedfp
retval
arg2'
arg1'
savedfp'
retval'

After poping one frame:


arg3  
arg2
arg1
savedfp      <- top
retval
arg2'
arg1'        <- fp
savedfp'
retval'

*/

static void push_frame()
{
  fts_atom_t a;

  /* return value */
  fts_set_void( &a);
  PUSH( a);

  /* saved frame pointer */
  fts_set_int( &a, fp);
  PUSH( a);
  fp = TOP;

  print_stack( "Stack after pushing frame");
}

static void pop_frame()
{
  int old_fp;

  print_stack( "Stack before poping frame");

  old_fp = fp;
  fp = fts_get_int( BASE + fp - 1);
  POP( TOP - old_fp + 1);

  print_stack( "Stack after poping frame");
}

static void push_value( const fts_atom_t *yylval)
{
  PUSH( *yylval);
}

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
static fts_metaclass_t *token_type;
static fts_object_t *unique_token;

#define is_token(at) fts_is_a( at, token_type)
#define set_token(at,t) (fts_set_object( at, unique_token), fts_word_set_int( &fts_get_value( at), t))
#define get_token(at) (fts_word_get_int( &fts_get_value( at)))

typedef struct {
  int ac;
  fts_atom_t *at;
} fts_parser_data_t;

int fts_parse_atoms( int ac, fts_atom_t *at)
{
  fts_parser_data_t data;

  data.ac = ac;
  data.at = at;

  return yyparse( &data);
}

static int yylex( YYSTYPE *lvalp, void *data)
{
  fts_parser_data_t *parser_data = (fts_parser_data_t *)data;
  fts_atom_t *at = parser_data->at;
  int token = -1;

  if (parser_data->ac <= 0)
    return 0; /* end of file */

  if (fts_is_int( at))
    {
      *lvalp = *at;
      token = FTS_TOKEN_INT;
    }
  else if (fts_is_float( at))
    {
      *lvalp = *at;
      token = FTS_TOKEN_FLOAT;
    }
  else if (fts_is_symbol( at))
    {
      *lvalp = *at;
      token = FTS_TOKEN_SYMBOL;
    }
  else if (is_token( at))
    {
      token = get_token( at);
    }

  parser_data->at++;
  parser_data->ac--;

  return token;
}

void fts_parser_init( /*fts_parser_t *parser,*/ int ac, fts_atom_t *at)
{
  int i;

  for ( i =0; i < ac; i++)
    {
      if ( fts_is_symbol( at+i))
	{
	  fts_atom_t k, v;

	  k = *at;
	  if (fts_hashtable_get( &token_table, &k, &v))
	    set_token( at+i, fts_get_int( &v));
	}
    }
}


/* **********************************************************************
 * 
 * Debug code
 *
 */

static void print_stack( const char *msg)
{
  int i, current_fp;
  fts_atom_t *p = BASE;

  post( "%s:\n", msg);

  current_fp = fp;

  for ( i = TOP - 1; i >= 0; i--)
    {
      post( "[%2d]", i);
      if ( i == current_fp - 1)
	{
	  post( "* ");
	  current_fp = fts_get_int( p+i);
	}
      else
	post( "  ");

      post_atoms( 1, p+i);
      post( "\n");
    }
}

#ifdef HACK_DEBUG

static void fts_stdoutstream_output(fts_bytestream_t *stream, int n, const unsigned char *buffer)
{
  write( 1, buffer, n);
}

static void fts_stdoutstream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  write( 1, &c, 1);
}

static void fts_stdoutstream_flush(fts_bytestream_t *stream)
{
}

static void fts_stdoutstream_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_bytestream_init( (fts_bytestream_t *) o);
  fts_bytestream_set_output( (fts_bytestream_t *) o, fts_stdoutstream_output, fts_stdoutstream_output_char, fts_stdoutstream_flush);
}

static void fts_stdoutstream_receive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post( "Parsing ?\n");
  fts_parse_string( NULL);
}

static fts_status_t fts_stdoutstream_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_bytestream_t), 0, 0, 0);
  fts_bytestream_class_init(cl);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, fts_stdoutstream_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, fts_stdoutstream_receive);

  return fts_Success;
}
#endif

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

void fts_kernel_parser_init( void)
{
  fts_atom_t k, v;

  fts_stack_init( &interpreter_stack, fts_atom_t);

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

static fts_status_t
token_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof(fts_object_t), 0, 0, 0);

  return fts_Success;
}

void fts_parser_config( void)
{
#ifdef HACK_DEBUG
  {
    fts_metaclass_t *stdoutstream_type;
    fts_bytestream_t *stream;

    stdoutstream_type = fts_class_install( fts_new_symbol("stdoutstream"), fts_stdoutstream_instantiate);
    stream = (fts_bytestream_t *)fts_object_create( stdoutstream_type, 0, 0);
    fts_set_default_console_stream( stream);

    fts_sched_add( (fts_object_t *)stream, FTS_SCHED_ALWAYS);  
  }
#endif

  token_type = fts_class_install( fts_new_symbol( "_token"), token_instantiate);
  unique_token = fts_object_create( token_type, 0, 0);
}
