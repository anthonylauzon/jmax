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

typedef struct 
{
  fts_object_t o;
  fts_atom_t *list;
  int size;
  int alloc;
} listreverse_t;

static void
listreverse_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;
  int i, j;

  if(ac > this->alloc)
    {
      if(this->alloc)
	fts_block_free(this->list, this->alloc);

      this->list = (fts_atom_t *)fts_block_alloc(ac * sizeof(fts_atom_t));
      this->alloc = ac;
    }

  for(i=0, j=ac-1; i<ac; i++, j--)
    this->list[i] = at[j];

  this->size = ac;
  
  fts_outlet_send(o, 0, fts_s_list, this->size, this->list);
}

static void
listreverse_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;

  this->size = 0;
  this->alloc = 0;
}

static void
listreverse_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;

  fts_block_free(this->list, this->alloc);
}

static fts_status_t
listreverse_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listreverse_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, listreverse_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, listreverse_delete, 0, 0);

  /* define the methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listreverse_list);

  /* Type the outlet */

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listreverse_config(void)
{
  fts_class_install(fts_new_symbol("listreverse"), listreverse_instantiate);
}
