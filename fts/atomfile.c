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
 */
#include <fts/fts.h>

#include <string.h>

#include "ftsconfig.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_IO_H
#include <io.h>
#endif

#define ATOM_FILE_BUF_SIZE 512

struct _fts_atom_file_t_
{
  FILE* fd;
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
#define IS_DIGIT(c) (('0' <= (c)) && ((c) <= '9'))
#define IS_SIGN(c) (((c) == '+') || ((c) == '-'))
#define IS_POINT(c) ('.' == (c))
#define IS_BACKSLASH_QUOTE(c) ('\\' == (c))
#define IS_DOUBLE_QUOTE(c) ('\"' == (c))
#define IS_ATOM_CHAR(c) (((c) == ';') || ((c) == ',') || ((c) == '$') || ((c) == '\''))
#define IS_EOF(c) (0xff == (c))

static void
fts_atom_file_read_more(fts_atom_file_t *f)
{
  f->count = fread(f->buf, 1, ATOM_FILE_BUF_SIZE, f->fd);
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
	int l;
	
	sscanf(buf, "%d", &l);
	fts_set_int(at, l);
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
    default:
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
  int i;

  if (!IS_SEPARATOR(separator))
    separator = ' ';

  if (fts_is_int(at))
    sprintf(buf, "%d%c", fts_get_int(at), separator);
  else if (fts_is_float(at))
    sprintf(buf, "%#f%c", fts_get_float(at), separator);
  else if (fts_is_symbol(at))
    {
      const char *sym_str = fts_symbol_name(fts_get_symbol(at));

      if(strlen(sym_str) == 1 && IS_ATOM_CHAR(sym_str[0]))
	{
	  buf[0] = sym_str[0];
	  buf[1] = separator;
	  buf[2] = '\0';
	  offset = 0;
	}
      else
	{
	  int needs_double_quotes = 0;
	  char *buf_ptr = buf;
	  
	  *buf_ptr++ = '\"';
	  offset = 1;

	  while(*sym_str != '\0')
	    {
	      if (IS_SEPARATOR(*sym_str) || IS_DIGIT(*sym_str) || IS_ATOM_CHAR(*sym_str))
		needs_double_quotes = 1;
	      else if (IS_BACKSLASH_QUOTE(*sym_str) || IS_DOUBLE_QUOTE(*sym_str))
		{
		  needs_double_quotes = 1;
		  *(buf_ptr++) = '\\';
		}

	      *buf_ptr++ = *sym_str++;
	    }

	  if(needs_double_quotes)
	    {
	      /* set double quotes at beginning and end of string */
	      offset = 0;
	      *buf_ptr++ = '\"';
	    }

	  /* concatenate speparator */
	  *buf_ptr++ = separator;

	  /* set end of string */
	  *buf_ptr++ = '\0';
	}
    }
  else
    /* write void atom: symbol "()" */
    sprintf(buf, "()%c", separator);

  fwrite(buf + offset, 1, strlen(buf) - offset, f->fd);

  return 1;
}
