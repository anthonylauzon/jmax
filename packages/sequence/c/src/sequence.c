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
#include "track.h"

/*********************************************************
 *
 *  sequence
 *
 */

void
sequence_init(sequence_t *sequence)
{
  sequence->tracks = 0;
  sequence->size = 0;
  sequence->open = 0;  
}

/*********************************************************
 *
 *  add/remove tracks
 *
 */

void
sequence_add_track(sequence_t *sequence, track_t *track)
{
  if(!sequence->tracks)
    {
      /* first track */
      sequence->tracks = track;
      sequence->size = 1;
    }
  else
    {
      /* append at end */
      track_t *last = sequence->tracks;

      while(last->next)
	last = last->next;

      last->next = track;
      sequence->size++;
    }
  
  track->sequence = sequence;
  track->next = 0;
}

void
sequence_remove_track(track_t *track)
{
  sequence_t *sequence = track_get_sequence(track);

  if(track == sequence->tracks)
    {
      /* first track */
      sequence->tracks = track->next;
      sequence->size--;
    }
  else
    {
      track_t *prev = sequence->tracks;
      track_t *this = track_get_next(prev);

      while(this && this != track)
	{
	  prev = this;
	  this = this->next;
	}

      if(this)
	{
	  prev->next = this->next;
	  sequence->size--;	  
	}
    }
}

track_t *
sequence_get_track_by_name(sequence_t *sequence, fts_symbol_t name)
{
  track_t *track = sequence->tracks;

  while(track && track_get_name(track) != name)
    track = track_get_next(track);

  return track;
}
