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

#include <fts/fts.h>
#include <fts/packages/sequence/sequence.h>

typedef struct _seqrec_
{
  fts_object_t head; /* sequence reference object */
  fts_timebase_t *timebase;
  track_t *track;
  track_t *recording; /* recording track */

  /* status */
  enum {status_reset, status_ready, status_recording} status;
  double start_location;
  double start_time;
} seqrec_t;

/************************************************************
 *
 *  user methods
 *
 */

static void 
seqrec_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  if(this->status != status_reset)
  {
    /* merge and upload track after recording */
    if(track_get_size(this->recording) > 0)
    {
      track_merge(this->track, this->recording);

      if(track_editor_is_open(this->track))
        fts_send_message((fts_object_t *)this->track, fts_s_upload, 0, 0);
    }

    this->start_location = 0.0;
    this->start_time = 0.0;
    this->status = status_reset;
  }
}

static void 
seqrec_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;
  double time = 0.0;

  switch(this->status)
    {
    case status_reset:
      if(ac > 0 && fts_is_number(at))
	time = fts_get_number_float(at);

      /* set start location */
      this->start_location = time;
      
    case status_ready:
      /* remember current time as start time */
      this->start_time = fts_timebase_get_time(this->timebase);

      /* record */
      this->status = status_recording;
      
    case status_recording:
      break;
    }
}

static void 
seqrec_pause(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;
  double now = fts_timebase_get_time(this->timebase);
      
  /* set location to restart */
  this->start_location += now - this->start_time;

  /* pause */
  this->status = status_ready;
}

static void 
seqrec_record_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqrec_t *this = (seqrec_t *)o;

  if(this->status == status_recording)
    {
      /* compute current time */
      double here = fts_timebase_get_time(this->timebase) - this->start_time + this->start_location;

      if(here > 0.0)
	{
	  fts_class_t *track_type = track_get_type(this->track);

	  if(track_type == NULL || fts_get_class(at) == track_type)
	    {
	      event_t *event = (event_t *)fts_object_create(event_class, 1, at);
	  
	      /* add event to recording track */
	      track_append_event(this->recording, here, event);
	    }
	  else
	    fts_object_error(o, "Cannot record event of type %s", fts_symbol_name(fts_get_class_name(at)));
	}
    }
}

static void
seqrec_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqrec_t *this = (seqrec_t *)o;

  seqrec_stop(o, 0, 0, 0, 0);

  fts_object_release(this->track);
  this->track = (track_t *)fts_get_object(at);
  fts_object_refer(this->track);
}

/************************************************************
 *
 *  class
 *
 */

static void
seqrec_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  this->status = status_reset;
  this->timebase = fts_get_timebase();
  this->track = 0;
  this->start_location = 0.0;
  this->start_time = 0.0;

  if(ac > 0 && fts_is_a(at, track_class))
    {
      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
      
      this->recording = (track_t *)fts_object_create(track_class, 0, 0);
    }
  else
    fts_object_error(o, "argument of track required");
}

static void 
seqrec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  if(this->track)
    fts_object_release(this->track);
}

static void
seqrec_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(seqrec_t), seqrec_init, seqrec_delete);
  
  fts_class_message_varargs(cl, fts_s_start, seqrec_start);
  fts_class_message_varargs(cl, fts_new_symbol("pause"), seqrec_pause);
  fts_class_message_varargs(cl, fts_s_stop, seqrec_stop);
  
  fts_class_inlet_atom(cl, 0, seqrec_record_atom);
  
  fts_class_inlet(cl, 1, track_class, seqrec_set_reference);
}

void
seqrec_config(void)
{
  fts_class_install(fts_new_symbol("record"), seqrec_instantiate);
}
