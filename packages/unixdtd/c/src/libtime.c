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
 */

#include <sys/time.h>

#include <stdio.h>

#include "libtime.h"

void get_current_milliseconds( double *absolute, double *elapsed)
{
  struct timeval tv;
  double now;
  static double previous = -1.0;

  gettimeofday( &tv, 0);

#if 0
  fprintf( stderr, "gettimeofday -> %ld sec %ld usec\n", tv.tv_sec, tv.tv_usec);
#endif

  now = (double)tv.tv_sec*1000.0 + (double)tv.tv_usec/1000.0;

  if (previous < 0.0)
    previous = now;
  *elapsed = now - previous;
  previous = now;

  *absolute = now;
}

