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
 * This file includes the jmax profile audio port, used for performance measurement.
 * The profile audio port outputs on its outlet the estimated sample rate,
 * measured by dividing the profile_interval by the elapsed system time.
 */

#include <sys/time.h>
#include <fts/fts.h>

#define DEFAULT_PROFILE_INTERVAL 10000

typedef struct {
  fts_audioport_t head;
  int profile_interval;
  int samples_count;
  int estimated_sample_rate;
  double last_time;
  fts_alarm_t output_alarm;
} profileaudioport_t;

static void profileaudioport_output_alarm( fts_alarm_t *alarm, void *p)
{
  profileaudioport_t *this = (profileaudioport_t *)p;

  fts_outlet_int( (fts_object_t *)this, 0, this->estimated_sample_rate);
}

static void profileaudioport_output( fts_word_t *argv)
{
  profileaudioport_t *this;
  int n;

  this = (profileaudioport_t *)fts_word_get_ptr( argv+0);
  n = fts_word_get_long(argv + 1);

  this->samples_count += n;

  if (this->samples_count >= this->profile_interval)
    {
      struct timeval tm;
      double now;

      gettimeofday( &tm, 0);

      now = (double)tm.tv_sec + (double)tm.tv_usec / 1000000.0;

      this->estimated_sample_rate = (int)(this->samples_count / (now - this->last_time));
      this->last_time = now;

      fts_alarm_set_delay( &this->output_alarm, 0.0f);

      this->samples_count = 0;
    }
}

static void profileaudioport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  profileaudioport_t *this = (profileaudioport_t *)o;
  struct timeval tm;

  fts_audioport_init( &this->head);

  fts_audioport_set_idle_function( (fts_audioport_t *)this, 0);
  fts_audioport_set_output_channels( (fts_audioport_t *)this, 2);
  fts_audioport_set_output_function( (fts_audioport_t *)this, profileaudioport_output);

  ac--;
  at++;

  this->profile_interval = fts_get_int_arg( ac, at, 0, DEFAULT_PROFILE_INTERVAL);

  fts_alarm_init( &(this->output_alarm), 0, profileaudioport_output_alarm, this);	

  gettimeofday( &tm, 0);
  this->last_time = tm.tv_sec + 1000000 * tm.tv_usec;

  this->samples_count = 0;
}

static void profileaudioport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  profileaudioport_t *this = (profileaudioport_t *)o;

  fts_alarm_reset(&this->output_alarm);	
  fts_audioport_delete( &this->head);
}

static void profileaudioport_get_state( fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, o);
}

static fts_status_t profileaudioport_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init( cl, sizeof( profileaudioport_t), 0, 1, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, profileaudioport_init);
  fts_method_define( cl, fts_SystemInlet, fts_s_delete, profileaudioport_delete, 0, 0);

  fts_class_add_daemon( cl, obj_property_get, fts_s_state, profileaudioport_get_state);

  return fts_Success;
}

void profileaudioport_config( void)
{
  fts_class_install( fts_new_symbol("profileaudioport"), profileaudioport_instantiate);
}
