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

#define MAX_length 128


typedef struct 
{
  fts_object_t ob;	 
} listsum_t;

static void
listsum_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  long i_sum = 0;
  float f_sum = 0.0f;

  for(i=0; i<ac; i++)
    {
      const fts_atom_t *atom = at + i;

      if(fts_is_int(atom))
	i_sum += fts_get_int(atom);
      else if(fts_is_float(atom))
	f_sum += fts_get_float(atom);
    }

  if(f_sum == 0.0f)
    fts_outlet_int(o, 0, i_sum);
  else
    fts_outlet_float(o, 0, (float)i_sum + f_sum);
}

static fts_status_t
listsum_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(listsum_t), 1, 1, 0); 

  fts_method_define_varargs(cl, 0, fts_s_list, listsum_list);

  return fts_Success;
}

void
listsum_config(void)
{
  fts_class_install(fts_new_symbol("listsum"), listsum_instantiate);
  fts_alias_install(fts_new_symbol("lsum"), fts_new_symbol("listsum"));
}
