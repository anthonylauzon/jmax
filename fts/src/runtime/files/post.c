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

/* This file include all the kernel provided post like
   (client printing) functions.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>


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

#define POST_LINE_MAXLENGTH 512
#define POST_BUFFER_LENGTH 4096

static char post_buffer[POST_BUFFER_LENGTH + POST_LINE_MAXLENGTH];
static char *fill_p = post_buffer;
static char *flush_p = post_buffer;

void post_flush( void)
{
  char *p;

  p = flush_p;

  while ( *p)
    {
      if (*p == '\n')
	{

	  /* At end of a line, so flush current line to client. */
	  *p = '\0';
	  p++;

	  fts_client_start_msg( POST_LINE_CODE);
	  fts_client_add_string( flush_p);
	  fts_client_done_msg();

	  /* 
	   * Test if we are past POST_BUFFER_LENGTH.
	   * If yes, back up to beginning of post_buffer
	   */
	  if ( p >= post_buffer + POST_BUFFER_LENGTH)
	    p = post_buffer;

	  flush_p = p;
	}
      else
	p++;
    }
}

void post( const char *format, ...)
{
  va_list ap;
  char buf[POST_LINE_MAXLENGTH];
  char *p;

  va_start( ap, format);
  vsprintf( buf, format, ap);
  va_end( ap);

  p = buf;
  while ( *p)
    {
      *fill_p = *p;
      fill_p++;

      /*
       * (fd) 
       * If at end of a line and past POST_BUFFER_LENGTH, then
       * back up to beginning of post_buffer.
       * We are sure that it will not overflow because post_buffer 
       * has a reserve at end to store one complete line. If this
       * overflows, then the 'vsprintf' above has already overflowed
       * and memory is already corrupted...
       */
      if (*p == '\n' && fill_p >= post_buffer + POST_BUFFER_LENGTH)
	fill_p = post_buffer;

      p++;
    }
  *fill_p = '\0';

  if (client_dev)
    post_flush();
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


