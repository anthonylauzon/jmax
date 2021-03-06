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
out_bang(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  out_t *this = (out_t *)o;

  fts_bytestream_flush(this->stream);
}

static void
out_number_and_flush(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  out_t *this = (out_t *)o;
  
  fts_bytestream_output_char(this->stream, (unsigned char)fts_get_number_int(at));
  fts_bytestream_flush(this->stream);
}

static void
out_varargs_and_flush(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  out_t *this = (out_t *)o;
  int i;

  for(i=0; i<ac; i++)
    if(fts_is_number(at + i))
      fts_bytestream_output_char(this->stream, (unsigned char)fts_get_number_int(at + i));

  fts_bytestream_flush(this->stream);
}

static void
out_number(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  out_t *this = (out_t *)o;
  
  fts_bytestream_output_char(this->stream, (unsigned char)fts_get_number_int(at));
}

static void
out_varargs(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  out_t *this = (out_t *)o;
  int i;

  for(i=0; i<ac; i++)
    if(fts_is_number(at + i))
      fts_bytestream_output_char(this->stream, (unsigned char)fts_get_number_int(at + i));
}

/************************************************************
 *
 *  class
 *
 */

static void
out_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{ 
  out_t *this = (out_t *)o;

  if(ac && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);

      if(fts_bytestream_check(obj))
      {
        if(fts_bytestream_is_output((fts_bytestream_t *)obj))
        {
          this->stream = (fts_bytestream_t *)obj;
          fts_object_refer(obj);
	}
        else
          fts_object_error(o, "bytestream is not output");
         
        return;
      }
    }

  fts_object_error(o, "bytestream argument required");
}

static void
out_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  out_t *this = (out_t *)o;

  if(this->stream)
    fts_object_release((fts_object_t *)this->stream);
}

static void
out_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(out_t), out_init, out_delete);

  fts_class_inlet_bang(cl, 0, out_bang);
  
  fts_class_inlet_number(cl, 0, out_number_and_flush);
  fts_class_inlet_varargs(cl, 0, out_varargs_and_flush);
  
  fts_class_inlet_number(cl, 1, out_number);
  fts_class_inlet_varargs(cl, 1, out_varargs);
}

void
out_config(void)
{
  fts_class_install(fts_new_symbol("out"), out_instantiate);
}
