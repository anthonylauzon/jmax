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
#define YYSTYPE fts_atom_t
#include "parser.h"
#include "ftsprivate/tokenizer.h"

#define YY_DECL int yylex( YYSTYPE *lvalp)
#define SET_INT(L,T) fts_set_int( (L), strtol( T, (char **)NULL, 10))
#define SET_HEXINT(L,T) fts_set_int( (L), strtol( T, (char **)NULL, 16))
#define SET_FLOAT(L,T) fts_set_float( (L), strtod( T, (char **)NULL))
#define SET_SYMBOL(L,S) fts_set_symbol( (L), (S)) 
#define SET_TOKEN(L,T) fts_set_token( (L), (T)) 
#define SET_SYMBOL_COPY(L,T) fts_set_symbol( (L), fts_new_symbol_copy( T)) 

%}

%option noyywrap

%option noyywrap

whites		[ \t\n]
letter		[A-Za-z_]
decdigit	[0-9]
hexdigit	[0-9a-fA-F]
decint		{decdigit}+
hexint		0x{hexdigit}+
float		{decdigit}+(\.{decdigit}*)?([eE][+-]?{decdigit}+)?
tilde		~
dollar		\$
semi		;
plus		\+
minus		\-
times		\*
div		\/
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
conditional	\?
colon		:
equal		=
operator  	({plus})|({minus})|({times})|({div})|({percent})|({shift_left})|({shift_right})|({logical_and})|({logical_or})|({logical_not})|({equal_equal})|({not_equal})|({greater})|({greater_equal})|({smaller})|({smaller_equal})
stdsymbol	{letter}[^\ ]*({tilde})?
opsymbol	({operator})*({tilde})
symbol		({stdsymbol})|({opsymbol})

%%

{whites}+
{decint}	{ SET_INT( lvalp, yytext); return FTS_TOKEN_INT; }
{hexint} 	{ SET_HEXINT( lvalp, yytext+2); return FTS_TOKEN_INT; }
{float} 	{ SET_FLOAT( lvalp, yytext); return FTS_TOKEN_FLOAT; }
{dollar}	{ SET_SYMBOL( lvalp, fts_s_dollar); return FTS_TOKEN_SYMBOL; }
{semi}		{ SET_SYMBOL( lvalp, fts_s_semi); return FTS_TOKEN_SYMBOL; }
{plus}		{ SET_SYMBOL( lvalp, fts_s_plus); return FTS_TOKEN_SYMBOL; }
{minus}		{ SET_SYMBOL( lvalp, fts_s_minus); return FTS_TOKEN_SYMBOL; }
{times}		{ SET_SYMBOL( lvalp, fts_s_times); return FTS_TOKEN_SYMBOL; }
{div}		{ SET_SYMBOL( lvalp, fts_s_div); return FTS_TOKEN_SYMBOL; }
{open_par}	{ SET_SYMBOL( lvalp, fts_s_open_par); return FTS_TOKEN_SYMBOL; }
{closed_par}	{ SET_SYMBOL( lvalp, fts_s_closed_par); return FTS_TOKEN_SYMBOL; }
{open_sqpar}	{ SET_SYMBOL( lvalp, fts_s_open_sqpar); return FTS_TOKEN_SYMBOL; }
{closed_sqpar}	{ SET_SYMBOL( lvalp, fts_s_closed_sqpar); return FTS_TOKEN_SYMBOL; }
{open_cpar}	{ SET_SYMBOL( lvalp, fts_s_open_cpar); return FTS_TOKEN_SYMBOL; }
{closed_cpar}	{ SET_SYMBOL( lvalp, fts_s_closed_cpar); return FTS_TOKEN_SYMBOL; }
{dot}		{ SET_SYMBOL( lvalp, fts_s_dot); return FTS_TOKEN_SYMBOL; }
{percent}	{ SET_SYMBOL( lvalp, fts_s_percent); return FTS_TOKEN_SYMBOL; }
{shift_left}	{ SET_SYMBOL( lvalp, fts_s_shift_left); return FTS_TOKEN_SYMBOL; }
{shift_right}	{ SET_SYMBOL( lvalp, fts_s_shift_right); return FTS_TOKEN_SYMBOL; }
{logical_and}	{ SET_SYMBOL( lvalp, fts_s_logical_and); return FTS_TOKEN_SYMBOL; }
{logical_or}	{ SET_SYMBOL( lvalp, fts_s_logical_or); return FTS_TOKEN_SYMBOL; }
{logical_not}	{ SET_SYMBOL( lvalp, fts_s_logical_not); return FTS_TOKEN_SYMBOL; }
{equal_equal}	{ SET_SYMBOL( lvalp, fts_s_equal_equal); return FTS_TOKEN_SYMBOL; }
{not_equal}	{ SET_SYMBOL( lvalp, fts_s_not_equal); return FTS_TOKEN_SYMBOL; }
{greater}	{ SET_SYMBOL( lvalp, fts_s_greater); return FTS_TOKEN_SYMBOL; }
{greater_equal}	{ SET_SYMBOL( lvalp, fts_s_greater_equal); return FTS_TOKEN_SYMBOL; }
{smaller}	{ SET_SYMBOL( lvalp, fts_s_smaller); return FTS_TOKEN_SYMBOL; }
{smaller_equal}	{ SET_SYMBOL( lvalp, fts_s_smaller_equal); return FTS_TOKEN_SYMBOL; }
{conditional}	{ SET_SYMBOL( lvalp, fts_s_conditional); return FTS_TOKEN_SYMBOL; }
{colon}		{ SET_SYMBOL( lvalp, fts_s_colon); return FTS_TOKEN_SYMBOL; }
{equal}		{ SET_SYMBOL( lvalp, fts_s_equal); return FTS_TOKEN_SYMBOL; }
{symbol}	{ SET_SYMBOL( lvalp, yytext); return FTS_TOKEN_SYMBOL; }

%%

void fts_tokenizer_init( fts_tokenizer_t *tokenizer, const char *s)
{
  tokenizer->p = yy_scan_string( s);
}

int fts_tokenizer_run( fts_tokenizer_t *tokenizer, fts_atom_t *a)
{
  yy_switch_to_buffer( (YY_BUFFER_STATE)tokenizer->p);

  return yylex( a);
}

void fts_tokenizer_destroy( fts_tokenizer_t *tokenizer)
{
  yy_delete_buffer( (YY_BUFFER_STATE)tokenizer->p);
}


