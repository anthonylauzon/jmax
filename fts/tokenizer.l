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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fts/fts.h>
#include "ftsprivate/tokenizer.h"

#define YY_DECL static int yylex( fts_atom_t *lvalp)

#define SET_INT(T) fts_set_int( (lvalp), strtol( T, (char **)NULL, 10))
#define SET_HEXINT(T) fts_set_int( (lvalp), strtol( T, (char **)NULL, 16))
#define SET_FLOAT(T) fts_set_float( (lvalp), strtod( T, (char **)NULL))
#define SET_SYMBOL(S) fts_set_symbol( (lvalp), (S)) 
#define SET_SYMBOL_COPY(T) fts_set_symbol( (lvalp), fts_new_symbol_copy( T)) 

%}

%option noyywrap
%option never-interactive

whites		[ \t\n]
decdigit	[0-9]
hexdigit	[0-9a-fA-F]
decint		[\-\+]?{decdigit}+
hexint		0x{hexdigit}+
float		-?{decdigit}+(\.{decdigit}*)?([eE][+-]?{decdigit}+)?
dollar		\$
semi		;
comma		,
plus		\+
minus		\-
times		\*
div		\/
power		\*\*
open_par	\(
closed_par	\)
open_sqpar	\[
closed_sqpar	\]
open_cpar	\{
closed_cpar	\}
dot		\.
percent		\%
shift_left	\<\<
shift_right	\>\>
logical_and	&&
logical_or	\|\|
logical_not	!
equal_equal	==
not_equal	!=
greater		\>
greater_equal	\>=
smaller		\<
smaller_equal	\<=
colon		:
equal		=
symbol		[^ \t=\.\$\(\)\[\]\{\};,:\-+][^ \t=\.\$\(\)\[\]\{\};,:]*
quoted_symbol	\"[^\"]+\"

%%

{whites}+
{decint}	{ SET_INT( yytext); return 1; }
{hexint} 	{ SET_HEXINT( yytext+2); return 1; }
{float} 	{ SET_FLOAT( yytext); return 1; }
{dollar}	{ SET_SYMBOL( fts_s_dollar); return 1; }
{semi}		{ SET_SYMBOL( fts_s_semi); return 1; }
{comma}		{ SET_SYMBOL( fts_s_comma); return 1; }
{plus}		{ SET_SYMBOL( fts_s_plus); return 1; }
{minus}		{ SET_SYMBOL( fts_s_minus); return 1; }
{times}		{ SET_SYMBOL( fts_s_times); return 1; }
{div}		{ SET_SYMBOL( fts_s_div); return 1; }
{power}		{ SET_SYMBOL( fts_s_power); return 1; }
{open_par}	{ SET_SYMBOL( fts_s_open_par); return 1; }
{closed_par}	{ SET_SYMBOL( fts_s_closed_par); return 1; }
{open_sqpar}	{ SET_SYMBOL( fts_s_open_sqpar); return 1; }
{closed_sqpar}	{ SET_SYMBOL( fts_s_closed_sqpar); return 1; }
{open_cpar}	{ SET_SYMBOL( fts_s_open_cpar); return 1; }
{closed_cpar}	{ SET_SYMBOL( fts_s_closed_cpar); return 1; }
{dot}		{ SET_SYMBOL( fts_s_dot); return 1; }
{percent}	{ SET_SYMBOL( fts_s_percent); return 1; }
{shift_left}	{ SET_SYMBOL( fts_s_shift_left); return 1; }
{shift_right}	{ SET_SYMBOL( fts_s_shift_right); return 1; }
{logical_and}	{ SET_SYMBOL( fts_s_logical_and); return 1; }
{logical_or}	{ SET_SYMBOL( fts_s_logical_or); return 1; }
{logical_not}	{ SET_SYMBOL( fts_s_logical_not); return 1; }
{equal_equal}	{ SET_SYMBOL( fts_s_equal_equal); return 1; }
{not_equal}	{ SET_SYMBOL( fts_s_not_equal); return 1; }
{greater}	{ SET_SYMBOL( fts_s_greater); return 1; }
{greater_equal}	{ SET_SYMBOL( fts_s_greater_equal); return 1; }
{smaller}	{ SET_SYMBOL( fts_s_smaller); return 1; }
{smaller_equal}	{ SET_SYMBOL( fts_s_smaller_equal); return 1; }
{colon}		{ SET_SYMBOL( fts_s_colon); return 1; }
{equal}		{ SET_SYMBOL( fts_s_equal); return 1; }
{quoted_symbol}	{ yytext[yyleng-1] = '\0'; SET_SYMBOL_COPY( yytext+1); return 1; }
{symbol}	{ SET_SYMBOL_COPY( yytext); return 1; }

%%

void fts_tokenizer_init_string( fts_tokenizer_t *tokenizer, const char *s)
{
  tokenizer->p = yy_scan_string( s);
}

void fts_tokenizer_init_buffer( fts_tokenizer_t *tokenizer, char *s, int size)
{
  tokenizer->p = yy_scan_buffer( s, (yy_size_t)size);
}

int fts_tokenizer_next( fts_tokenizer_t *tokenizer, fts_atom_t *a)
{
  yy_switch_to_buffer( (YY_BUFFER_STATE)tokenizer->p);

  return yylex( a);
}

void fts_tokenizer_destroy( fts_tokenizer_t *tokenizer)
{
  yy_delete_buffer( (YY_BUFFER_STATE)tokenizer->p);
}


#ifdef TEST
static void print_atom( const fts_atom_t *p)
{
  if ( fts_is_int( p))
    printf( "int: %d\n", fts_get_int( p));
  else if ( fts_is_float( p))
    printf( "float: %g\n", fts_get_float( p));
  else if ( fts_is_symbol( p))
    printf( "symbol: %s\n", fts_get_symbol( p));
}

int main( int argc, char **argv)
{
  fts_atom_t a;
  fts_tokenizer_t tokenizer;

  fts_tokenizer_init( &tokenizer, argv[1]);
  while (fts_tokenizer_run( &tokenizer, &a) != 0)
    {
      print_atom( &a);
    }
  fts_tokenizer_destroy( &tokenizer);

  return 0;
}

#endif
