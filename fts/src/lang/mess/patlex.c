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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/patlex.h"


/**
 * A lexical analyser for .pat files
 * It reconize int as separate then floats, and
 * it parse the ending ';' in TT_EOC (end of command). <br>
 * Also, it do backslash to quote transformation for message box , and backslash quoting suppression. <p>
 * It handle also variable substitution
 * for abstractions (in max, they are handled before the lexical
 * analisys: $0-foo is expanded to a single string). <p>
 *
 */

static int unique_count = 3333; /* the unique number generation */

fts_patlex_t *
fts_patlex_open(const char *filename, int env_argc, const fts_atom_t *env_argv)
{
  fts_patlex_t *this;
  FILE *file;

  file  = fopen(filename, "r");

  if (file == 0)
    return 0;

  return fts_patlex_open_file( file, env_argc, env_argv);
}

/* Version of the above that already get a FILE * instead of the file name.
   Used for abstractions, where we need to look for a file 
   */

fts_patlex_t *
fts_patlex_open_file(FILE *file, int env_argc, const fts_atom_t *env_argv)
{
  fts_patlex_t *this;

  this = (fts_patlex_t *) fts_malloc(sizeof(fts_patlex_t));
  this->fd = file;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->unique_var = unique_count++; /* the unique number used in variable  0 substitution */
  this->pushedBack = 0;
  this->lookahead_valid = 0;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->buf_fill = 0;
  this->messbox_mode = 0;

  return this;
}

void
fts_patlex_close(fts_patlex_t *this)
{
  fclose(this->fd);
  fts_free(this);
}

/* private predicates for character classes */

static int fts_patlex_is_dollar(int c)
{
  return c == '$';
}

static int fts_patlex_is_blank(int c)
{
  return (c == '\t') || (c == ' ') || (c == '\n') || (c == '\r');
}

static int fts_patlex_is_semi(int c)
{
  return (c == ';');
}

static int fts_patlex_is_digit(int c)
{
  return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	  (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
}

static int fts_patlex_is_decimal_point(int c)
{
  return (c == '.');
}

static int fts_patlex_is_sign(int c)
{
  return (c == '-');
}

static int fts_patlex_is_backslash(int c)
{
  return (c == '\\');
}

/* states for the automata */

#define tt_waiting      0
#define tt_in_var       1
#define tt_in_string    2
#define tt_in_quoted_char  3
#define tt_in_number       5
#define tt_in_number_or_sign 6
#define tt_in_float    7

void fts_patlex_next_token(fts_patlex_t *this)
{
  this->buf_fill = 0;

  if (this->pushedBack)
    {
      this->pushedBack = 0;
      return;
    }
  else
    {
      int c, status;
      
      if (this->lookahead_valid)
	{
	  c = this->lookahead;
	  this->lookahead_valid = 0;
	}
      else
	{
	  c = getc(this->fd);
	}

      status = tt_waiting;

      while (1)
	{
	  switch (status)
	    {
	    case tt_waiting:
	      if (feof(this->fd))
		{
		  this->ttype = FTS_LEX_EOF;
		  return;
		}
	      else if (fts_patlex_is_dollar(c))
		{
		  status = tt_in_var;
		}
	      else if (fts_patlex_is_semi(c))
		{
		  this->ttype = FTS_LEX_EOC;
		  return;
		}
	      else if (fts_patlex_is_digit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number;
		}
	      else if (fts_patlex_is_sign(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number_or_sign;
		}
	      else if (fts_patlex_is_backslash(c))
		{
		  status = tt_in_quoted_char;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  status = tt_waiting;
		}
	      else
		{	  
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;
		
	    case tt_in_var:

	      if (fts_patlex_is_digit(c))
		{
		  if (this->env_argv != 0)
		    {
		      int v = c - '0'; 

		      if (v == 0)
			{
			  sprintf(this->buf, "%d", this->unique_var);
			  this->buf_fill = strlen(this->buf);
			  status = tt_in_string;
			}
		      else
			{
			  /** Getting an env value. */

			  if (v > this->env_argc)
			    {
			      this->buf[this->buf_fill++] = '0';
			      status = tt_in_number;
			    }
			  else
			    {
			      const fts_atom_t *value;

			      value = &(this->env_argv[v - 1]);
			      
			      if (fts_is_int(value))
				{
				  status = tt_in_number;
				  sprintf(this->buf, "%d", fts_get_int(value));
				  this->buf_fill = strlen(this->buf);
				}
			      else if (fts_is_float(value))
				{
				  status = tt_in_float;
				  sprintf(this->buf, "%f", fts_get_float(value));
				  this->buf_fill = strlen(this->buf);
				}
			      else if (fts_is_symbol(value))
				{
				  status = tt_in_string;
				  strcpy(this->buf, fts_symbol_name(fts_get_symbol(value)));
				  this->buf_fill = strlen(this->buf);
				}
			      else
				this->buf[this->buf_fill++] = '0';
			    }
			}
		    }
		  else
		    {
		      this->buf[this->buf_fill++] = '#';
		      this->buf[this->buf_fill++] = c;
		      status = tt_in_string;
		    }
		}
	      else
		{
		  this->buf[this->buf_fill++] = '$';
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;

	    case tt_in_string:
	      if (feof(this->fd))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_dollar(c))
		{
		  /* A dollar at the beginning of a word become
		     a sigle $ symbol, otherwise is just another char */

		  if (this->buf_fill == 0)
		    {
		      this->buf[this->buf_fill++] = c;
		      this->buf[this->buf_fill++] = '\0';
		      fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		      this->ttype = FTS_LEX_SYMBOL;
		      return;
		    }
		  else
		    {
		      this->buf[this->buf_fill++] = c;
		      status = tt_in_string;
		    }
		}
	      else if (fts_patlex_is_semi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
	       }
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_backslash(c))
		{
		  status = tt_in_quoted_char;
		}
	      else
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;

	    case tt_in_quoted_char:
	      if (feof(this->fd))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_dollar(c))
		{
		  /* A dollar at the beginning of a word become
		     a single $ symbol */
		  this->buf[this->buf_fill++] = c;
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_backslash(c))
		{
		  if (this->messbox_mode)
		    {
		      /* in messbox, a double '\' become a
			 single quote token 
			 */

		      this->buf[this->buf_fill++] = '\'';
		      this->buf[this->buf_fill++] = '\0';
		      fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		      this->ttype = FTS_LEX_SYMBOL;
		      return;
		    }
		  else
		    {
		      /* In normal mode, a double '\' is just consumed
		       */

		      status = tt_in_string;
		    }
		}
	      else
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;

	    case tt_in_number:
	      if (feof(this->fd))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_int(&(this->val), atoi(this->buf));
		  this->ttype = FTS_LEX_NUMBER;
		  return;
		}
	      else if (fts_patlex_is_semi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_int(&(this->val), atoi(this->buf));
		  this->ttype = FTS_LEX_NUMBER;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_int(&(this->val), atoi(this->buf));
		  this->ttype = FTS_LEX_NUMBER;
		  return;
		}
	      else if (fts_patlex_is_digit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number;
		}
	      else if (fts_patlex_is_decimal_point(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_float;
		}
	      else
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;


	    case tt_in_number_or_sign:
	      if (feof(this->fd))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_semi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_digit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number;
		}
	      else if (fts_patlex_is_decimal_point(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_float;
		}
	      else
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;

	    case tt_in_float:
	      if (feof(this->fd))
		{
		  float f;	
		  this->buf[this->buf_fill++] = '\0';
		  sscanf(this->buf, "%f", &f);
		  fts_set_float(&(this->val), f);
		  this->ttype = FTS_LEX_FLOAT;
		  return;
		}
	      if (fts_patlex_is_semi(c))
		{
		  float f;
		  this->buf[this->buf_fill++] = '\0';
		  sscanf(this->buf, "%f", &f);
		  fts_set_float(&(this->val), f);
		  this->ttype = FTS_LEX_FLOAT;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  float f;
		  this->buf[this->buf_fill++] = '\0';
		  sscanf(this->buf, "%f", &f);
		  fts_set_float(&(this->val), f);
		  this->ttype = FTS_LEX_FLOAT;
		  return;
		}
	      else if (fts_patlex_is_digit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_float;
		}
	      else
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_string;
		}
	      break;
	    }

	  c = getc(this->fd);
	}
    }
}

