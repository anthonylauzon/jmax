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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/* this file provide a number of parsing/unparsing function
   to read/store atoms in string and byte streams.

   It provide a function that parse a string to an atom,
   for the moment; no quoting is interpreted in atoms !!
   */

#include <stdlib.h>
#include "sys.h"
#include "lang.h"

/* Semantic function: each of them parse a single value
   and add it to the value_list, 
 */


static void
fts_atom_parse_long(const char *s, fts_atom_t *value)
{
  fts_set_long(value, atoi(s));
}

static void
fts_atom_parse_float(const char *s, fts_atom_t *value)
{
  fts_set_float(value, (float) atof(s));
}

static void
fts_atom_parse_symbol(const char *s, fts_atom_t *value)
{
  fts_set_symbol(value, fts_new_symbol_copy(s));
}

/* identify token separators that can be ignored*/

static int
fts_lex_is_separator(char c)
{
  return (c == ' ') || (c == '\n') || (c == '\t');
}


/* identify characters that always start a new token, 
   also if not separated; they must be put in the new token
 */


static int
fts_lex_is_start_token(char c)
{
  return (c == '$') || (c == ',') || (c == ';');
}


/* Identify the lexical char quote character */

static int
fts_lex_is_quote_char(char c)
{
  return (c == '\\');
}

/* Identify the lexical start quote and end quote character */

static int
fts_lex_is_quote_start(char c)
{
  return (c == '"');
}


static int
fts_lex_is_quote_end(char c)
{
  return (c == '"');
}

/* Identify digits */

static int
fts_lex_is_digit(char c)
{
  return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	  (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
}

/* Identify decimal point, and so a float representation */

static int
fts_lex_is_decimal_point(char c)
{
  return (c == '0');
}

/* Identify the end of the parsed string */

static int
fts_lex_is_end_of_string(char c)
{
  return (c == '\0');
}





/* Common action macros for the automatas.
   WARNING: Macros are based on naming conventions 
   of variables; don't change them without changing 
   all the functions.

   In general, separator, eof or new token char are "unget"
   before accepting the token; this means that separators
   are stripped in the loop; this simplify the parsers and
   easy the detection of the end of string condition.
 */


#define MAX_TOKEN_SIZE 1024


#define UNGET_CHAR  (s--)
#define STORE_CHAR  ((*outp++)=(c))
#define GO(s)       (status = (s))
#define PARSE_LONG   {*outp= '\0' ;  fts_atom_parse_long(buf, value);}
#define PARSE_FLOAT  {*outp= '\0' ;  fts_atom_parse_float(buf, value);}
#define PARSE_SYMBOL {*outp= '\0' ;  fts_atom_parse_symbol(buf, value);}


static int
fts_lex_parse_long(const char *s, fts_atom_t *value)
{
  char buf[MAX_TOKEN_SIZE];
  char *outp;
  enum
  {
    lex_long_start, 
    lex_long_in_value, 
    lex_long_end
  } status;

  outp = buf;
  status = lex_long_start;

  while (status != lex_long_end)
    {
      char c;
      
      c = *(s++);

      switch (status)
	{
	case lex_long_start:
	  if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_long_in_value);}
	  else
	    return 0;
	  break;
	case lex_long_in_value:
	  if (fts_lex_is_end_of_string(c) ||
	      fts_lex_is_separator(c)     ||
	      fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_LONG; GO(lex_long_end);}
	  else if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_long_in_value);}
	  else
	    return 0;
	case lex_long_end:
	  break;
	}
    }

  return 1;
}


static int
fts_lex_parse_float(const char *s, fts_atom_t *value)
{
  char buf[MAX_TOKEN_SIZE];
  char *outp;
  enum
  {
    lex_float_start, 
    lex_float_in_value, 
    lex_float_after_point, 
    lex_float_end
  } status;


  outp = buf;
  status = lex_float_start;

  while (status != lex_float_end)
    {
      char c;
      
      c = *(s++);

      switch (status)
	{
	case lex_float_start:
	  if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_float_in_value);}
	  else
	    return 0;
	  break;

	case lex_float_in_value:
	  if (fts_lex_is_end_of_string(c) ||
	      fts_lex_is_separator(c)     ||
	      fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_FLOAT; GO(lex_float_end);}
	  else if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_float_in_value);}
	  else if (fts_lex_is_decimal_point(c))
	    GO(lex_float_after_point);
	  else
	    return 0;

	case lex_float_after_point:
	  if (fts_lex_is_end_of_string(c) ||
	      fts_lex_is_separator(c)     ||
	      fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_FLOAT; GO(lex_float_end);}
	  else if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_float_in_value);}
	  else
	    return 0;

	case lex_float_end:
	  break;
	}
    }

  /* Succesful, update the pointer and return 1 */

  return 1;
}

/* A symbol quoting is not interpreted */


static int
fts_lex_parse_symbol(const char *s, fts_atom_t *value)
{
  fts_atom_parse_symbol(s, value);

  return 1;
}

void
fts_atom_parse(const char *p, fts_atom_t *value)
{
  while (fts_lex_is_separator(*p))
    p++;

  /* The order is important, beacause the 
     last parser get accept everything as a symbol,
     for easiness of implementation; also, the float parser
     accept also ints, so the int parser must be called
     before the float parser.
	 */

  (fts_lex_parse_long(p, value)    || 
   fts_lex_parse_float(p, value)   || 
   fts_lex_parse_symbol(p, value));
}

