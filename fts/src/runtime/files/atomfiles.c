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

/* 
   Functions to read and write an atom from a file; should go in parser.c,
   after a good clean up.

   fts_read_atom return 1 if it read it, and zero if the
   file is at an end.

   the file format used is a human readible format; 

   newline, carriage return, space and tabs and nulls are considered token separators

   backslash quote special caracters, including itself,

   ; , $ and ' are considered as single char atoms, unless backquoted, when they are
   always a token by themselves.

   Only int, float and symbols are read.

   Anything between " " is considered an atom name, including
   the null string; " always start an new atom , unless quoted.

   An atom should not be longer than 1024 chars.

   Atoms are read and written to/from "fts_atom_file_t" 
   
*/

#include <unistd.h>
#include <string.h>
#include "sys.h"
#include "lang.h"
#include "runtime/files/files.h"
#include "runtime/files/atomfiles.h"

#define ATOM_FILE_BUF_SIZE 512

struct _fts_atom_file_t_
{
  int fd;
  char mode;			/* "r" for read, "w" for write */

  /* read part  */

  char buf[ATOM_FILE_BUF_SIZE];	/* buffer of character read */
  int  count;			/* the current size of the buffer */
  int  read;			/* the index of the last char read */

  /* write part: nothing needed */

};



fts_atom_file_t *
fts_atom_file_open(const char *name, const char *mode)
{
  fts_atom_file_t *f;

  f = (fts_atom_file_t *) fts_malloc(sizeof(fts_atom_file_t));

  f->mode = *mode;

  if (f->mode == 'w')
    {
      f->fd = fts_file_open(name, mode);
    }
  else
    {
      /* anything else is like 'r' */

      f->fd = fts_file_open(name, mode);
      f->count = 0;
      f->read = 0;
    }

  if (f->fd == -1)
    return 0;
  else
    return f;
}


void
fts_atom_file_close(fts_atom_file_t *f)
{
  fts_file_close(f->fd);
  fts_free(f);
}

#define IS_SEPARATOR(c) ((((c) == ' ') || ((c) == '\t') || ((c) == '\r') || ((c) == '\n') || ((c) == '\0'))? (c): 0)
#define IS_DIGIT(c)     (('0' <= (c)) && ((c) <= '9'))
#define IS_SIGN(c)      (((c) == '+') || ((c) == '-'))
#define IS_POINT(c)     ('.' == (c))
#define IS_BACKSLASH_QUOTE(c)     ('\\' == (c))
#define IS_DOUBLE_QUOTE(c)     ('\"' == (c))
#define IS_ATOM_CHAR(c) (((c) == ';') || ((c) == ',') || ((c) == '$') || ((c) == '\''))
#define IS_EOF(c)       (0xff == (c))

static void
fts_atom_file_read_more(fts_atom_file_t *f)
{
  f->count = read(f->fd, f->buf, ATOM_FILE_BUF_SIZE);
  f->read = 0;
}

int
fts_atom_file_read(fts_atom_file_t *f, fts_atom_t *at, char *separator)
{
  char buf[1024];
  enum
    {
      read_begin,
      read_in_int, 
      read_in_float,
      read_in_symbol, 
      read_in_quoted_symbol,
      read_separator,
      read_end
    } status = read_begin;

  enum {unkown, an_int, a_float, a_symbol} read_type = unkown;

  int fill_p = 0;
  int backslash_quoted = 0;
  unsigned char c = '\0';
  char sep = 0;

  while(status != read_end)
    {
      if(!IS_EOF(c))
	{
	  if (f->read >= f->count)
	    fts_atom_file_read_more(f);
	  
	  if (f->count == 0)
	    c = 0xff;
	  else
	    c = f->buf[f->read];
	}

      if (IS_BACKSLASH_QUOTE(c) && !backslash_quoted)
	{
	  
	  (f->read)++;
	  backslash_quoted = 1;
	}
      else
	{
	  switch (status)
	    {
	    case read_begin:
	      if (IS_EOF(c))
		{
		  /* file end before anything found */
		  return 0;
		}
	      else if (IS_SEPARATOR(c))
		{
		  /* skip seperators at file beginning */
		  (f->read)++;
		}
	      else if (IS_DIGIT(c) || IS_SIGN(c))
		{
		  /* might be an integer (might be just a "-") */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_int;
		}
	      else if (IS_DOUBLE_QUOTE(c) && !backslash_quoted)
		{
		  /* begin of quoted symbol */
		  (f->read)++;
		  status = read_in_quoted_symbol;
		}
	      else if (IS_ATOM_CHAR(c))
		{
		  buf[fill_p++] = c;
		  (f->read)++;
		  
		  if(backslash_quoted)
		    /* begin of symbol starting with quoted single atom char */
		    status = read_in_symbol;
		  else
		    {
		      /* end of single atom character */
		      buf[fill_p++] = '\0';
		      read_type = a_symbol;
		      status = read_separator;
		    }
		}
	      else
		{
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_symbol;
		}
	      
	      break;
	      
	    case read_in_int:
	      if (IS_EOF(c))
		{
		  /* end of file */
		  buf[fill_p++] = '\0';
		  read_type = an_int;
		  status = read_end;
		}
	      else if ((IS_SEPARATOR(c) || IS_DOUBLE_QUOTE(c) || IS_ATOM_CHAR(c)) && !backslash_quoted)
		{
		  /* end of integer atom (do not avance the read pointer) */
		  if(buf[fill_p - 1] == '-')
		    read_type = a_symbol;
		  else
		    read_type = an_int;
		    
		  buf[fill_p++] = '\0';
		  status = read_separator;
		}
	      else if (IS_DIGIT(c))
		{
		  /* continue with integer */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_int;
		}
	      else if (IS_POINT(c))
		{
		  /* change to float */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_float;
		}
	      else
		{
		  /* change to symbol */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_symbol;
		}
	      break;
	      
	    case read_in_float:
	      if (IS_EOF(c))
		{
		  /* end of file */
		  buf[fill_p++] = '\0';
		  read_type = a_float;
		  status = read_end;
		}
	      else if ((IS_SEPARATOR(c) || IS_DOUBLE_QUOTE(c) || IS_ATOM_CHAR(c)) && !backslash_quoted)
		{
		  /* end of float atom (do not avance the read pointer) */
		  buf[fill_p++] = '\0';
		  read_type = a_float;
		  status = read_separator;
		}
	      else if (IS_DIGIT(c))
		{
		  /* go on in float */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_float;
		}
	      else
		{
		  /* change to symbol */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_symbol;
		}
	      break;
	      
	    case read_in_symbol:
	      if (IS_EOF(c))
		{
		  /* end of file */
		  buf[fill_p++] = '\0';
		  read_type = a_symbol;
		  status = read_end;
		}
	      else if ((IS_SEPARATOR(c) || IS_DOUBLE_QUOTE(c) || IS_ATOM_CHAR(c)) && !backslash_quoted)
		{
		  /* end of symbol atom (do not avance the read pointer) */
		  buf[fill_p++] = '\0';
		  read_type = a_symbol;
		  status = read_separator;
		}
	      else
		{
		  /* go on in symbol */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_symbol;
		}
	      break;
	      
	    case read_in_quoted_symbol:
	      if (IS_EOF(c))
		{
		  /* end of file */
		  buf[fill_p++] = '\0';
		  read_type = a_symbol;
		  status = read_end;
		}
	      else if (IS_DOUBLE_QUOTE(c) && !backslash_quoted)
		{
		  /* end of quoted symbol */
		  buf[fill_p++] = '\0';
		  (f->read)++;
		  read_type = a_symbol;
		  status = read_separator;
		}
	      else
		{
		  /* go on in quoted symbol */
		  buf[fill_p++] = c;
		  (f->read)++;
		  status = read_in_quoted_symbol;
		}
	      break;
	      
	    case read_separator:
	      if(IS_SEPARATOR(c))
		{
		  switch(sep)
		    {  
		    case 0:
		      if(c == ' ')
			{
			  sep = ' ';	  
			  break;
			}
		    case ' ':
		      if(c == '\r')
			{
			  sep = '\r';
			  break;
			}
		    case '\r':
		      if(c == '\t')
			{
			  sep = '\t';
			  break;
			}
		    case '\t':
		      if(c == '\n')
			{
			  sep = '\n';
			  break;
			}
		    }
		  
		  (f->read)++;
		}
	      else
		status = read_end;
	      break;
	      
	    case read_end:
	      break;
	    }

	  if(backslash_quoted)
	    backslash_quoted = 0;
	}
    }
  
  switch (read_type)
    {
    case an_int:
      {
	long l;
	
	sscanf(buf, "%ld", &l);
	fts_set_long(at, l);
      }
    break;
    case a_float:
      {
	float f;
	
	sscanf(buf, "%f", &f);
	fts_set_float(at, f);
      }
    break;
    case a_symbol:
      if(strcmp("()", buf) == 0)
	fts_set_void(at);
      else
	fts_set_symbol(at, fts_new_symbol_copy(buf));
      break;
    }

  *separator = sep;
  return 1; /* 1 is for atom found */
}

/* return zero if the atom was not writable,
   like a pointer or so.
   The separator char is added after the symbol; in must be
   a valid (IS_SEPARATOR(c)) chars, otherwise is substituted
   by a blank.
*/
int
fts_atom_file_write(fts_atom_file_t *f, const fts_atom_t *at, char separator)
{
  char buf[1024];
  int offset = 0;

  if (! IS_SEPARATOR(separator))
    separator = ' ';
    
  if (fts_is_long(at))
    sprintf(buf, "%d", fts_get_int(at));
  else if (fts_is_float(at))
    sprintf(buf, "%#f", fts_get_float(at));
  else if (fts_is_symbol(at))
    {
      int do_quote = 0;
      const char *src = fts_symbol_name(fts_get_symbol(at));
      char *dst = buf + 1;
      
      offset = 1;
      
      while(*src)
	{
	  if (IS_SEPARATOR(*src) || IS_DIGIT(*src) || IS_ATOM_CHAR(*src))
	    do_quote = 1;
	  else if (IS_BACKSLASH_QUOTE(*src) || IS_DOUBLE_QUOTE(*src))
	    {
	      do_quote = 1;
	      *(dst++) = '\\';
 	    }
	  
 	  *dst++ = *src++;
 	}
      
      if(do_quote)
 	{
	  /* set double quotesat beginning and end of string */
 	  buf[0] = '\"';
 	  offset = 0;
 	  *dst++ = '\"';	  
 	}
      
      /* set end of string */
      *dst++ = '\0';

      /* sprintf(buf, "%s", fts_symbol_name(fts_get_symbol(at))); */
    }
  else
    /* write void atom: symbol "()" */
    sprintf(buf, "()");

  sprintf(buf + strlen(buf), "%c", separator);

  write(f->fd, buf + offset, strlen(buf) - offset);

  return 1;
}
