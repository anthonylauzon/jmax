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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include <fts/fts.h>
#include "seqsym.h"
#include "sequence.h"
#include "track.h"
#include "event.h"
#include "track.h"

typedef struct _seqrec_
{
  fts_object_t head; /* sequence reference object */
  fts_timebase_t *timebase;
  track_t *track;
  fts_class_t *class;
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
      track_merge(this->track, this->recording);
      
      if(fts_object_has_id((fts_object_t *)this->track))
	fts_send_message((fts_object_t *)this->track, fts_SystemInlet, fts_s_upload, 0, 0);

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
      if(ac && fts_is_number(at))
	time = fts_get_number_float(at);

      /* set start location  */
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
      
  /* set location to restart*/
  this->start_location += now - this->start_time;

  /* pause */
  this->status = status_ready;
}

static void 
seqrec_record(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqrec_t *this = (seqrec_t *)o;

  if(this->start_time != 0.0)
    {
      /* compute current time */
      double here = fts_timebase_get_time(this->timebase) - this->start_time + this->start_location;

      if(here > 0.0)
	{
	  event_t *event;
	  
	  /* create event */
	  if(this->class)
	    {
	      fts_object_t *obj = fts_object_create(this->class, ac, at);
	      fts_atom_t a[1];
	      
	      fts_set_object(a, obj);
	      event = (event_t *)fts_object_create(event_class, 1, a);
	    }
	  else
	    event = (event_t *)fts_object_create(event_class, 1, at);
	  
	  /* add event to recording track */
	  track_append_event(this->recording, here, event);
	}
    }
}

static void
seqrec_set_reference(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqrec_t *this = (seqrec_t *)o;

  seqrec_stop(o, 0, 0, 0, 0);

  fts_object_release(this->track);
  this->track = track_atom_get(at);
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

  ac--;
  at++;

  this->status = status_reset;
  this->timebase = fts_get_timebase();
  this->track = 0;
  this->start_location = 0.0;
  this->start_time = 0.0;

  if(track_atom_is(at))
    {
      fts_symbol_t type;
      fts_atom_t a;

      this->track = (track_t *)fts_get_object(at);
      fts_object_refer(this->track);
      
      type = track_get_type(this->track);

      if(type == seqsym_int || type == seqsym_float || type == seqsym_symbol)
	this->class = 0;
      else
	this->class = fts_class_get_by_name(type);

      fts_set_symbol(&a, type);
      this->recording = (track_t *)fts_object_create(track_class, 1, &a);
    }
  else
    fts_object_set_error(o, "Argument of event track required");
}

static void 
seqrec_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqrec_t *this = (seqrec_t *)o;

  if(this->track)
    fts_object_release(this->track);
}

static fts_status_t
seqrec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(seqrec_t), 2, 0, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqrec_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqrec_delete);
  
  fts_method_define_varargs(cl, 0, fts_new_symbol("start"), seqrec_start);
  fts_method_define_varargs(cl, 0, fts_new_symbol("pause"), seqrec_pause);
  fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqrec_stop);
  
  fts_method_define_varargs(cl, 0, fts_s_int, seqrec_record);
  fts_method_define_varargs(cl, 0, fts_s_float, seqrec_record);
  fts_method_define_varargs(cl, 0, fts_s_symbol, seqrec_record);
  fts_method_define_varargs(cl, 0, fts_s_list, seqrec_record);
  
  fts_method_define_varargs(cl, 1, seqsym_track, seqrec_set_reference);
  
  return fts_Success;
}

void
seqrec_config(void)
{
  fts_metaclass_install(fts_new_symbol("seqrec"), seqrec_instantiate, fts_arg_type_equiv);
}
