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
 *
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>

/***********************************************************************
 *
 * Functions that print in memory, varargs and va_list versions
 * The buffer that is passed as an argument is reallocated if necessary
 * using fts_realloc()
 *
 * Note: *pp can be NULL, but if != NULL, then *psize must be the
 * true size of memory pointed by *pp.
 *
 */

/* Forward declaration */
static int mempost_atoms( char **pp, int *psize, int offset, int ac, const fts_atom_t *at);

static int vmempost( char **pp, int *psize, int offset, const char *format, va_list ap)
{
  int n;

  if ( *pp == NULL)
    {
      *psize = 2048;
      *pp = fts_malloc( *psize);
    }
  else assert (*psize != 0);

  /* from man vsnprintf */
  while (1) 
    {
      /* Try to print in the allocated space. */
      n = vsnprintf( *pp+offset, *psize-offset, format, ap);

      /* If that worked, exit the loop. */
      if (n > -1 && n < *psize-offset)
	break;

      /* Else try again with more space. */
      if (n > -1)    /* glibc 2.1 */
	*psize = offset + n + 1; /* precisely what is needed */
      else           /* glibc 2.0 */
	*psize *= 2;  /* twice the old size */

      if ((*pp = fts_realloc( *pp, *psize)) == NULL)
	return -1;
    }

  return n;
}

static int mempost( char **pp, int *psize, int offset, const char *format, ...)
{
  va_list ap;
  int n;

  va_start( ap, format);
  n = vmempost( pp, psize, offset, format, ap);
  va_end( ap);

  return n;
}

static int mempost_float( char **pp, int *psize, int offset, double f)
{
  if(f == 0.0)
     return mempost( pp, psize, offset, "0.");
  else
    {
      double intr, frac;
      int n = 0;
      
      if(f < 0.0)
	{
	  f = -f;
	  mempost( pp, psize, offset, "-");
	  n++;
	}

      frac = modf(f, &intr);

      if(frac == 0.0)
	{
	  if(f < 10000000.0)
	    n += mempost( pp, psize, offset + n, "%.7g.", f);
	  else
	    n += mempost( pp, psize, offset + n, "%.7g", f);	    
	}
      else
	{
	  if(f >= 0.0001)
	    n += mempost( pp, psize, offset + n, "%.7g", f);
	  else if(f >= 0.0000001)
	    n += mempost( pp, psize, offset + n, "%.7f", f);
	  else
	    n += mempost( pp, psize, offset + n, "%.6e", f);
	}

      return n;
    }
}

static int mempost_symbol( char **pp, int *psize, int offset, fts_symbol_t s)
{
  if ( strchr( s, ' ') != NULL)
    return mempost( pp, psize, offset, "\"%s\"", s);

  return mempost( pp, psize, offset, "%s", s);
}

static int 
mempost_object( char **pp, int *psize, int offset, fts_object_t *obj)
{
  int n = 0;

  if(obj)
    {
      int ac = fts_object_get_description_size( obj);
      const fts_atom_t *at = fts_object_get_description_atoms( obj);

      if(ac > 0)
	{
	  if(fts_is_symbol(at + 1) && fts_get_symbol(at + 1) == fts_s_colon)
	    {
	      ac -= 2;
	      at += 2;
	    }
	  
	  n = mempost( pp, psize, offset, "(:");
	  n += mempost_atoms( pp, psize, offset + n, ac, at);
	  n += mempost( pp, psize, offset + n, ")");
	}
      else if(fts_object_get_class_name(obj) != NULL)
	n = mempost( pp, psize, offset, "(:%s)", fts_object_get_class_name(obj));
      else
	n = mempost( pp, psize, offset, "(:\?\?\?)");
    }
  else
    n = mempost( pp, psize, offset, "<null object>");
  
  return n;
}

static int mempost_tuple( char **pp, int *psize, int offset, fts_tuple_t *tup)
{
  int ac = fts_tuple_get_size(tup);
  const fts_atom_t *at = fts_tuple_get_atoms(tup);
  int n = 0;
  
  n += mempost( pp, psize, offset+n, "(");  
  n += mempost_atoms( pp, psize, offset + n, ac, at);
  n += mempost( pp, psize, offset + n, ")");

  return n;
}

static int 
mempost_atoms( char **pp, int *psize, int offset, int ac, const fts_atom_t *at)
{
  int i, n = 0;

  for ( i = 0; i < ac; i++, at++)
    {
      if ( fts_is_void( at))
	n += mempost( pp, psize, offset+n, "<void>");
      else if ( fts_is_int( at))
	n += mempost( pp, psize, offset+n, "%d", fts_get_int( at));
      else if ( fts_is_float( at))
	n += mempost_float( pp, psize, offset+n, fts_get_float( at));
      else if ( fts_is_symbol( at))
	n += mempost_symbol( pp, psize, offset+n, fts_get_symbol( at));
      else if ( fts_is_tuple( at))
	n += mempost_tuple( pp, psize, offset+n, (fts_tuple_t *)fts_get_object( at));
      else if ( fts_is_object( at))
	n += mempost_object( pp, psize, offset+n, fts_get_object( at));
      else if ( fts_is_pointer( at) )
	n += mempost( pp, psize, offset+n, "%p", fts_get_pointer( at));
      else if ( fts_is_string( at))
	n += mempost( pp, psize, offset+n, "%s", fts_get_string( at));
      else
	n += mempost( pp, psize, offset+n, "<unknown 0x%x>", fts_get_int( at));

      if ( i != ac-1)
	n += mempost( pp, psize, offset+n, " ");
    }

  return n;
}

/***********************************************************************
 * 
 * Functions that print on a bytestream
 *
 */

static char *post_buffer = NULL;
static int post_buffer_size;

static int 
check_symbol_in( fts_symbol_t s, fts_symbol_t *symbols)
{
  while (*symbols)
    {
      if ( *symbols == s)
	return 1;

      symbols++;
    }

  return 0;
}

static fts_symbol_t want_a_space_before_symbols[] = {"+", "-", "*", "/", "%", "&&", "&", "||", "|", "==", "=", "!=", "!", ">=", "^", ">>", ">", "<<", "<=", "<", "?", 0 };
static fts_symbol_t dont_want_a_space_before_symbols[] = {")", "[", "]", "}", ",", ";", ".", "=", ":", 0};
static fts_symbol_t want_a_space_after_symbols[] = { "+", "-", "*", "/", "%", ",", "&&", "&", "||", "|", "==", "=", "!=", "!", ">=", ">>", ">", "<<", "<=", "<", "?", "^", ";", 0 };
static fts_symbol_t dont_want_a_space_after_symbols[] = { "(", "[", "{", "$", "'", "." , "=", ":", 0 };
static fts_symbol_t operators[] = { "$", ";", ",", "+", "-", "*", "/", "(", ")", "[", "]", "{", "}", ".", "%", "<<", ">>", "&&", "||", "!", "==", "!=", ">", ">=", "<", "<=", ":", "=", 0};

static void init_punctuation( void)
{
  int i, j;
  fts_symbol_t *p, *tab[] = { want_a_space_before_symbols, dont_want_a_space_before_symbols, want_a_space_after_symbols, dont_want_a_space_after_symbols, operators};

  for ( i = 0; i < sizeof( tab)/sizeof( fts_symbol_t *); i++)
    for ( j = 0, p = tab[i]; *p; p++)
      *p = fts_new_symbol( *p);
}

#define want_a_space_before(v) (fts_is_symbol(v)? check_symbol_in( fts_get_symbol(v), want_a_space_before_symbols): 0)
#define dont_want_a_space_before(v) (fts_is_symbol(v)? check_symbol_in( fts_get_symbol(v), dont_want_a_space_before_symbols): 0)
#define want_a_space_after(v) (fts_is_symbol(v)? check_symbol_in( fts_get_symbol(v), want_a_space_after_symbols): 0)
#define dont_want_a_space_after(v) (fts_is_symbol(v)? check_symbol_in( fts_get_symbol(v), dont_want_a_space_after_symbols): 0)

static int 
needs_quote( fts_symbol_t s)
{
  if (check_symbol_in( s, operators))
    return 0;

  while( *s != '\0')
    {	    
      switch (*s) {
      case ' ':
      case '\n':
      case '\t':
      case '$':
      case ',':
      case '(':
      case ')':
      case '[':
      case ']':
      case '{':
      case '}':
      case ':':
      case ';':
      case '=':
      case '\'':
      case '.':
	return 1;
      }

      s++;
    }

  return 0;
}

void fts_vspost( fts_bytestream_t *stream, const char *format, va_list ap)
{
  int n;

  n = vmempost( &post_buffer, &post_buffer_size, 0, format, ap);

  fts_bytestream_output( stream, n, post_buffer);
  fts_bytestream_flush( stream);
}  

void fts_spost( fts_bytestream_t *stream, const char *format, ...)
{
  va_list ap;

  va_start( ap, format);
  fts_vspost( stream, format, ap);
  va_end( ap);
}

void fts_spost_float( fts_bytestream_t *stream, double f)
{
  int n = mempost_float( &post_buffer, &post_buffer_size, 0, f);

  fts_bytestream_output( stream, n, post_buffer);
  fts_bytestream_flush( stream);
}

void fts_spost_symbol( fts_bytestream_t *stream, fts_symbol_t s)
{
  if( needs_quote(s))
    fts_spost( stream, "\"%s\"", s);
  else
    fts_spost( stream, "%s", s);
}

void 
fts_spost_complex(fts_bytestream_t *stream, double re, double im)
{
  int n = mempost_float( &post_buffer, &post_buffer_size, 0, re);

  if(im > 0.0)
    {
      n += mempost( &post_buffer, &post_buffer_size, n, "+j");
      n += mempost_float( &post_buffer, &post_buffer_size, n, im);
    }
  else if(im < 0.0)
    {
      n += mempost( &post_buffer, &post_buffer_size, n, "-j");
      n += mempost_float( &post_buffer, &post_buffer_size, n, -im);
    }

  fts_bytestream_output( stream, n, post_buffer);
  fts_bytestream_flush( stream);
}

void 
fts_spost_object(fts_bytestream_t *stream, fts_object_t *obj)
{
  int n = 0;

  if(obj)
    {
      fts_method_t meth_post = fts_class_get_method_varargs(fts_object_get_class(obj), fts_s_post);

      if(meth_post)
	{
	  fts_atom_t a;

	  fts_set_object(&a, stream);
	  meth_post(obj, fts_system_inlet, fts_s_post, 1, &a);
	}
      else
	{
	  int ac = fts_object_get_description_size( obj);
	  const fts_atom_t *at = fts_object_get_description_atoms( obj);
	  
	  if(ac > 0)
	    {
	      n += mempost( &post_buffer, &post_buffer_size, 0, "(");
	      n += mempost_atoms( &post_buffer, &post_buffer_size, n, ac, at);
	      n += mempost( &post_buffer, &post_buffer_size, n, ")");
	    }
	  else if(fts_object_get_class_name(obj) != NULL)
	    n += mempost( &post_buffer, &post_buffer_size, 0, "(:%s)", fts_object_get_class_name(obj));
	  else
	    n += mempost( &post_buffer, &post_buffer_size, 0, "(:\?\?\?)");
	}
    }
  else
    n =  mempost( &post_buffer, &post_buffer_size, 0, "<null object>");

  fts_bytestream_output( stream, n, post_buffer);
  fts_bytestream_flush( stream);
}

void 
fts_spost_atoms( fts_bytestream_t *stream, int ac, const fts_atom_t *at)
{
  int i;

  for ( i = 0; i < ac; i++, at++)
    {
      if ( fts_is_void( at))
	fts_spost(stream, "<void>");
      else if ( fts_is_int( at))
	fts_spost(stream, "%d", fts_get_int( at));
      else if ( fts_is_float( at))
	fts_spost_float(stream, fts_get_float( at));
      else if ( fts_is_symbol( at))
	fts_spost_symbol(stream, fts_get_symbol( at));
      else if ( fts_is_object( at))
	fts_spost_object(stream, fts_get_object( at));
      else if ( fts_is_pointer( at) )
	fts_spost(stream, "%p", fts_get_pointer( at));
      else if ( fts_is_string( at))
	fts_spost(stream, "\'%p\'", fts_get_string( at));
      else
	fts_spost(stream, "<unknown 0x%x>", fts_get_int( at));

      if ( i != ac-1)	
	fts_spost(stream, " ");
    }
}

void 
fts_spost_object_description( fts_bytestream_t *stream, fts_object_t *obj)
{
  int ac;
  fts_atom_t *at;

  ac = fts_object_get_description_size( obj);
  at = fts_object_get_description_atoms( obj);

  /* skip the leading : only if it is the first atom */
  if ( ac > 0 && fts_is_symbol( at) && fts_get_symbol( at) == fts_s_colon) 
    {
      ac--;
      at++;
    }

  fts_spost_object_description_args( stream, ac, at);
}

void 
fts_spost_object_description_args( fts_bytestream_t *stream, int ac, fts_atom_t *at)
{
  int do_new_line = 0;
  int add_blank = 0;
  fts_atom_t *value1;
  fts_atom_t *value2;
  int i;

  if (ac <= 0)
    return;
  
  value2 = at;
  value1 = value2;
  i = 1;

  while (value1)
    {
      if (do_new_line)
	fts_spost( stream, "\n"); 
      else if ( add_blank)
	fts_spost( stream, " ");

      do_new_line = 0;

      if (i >= ac)
	value2 = 0;
      else
	value2 = at + i;

      if ( fts_is_int( value1))
	fts_spost( stream, "%d", fts_get_int( value1));
      else if ( fts_is_float( value1))
	fts_spost_float( stream, fts_get_float( value1));
      else if ( fts_is_symbol( value1))
	fts_spost_symbol( stream, fts_get_symbol( value1));
      else 
	fts_spost( stream, "\?\?\?");

      /* decide to put or not a blank between the two */
      if (want_a_space_after( value1))
	add_blank = 1;
      else if (dont_want_a_space_after( value1))
	add_blank = 0;
      else if (value2)
	{
	  if (want_a_space_before( value2))
	    add_blank = 1;
	  else if (dont_want_a_space_before( value2))
	    add_blank = 0;
	  else
	    add_blank = 1;	// if no body care, do a blank
	}

      value1 = value2;
      i++;
    }

  fts_bytestream_flush( stream);
}

/***********************************************************************
 *
 * Compatibility
 *
 */

static fts_stack_t *post_stack = NULL;

fts_bytestream_t *
fts_post_get_stream( int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);

      if(fts_bytestream_check(obj))
	return (fts_bytestream_t *)obj;
    }

  return fts_get_default_console_stream();
}

static void post_output_chars( char *buffer, int n)
{
  if (fts_get_default_console_stream())
    {
      fts_bytestream_output( fts_get_default_console_stream(), n, post_buffer);
      fts_bytestream_flush( fts_get_default_console_stream());
    }
  else
    /* May be this is to be done in all cases */
    {
      int i;

      if (post_stack == NULL)
	{
	  post_stack = (fts_stack_t *)fts_malloc( sizeof( fts_stack_t));
	  fts_stack_init( post_stack, char);
	}

      for ( i = 0; i < n; i++)
	fts_stack_push( post_stack, char, post_buffer[i]);
    }    
}

void post( const char *format, ...)
{
  va_list ap;
  int n;

  va_start( ap, format);
  n = vmempost( &post_buffer, &post_buffer_size, 0, format, ap);
  va_end( ap);

  post_output_chars( post_buffer, n);
}

void post_atoms( int ac, const fts_atom_t *at)
{
  int n;

  n = mempost_atoms( &post_buffer, &post_buffer_size, 0, ac, at);

  post_output_chars( post_buffer, n);
}

/***********************************************************************
 *
 * log functions
 * (equivalent of post functions, but printing on a log file)
 *
 */

/*
 * it may seem strange that in the following functions,
 * we reopen the file at each call, but it is ***not***.
 * this way, you are guaranteed that the file's content
 * is updated after each log, even in case of fts crash !!!
 *
 * question: ok for windows, but is this really necessary on linux ? 
 * Anyway, it does not hurt...
 */

static double log_time = 0.0;
static const char *log_file_name = NULL;
static char *log_buffer = NULL;
static int log_buffer_size;

static void log_date( FILE *log)
{
#ifdef win32
  char t[9];
  char d[9];

  _strdate(d);
  _strtime(t);
  fprintf( log, "%s %s", d, t);
#else
  time_t t;

  time( &t);
  fprintf( log, "%s", ctime( &t));
#endif
}

static void log_init( void)
{
  FILE *log;
  char buf[1024];

#ifdef WIN32
  log_file_name = "c:\\fts_log.txt";
#else
  if (getenv("HOME"))
    {
      snprintf( buf, sizeof( buf), "%s/.fts_log", getenv("HOME"));
      log_file_name = strdup( buf);
    }
  else
    log_file_name = "/tmp/fts_log";
#endif

  /* truncate the file */
  log = fopen( log_file_name, "w");

  if (log != NULL)
    {
      fprintf( log, "[log]: started logging on ");
      log_date( log);
      fprintf( log, "\n");

      fclose(log);
    }
}

void fts_log( char *format, ...)
{
  va_list ap; 
  FILE *log;

  if (log_file_name == NULL)
    {
      log_init();
      log_time = fts_systime();
    }

  log = fopen( log_file_name, "a");
  if (log == NULL)
    return;

  if(format[0] == '\n' && format[1] == '\0')
    fprintf( log, "\n");
  else
    {
      fprintf( log, "[%u]", (unsigned int) (fts_systime() - log_time));

      va_start( ap, format); 
      vfprintf( log, format, ap);
      va_end( ap);
    }

  fflush( log);
  fclose( log);
}

void fts_log_atoms( int ac, const fts_atom_t *at)
{
  FILE *log;
  int n;

  if (log_file_name == NULL)
    {
      log_init();
      log_time = fts_systime();
    }

  log = fopen( log_file_name, "a");
  if (log == NULL)
    return;

  n = mempost_atoms( &log_buffer, &log_buffer_size, 0, ac, at);

  fwrite( log_buffer, n, 1, log);

  fflush(log);
  fclose( log);
}

/***********************************************************************
 *
 * Default console
 * 
 * Installed by client
 *
 */

static fts_bytestream_t *default_console_stream;

fts_bytestream_t *fts_get_default_console_stream( void)
{
  return default_console_stream;
}

void fts_set_default_console_stream( fts_bytestream_t *stream)
{
  if (default_console_stream)
    fts_object_release( (fts_object_t *)default_console_stream);

  default_console_stream = stream;

  if (default_console_stream)
    fts_object_refer( (fts_object_t *)stream);

  /* Post accumulated messages */
  if (post_stack != NULL)
    {
      fts_bytestream_output( stream, fts_stack_size( post_stack), fts_stack_base( post_stack));
      fts_bytestream_flush( stream);
    }
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_post_init( void)
{
  init_punctuation();
}

