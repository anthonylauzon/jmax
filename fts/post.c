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

static int mempost_symbol( int *psize, char **pp, int offset, fts_symbol_t s)
{
  if ( strchr( s, ' ') != NULL)
    return mempost( pp, psize, offset, "\"%s\"", s);

  return mempost( pp, psize, offset, "%s", s);
}

static int mempost_object( char **pp, int *psize, int offset, fts_object_t *obj)
{
  if (! obj)
    return mempost( pp, psize, offset, "{NULL OBJ}");

  if (obj->argv)
    {
      int n = 0;

      n += mempost( pp, psize, offset+n, "{");
      n += mempost_atoms( pp, psize, offset+n, obj->argc, obj->argv);
      n += mempost( pp, psize, offset+n, "}");

      return n;
    }

  return mempost( pp, psize, offset, "<\"%s\" #%d>", fts_object_get_class_name(obj), fts_object_get_id(obj));
}

static int mempost_atoms( char **pp, int *psize, int offset, int ac, const fts_atom_t *at)
{
  int i, n = 0;

  for ( i = 0; i < ac; i++, at++)
    {
      if ( fts_is_void( at))
	n += mempost( pp, psize, offset+n, "<void>");
      else if ( fts_is_int( at))
	n += mempost( pp, psize, offset+n, "%d", fts_get_int( at));
      else if ( fts_is_float( at))
	n += mempost( pp, psize, offset+n, "%f", fts_get_float( at));
      else if ( fts_is_symbol( at))
	n += mempost( pp, psize, offset+n, "%s", fts_get_symbol( at));
      else if ( fts_is_object( at))
	n += mempost_object( pp, psize, offset+n, fts_get_object( at));
      else if ( fts_is_pointer( at) )
	n += mempost( pp, psize, offset+n, "%p", fts_get_pointer( at));
      else if ( fts_is_string( at))
	n += mempost( pp, psize, offset+n, "%s", fts_get_string( at));
      else
	n += mempost( pp, psize, offset+n, "<UNKNOWN TYPE>%x", fts_get_int( at));

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

static void fts_vspost( fts_bytestream_t *stream, const char *format, va_list ap)
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

void fts_spost_atoms( fts_bytestream_t *stream, int ac, const fts_atom_t *at)
{
  int n;

  n = mempost_atoms( &post_buffer, &post_buffer_size, 0, ac, at);

  fts_bytestream_output( stream, n, post_buffer);
  fts_bytestream_flush( stream);
}


static int check_symbol_in( fts_atom_t *p, fts_symbol_t *symbols)
{
  int i;

  if ( !fts_is_symbol( p))
    return 0;

  for ( i = 0; symbols[i]; i++)
    if ( symbols[i] == fts_get_symbol( p))
      return 1;

  return 0;
}

static fts_symbol_t want_a_space_before_symbols[] = {"+", "-", "*", "/", "%", "&&", "&", "||", "|", "==", "=", "!=", "!", ">=", "^", ">>", ">", "<<", "<=", "<", "?", "::", ":", 0 };
static fts_symbol_t dont_want_a_space_before_symbols[] = {")", "[", "]", "}", ",", ";", ".", "=", 0};
static fts_symbol_t want_a_space_after_symbols[] = { "+", "-", "*", "/", "%", ",", "&&", "&", "||", "|", "==", "=", "!=", "!", ">=", ">>", ">", "<<", "<=", "<", "?", "::", ":", "^", ";", 0 };
static fts_symbol_t dont_want_a_space_after_symbols[] = { "(", "[", "{", "$", "'", "." , "=", 0 };
static fts_symbol_t operators[] = { "$", ";", "+", "-", "*", "/", "(", ")", "[", "]", "{", "}", ".", "%", "<<", ">>", "&&", "||", "!", "==", "!=", ">", ">=", "<", "<=", ":", "=", 0};

static void init_punctuation( void)
{
  int i, j;
  fts_symbol_t *p, *tab[] = { want_a_space_before_symbols, dont_want_a_space_before_symbols, want_a_space_after_symbols, dont_want_a_space_after_symbols, operators};

  for ( i = 0; i < sizeof( tab)/sizeof( fts_symbol_t *); i++)
    for ( j = 0, p = tab[i]; *p; p++)
      *p = fts_new_symbol( *p);
}

#define want_a_space_before(value) check_symbol_in( value, want_a_space_before_symbols)
#define dont_want_a_space_before(value) check_symbol_in( value, dont_want_a_space_before_symbols)
#define want_a_space_after(value) check_symbol_in( value, want_a_space_after_symbols)
#define dont_want_a_space_after(value) check_symbol_in( value, dont_want_a_space_after_symbols)

static int needs_quote( fts_atom_t *p)
{
  fts_symbol_t s;
  if (check_symbol_in( p, operators))
    return 0;
  else
    {
      s = fts_get_symbol( p);      
      if( !isalnum( *s) )
	return 1;
      
      s++;
      while( *s != '\0')
	{	    
	  if( !(isalnum( *s) | ( *s == '+') | ( *s == '-')))
	    return 1;
	  s++;
	}
      return 0;
    }    
}

void fts_spost_object_description( fts_bytestream_t *stream, fts_object_t *obj)
{
  fts_spost_object_description_args( stream, fts_object_get_description_size( obj), fts_object_get_description_atoms( obj));
}

void fts_spost_object_description_args( fts_bytestream_t *stream, int ac, fts_atom_t *at)
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
	fts_spost( stream, "%g", fts_get_float( value1));
      else if ( fts_is_symbol( value1))
	if( needs_quote( value1))
	  fts_spost( stream, "%c%s%c", '"', fts_get_symbol( value1), '"');
	else
	  fts_spost( stream, "%s", fts_get_symbol( value1));
      else
	fts_spost( stream, "??");

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
    log_init();

  log = fopen( log_file_name, "a");
  if (log == NULL)
    return;

  fprintf( log, "[%u]", (unsigned int) (fts_systime() - log_time));

  va_start( ap, format); 
  vfprintf( log, format, ap);
  va_end( ap);

  fflush( log);
  fclose( log);
}

void fts_log_atoms( int ac, const fts_atom_t *at)
{
  FILE *log;
  int n;

  if (log_file_name == NULL)
    log_init();

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
      fts_bytestream_output( stream, fts_stack_get_top( post_stack), fts_stack_get_base( post_stack));
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

