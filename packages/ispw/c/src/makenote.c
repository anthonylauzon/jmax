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

typedef struct 
{
  fts_object_t head;
  int velocity;
  float duration;
} makenote_t;

static void
makenote_send_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  fts_outlet_int(o, 1, 0);
  fts_outlet_int(o, 0, fts_get_int(at));
}

static void
makenote_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  if(fts_is_number(at))
    {
      fts_timebase_add_call(fts_get_timebase(), o, makenote_send_off, at, this->duration);
      
      /* send the output messages */
      fts_outlet_int(o, 1, this->velocity);
      fts_outlet_int(o, 0, fts_get_number_int(at));
    }
}

static void
makenote_set_velocity(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  this->velocity = fts_get_number_int(at);
}

static void
makenote_set_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;
  double duration = fts_get_number_float(at);

  if(duration > 0.0)
    this->duration = duration;
  else
    this->duration = 0.0;
}

static void
makenote_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	makenote_set_duration(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	makenote_set_velocity(o, 0, 0, 1, at + 1);
    case 1:
      makenote_pitch(o, winlet, s, 1, at);
    case 0:
      break;
    }      
}

static void
makenote_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  fts_timebase_flush_object(fts_get_timebase(), o);
}

static void
makenote_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  fts_timebase_remove_object(fts_get_timebase(), o);
}


static void
makenote_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  switch(ac)
    {
    default:
    case 2:
      makenote_set_duration(o, 0, 0, 1, at + 1);
    case 1:
      makenote_set_velocity(o, 0, 0, 1, at + 0);
    case 0:
      break;
    }
}

static void
makenote_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(makenote_t), makenote_init, 0);

  fts_class_method_varargs(cl, fts_s_clear, makenote_clear);
  fts_class_method_varargs(cl, fts_s_stop, makenote_stop);

  fts_class_inlet_int(cl, 0, makenote_pitch);
  fts_class_inlet_float(cl, 0, makenote_pitch);

  fts_class_inlet_int(cl, 1, makenote_set_velocity);
  fts_class_inlet_float(cl, 1, makenote_set_velocity);

  fts_class_inlet_int(cl, 2, makenote_set_duration);
  fts_class_inlet_float(cl, 2, makenote_set_duration);

  fts_class_inlet_varargs(cl, 0, makenote_list);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
}


void
makenote_config(void)
{
  fts_class_install(fts_new_symbol("makenote"),makenote_instantiate);
}
