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

/************************************************************
 *
 *  object
 *
 */

typedef struct _out_
{
  fts_object_t o;
  fts_bytestream_t *stream;
} out_t;

static void
out_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_t *this = (out_t *)o;

  fts_bytestream_flush(this->stream);
}

static void
out_int_and_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_t *this = (out_t *)o;
  
  fts_bytestream_output_char(this->stream, (unsigned char)fts_get_int(at));
  fts_bytestream_flush(this->stream);
}

static void
out_list_and_flush(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_t *this = (out_t *)o;
  int i;

  for(i=0; i<ac; i++)
    if(fts_is_int(at + i))
      fts_bytestream_output_char(this->stream, (unsigned char)fts_get_int(at + i));

  fts_bytestream_flush(this->stream);
}

static void
out_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_t *this = (out_t *)o;
  
  fts_bytestream_output_char(this->stream, (unsigned char)fts_get_int(at));
}

static void
out_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  out_t *this = (out_t *)o;
  int i;

  for(i=0; i<ac; i++)
    if(fts_is_int(at + i))
      fts_bytestream_output_char(this->stream, (unsigned char)fts_get_int(at + i));
}

/************************************************************
 *
 *  class
 *
 */

static void
out_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  out_t *this = (out_t *)o;

  if(ac && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);

      if(fts_bytestream_check(obj) && fts_bytestream_is_output((fts_bytestream_t *)obj))
	{
	  this->stream = (fts_bytestream_t *)obj;
	  return;
	}
    }
    
  fts_object_set_error(o, "First argument of input bytestream required");
}

static void
out_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(out_t), out_init, 0);
  
  fts_class_message_varargs(cl, fts_s_bang, out_bang);
  fts_class_inlet_int(cl, 0, out_int_and_flush);
  fts_class_inlet_varargs(cl, 0, out_list_and_flush);
  fts_class_inlet_int(cl, 1, out_int);
  fts_class_inlet_varargs(cl, 1, out_list);
  }

void
out_config(void)
{
  fts_class_install(fts_new_symbol("out"), out_instantiate);
}
