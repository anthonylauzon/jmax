/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* The "f9" object, by Miller Puckette */
/* copyright 1994 IRCAM */

/*
  This is a score follower that runs on two timescales.  Provisional
  pitches, on the right, can trigger outputs only if the follower
  is already up to the previous note.  "real", confirmed pitches, go in
  the left. They go to a Dannenburg-style score follower.
  
  Both provisional and confirmed pitches are floating-point.
*/

#include "fts.h"
#include "seqsym.h"
#include "sequence.h"
#include "eventtrk.h"
#include "noteevt.h"

#define NNOTES 32

typedef struct _note
{
  noteevt_t *n_evt;
  struct _note *n_next;
  float n_value;
} note_t;

typedef struct _seqf9
{
  fts_object_t ob; /* object header */  
  sequence_t *sequence;
  fts_symbol_t track_name;
  eventtrk_t *track;

  char running; /* true if we're turned on */
  char atend; /* true if we've seen the last event */
  char spoofed; /* true if we've output a speculative event */
  note_t notes[NNOTES];
  note_t *tail;
  note_t *head;
  noteevt_t *firstev;
  note_t *lastoutput; /* last event that has been output */
  float hit_score_in_tune; /* parameters */
  float pitch_accuracy;
} seqf9_t;


/******************************************************************
 *
 *  utils
 *
 */
static void seqf9_refill(seqf9_t *this)
{
  if(!this->atend)
    {
      note_t *note = this->head->n_next;
      float value = this->head->n_value;
      noteevt_t *e = (noteevt_t *)event_get_next((event_t *)this->head->n_evt);
      
      while (e && note != this->tail)
	{
	  value -= noteevt_get_midi_velocity(e);
	  note->n_evt = e;
	  note->n_value = value;
	  this->head = note;
	  note = note->n_next;
	  
	  e = (noteevt_t *)event_get_next((event_t *)e);
	}
      
      if(!e) 
	this->atend = 1;
    }
}

/* advance to note pointed to by "newnote" */ 
static void 
seqf9_advanceto(seqf9_t *this, note_t *newnote)
{
  noteevt_t *e;
  note_t *wanttail;

  if(sequence_editor_is_open(this->sequence))
    {
      fts_atom_t a[1];

      fts_set_object(a, (fts_object_t *)newnote->n_evt);
      fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, 1, a);
    }

  /* send the matched note (helps monitor progress) */
  fts_outlet_float((fts_object_t *)this, 0, event_get_time((event_t *)newnote->n_evt));

  /* update value of "lastoutput" */
  this->lastoutput = newnote;
  
  if (this->atend)
    {
      if (newnote == this->head)
	{
	  post("end 1\n");

	  /* stop */
	  this->running = 0;
	}
    }
  else if (!this->atend)
    {
      /* scoop up more notes for score follower */
      wanttail = newnote + (NNOTES/2);
      if (wanttail > this->tail + NNOTES) 
	wanttail -= NNOTES;

      if (wanttail < this->tail + NNOTES/2 && wanttail > this->tail)
	{
	  this->tail = (wanttail >= this->notes + NNOTES)? (wanttail - NNOTES): wanttail;
	  seqf9_refill(this);
	}
    }
}

/******************************************************************
 *
 *  user methods
 *
 */

static void
seqf9_hit_score_in_tune(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;
  this->hit_score_in_tune = fts_get_float_arg(ac, at, 0, 0.0f);
}
     
static void
seqf9_pitch_accuracy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;
  this->pitch_accuracy = fts_get_float_arg(ac, at, 0, 0.0f); 
}
     
static void
seqf9_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;
  
  if(this->running)
    {
      fts_send_message((fts_object_t *)this->track, fts_SystemInlet, seqsym_unlock, 0, 0);
      
      this->track = 0;
      this->firstev = 0;
      
      this->running = 0;
    }
}

static void
seqf9_stable_note(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;

  if(this->running) 
    {
      float f = fts_get_float_arg(ac, at, 0, 0.0f);
      float extra_note_penalty_in_tune = fts_get_float_arg(ac, at, 1, 0.0f);
      float extra_note_penalty_out_of_tune = fts_get_float_arg(ac, at, 2, 0.0f);
      float pitch = f;
      float lastvaluebefore = 0;
      float lastvalueafter = 0;
      float bestvalue = -1000;
      int hitlast = 0;
      int goforward = 0;
      float ACCURACY = (this->pitch_accuracy > .001 ? this->pitch_accuracy : .001);
      float recip = 1./ACCURACY;
      float MATCHVALUE = this->hit_score_in_tune;
      float EXTRAVALUE = -extra_note_penalty_in_tune;
      float EXTRADETUNE = (extra_note_penalty_in_tune - extra_note_penalty_out_of_tune) * recip;
      note_t *bestnote = 0;
      note_t *current, *end;
      
      for(current = this->tail, end = this->head; 1; current = current->n_next)
	{
	  float deviation = pitch - noteevt_get_pitch(current->n_evt);
	  float v1, v2, v3;

	  if (deviation < 0) 
	    deviation = -deviation;

	  if (deviation > ACCURACY) 
	    deviation = ACCURACY;

	  v1 = current->n_value + EXTRAVALUE + EXTRADETUNE * deviation;
	  v2 =  lastvaluebefore + MATCHVALUE - (noteevt_get_midi_velocity(current->n_evt) + MATCHVALUE) * deviation * recip;
	  v3 = lastvalueafter - noteevt_get_midi_velocity(current->n_evt);
      
	  lastvaluebefore = current->n_value;
	  lastvalueafter = v1;

	  if (v2 > lastvalueafter) 
	    lastvalueafter = v2;

	  if (v3 > lastvalueafter) 
	    lastvalueafter = v3;

	  current->n_value = lastvalueafter;

	  if (lastvalueafter > bestvalue)
	    {
	      bestvalue = lastvalueafter;
	      bestnote = current;
	      if (hitlast || (!this->lastoutput && bestvalue > 0.)) goforward = 1;
	    }

	  if (current == this->lastoutput) 
	    hitlast = 1;

	  if (current == end) 
	    break;
	}
  
      if (goforward)
	{
	  seqf9_advanceto(this, bestnote);
	  this->spoofed = 0;
	}
      else if (this->spoofed && bestnote == this->lastoutput) 
	this->spoofed = 0;
    }
}

static void
seqf9_continuous_pitch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;
  float f = fts_get_number_float(at);
  note_t *note;
  double deviation;

  if(this->running && !this->spoofed && this->lastoutput != this->head) 
    {
      note = (this->lastoutput ? this->lastoutput->n_next : this->tail);
      deviation = noteevt_get_pitch(note->n_evt) - f;
      
      if (deviation < 0.) 
	deviation = -deviation;
      if (deviation <= .5 * this->pitch_accuracy)
	{
	  seqf9_advanceto(this, note);
	  this->spoofed = 1;
	}
    }
}

static void 
seqf9_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqf9_t *this = (seqf9_t *)o;
  double locate;

  seqf9_stop(o, 0, 0, 0, 0);
  
  if(ac && fts_is_number(at))
    locate = fts_get_number_float(at);
  else
    locate = 0.0;

  if(this->sequence)
    {
      eventtrk_t *track = (eventtrk_t *)sequence_get_track_by_name(this->sequence, this->track_name);
      
      if(track)
	{
	  noteevt_t *event = (noteevt_t *)eventtrk_get_event_by_time(track, locate);
	  
	  if(event)
	    {
	      fts_send_message((fts_object_t *)track, fts_SystemInlet, seqsym_lock, 0, 0);

	      if(sequence_editor_is_open(this->sequence))
		{
		  fts_atom_t a[1];
		  
		  fts_set_object(a, (fts_object_t *)event);
		  fts_client_send_message((fts_object_t *)track, seqsym_highlightEvents, 1, a);
		}
	      
	      fts_outlet_float((fts_object_t *)this, 0, event_get_time((event_t *)event));
	  
	      this->track = track;

	      this->spoofed = 0;

	      this->atend = 0;
	      this->lastoutput = 0;
	      this->firstev = event;

	      /* reset notes */
	      this->notes[0].n_evt = event;
	      this->notes[0].n_value = -noteevt_get_midi_velocity(event);
	      this->head = this->tail = this->notes;
	      seqf9_refill(this);

	      this->running = 1;
	    }
	}
    }
}

/******************************************************************
 *
 *  class
 *
 */

static void
seqf9_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;
  fts_object_t *seqobj = fts_get_object(at + 1);
  fts_symbol_t track_name = fts_get_symbol(at + 2);
  note_t *n;
  int i;

  if(fts_object_get_class_name(seqobj) == seqsym_sequence)
    this->sequence = (sequence_t *)seqobj;
  else
    this->sequence = 0;

  this->track_name = track_name;
  this->track = 0;

  /* initialize match notes */
  for (i=0, n=this->notes; i<NNOTES; i++, n++)
    n->n_next = (i == NNOTES-1 ? this->notes : n+1);

  this->hit_score_in_tune = 5.0f;
  this->pitch_accuracy = 1.0f;
  
  this->running = 0;
}

static void
seqf9_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqf9_t *this = (seqf9_t *)o;

  seqf9_stop(o, 0, 0, 0, 0);
}

static fts_status_t
seqf9_instantiate( fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init( cl, sizeof(seqf9_t), 2, 1, 0);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_s_init, seqf9_init);

  fts_method_define_varargs( cl, 0, fts_s_list, seqf9_stable_note);
  fts_method_define_varargs( cl, 1, fts_s_int, seqf9_continuous_pitch);
  fts_method_define_varargs( cl, 1, fts_s_float, seqf9_continuous_pitch);

  /* sequence reference interface methods */
  fts_method_define_varargs( cl, 0, fts_new_symbol("locate"), seqf9_locate);
  fts_method_define_varargs( cl, 0, fts_new_symbol("stop"), seqf9_stop);

  fts_method_define_varargs( cl, 0, fts_new_symbol("hit-score-in-tune"), seqf9_hit_score_in_tune);
  fts_method_define_varargs( cl, 0, fts_new_symbol("pitch-accuracy"), seqf9_pitch_accuracy);

  return fts_Success;
}

void
seqf9_config(void)
{
  fts_class_install(fts_new_symbol("seqf9"), seqf9_instantiate);
}
