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
#include "midival.h"

fts_class_t *midival_class = 0;

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
midival_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midival_t *this = (midival_t *)o;
  
  switch(ac)
    {
    case 3:
      this->channel = fts_get_number_int(at + 2);
    case 2:
      this->number = fts_get_number_int(at + 1);
    case 1:
      this->value = fts_get_number_int(at + 0);
    default:
      break;
    }
}

void 
midival_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midival_t *this = (midival_t *)o;
  fts_array_t *array = fts_get_array(at);
  int onset = fts_array_get_size(array);
  fts_atom_t *atoms;
  int i;
  
  fts_array_set_size(array, onset + 3);
  atoms = fts_array_get_atoms(array) + onset;

  fts_set_int(atoms + 0, this->value);
  fts_set_int(atoms + 1, this->number);
  fts_set_int(atoms + 2, this->channel);
}

/**************************************************************
 *
 *  class
 *
 */

static void
midival_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midival_t *this = (midival_t *)o;
  
  this->value = fts_get_int_arg(ac, at, 1, MIDIVAL_DEF_VALUE);
  this->number = fts_get_int_arg(ac, at, 2, MIDIVAL_DEF_NUMBER);
  this->channel = fts_get_int_arg(ac, at, 3, MIDIVAL_DEF_CHANNEL);
}

static fts_status_t
midival_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(midival_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midival_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, midival_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, midival_set_from_array);

  return fts_Success;
}

void
midival_config(void)
{
  fts_class_install(seqsym_midival, midival_instantiate);
  midival_class = fts_class_get_by_name(seqsym_midival);
}
