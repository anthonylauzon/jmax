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

fts_class_t *fts_tuple_class = 0;

static void
tuple_compare(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  fts_tuple_t *in = (fts_tuple_t *)fts_get_object(at);
  int in_n = fts_tuple_get_size(in);
  int n = fts_tuple_get_size(this);
  fts_atom_t ret;

  if(in_n == n)
    {
      fts_atom_t *in_a = fts_tuple_get_atoms(in);
      fts_atom_t *a = fts_tuple_get_atoms(this);
      int sum = 0;
      int i;
      
      for(i=0; i<n; i++)
	sum += fts_atom_compare(in_a + i, a + i);

      fts_set_int(&ret, (sum == n));
    }
  else
    fts_set_int(&ret, 0);

  fts_return(&ret);
}

static void
tuple_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  
  fts_spost(stream, "(");
  fts_spost_atoms(stream, fts_tuple_get_size(this), fts_tuple_get_atoms(this));
  fts_spost(stream, ")");
}

static void
tuple_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_array_init(&this->args, ac, at);
}

static void
tuple_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_tuple_t *this = (fts_tuple_t *)o;

  fts_array_destroy(&this->args);
}

static void
tuple_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_tuple_t), tuple_init, tuple_delete);

  fts_class_message_varargs(cl, fts_s_post, tuple_post);

  fts_class_message_varargs(cl, fts_s_compare, tuple_compare);
  }

void
fts_kernel_tuple_init(void)
{
  fts_tuple_class = fts_class_install(NULL, tuple_instantiate);
}
