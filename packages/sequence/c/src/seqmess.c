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
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "seqmess.h"

/**************************************************************
 *
 *  set event methods
 *
 */

static void
seqmess_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqmess_t *this = (seqmess_t *)o;

  switch(ac)
    {
    default:
      if(fts_is_number(at + ac - 1))
	this->position = fts_get_number_int(at + ac - 1);
    case 1:
      if(fts_is_symbol(at + 0))
	this->s = fts_get_symbol(at + 0);
    case 0:
      break;
    }
}

void 
seqmess_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqmess_t *this = (seqmess_t *)o;
  fts_array_t *array = fts_get_pointer(at);
  fts_atom_t a;
  
  fts_set_symbol(&a, this->s);
  fts_array_append(array, 1, &a);

  fts_set_int(&a, this->position);
  fts_array_append(array, 1, &a);
}

void 
seqmess_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqmess_t *this = (seqmess_t *)o;

  post("%s\n", fts_symbol_name(this->s));
}

/**************************************************************
 *
 *  class
 *
 */

static void
seqmess_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqmess_t *this = (seqmess_t *)o;

  ac--;
  at++;
  
  this->s = fts_s_bang;
  this->position = 0;

  seqmess_set_from_array(o, 0, 0, ac, at);
}

static fts_status_t
seqmess_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqmess_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqmess_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, seqmess_print);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, seqmess_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, seqmess_set_from_array);

  return fts_Success;
}

void
seqmess_config(void)
{
  fts_class_install(seqsym_seqmess, seqmess_instantiate);
}
