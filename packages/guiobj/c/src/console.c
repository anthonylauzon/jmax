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
 */
#include <fts/fts.h>

/***********************************************************************
 *
 * console stream
 * This object maintains an internal stack for line buffering.
 * 
 * Console stream object has a property named "default" that if set to yes,
 * tells the object to install itself as default console
 */

typedef struct consolestream {
  fts_bytestream_t bytestream;
  fts_stack_t line_buffer;
} consolestream_t;

static fts_symbol_t s_print_line;

static void consolestream_output(fts_bytestream_t *stream, int count, const unsigned char *buf)
{
  consolestream_t *this = (consolestream_t *)stream;
  int i;

  for ( i = 0; i < count; i++)
    {
      if ( buf[i] == '\n')
	{
	  fts_atom_t a[1];

	  fts_stack_push( &this->line_buffer, char, '\0');

	  fts_set_string( a, fts_stack_base( &this->line_buffer));
	  fts_client_send_message( (fts_object_t *)this, s_print_line, 1, a);

	  fts_stack_clear( &this->line_buffer);
	}
      else
	fts_stack_push( &this->line_buffer, char, buf[i]);
    }
}

static void consolestream_output_char(fts_bytestream_t *stream, unsigned char c)
{
  consolestream_output(stream, 1, &c);
}

static void consolestream_flush( fts_bytestream_t *stream)
{
}

static void consolestream_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  consolestream_t *this = (consolestream_t *) o;

  fts_bytestream_init( (fts_bytestream_t *)this);

  fts_bytestream_set_output( (fts_bytestream_t *)this, 
			     consolestream_output,
			     consolestream_output_char,
			     consolestream_flush);

  fts_stack_init( &this->line_buffer, char);
}

static void consolestream_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_set_default_console_stream( (fts_bytestream_t *)NULL);
}

static void consolestream_set_default( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_set_default_console_stream( (fts_bytestream_t *)o);
}

static void consolestream_instantiate(fts_class_t *cl)
{
  fts_class_init( cl, sizeof(consolestream_t), consolestream_init, consolestream_delete);

  fts_class_method_varargs(cl, fts_new_symbol("set_default"), consolestream_set_default);
}

void console_config( void)
{
  fts_class_install( fts_new_symbol("console_stream"), consolestream_instantiate);
  s_print_line = fts_new_symbol( "print_line");
}


