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
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */

#include "fts.h"
#include "seqsym.h"
#include "sequence.h"
#include "eventtrk.h"
#include "noteevt.h"

static fts_heap_t *seqplode_skip_heap = 0;

/* default values for f_nfwd and f_ftime */
#define NFWD 2 
#define FTIME 150

typedef struct _skip
{
  noteevt_t *evt;
  struct _skip *next;
} skip_t;

typedef struct 
{
  fts_object_t obj;
  sequence_t *sequence;
  int index;
  eventtrk_t *track;
  noteevt_t *event; /* current event */

  int matchscoretime;
  char oct;
  int nfwd;
  int ftime;
  skip_t *skip;
} seqplode_t;

/****************************************************************************
 *
 *  user methods
 *
 */

static void
seqplode_params(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplode_t *this = (seqplode_t *)o;

  switch(ac)
    {
    default:
    case 3:
      this->oct = fts_get_int(at + 2);
    case 2:
      this->ftime = fts_get_int(at + 1);
    case 1:      
      this->nfwd = fts_get_int(at + 0);
    case 0:
      break;
    }
}

/* int/float method, inlet 0 */
static void
seqplode_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplode_t *this = (seqplode_t *)o;
  noteevt_t *evtwas = this->event;
  int tquit = 0x7fffffff;
  int count = 0, gotem = 0;
  skip_t newskip, *sp, *sp2;
  int eat = 0;
  noteevt_t *e;
  int pitch = fts_get_number_int(at);
  
  if(pitch < 0)
    pitch = 0;

  newskip.next = this->skip;

  /* check against skip array */
  for(sp=&newskip; (sp2=sp->next);)
    {
      int drop = 0;
      int pit = noteevt_get_pitch(sp2->evt);

      /* if a skip is too old (score time skipped  >= FTIME) then kill it */
      if (this->matchscoretime - event_get_time((event_t *)sp2->evt) >= this->ftime)
	drop = 1;

      /* here's the payoff for the skip array: if the pitch is in the array eat it. */
      if (pit == pitch || (this->oct && (pit == pitch + 12 || pit == pitch - 12)))
	{
	  /* if you haven't already sent one */
	  if (!eat)	
	    {
	      if(sequence_editor_is_open(this->sequence))
		{
		  fts_atom_t a[1];
		  
		  fts_set_object(a, (fts_object_t *)sp2->evt);
		  fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, 1, a);
		}
	      
	      fts_outlet_float((fts_object_t *)this, 0, event_get_time((event_t *)sp2->evt)); /* send the match */
	    }

	  drop = eat = 1;
	}

      if (drop)
	{
	  sp->next = sp2->next;
	  fts_heap_free((char *) sp2, seqplode_skip_heap);
	}
      else
	sp = sp2;
    }

  this->skip = newskip.next;

  if (eat || !evtwas)
    return;

  /* search forward in list for matching note */
  for (; ; this->event=(noteevt_t *)event_get_next((event_t *)e))
    {
      e = this->event;

      /* if at end of score no match */
      if(!e)
	{
	  this->event = evtwas;
	  return;
	}

      /* if already ate all the anytime-skippable notes */
      if(gotem)	
	{
	  /* and if, past that, you ate ftime into the future */
	  if (event_get_time((event_t *)e) > tquit)
	    {
	      /* then there is no match. */
	      this->event = evtwas;
	      return;
	    }
	}

      /* break if got the good note */
      if(noteevt_get_pitch(e) == pitch || (this->oct && (noteevt_get_pitch(e) == pitch + 12 || noteevt_get_pitch(e) == pitch - 12)))
	break;

      /* if you got NFWD notes already */
      if(!gotem && ++count > this->nfwd)
	{
	  gotem = 1;
	  tquit = event_get_time((event_t *)e) + this->ftime;
	}
    }

  /* If you break out here, it means you just matched x->event. */
  this->matchscoretime = event_get_time((event_t *)this->event);

  /* put skipped notes into skip list; output any notes in other channels we have jumped get output */
  for(e=evtwas; e!=this->event; e=(noteevt_t *)event_get_next((event_t *)e))
    {
      sp = (skip_t *) fts_heap_alloc(seqplode_skip_heap);
      sp->evt = e;
      sp->next = this->skip;
      this->skip = sp;
    }

  /* output the winning note */
  if(sequence_editor_is_open(this->sequence))
    {
      fts_atom_t a[1];
      
      fts_set_object(a, (fts_object_t *)this->event);
      fts_client_send_message((fts_object_t *)this->track, seqsym_highlightEvents, 1, a);
    }
  
  fts_outlet_float((fts_object_t *)this, 0, event_get_time((event_t *)this->event));

  /* and set pointer to the note after that */
  this->event = (noteevt_t *)event_get_next((event_t *)this->event);
}

static void
seqplode_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplode_t *this = (seqplode_t *)o;
  
  if(this->event)
    {
      skip_t *skip = this->skip;

      track_unlock((track_t *)this->track);

      this->track = 0;
      this->event = 0;
      
      while(skip)
	{
	  skip_t *next = skip->next;
	  
	  fts_heap_free(skip, seqplode_skip_heap);
	  skip = next;
	}
      
      this->skip = 0;
    }
}

static void 
seqplode_locate(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  seqplode_t *this = (seqplode_t *)o;
  double locate;

  seqplode_stop(o, 0, 0, 0, 0);
  
  if(ac && fts_is_number(at))
    locate = fts_get_number_float(at);
  else
    locate = 0.0;

  if(this->sequence)
    {
      eventtrk_t *track = (eventtrk_t *)sequence_get_track_by_index(this->sequence, this->index);
      
      if(track && eventtrk_get_type(track) == seqsym_noteevt)
	{
	  noteevt_t *event = (noteevt_t *)eventtrk_get_event_by_time(track, locate);
	  
	  if(event)
	    {
	      track_lock((track_t *)this->track);
	      fts_outlet_float((fts_object_t *)this, 0, event_get_time((event_t *)event));
	  
	      this->track = track;
	      this->event = event;
	    }
	}
    }
}

/****************************************************************************
 *
 *  class
 *
 */

static void
seqplode_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplode_t *this = (seqplode_t *)o;
  fts_object_t *seqobj = fts_get_object(at + 1);
  int index = fts_get_int(at + 2);

  if(fts_object_get_class_name(seqobj) == seqsym_sequence)
    this->sequence = (sequence_t *)seqobj;
  else
    this->sequence = 0;

  this->index = index;
  this->track = 0;

  this->oct = 0;
  this->nfwd = NFWD;
  this->ftime = FTIME;
  this->skip = 0;
}

static void
seqplode_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  seqplode_t *this = (seqplode_t *)o;

  seqplode_stop(o, 0, 0, 0, 0);
}

static fts_status_t
seqplode_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 2 && fts_is_symbol(at) && fts_is_object(at + 1) && fts_is_int(at + 2))
    {
      fts_symbol_t a[1];
      
      /* initialize the class */
      fts_class_init(cl, sizeof(seqplode_t), 1, 1, 0); 
      
      /* define the system methods */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, seqplode_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, seqplode_delete);
      
      /* Seqplode number methods */
      fts_method_define_varargs(cl, 0, fts_s_int, seqplode_number);
      fts_method_define_varargs(cl, 0, fts_s_float, seqplode_number);
      
      /* lists */
      fts_method_define_varargs(cl, 0, fts_new_symbol("params"), seqplode_params);
      
      /* sequence reference interface methods */
      /*fts_method_define_varargs(cl, 0, fts_new_symbol("start"), seqplode_start);*/
      fts_method_define_varargs(cl, 0, fts_new_symbol("stop"), seqplode_stop);
      /*fts_method_define_varargs(cl, 0, fts_new_symbol("continue"), seqplode_continue);*/
      fts_method_define_varargs(cl, 0, fts_new_symbol("locate"), seqplode_locate);
      /*fts_method_define_varargs(cl, 0, fts_new_symbol("sync"), seqplode_sync);*/
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;    
}

void
seqplode_config(void)
{
  seqplode_skip_heap = fts_heap_new(sizeof(skip_t));

  fts_metaclass_install(fts_new_symbol("seqplode"), seqplode_instantiate, fts_arg_type_equiv);
}
