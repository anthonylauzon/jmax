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

/*
  This version treats . as a standard character
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
#define SET_SYMBOL_NEW(S) fts_set_symbol( (lvalp), fts_new_symbol(S)) 

%}

%option noyywrap
%option never-interactive
%x AFTER_DOLLAR

decint		[\-\+]?[0-9]+
hexint		0x[0-9a-fA-F]+
float		[\-\+]?[0-9]+(\.[0-9]*)?([eE][+-]?[0-9]+)?

/* $;,(){}[] cannot appear inside a symbol (unless quoted) */
symbol		[^\$\;\,\(\)\{\}\[\] \t\n\r]+
quoted_symbol	\'[^\']+\'

%%

[ \t\n\r]+
{decint}		{ SET_INT( yytext); return 1; }
{hexint} 		{ SET_HEXINT( yytext+2); return 1; }
{float}		 	{ SET_FLOAT( yytext); return 1; }

\$			{ BEGIN(AFTER_DOLLAR); SET_SYMBOL( fts_s_dollar); return 1; }
<AFTER_DOLLAR>[0-9]+	{ BEGIN(INITIAL); SET_INT( yytext); return 1; }
<AFTER_DOLLAR>.		{ BEGIN(INITIAL); yyless(0); }

\;			{ SET_SYMBOL( fts_s_semi); return 1; }
\,			{ SET_SYMBOL( fts_s_comma); return 1; }
\(			{ SET_SYMBOL( fts_s_open_par);return 1; }
\)			{ SET_SYMBOL( fts_s_closed_par); return 1; }
\{			{ SET_SYMBOL( fts_s_open_cpar); return 1; }
\}			{ SET_SYMBOL( fts_s_closed_cpar); return 1; }
\[			{ SET_SYMBOL( fts_s_open_sqpar); return 1; }
\]			{ SET_SYMBOL( fts_s_closed_sqpar); return 1; }

{symbol}		{ SET_SYMBOL_NEW( yytext); return 1; }
{quoted_symbol}		{ yytext[yyleng-1] = '\0'; SET_SYMBOL_NEW( yytext+1); return 1; }

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
