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

#include "fts.h"

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
  this->stream = (fts_bytestream_t *)fts_get_object(at + 1);
}

static int 
out_check(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1))
    {
      fts_object_t *obj = fts_get_object(at + 1);

      if(fts_bytestream_has_superclass(obj) && fts_bytestream_is_output((fts_bytestream_t *)obj))
	return 1;
    }
  
  return 0;
}

static fts_status_t
out_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(out_check(ac, at))
    {
      fts_class_init(cl, sizeof(out_t), 2, 0, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, out_init);

      fts_method_define_varargs(cl, 0, fts_s_bang, out_bang);
      fts_method_define_varargs(cl, 0, fts_s_int, out_int_and_flush);
      fts_method_define_varargs(cl, 0, fts_s_list, out_list_and_flush);
      fts_method_define_varargs(cl, 1, fts_s_int, out_int);
      fts_method_define_varargs(cl, 1, fts_s_list, out_list);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static int 
out_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  return out_check(ac1, at1);
}

void
out_config(void)
{
  fts_metaclass_install(fts_new_symbol("out"), out_instantiate, out_equiv);
}


