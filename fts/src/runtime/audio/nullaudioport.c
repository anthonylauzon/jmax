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

/*
 * This file's authors: 
 * François Déchelle (dechelle@ircam.fr)
 */

/*
 * A null audio port that prevents FTS from locking the machine at startup:
 * it just sleeps for a while after receiving enough samples
 */

#include <sys/time.h>
#include <unistd.h>
#include <fts/fts.h>

#include <stdio.h>

/* ---------------------------------------------------------------------- */
/* The fts_audioport_t derived class                                      */
/* ---------------------------------------------------------------------- */
typedef struct {
  fts_audioport_t head;
  fts_timer_t timer;
} nullaudioport_t;

static void nullaudioport_output( fts_word_t *argv)
{
  nullaudioport_t *port = (nullaudioport_t *)fts_word_get_ptr( argv+0);

  if ( fts_timer_elapsed_time( &port->timer) >= (double)100.0)
    {
      struct timespec pause_time;

      pause_time.tv_sec = 0;
      pause_time.tv_nsec = 100000000L;

      nanosleep( &pause_time, 0);

      fts_timer_zero( &port->timer);
    }
}

static void nullaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  nullaudioport_t *this = (nullaudioport_t *)o;

  fts_audioport_init( &this->head);

  fts_audioport_set_output_function( (fts_audioport_t *)this, nullaudioport_output);
  fts_audioport_set_output_channels( (fts_audioport_t *)this, 2);

  fts_timer_init( &this->timer, 0);
  fts_timer_start( &this->timer);
}

static void nullaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t nullaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( nullaudioport_t), 0, 0, 0);
  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, nullaudioport_init);
  fts_class_add_daemon( cl, obj_property_get, fts_s_state, nullaudioport_get_state);

  return fts_Success;
}

void nullaudioport_config( void)
{
  fts_class_install( fts_new_symbol("nullaudioport"), nullaudioport_instantiate);
}
