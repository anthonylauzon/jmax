
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/patlex.h"


/**
 * A lexical analyser for .pat files
 * But: it reconize int as separate then floats, and
 * it parse the ending ';' in TT_EOC (end of command). <br>
 * Also, it do backslash quoting. <p>
 * It handle also variable substitution
 * for abstractions (in max, they are handled before the lexical
 * analisys: $0-foo is expanded to a single string). <p>
 *
 */

static int unique_count = 3333; /* the unique number generation */

fts_pat_lexer_t *
fts_open_pat_lexer(const char *filename, int env_argc, const fts_atom_t *env_argv)
{
  fts_pat_lexer_t *this;
  FILE *fd;

  fd  = fopen(filename, "r");

  if (fd == 0)
    return 0;

  this = (fts_pat_lexer_t *) fts_malloc(sizeof(fts_pat_lexer_t));
  this->fd = fd;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->unique_var = unique_count++; /* the unique number used in variable  0 substitution */
  this->pushedBack = 0;
  this->lookahead_valid = 0;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->buf_fill = 0;

  return this;
}

/* Version of the above that already get a FILE * instead of the file name.
   Used for abstractions, where we need to look for a file 
   */

fts_pat_lexer_t *
fts_open_pat_lexer_file(FILE *file, int env_argc, const fts_atom_t *env_argv)
{
  fts_pat_lexer_t *this;

  this = (fts_pat_lexer_t *) fts_malloc(sizeof(fts_pat_lexer_t));
  this->fd = file;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->unique_var = unique_count++; /* the unique number used in variable  0 substitution */
  this->pushedBack = 0;
  this->lookahead_valid = 0;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->buf_fill = 0;

  return this;
}

void
fts_close_pat_lexer(fts_pat_lexer_t *this)
{
  fclose(this->fd);
  fts_free(this);
}

/* private predicates for character classes */

static int isDollar(int c)
{
  return c == '$';
}

static int isBlank(int c)
{
  return (c == '\t') || (c == ' ') || (c == '\n') || (c == '\r');
}

static int isSemi(int c)
{
  return (c == ';');
}

static int isDigit(int c)
{
  return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	  (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
}

static int isDecimalPoint(int c)
{
  return (c == '.');
}

static int isSign(int c)
{
  return (c == '-');
}

static int isBackSlash(int c)
{
  return (c == '\\');
}

/* states for the automata */

#define tt_waiting      0
#define tt_in_var       1
#define tt_in_string    2
#define tt_in_quoted_char  3
#define tt_in_qquoted_char 4
#define tt_in_number       5
#define tt_in_number_or_sign 6
#define tt_in_float    7

void nextToken(fts_pat_lexer_t *this)
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
	      else if (isDollar(c))
		{
		  status = tt_in_var;
		}
	      else if (isSemi(c))
		{
		  this->ttype = FTS_LEX_EOC;
		  return;
		}
	      else if (isDigit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number;
		}
	      else if (isSign(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number_or_sign;
		}
	      else if (isBackSlash(c))
		{
		  status = tt_in_quoted_char;
		}
	      else if (isBlank(c))
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

	      if (isDigit(c) && (this->env_argv != 0))
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
	      
	      break;

	    case tt_in_string:
	      if (feof(this->fd))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (isSemi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
	       }
	      else if (isBlank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (isBackSlash(c))
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
	      if (isBackSlash(c))
		{
		  status = tt_in_qquoted_char;
		}
	      else if (isDollar(c))
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
	      break;

	    case tt_in_qquoted_char:
	      if (isBackSlash(c))
		{
		  status = tt_in_quoted_char;
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
	      if (isSemi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_int(&(this->val), atoi(this->buf));
		  this->ttype = FTS_LEX_NUMBER;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
		}
	      else if (isBlank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_int(&(this->val), atoi(this->buf));
		  this->ttype = FTS_LEX_NUMBER;
		  return;
		}
	      else if (isDigit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number;
		}
	      else if (isDecimalPoint(c))
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
	      if (isSemi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
		}
	      else if (isBlank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol_copy(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (isDigit(c))
		{
		  this->buf[this->buf_fill++] = c;
		  status = tt_in_number;
		}
	      else if (isDecimalPoint(c))
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
	      if (isSemi(c))
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
	      else if (isBlank(c))
		{
		  float f;
		  this->buf[this->buf_fill++] = '\0';
		  sscanf(this->buf, "%f", &f);
		  fts_set_float(&(this->val), f);
		  this->ttype = FTS_LEX_FLOAT;
		  return;
		}
	      else if (isDigit(c))
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


    
    

