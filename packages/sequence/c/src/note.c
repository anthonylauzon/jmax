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
#include "note.h"

fts_class_t *note_class = 0;

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
note_set_state_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  
  this->pitch = fts_get_number_int(at + 0);
  this->duration = fts_get_number_float(at + 1);
}

void 
note_append_state_to_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  fts_array_t *array = fts_get_array(at);
  fts_atom_t a;
  int i;
  
  fts_set_int(&a, this->pitch);
  fts_array_append(array, 1, &a);

  fts_set_float(&a, (float)this->duration);
  fts_array_append(array, 1, &a);
}

/**************************************************************
 *
 *  class
 *
 */

static void
note_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  
  this->pitch = fts_get_int_arg(ac, at, 1, NOTE_DEF_PITCH);
  this->duration = fts_get_float_arg(ac, at, 2, NOTE_DEF_DURATION);
}

static fts_status_t
note_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(note_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, note_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_append_state_to_array, note_append_state_to_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_state_from_array, note_set_state_from_array);

  return fts_Success;
}

void
note_config(void)
{
  fts_class_install(seqsym_note, note_instantiate);
  note_class = fts_class_get_by_name(seqsym_note);
}
