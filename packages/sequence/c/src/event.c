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
#include <event.h>
#include <note.h>
#include <seqsym.h>
#include "seqmess.h"

fts_class_t *event_class = 0;

/**************************************************************
 *
 *  generic event functions
 *
 */

double 
event_get_duration(event_t *event)
{
  fts_atom_t *value = &event->value;
  double duration = 0.0;

  if(fts_is_a(value, note_type))
    {
      note_t *note = (note_t *)fts_get_object(value);

      duration = note_get_duration(note);
    }
  else if(fts_is_object(value))
    {
      /* get duration property ?? */
    }
  
  return duration;
}

/**************************************************************
 *
 *  generic event methods
 *
 */
static void
event_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;

  if(fts_is_object(&this->value))
    {
      fts_object_t *obj = (fts_object_t *)fts_get_object(&this->value);

      fts_send_message(obj, fts_s_set, ac, at);
    }
  else if(!fts_is_void(&this->value))
    this->value = at[0];
}

/**************************************************************
 *
 *  event class
 *
 */
static void
event_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;  

  fts_set_void(&this->value);

  this->time = 0.0;
  this->track = 0;
  this->prev = 0;
  this->next = 0;

  if(ac > 0)
    fts_atom_assign(&this->value, at);
}

static void
event_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  event_t *this = (event_t *)o;  

  fts_atom_void(&this->value);
}

static void
event_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(event_t), event_init, event_delete);

  fts_class_message_varargs(cl, fts_s_set, event_set);
}

/*****************************************************************
 *
 *  config & install
 *
 */

void
event_config(void)
{
  event_class = fts_class_install(seqsym_event, event_instantiate);
}
