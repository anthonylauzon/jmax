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

#include <fts/fts.h>

typedef struct _in_
{
  fts_object_t o;
  fts_bytestream_t *stream;
} in_t;

static void
in_input(fts_object_t *this, int n, const unsigned char *c)
{
  int i;

  for(i=0; i<n; i++)
    fts_outlet_int(this, 0, c[i]);
}

/************************************************************
 *
 *  object
 *
 */

static void
in_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  in_t *this = (in_t *)o;

  ac--;
  at++;

  if(ac > 0 && fts_is_object(at))
    {
      fts_object_t *obj = fts_get_object(at);

      if(fts_bytestream_check(obj) && fts_bytestream_is_input((fts_bytestream_t *)obj))
	{  
	  this->stream = (fts_bytestream_t *)obj;
	  fts_bytestream_add_listener(this->stream, o, in_input);
	  
	  return;
	}
    }
    
  fts_object_set_error(o, "First argument of input bytestream required");
}

static void 
in_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  in_t *this = (in_t *)o;

  if(this->stream)
    fts_bytestream_remove_listener(this->stream, o);
}

/************************************************************
 *
 *  class
 *
 */

static int 
in_check(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1))
    {
      fts_object_t *obj = fts_get_object(at + 1);

      if(fts_bytestream_check(obj) && fts_bytestream_is_input((fts_bytestream_t *)obj))
	return 1;
    }
  
  return 0;
}

static fts_status_t
in_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(in_t), 0, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, in_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, in_delete);
  
  return fts_Success;
}

void
in_config(void)
{
  fts_class_install(fts_new_symbol("in"), in_instantiate);
}
