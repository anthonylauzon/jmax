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

#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime/devices/bytestream.h>

static fts_symbol_t fts_s__superclass = 0;
static fts_symbol_t fts_s_bytestream = 0;

void
fts_bytestream_config(void)
{
  fts_s_bytestream = fts_new_symbol("bytestream");
  fts_s__superclass = fts_new_symbol("_superclass");
}

void
fts_bytestream_init(fts_bytestream_t *stream)
{
  stream->input = 0;
  stream->listeners = 0;

  stream->output = 0;
  stream->output_char = 0;
  stream->flush = 0;
}

void
fts_bytestream_set_input(fts_bytestream_t *stream)
{
  stream->input = 1;
}

void
fts_bytestream_set_output(fts_bytestream_t *stream, fts_bytestream_output_t write, fts_bytestream_output_char_t put, fts_bytestream_flush_t flush)
{
  stream->output = write;
  stream->output_char = put;
  stream->flush = flush;
}

void 
fts_bytestream_class_init(fts_class_t *cl)
{
  fts_atom_t a[1];

  fts_set_symbol(a, fts_s_bytestream);

  fts_class_put_prop(cl, fts_s__superclass, a); /* set _superclass property to "bytestream" */
}

int 
fts_bytestream_check(fts_object_t *obj)
{
  fts_atom_t a[1];

  fts_object_get_prop(obj, fts_s__superclass, a);

  if(fts_is_symbol(a) && fts_get_symbol(a) == fts_s_bytestream)
    return 1;
  else
    return 0;
}

void
fts_bytestream_input(fts_bytestream_t *stream, int n, const unsigned char *c)
{
  fts_bytestream_listener_t *l = stream->listeners;

  while(l)
    {
      fts_bytestream_listener_t *next = l->next;

      l->callback(l->listener, n, c);
      l = next;
    }
}

void 
fts_bytestream_add_listener(fts_bytestream_t *stream, fts_object_t *listener, fts_bytestream_callback_t fun)
{
  fts_bytestream_listener_t *l = (fts_bytestream_listener_t *)fts_malloc(sizeof(fts_bytestream_listener_t));

  l->callback = fun;
  l->listener = listener;
  l->next = stream->listeners;
  
  stream->listeners = l;      
}

void 
fts_bytestream_remove_listener(fts_bytestream_t *stream, fts_object_t *listener)
{
  fts_bytestream_listener_t *l = stream->listeners;

  if(l)
    {
      fts_bytestream_listener_t *freeme = 0;
      
      if(l && l->listener == listener)
	{
	  freeme = l;
	  stream->listeners = l->next;
	}
      else
	{
	  while(l->next)
	    {
	      if(l->next->listener == listener)
		{
		  freeme = l->next;
		  l->next = l->next->next;
		  
		  break;
		}
	      
	      l = l->next;
	    }
	}
      
      /* free removed listener */
      if(freeme)
	fts_free(freeme);
    }
}
