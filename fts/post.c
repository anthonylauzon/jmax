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

/* This file include all the kernel provided post like
   (client printing) functions.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <fts/fts.h>
#include <ftsprivate/OLDclient.h>
#include <ftsconfig.h>


/******************************************************************************/
/*                                                                            */
/*              Basic Post Functions                                          */
/*                                                                            */
/******************************************************************************/

/*
   Due to limitation in the handling of  string type argument,
   post cannot be buffer events, must be immediate events.
*/

static int
symbol_contains_blank(fts_symbol_t s)
{
  const char *str = fts_symbol_name(s);
  int n = strlen(str);
  int i;

  for(i=0; i<n; i++)
    {
      if(str[i] == ' ')
	return 1;
    }

  return 0;
}

void
post_vector(int n, float *fp)
{
  int i;
  
  for (i = 0; i < n; i+=4)
    post("%4d: %f %f %f %f\n", i, fp[i], fp[i+1], fp[i+2], fp[i+3]);

  post("\n");
}

void
post_symbol(fts_symbol_t sym)
{
  if(symbol_contains_blank(sym))
    post("\"%s\"", fts_symbol_name(sym));
  else
    post("%s", fts_symbol_name(sym));
}

void
post_atoms(int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      char *ps;

      if (i == (ac - 1))
	ps = "";
      else
	ps = " ";

      if (fts_is_int(at + i))
	post("%d%s", fts_get_int(at + i), ps);
      else if (fts_is_float(at + i))
	post("%g%s", fts_get_float(at + i), ps);
      else if (fts_is_symbol(at + i))
	{
	  fts_symbol_t sym = fts_get_symbol(at + i);
	  
	  if(symbol_contains_blank(sym))
	    post("\"%s\"%s", fts_symbol_name(sym), ps);
	  else
	    post("%s%s", fts_symbol_name(sym), ps);
	}
      else if (fts_is_void(at + i))
	post("<void>%s", ps);
      else
	post("<%s>%s", fts_symbol_name(fts_get_type(at + i)), ps);
    }
}

#define POST_LINE_MAXLENGTH 2048

void post( const char *format, ...)
{
  va_list ap;
  char buf[POST_LINE_MAXLENGTH];
  char *p;
  static char post_buffer[POST_LINE_MAXLENGTH];
  static char *fill_p = post_buffer;

  va_start( ap, format);
  vsprintf( buf, format, ap);
  va_end( ap);

  p = buf;
  while ( *p)
    {
      if (*p == '\n')
	{
	  *fill_p = '\0';

	  if (fts_client_is_up())
	    {
	      fts_client_start_msg( POST_LINE_CODE);
	      fts_client_add_string( post_buffer);
	      fts_client_done_msg();
	    }
	  else
	    {
	      fprintf( stderr, "%s\n", post_buffer);
	    }

	  fill_p = post_buffer;
	}
      else
	{
	  *fill_p = *p;
	  fill_p++;
	}

      p++;
    }
}

/* 
 * The way to post object error message is to use this function;
 * Same syntax as post, add a first object argument; this argument
 * will be used to allow the user to retrieve the patch where the 
 * objetc is.
 *
 * Currently, the strategy is to use the error and errdesc properties;
 * it is actually a temporary hack, a special "console" should be written.
 * 
 */

void post_error(fts_object_t *obj, const char *format , ...)
{
  fts_atom_t a;
  va_list ap;
  char buf[512];

  va_start( ap, format);

  vsprintf(buf, format, ap);

  va_end(ap);

  fts_set_int(&a, 1);
  fts_object_put_prop(obj, fts_s_error, &a);

  if (fts_object_has_id(obj))
    fts_object_ui_property_changed(obj, fts_s_error);

  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  fts_object_put_prop(obj, fts_s_error_description, &a);

  fts_client_start_msg(POST_CODE);
  fts_client_add_string(buf);
  fts_client_done_msg();
}



/*
 * iiwu_log
 */

static char* log_file = NULL;

char* fts_log_date(char* buf, int len)
{
#ifdef WIN32
  char t[9];
  char d[9];
  _strdate(d);
  _strtime(t);
  snprintf(buf, len, "%s %s", d, t);
#else
  snprintf(buf, len, "%s", ctime(time(NULL)));
#endif
  return buf;
}

void fts_log_init(void)
{
  FILE* log;
  char buf[1024];

#ifdef WIN32
  log_file = "C:\\fts_log.txt";
#else
  char* home = getenv("HOME");
  if (home) {
    snprintf(buf, 1024, "%s/.fts_log", home);
    log_file = strdup(buf);
  } else {
    log_file = "/tmp/fts_log";
  }
#endif

  /* truncate the file */
  log = fopen(log_file, "w");
  fprintf(log, "\n[log]: started logging at %s\n", fts_log_date(buf, 1024));
  fclose(log);
}

/*
 * It may seem strange that in the following functions,
 * we reopen the file at each call, but it is *not*.
 * This way, you are guaranteed that the file's content
 * is updated after each log, even in case of FTS crash !!!
 */
void fts_log(char* fmt, ...)
{
  FILE* log;
  va_list args; 

  if (log_file == NULL) {
    fts_log_init();
  }

  log = fopen(log_file, "a");
  if (log == NULL) {
    return;
  }

  va_start (args, fmt); 
  vfprintf(log, fmt, args); 
  va_end (args); 

  fflush(log);
  fclose(log);
}

void fts_log_atoms( int ac, const fts_atom_t *at)
{
  FILE* log;

  if (log_file == NULL) {
    fts_log_init();
  }

  log = fopen(log_file, "a");
  if (log == NULL) {
    return;
  }

  fprintf_atoms( log, ac, at);

  fflush(log);
  fclose(log);
}
