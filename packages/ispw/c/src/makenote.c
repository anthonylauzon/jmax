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
  int vel;
  float dur;
  fts_timer_t *timer;
} makenote_t;

static void
makenote_alarm(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  fts_outlet_int(o, 0, 0);
  fts_outlet_send(o, 0, 0, 1, at);
}

static void
makenote_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  if(fts_is_number(at) && this->dur > 0.0)
    {
      fts_timer_set_delay(this->timer, this->dur, at);
      
      /* send the output messages */
      fts_outlet_int(o, 1, this->vel);
      fts_outlet_int(o, 0, fts_get_number_int(at));
    }
}

static void
makenote_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  this->vel = fts_get_number_int(at);
}

static void
makenote_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  this->dur = fts_get_number_float(at);
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
	this->dur = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->vel = fts_get_number_int(at + 1);
    case 1:
      makenote_number(o, winlet, s, 1, at);
    case 0:
      break;
    }      
}

/* makenote stop method is also installed as method for the $delete message */

static void
makenote_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  fts_timer_reset(this->timer);
}

static void
makenote_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  fts_timer_reset(this->timer);
}


static void
makenote_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;

  this->vel = fts_get_int_arg(ac, at, 1, 0);
  this->dur = fts_get_float_arg(ac, at, 2, 0.0f);

  this->timer = fts_timer_new(o, 0);
}

static void
makenote_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  makenote_t *this = (makenote_t *)o;
  
  fts_timer_delete(this->timer);  
}


static fts_status_t
makenote_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(makenote_t), 3, 2, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, makenote_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, makenote_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_timer_alarm, makenote_alarm);

  fts_method_define_varargs(cl, 0, fts_new_symbol("clear"), makenote_clear);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), makenote_stop);

  fts_method_define_varargs(cl, 0, fts_s_int, makenote_number);
  fts_method_define_varargs(cl, 0, fts_s_float, makenote_number);

  fts_method_define_varargs(cl, 1, fts_s_int, makenote_number_1);
  fts_method_define_varargs(cl, 1, fts_s_float, makenote_number_1);

  fts_method_define_varargs(cl, 2, fts_s_int, makenote_number_2);
  fts_method_define_varargs(cl, 2, fts_s_float, makenote_number_2);

  fts_method_define_varargs(cl, 0, fts_s_list, makenote_list);

  /* Type the outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_int);
  fts_outlet_type_define_varargs(cl, 1,	fts_s_int);

  return fts_Success;
}


void
makenote_config(void)
{
  fts_class_install(fts_new_symbol("makenote"),makenote_instantiate);
}
