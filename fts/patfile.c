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

#include <string.h>
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H 
#include <sys/types.h>
#endif
#include <sys/stat.h>
#include <fcntl.h>

#include <fts/fts.h>
#include <ftsprivate/object.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/patfile.h>


/* **********************************************************************
 *
 * Lexical analyser
 *
 */


/*
 * A lexical analyser for .pat files
 * It reconize int as separate then floats, and
 * it parse the ending ';' in TT_EOC (end of command). <br>
 * Also, it do backslash to quote transformation for message box , and backslash quoting suppression. <p>
 * It handle also variable substitution
 * for abstractions (in max, they are handled before the lexical
 * analisys: $0-foo is expanded to a single string). <p>
 *
 */

/* Token types */

#define FTS_LEX_NUMBER  0
#define FTS_LEX_FLOAT   1
#define FTS_LEX_SYMBOL  2
#define FTS_LEX_EOC     3
#define FTS_LEX_EOF  4


typedef struct fts_patlex {
  int ttype;
  fts_atom_t val;

  const fts_atom_t *env_argv;
  int env_argc;

  FILE *fd;
  int line_number;

  int pushedBack;
  int lookahead_valid;
  int lookahead;	

  int unique_var;

  char buf[512];
  int buf_fill;

  int messbox_mode;
} fts_patlex_t;


static int unique_count = 3333; /* the unique number generation */

static fts_status_description_t patfile_syntax_error_description = { ".pat file syntax error" };
static fts_status_description_t patfile_unknown_object_type_error_description = { "unknown object type error" };
static fts_status_t patfile_syntax_error = &patfile_syntax_error_description;
static fts_status_t patfile_unknown_object_type_error = &patfile_unknown_object_type_error_description;

static fts_patlex_t *fts_patlex_open(const char *filename, int env_argc, const fts_atom_t *env_argv)
{
  fts_patlex_t *this;
  FILE *file;

  file = fopen(filename, "rb");

  if (file == 0)
    return 0;

  this = (fts_patlex_t *) fts_malloc(sizeof(fts_patlex_t));
  this->fd = file;
  this->line_number = 0;
  this->env_argc = env_argc;
  this->env_argv = env_argv;
  this->unique_var = unique_count++; /* the unique number used in variable  0 substitution */
  this->pushedBack = 0;
  this->lookahead_valid = 0;
  this->buf_fill = 0;
  this->messbox_mode = 0;

  return this;
}

static void fts_patlex_close(fts_patlex_t *this)
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

#define fts_patlex_push_back(this) ((this)->pushedBack = 1)

/* Convenience macros: test of a values against a symbol */
#define token_sym_equals(in, sym)  (((in)->ttype == FTS_LEX_SYMBOL) && (fts_get_symbol(&((in)->val)) == (sym)))

/* states for the automata */

#define tt_waiting      0
#define tt_in_var       1
#define tt_in_string    2
#define tt_in_quoted_char  3
#define tt_in_number       5
#define tt_in_number_or_sign 6
#define tt_in_float    7

static void fts_patlex_next_token(fts_patlex_t *this)
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
		  if (c == '\n' || c == '\r')
		    this->line_number++;

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
				  strcpy(this->buf, fts_get_symbol(value));
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
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
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
		      fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
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
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
	       }
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
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
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_dollar(c))
		{
		  /* A dollar at the beginning of a word become
		     a single $ symbol */
		  this->buf[this->buf_fill++] = c;
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
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
		      fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
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
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  return;
		}
	      else if (fts_patlex_is_semi(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
		  this->ttype = FTS_LEX_SYMBOL;
		  this->lookahead = c;
		  this->lookahead_valid = 1;
		  return;
		}
	      else if (fts_patlex_is_blank(c))
		{
		  this->buf[this->buf_fill++] = '\0';
		  fts_set_symbol(&(this->val), fts_new_symbol(this->buf));
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



/* **********************************************************************
 *
 * Parser
 *
 */

/* Local symbols */

static fts_symbol_t patlex_sym_max;
static fts_symbol_t patlex_sym_v2;
static fts_symbol_t patlex_sym_diesN;
static fts_symbol_t patlex_sym_diesn;
static fts_symbol_t patlex_sym_diesP;
static fts_symbol_t patlex_sym_diesp;
static fts_symbol_t patlex_sym_diesT;
static fts_symbol_t patlex_sym_diest;
static fts_symbol_t patlex_sym_diesX;
static fts_symbol_t patlex_sym_diesx;
static fts_symbol_t patlex_sym_newex;
static fts_symbol_t patlex_sym_newobj;
static fts_symbol_t patlex_sym_vpatcher;
static fts_symbol_t patlex_sym_vtable;
static fts_symbol_t patlex_sym_slider;
static fts_symbol_t patlex_sym_flonum;
static fts_symbol_t patlex_sym_intbox;
static fts_symbol_t patlex_sym_floatbox;
static fts_symbol_t patlex_sym_messbox;
static fts_symbol_t patlex_sym_button;
static fts_symbol_t patlex_sym_toggle;
static fts_symbol_t patlex_sym_message;
static fts_symbol_t patlex_sym_table;
static fts_symbol_t patlex_sym_qlist;
static fts_symbol_t patlex_sym_explode;
static fts_symbol_t patlex_sym_connect;
static fts_symbol_t patlex_sym_comment;
static fts_symbol_t patlex_sym_pop;

/* Forward declarations */

/* auxiliary structures: store the graphic information for
 *  an object 
 */

typedef struct fts_graphic_description {
  fts_atom_t x;
  fts_atom_t y;
  fts_atom_t width;
  fts_atom_t range;
  fts_atom_t fontSize;
} fts_graphic_description_t;


#define  fts_patparse_set_messbox_mode(in) ((in)->messbox_mode = 1)
#define  fts_patparse_set_normal_mode(in) ((in)->messbox_mode = 0)

static void fts_patparse_graphic_description_init(fts_graphic_description_t *this);
static fts_status_t fts_patparse_parse_patcher(fts_object_t *parent, fts_patlex_t *in);
static fts_object_t *fts_get_child(fts_object_t *obj, int idx);
static void fts_patparse_parse_connection(fts_object_t *parent, fts_patlex_t *in);
static int fts_patparse_read_object_arguments(fts_atom_t *args, fts_patlex_t *in);
static fts_status_t fts_patparse_parse_object(fts_object_t *parent, fts_patlex_t *in,
				 fts_object_t *lastNObject, fts_symbol_t  lastNObjectType);
static fts_graphic_description_t *fts_patparse_parse_graphic_description(fts_patlex_t *in, fts_graphic_description_t *g);
static void fts_patparse_parse_window_properties(fts_object_t *parent, fts_patlex_t *in);
static void pat_warning(const char *description);

/*
 * Parse a .pat file.
 * A module providing  a parser for old Max .pat format files.
 */


static void
fts_patparse_graphic_description_init(fts_graphic_description_t *this)
{
  fts_set_int(&(this->x), 0);
  fts_set_int(&(this->y), 0);
  fts_set_int(&(this->width), 0);
  fts_set_int(&(this->range), 0);
  fts_set_int(&(this->fontSize), 10);
}

/* Called only if it make sense */

static void fts_patparse_set_range(fts_graphic_description_t *this, fts_patlex_t *in)
{
  fts_patlex_next_token(in);
  this->range = in->val;
}


#define SIZE_BITS 6
#define SIZE_RANGE (1 << 6)

/*
In the Max ISPW .pat format the font size is saved like an index (from 0 to 7) of a vector containing sizes from 9 to 24.
In the actual Max .pat format the Mac font Id and the font size are saved together like a long integer. There is only a 
little set of Mac Fonts with a fixed Id, the others Id are platform dependents and the conversion
Id -->font_name is possible only with a systen_call on Mac Systems. So for the moment we are using font size only.    
 */

static int fontIndexTable[8] = {9, 10, 12, 14, 16, 18, 20, 24};
#define FONTINDEXTABLE_SIZE (sizeof(fontIndexTable)/sizeof(int))

static void fts_patparse_set_font_index(fts_graphic_description_t *this, fts_patlex_t *in)
{
  int i;
  int font_id, font_size;
 
  fts_patlex_next_token(in);

  i = fts_get_int(&(in->val));

  if(i >= FONTINDEXTABLE_SIZE)
    {
      font_id = i >> SIZE_BITS;
      font_size = i & (SIZE_RANGE - 1);
      fts_set_int(&(this->fontSize), font_size);
    }
  else
    {
      if ( i < 0 )
	i = 2;
    
      fts_set_int(&(this->fontSize), fontIndexTable[i]);
    }
}

static void fts_patparse_set_text_graphic_properties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));

  if (fts_get_int(&(this->fontSize)) != 10)
    fts_object_put_prop(obj, fts_s_fontSize, &(this->fontSize));
}


static void fts_patparse_set_slider_graphic_properties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_atom_t zero;
  fts_atom_t max_value;
  fts_set_int(&zero, 0);

  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));
  fts_object_put_prop(obj, fts_s_height, &(this->range));
  fts_object_put_prop(obj, fts_s_min_value, &zero);

  fts_set_int(&max_value, fts_get_int(&(this->range)) - 1);
  fts_object_put_prop(obj, fts_s_max_value, &max_value);
}


static void fts_patparse_set_square_graphic_properties(fts_graphic_description_t *this, fts_object_t *obj)
{
  fts_object_put_prop(obj, fts_s_x, &(this->x));
  fts_object_put_prop(obj, fts_s_y, &(this->y));
  fts_object_put_prop(obj, fts_s_width, &(this->width));
  fts_object_put_prop(obj, fts_s_height, &(this->width));
}


/*
 * Method implementing the actual reading and parsing.
 *
 */
  
static fts_status_t fts_patparse_parse_patlex(fts_object_t *parent, fts_patlex_t *in)
{
  /* skip the header from the file, */

  fts_patlex_next_token(in);

  if (! token_sym_equals(in, patlex_sym_max))
    return fts_invalid_file_format_error;

  fts_patlex_next_token(in); 

  if (! token_sym_equals(in, patlex_sym_v2))
    return fts_invalid_file_format_error;

  /* Skip possible declarations */

  while (in->ttype != FTS_LEX_EOC)
    fts_patlex_next_token(in);

  fts_patlex_next_token(in); /* Skip the '#N' */

  /*
   * call the parser for a patcher, implemented
   * as a patcher constructor, with the stream pointing
   * to the "vpatcher" word.
   */

  return fts_patparse_parse_patcher(parent, in);
}


/*
 * Parse a patcher.
 *
 */

static fts_status_t fts_patparse_parse_patcher(fts_object_t *parent, fts_patlex_t *in) 
{
  fts_object_t *lastNObject = 0;
  fts_symbol_t  lastNObjectType = 0;

  /* We don't try to do many checks, for the moment */

  fts_patlex_next_token(in); /* Skip the 'vpatcher' */

  fts_patparse_parse_window_properties(parent, in); 

  /* skip the optional processor number */

  fts_patlex_next_token(in);

  if (in->ttype !=  FTS_LEX_EOC)
    fts_patlex_next_token(in);	/* skip ';' */

  while (in->ttype != FTS_LEX_EOF)
    {
      fts_patlex_next_token(in); 

      if (token_sym_equals(in, patlex_sym_diesN))
	{
	  fts_patlex_next_token(in);

	  if (token_sym_equals(in, patlex_sym_vpatcher))
	    {
	      fts_atom_t description[1];

	      fts_patlex_push_back(in);

	      fts_set_symbol(&description[0], fts_s_jpatcher);

	      lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

	      fts_patparse_parse_patcher(lastNObject, in);

	      lastNObjectType = fts_s_jpatcher;
	    }
	  else if (token_sym_equals(in, patlex_sym_qlist))
	    {
	      fts_atom_t description[1];

	      fts_set_symbol(&description[0], patlex_sym_qlist);
	      lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
	      lastNObjectType = patlex_sym_qlist;

	      /* skip the rest of the command: #N qlist argument are ignored */

	      while (in->ttype != FTS_LEX_EOC)
		fts_patlex_next_token(in);
	    }
	  else if (token_sym_equals(in, patlex_sym_vtable))
	    {
	      int haveName = 0;
	      int argc;
	      fts_atom_t vargs[10];
	      fts_atom_t description[10];

	      /* 
	       * Get the size and the name,
	       * ignore everything else (data
	       * that regard the editor)
	       */

	      argc = fts_patparse_read_object_arguments(vargs, in);
	      
	      /* get the name */

	      fts_set_symbol(&description[0], patlex_sym_table);

	      if (argc >= 8)
		{
		  description[1] = vargs[7];
		  haveName = 1;
		}


	      /* Make the table */

	      if (haveName)
		{
		  /* get the size */

		  description[2] = vargs[0];
		  lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 3, description);
		}
	      else
		{
		  /* get the size */

		  description[1] = vargs[0];
		  lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 2, description);
		}

	      lastNObjectType = patlex_sym_table;

	      /* skip the ';' */

	      fts_patlex_next_token(in);
	    }
	  else if (token_sym_equals(in, patlex_sym_explode))
	    {
	      fts_atom_t description[1];

	      fts_set_symbol(&description[0], patlex_sym_explode);

	      lastNObject = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
	      lastNObjectType = patlex_sym_explode;

	      /* skip the rest of the command */

	      while (in->ttype != FTS_LEX_EOC)
		fts_patlex_next_token(in);
	    }
	}
      else if (token_sym_equals(in, patlex_sym_diesP))
	{
	  /* get the line name */

	  fts_patlex_next_token(in); 

	  if (token_sym_equals(in, patlex_sym_connect))
	    {
	      fts_patparse_parse_connection(parent, in);
	    }
	  else if (token_sym_equals(in, patlex_sym_pop))
	    {
	      fts_patlex_next_token(in);	/* skip ';' */
	      
	      if  (in->ttype != FTS_LEX_EOC)
		{
		  /* the open patcher flag has been specified */

		  fts_patlex_next_token(in);	/* skip ';' */
		}
		    
	      return fts_ok;
	    }
	  else
	    {
	      /* Otherwise, we are parsing an object */
	      fts_patparse_parse_object(parent, in, lastNObject, lastNObjectType);
	      lastNObject = 0;
	      lastNObjectType = 0;
	    }

	  fts_patlex_next_token(in);/* skip ';' */
	}
      else if (token_sym_equals(in, patlex_sym_diesT))
	{
	  fts_patlex_next_token(in); /* get the command */

	  if (token_sym_equals(in, fts_s_set))
	    {
	      int argc;
	      fts_atom_t args[512];

	      argc = fts_patparse_read_object_arguments(args, in);

	      fts_send_message_varargs(lastNObject, fts_s_set, argc, args);

	      fts_patlex_next_token(in);/* skip ';' ?? */
	    }
	  else
	    return patfile_syntax_error;
	}
      else if (token_sym_equals(in, patlex_sym_diesX))
	{

	  if (lastNObjectType == patlex_sym_qlist)
	    {
	      int argc;
	      fts_atom_t args[512];
	      fts_symbol_t selector;

	      fts_patlex_next_token(in); /*  get the command */

	      selector = fts_get_symbol(&(in->val));

	      argc = fts_patparse_read_object_arguments(args, in);

	      fts_send_message_varargs(lastNObject, selector, argc, args);
	      
	      fts_patlex_next_token(in);/* skip ';' ?? */
	    }
	  else if (lastNObjectType == patlex_sym_explode)
	    {
	      fts_symbol_t selector;
	      int argc;
	      fts_atom_t args[512];

	      fts_patlex_next_token(in); /*  get the command */

	      if (in->ttype == FTS_LEX_SYMBOL)
		selector = fts_get_symbol(&(in->val));
	      else
		{
		  selector = fts_s_append; /* append command (without the keyword, sic. !!) */
		  fts_patlex_push_back(in);
		}

	      argc = fts_patparse_read_object_arguments(args, in);

	      fts_send_message_varargs(lastNObject, selector, argc, args);

	      fts_patlex_next_token(in); /* skip ';' ?? */
	    }
	  else
	    {
	      pat_warning("Syntax error: #X in a .pat file, after something different from qlist or table");

	      while ((in->ttype != FTS_LEX_EOC) && (in->ttype != FTS_LEX_EOF))
		fts_patlex_next_token(in);/* skip ';' */
	    }
	}
      else
	{
	  if ( in->ttype != FTS_LEX_SYMBOL)
	    fts_post( "[%d] Expecting symbol, got %d\n", in->line_number, in->ttype);
	  else
	    fts_post("[%d] Format not supported (%s)\n", in->line_number, fts_get_symbol( &in->val));
	  
	  /* skip until the next ';' */

	  while ((in->ttype != FTS_LEX_EOC) && (in->ttype != FTS_LEX_EOF))
	    fts_patlex_next_token(in);/* skip ';' */
	}
    }

  return fts_ok;
}


/*
 * Load an abstraction, substituing the arguments.
 * 
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found in the file,
 * or the file contains an unimplemented construct.
 */


/*
 * Parse a connection. 
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */

static fts_object_t *
fts_get_child(fts_object_t *obj, int idx)
{
  fts_patcher_t *patcher = (fts_patcher_t *) obj;
  fts_object_t *p;
  
  idx = fts_patcher_get_objects_count( (fts_patcher_t *)obj ) - idx - 1;

  for (p = patcher->objects; p ; p = fts_object_get_next_in_patcher(p))
    {
      if (idx == 0)
	return p;
      else
	idx--;
    }

  return 0;
}

static void fts_patparse_parse_connection(fts_object_t *parent, fts_patlex_t *in)
{
  int fromIdx;
  int toIdx;
  int inlet;
  int outlet;
  fts_object_t *from;
  fts_object_t *to;

  fts_patlex_next_token(in); 

  fromIdx = fts_get_int(&(in->val));

  fts_patlex_next_token(in); 
  outlet = fts_get_int(&(in->val));

  fts_patlex_next_token(in);
  toIdx = fts_get_int(&(in->val));

  fts_patlex_next_token(in); 
  inlet  = fts_get_int(&(in->val));

  from = fts_get_child(parent, fromIdx);
  to   = fts_get_child(parent, toIdx);

  fts_connection_new(from, outlet, to, inlet, fts_c_anything);
}


/*
 * Read object description arguments in a vect.
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */


/* Arrivato fino a qui  */

static int fts_patparse_read_object_arguments(fts_atom_t *args, fts_patlex_t *in) 
{
  int count = 0;

  fts_patlex_next_token(in);

  while (in->ttype != FTS_LEX_EOC)
    {
      args[count++] = in->val;

      fts_patlex_next_token(in);
    }

  fts_patlex_push_back(in);

  return count;
}


/*
 * Parse an object from the a tokenized stream.
 *
 */


static fts_status_t fts_patparse_parse_object(fts_object_t *parent, fts_patlex_t *in,
				 fts_object_t *lastNObject, fts_symbol_t lastNObjectType)
{
  fts_object_t *obj;
  fts_symbol_t objclass = fts_get_symbol(&(in->val));
  fts_graphic_description_t gd;
  fts_graphic_description_t *graphicDescr = &gd;

  /* get the graphic information */

  fts_patparse_parse_graphic_description(in, graphicDescr);

  if (objclass == patlex_sym_slider)
    {
      fts_atom_t description[1];

      fts_patparse_set_range(graphicDescr, in);

      fts_set_symbol(&description[0], patlex_sym_slider);

      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
      fts_patparse_set_slider_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_newex)
    {
      fts_atom_t description[512];
      int argc;

      fts_patparse_set_font_index(graphicDescr, in);

      fts_patlex_next_token(in);/* get the object name */

      /* Abstraction are handled directly by the fts_eval_object_description function */

      if (in->ttype == FTS_LEX_EOC)
	{
	  /*
	   * Empty object, built and return an
	   * object with an empty description , that will be an error object.
	   */

	  fts_patlex_push_back(in);

	  obj = fts_eval_object_description((fts_patcher_t *) parent, 0, 0);

	  fts_patparse_set_text_graphic_properties(graphicDescr, obj);
	}

      if (in->ttype == FTS_LEX_SYMBOL)
	description[0] = in->val;
      else if (in->ttype == FTS_LEX_NUMBER)
	{
	  fts_set_symbol(&description[0], fts_s_int);
	  fts_patlex_push_back(in);
	}
      else if (in->ttype == FTS_LEX_FLOAT)
	{
	  fts_set_symbol(&description[0], fts_s_float);
	  fts_patlex_push_back(in);
	}

      argc = fts_patparse_read_object_arguments(description + 1, in);

      obj = fts_eval_object_description((fts_patcher_t *)parent, argc + 1 , description);

      if (obj)
	fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_newobj)
    {
      /* Special handling for patchers, qlist, explode and table */

      int argc;
      fts_atom_t args[512];

      fts_patparse_set_font_index(graphicDescr,in);

      argc = fts_patparse_read_object_arguments(args, in);

      /* Check if it is the patcher, otherwise just skip it */

      if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == fts_s_old_patcher ) &&
	  (lastNObjectType == fts_s_jpatcher))
	{
	  /* add the two ninlet and noutlet and the arguments to description */

	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);

	  if (argc > 1)
	    {
	      fts_atom_t new_args[512];
	      int i;

	      fts_patcher_order_inoutlets_regarding_position((fts_patcher_t *)lastNObject);

	      fts_set_symbol(&new_args[0], fts_s_jpatcher);

	      for (i = 1; (i < argc) && (i < 512); i++)
		new_args[i] = args[i];

	      /*fts_patcher_redefine((fts_patcher_t *)lastNObject, argc, new_args);*/
	    }
	  else
	    fts_patcher_order_inoutlets_regarding_position((fts_patcher_t *)lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == patlex_sym_qlist) &&
	       (lastNObjectType == patlex_sym_qlist))
	{
	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == patlex_sym_table) &&
	       (lastNObjectType == patlex_sym_table))
	{
	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);
	}
      else if (fts_is_symbol(&args[0]) && (fts_get_symbol(&args[0]) == patlex_sym_explode) &&
	       (lastNObjectType == patlex_sym_explode))
	{
	  /* first, look if we have a name there */
	  if (argc >= 2)
	    fts_object_put_prop(lastNObject, fts_s_name, &args[1]);

	  fts_patparse_set_text_graphic_properties(graphicDescr, lastNObject);
	}
      else
	{
	  if (fts_is_symbol(&args[0]))
	    fts_post("Object %s not yet Supported in .pat files",
		 fts_get_symbol(&(args[0])));
	}
    }
  else if (objclass == fts_s_inlet)
    {
      fts_atom_t description[2];

      /* skip optional dsp flag  */
      fts_patlex_next_token(in);

      if (in->ttype == FTS_LEX_EOC)
	fts_patlex_push_back(in);

      /* Create a non assigned inlet */


      fts_set_symbol(&description[0], fts_s_inlet);
      fts_set_int(&description[1], 0); 
      obj = fts_eval_object_description((fts_patcher_t *)parent, 2, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_outlet)
    {
      fts_atom_t description[2];

      /* skip optional dsp flag  */

      fts_patlex_next_token(in);

      if (in->ttype == FTS_LEX_EOC)
	fts_patlex_push_back(in);

      /* Create a non assigned outlet */

      fts_set_symbol(&description[0], fts_s_outlet);
      fts_set_int(&description[1], 0); 
      obj = fts_eval_object_description((fts_patcher_t *)parent, 2, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == fts_s_number)
    {
      fts_atom_t description[1];

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], patlex_sym_intbox);
      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_flonum)
    {
      fts_atom_t description[1];

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], patlex_sym_floatbox);
      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_button)
    {
      fts_atom_t description[1];

      fts_set_symbol(&description[0], patlex_sym_button);
      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_toggle)
    {
      fts_atom_t description[1];

      fts_set_symbol(&description[0], patlex_sym_toggle);

      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

      fts_patparse_set_square_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_message)
    {
      fts_atom_t description[512];
      int argc;

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], patlex_sym_messbox);

      fts_patparse_set_messbox_mode(in);
      argc = fts_patparse_read_object_arguments(description + 1, in);
      fts_patparse_set_normal_mode(in);

      obj = fts_eval_object_description((fts_patcher_t *)parent, 1, description);
      fts_send_message_varargs(obj, fts_s_clear, 0, 0);
      fts_send_message_varargs(obj, fts_s_append, argc, description + 1);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else if (objclass == patlex_sym_comment)
    {
      fts_atom_t description[512];
      int argc;

      fts_patparse_set_font_index(graphicDescr, in);

      fts_set_symbol(&description[0], patlex_sym_comment);

      argc = fts_patparse_read_object_arguments(description + 1, in);

      obj = fts_eval_object_description((fts_patcher_t *)parent, argc + 1, description);

      fts_patparse_set_text_graphic_properties(graphicDescr, obj);
    }
  else
    return patfile_unknown_object_type_error;

  return fts_ok;
}

	    
/*
 * Parse a fts Graphic description.
 *
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */




static fts_graphic_description_t *fts_patparse_parse_graphic_description(fts_patlex_t *in, fts_graphic_description_t *g)
{
  fts_patparse_graphic_description_init(g);

  fts_patlex_next_token(in);

  if (in->ttype == FTS_LEX_NUMBER)
    fts_set_int(&(g->x), fts_get_int(&(in->val)) );
  else
    {
      fts_patlex_push_back(in);
      return g;
    }

  fts_patlex_next_token(in);

  if (in->ttype == FTS_LEX_NUMBER)
    fts_set_int(&(g->y), fts_get_int(&(in->val)) );
  else
    {
      fts_patlex_push_back(in);
      return g;
    }

  fts_patlex_next_token(in);

  if (in->ttype == FTS_LEX_NUMBER)
    fts_set_int(&(g->width), fts_get_int(&(in->val)) );
  else
    {
      fts_patlex_push_back(in);
      return g;
    }

  return g;
}


/*
 * Parse a window description.
 * @exception ircam.jmax.fts.FtsException thown if a syntax error is found,
 * or the file contains an unimplemented construct.
 */

static void fts_patparse_parse_window_properties(fts_object_t *parent, fts_patlex_t *in)
{
  int x_left, y_top, x_right, y_bottom;
  fts_atom_t x, y, height, width;

  fts_patlex_next_token(in);
  x_left = fts_get_int(&(in->val));
    
  fts_patlex_next_token(in);
  y_top = fts_get_int(&(in->val));

  fts_patlex_next_token(in);
  x_right = fts_get_int(&(in->val));

  fts_patlex_next_token(in);
  y_bottom = fts_get_int(&(in->val));

  /* If patcher window has big negative coordinates */
  if (x_left < -10000 || y_top < -10000)
    {
      fts_atom_t a;

      fts_set_int( &a, 1);
      fts_object_put_prop( parent, fts_new_symbol( "no_upload"), &a);

      x_left = -x_left;
      y_top = -y_top;

      return;
    }

  fts_set_int( &x, x_left);
  fts_object_put_prop(parent, fts_s_wx, &x);

  fts_set_int( &y, y_top);
  fts_object_put_prop(parent, fts_s_wy, &y);

  fts_set_int( &width, (x_right - x_left) );
  fts_object_put_prop(parent, fts_s_ww, &width);

  fts_set_int( &height, (y_bottom - y_top) );
  fts_object_put_prop(parent, fts_s_wh, &height);
}

static void pat_warning(const char *description) 
{
  fts_post("Warning loading .pat file: %s\n", description);
}


/***********************************************************************
 *
 * Public functions
 *
 */

/*
 * Create a new patcher from a .pat file.
 * The patcher is always a top level patcher.
 */
    
fts_status_t fts_pat_file_load( fts_symbol_t filename, fts_object_t *parent, int ac, const fts_atom_t *at, fts_object_t **ret) 
{
  fts_patlex_t *in; 
  fts_atom_t description[1];
  fts_object_t *patcher;

  in = fts_patlex_open(filename, 0, 0);

  if (in == 0)
    return fts_cannot_open_file_error;

  fts_set_symbol(&description[0], fts_s_jpatcher);
  patcher = fts_eval_object_description((fts_patcher_t *)parent, 1, description);

  fts_patparse_parse_patlex(patcher, in);
  fts_patlex_close(in);
  fts_patcher_order_inoutlets_regarding_position((fts_patcher_t *) patcher);

  *ret = patcher;

  return fts_ok;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_patfile_init()
{
  patlex_sym_max = fts_new_symbol("max");
  patlex_sym_v2 = fts_new_symbol("v2");
  patlex_sym_diesN = fts_new_symbol("#N");
  patlex_sym_diesn = fts_new_symbol("#n");
  patlex_sym_diesP = fts_new_symbol("#P");
  patlex_sym_diesp = fts_new_symbol("#p");
  patlex_sym_diesT = fts_new_symbol("#T");
  patlex_sym_diest = fts_new_symbol("#t");
  patlex_sym_diesX = fts_new_symbol("#X");
  patlex_sym_diesx = fts_new_symbol("#x");
  patlex_sym_newex = fts_new_symbol("newex");
  patlex_sym_newobj = fts_new_symbol("newobj");
  patlex_sym_vpatcher = fts_new_symbol("vpatcher");
  patlex_sym_vtable = fts_new_symbol("vtable");
  patlex_sym_slider = fts_new_symbol("slider");
  patlex_sym_flonum = fts_new_symbol("flonum");
  patlex_sym_intbox = fts_new_symbol("intbox");
  patlex_sym_floatbox = fts_new_symbol("floatbox");
  patlex_sym_messbox = fts_new_symbol("messbox");
  patlex_sym_button = fts_new_symbol("button");
  patlex_sym_toggle = fts_new_symbol("toggle");
  patlex_sym_message = fts_new_symbol("message");
  patlex_sym_table = fts_new_symbol("table");
  patlex_sym_qlist = fts_new_symbol("qlist");
  patlex_sym_explode = fts_new_symbol("explode");
  patlex_sym_connect = fts_new_symbol("connect");
  patlex_sym_comment = fts_new_symbol("comment");
  patlex_sym_pop = fts_new_symbol("pop");
}
