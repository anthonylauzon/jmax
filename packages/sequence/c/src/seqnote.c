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
#include "sequence.h"
#include "seqobj.h"
#include "seqnote.h"

#define SEQNOTE_DEF_PITCH 64
#define SEQNOTE_DEF_DURATION 400

fts_symbol_t seqnote_symbol = 0;

static void
seqnote_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqnote_t *this = (seqnote_t *)o;
  
  sequence_event_init(&this->head);

  this->pitch = fts_get_int_arg(ac, at, 1, SEQNOTE_DEF_PITCH);
  this->duration = fts_get_float_arg(ac, at, 2, SEQNOTE_DEF_DURATION);
}

/**************************************************************
 *
 *  mandatory event methods
 *
 */

static void
seqnote_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqnote_t *this = (seqnote_t *)o;
  fts_atom_t a[4];

  fts_set_float(a + 0, 1000.0 * sequence_event_get_time(&this->head));
  fts_set_symbol(a + 1, seqnote_symbol);
  fts_set_int(a + 2, this->pitch);

  fts_set_int(a + 3, (int)(1000.0 * this->duration));
  /*  fts_set_float(a + 3, this->duration); @@@@*/

  fts_client_upload(o, seqevent_symbol, 4, a);
}

static void
seqnote_move(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* generic event "super class" */
  sequence_event_t *this = (sequence_event_t *)o;
  float time = fts_get_float(at + 0);

  sequence_move_event(this, time);
}

static void
seqnote_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqnote_t *this = (seqnote_t *)o;

  if(ac > 0 && fts_is_number(at + 0))
    this->pitch = fts_get_number_int(at + 0);

  if(ac > 1 && fts_is_number(at + 1))
    this->duration = fts_get_number_float(at + 1);
}

void 
seqnote_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqnote_t *this = (seqnote_t *)o;

  post("pitch: %d, duration %lf", this->pitch, this->duration);
}

/**************************************************************
 *
 *  set event methods
 *
 */

static void
seqnote_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqnote_t *this = (seqnote_t *)o;

  if(ac > 0 && fts_is_number(at + 0))
    this->pitch = fts_get_number_int(at + 0);
}

static void
seqnote_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqnote_t *this = (seqnote_t *)o;

  if(ac > 0 && fts_is_number(at + 0))
    this->duration = fts_get_number_float(at + 0);
}

/**************************************************************
 *
 *  class
 *
 */
static fts_status_t
seqnote_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqnote_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqnote_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("upload"), seqnote_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("move"), seqnote_move);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set"), seqnote_set);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("pitch"), seqnote_pitch);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("duration"), seqnote_duration);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("print"), seqnote_print);

  return fts_Success;
}

void
seqnote_config(void)
{
  seqnote_symbol = fts_new_symbol("ambitus");

  fts_class_install(seqnote_symbol, seqnote_instantiate);
}
