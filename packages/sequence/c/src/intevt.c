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
#include "eventtrk.h"
#include "intevt.h"

fts_symbol_t intevt_symbol = 0;

static void
intevt_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  intevt_t *this = (intevt_t *)o;
  
  event_init(&this->head);

  this->value = at[1];
}

/**************************************************************
 *
 *  mandatory event methods
 *
 */

void
intevt_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  intevt_t *this = (intevt_t *)o;
  fts_atom_t a[3];

  fts_set_float(a + 0, 1000.0 * event_get_time(&this->head));
  fts_set_symbol(a + 1, intevt_symbol);
  a[2] = this->value;

  fts_client_upload(o, event_symbol, 3, a);
}

static void
intevt_move(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* generic event "super class" */
  event_t *this = (event_t *)o;
  float time = fts_get_float(at + 0);

  eventtrk_move_event(this, time);
}

static void
intevt_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  intevt_t *this = (intevt_t *)o;

  if(ac > 0)
    this->value = at[0];
}

void 
intevt_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  intevt_t *this = (intevt_t *)o;

  post_atoms(1, &this->value);
  post("\n");
}

/**************************************************************
 *
 *  set event methods
 *
 */

/**************************************************************
 *
 *  class
 *
 */
static fts_status_t
intevt_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(intevt_t), 0, 0, 0); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, intevt_init);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("upload"), intevt_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("move"), intevt_move);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set"), intevt_set);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("print"), intevt_print);

  return fts_Success;
}

void
intevt_config(void)
{
  intevt_symbol = fts_new_symbol("intevt");

  fts_class_install(intevt_symbol, intevt_instantiate);
}
