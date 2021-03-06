/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/* 
 * This file includes the jmax profile audio port, used for performance measurement.
 * The profile audio port outputs on its outlet the estimated sample rate,
 * measured by dividing the profile_interval by the elapsed system time.
 */

#include <fts/fts.h>

#if HAVE_WINDOWS_H
#include <windows.h>
#else
#include <sys/time.h>
#endif


#define DEFAULT_PROFILE_INTERVAL 10000

typedef struct {
  fts_audioport_t head;
  int profile_interval;
  int samples_count;
  int estimated_sample_rate;
  double last_time;
} profileaudioport_t;

static void profileaudioport_output_estimation(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  profileaudioport_t *this = (profileaudioport_t *)o;

  fts_outlet_int( (fts_object_t *)this, 0, this->estimated_sample_rate);
}

static void profileaudioport_output( fts_word_t *argv)
{
  profileaudioport_t *this = (profileaudioport_t *)fts_word_get_pointer( argv+0);
  int n = fts_word_get_int(argv + 1);

  this->samples_count += n;

  if (this->samples_count >= this->profile_interval)
    {
#ifdef WIN32
      double now = GetTickCount() * 1000.0;
#else
      struct timeval tm;
      double now;

      gettimeofday( &tm, 0);
      now = (double)tm.tv_sec + (double)tm.tv_usec * 1000000.0;
#endif

      this->estimated_sample_rate = (int)(this->samples_count / (now - this->last_time));
      this->last_time = now;

      fts_timebase_add_call(fts_get_timebase(), (fts_object_t *)this, profileaudioport_output_estimation, 0, 0.0);

      this->samples_count = 0;
    }
}

static void profileaudioport_init(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  profileaudioport_t *this = (profileaudioport_t *)o;
#ifndef WIN32
  struct timeval tm;
#endif

  fts_audioport_init( &this->head);

  fts_audioport_set_idle_function( (fts_audioport_t *)this, 0);
  fts_audioport_set_output_channels( (fts_audioport_t *)this, 2);
  fts_audioport_set_output_function( (fts_audioport_t *)this, profileaudioport_output);

  this->profile_interval = fts_get_int_arg( ac, at, 0, DEFAULT_PROFILE_INTERVAL);

#ifdef WIN32
  this->last_time = GetTickCount() * 1000.0;
#else
  gettimeofday( &tm, 0);
  this->last_time = tm.tv_sec + 1000000 * tm.tv_usec;
#endif

  this->samples_count = 0;
}

static void profileaudioport_delete(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  profileaudioport_t *this = (profileaudioport_t *)o;

  fts_audioport_delete( &this->head);
}

static void profileaudioport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( profileaudioport_t), profileaudioport_init, profileaudioport_delete);
}

void profileaudioport_config( void)
{
  fts_class_install( fts_new_symbol("profileaudioport"), profileaudioport_instantiate);
}
