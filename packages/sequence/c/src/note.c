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

fts_metaclass_t *note_type = 0;

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
note_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  if(fts_is_number(at))
    {
      int pitch = fts_get_number_int(at);
      
      if(pitch < 0)
	pitch = 0;

      this->pitch = pitch;
    }
}
  
static void
note_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  if(fts_is_number(at))
    {
      double duration = fts_get_number_float(at);

      if(duration < 0.0)
	duration = 0.0;
      
      this->duration = duration;
    }
}

static void
note_set_from_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  
  switch(ac)
    {
    default:
    case 2:
      note_duration(o, 0, 0, 1, at + 1);
    case 1:
      note_pitch(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

void 
note_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  fts_array_t *array = fts_get_pointer(at);
  
  fts_array_append_int(array, this->pitch);
  fts_array_append_float(array, (float)this->duration);
}

void 
note_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);

  fts_spost(stream, "(:note %d %d)", this->pitch, (float)this->duration);
}

static void
note_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  note_t *this = (note_t *)o;

  this->pitch = NOTE_DEF_PITCH;
  this->duration = NOTE_DEF_DURATION;

  note_set_from_array(o, 0, 0, ac, at);  
}

static fts_status_t
note_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(note_t), 1, 0, 0); 
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, note_init);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_get_array, note_get_array);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_array, note_set_from_array);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_post, note_post);

  fts_method_define_varargs(cl, 0, fts_new_symbol("duration"), note_duration);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pitch"), note_pitch);

  return fts_ok;
}

void
note_config(void)
{
  note_type = fts_class_install(seqsym_note, note_instantiate);
}
