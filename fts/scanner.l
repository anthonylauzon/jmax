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
#define YYSTYPE fts_atom_t
#include <ftsprivate/parserdata.h>
#include "parser.h"

#define YY_DECL int fts_string_lex( YYSTYPE *lvalp, void *data)
/*  #define RETURN(S) fprintf( stderr, "Token %s\n", #S); return S;  */
#define RETURN(S) return S; 

static fts_hashtable_t token_table;

%}

%option noyywrap

separator	[ \t\n]
whites		{separator}+
letter		[A-Za-z_]
decdigit	[0-9]
hexdigit	[0-9a-fA-F]
alpha		({letter}|{decdigit})
symbol		{letter}{alpha}*
decint		[+-]?{decdigit}+
hexint		0x{hexdigit}+
float		[+-]?{decdigit}+(\.{decdigit}*)?([eE][+-]?{decdigit}+)?

%%
{whites}
{decint}  {
            int i = atoi( yytext);
            fts_set_int( lvalp, i);
            RETURN( FTS_TOKEN_INT);
          }

{hexint}  {
            int i;
            sscanf( yytext, "0x%x", &i);
            fts_set_int( lvalp, i);
            RETURN( FTS_TOKEN_INT);
          }

{float}	  {
            float f;
            sscanf( yytext, "%f", &f);
            fts_set_float( lvalp, f);
            RETURN( FTS_TOKEN_FLOAT);
          }

{symbol}  {
            fts_symbol_t s = fts_new_symbol_copy( yytext); 
            fts_set_symbol( lvalp, s); 
            RETURN( FTS_TOKEN_SYMBOL);
          }

;         { RETURN( FTS_TOKEN_SEMICOLON); }
\(        { RETURN( FTS_TOKEN_LPAR); }
\)        { RETURN( FTS_TOKEN_RPAR); }
\.        { RETURN( FTS_TOKEN_DOT); }
\[        { RETURN( FTS_TOKEN_LSBRA); }
\]        { RETURN( FTS_TOKEN_RSBRA); }
\$        { RETURN( FTS_TOKEN_DOLLAR); }

%%

int fts_atoms_lex( YYSTYPE *lvalp, void *data)
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
      fts_atom_t k, v;

      k = *at;
      if (fts_hashtable_get( &token_table, &k, &v))
	token = fts_get_int( &v);
      else
	{
	  *lvalp = *at;
	  token = FTS_TOKEN_SYMBOL;
	}
    }

  parser_data->at++;
  parser_data->ac--;

  return token;
}

/* **********************************************************************
 *
 * Kernel initialization
 *
 */

static void scanner_put_symbol_token( fts_symbol_t s, int token)
{
  fts_atom_t k, v;

  fts_set_symbol( &k, s);
  fts_set_int( &v, token);
  fts_hashtable_put( &token_table, &k, &v);
}

void fts_kernel_scanner_init( void)
{
  fts_hashtable_init( &token_table, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_MEDIUM);

  scanner_put_symbol_token( fts_s_dollar, FTS_TOKEN_DOLLAR);
  scanner_put_symbol_token( fts_s_semi, FTS_TOKEN_SEMICOLON);
  scanner_put_symbol_token( fts_s_open_par, FTS_TOKEN_LPAR);
  scanner_put_symbol_token( fts_s_closed_par, FTS_TOKEN_RPAR);
  scanner_put_symbol_token( fts_s_open_sqpar, FTS_TOKEN_LSBRA);
  scanner_put_symbol_token( fts_s_closed_sqpar, FTS_TOKEN_RSBRA);
  scanner_put_symbol_token( fts_s_dot, FTS_TOKEN_DOT);
}
