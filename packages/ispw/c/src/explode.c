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


#include <stdio.h>
#include "explode.h"

static long explode_nextserial;

static fts_heap_t *explode_evt_heap;
static fts_heap_t *explode_hang_heap;
static fts_heap_t *explode_skip_heap;

static fts_symbol_t explode_symbol    = 0;
static fts_symbol_t sym_loadStart     = 0;
static fts_symbol_t sym_loadAppend    = 0;
static fts_symbol_t sym_loadEnd       = 0;
static fts_symbol_t sym_clean         = 0;
static fts_symbol_t sym_setName       = 0;
static fts_symbol_t sym_change_time   = 0;
static fts_symbol_t sym_change_event  = 0;
static fts_symbol_t sym_remove_event  = 0;
static fts_symbol_t sym_add_event     = 0;

static fts_hashtable_t explode_table;

explode_t *
explode_get_by_name(fts_symbol_t name)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(&explode_table, &k, &data))
    return (explode_t *) fts_get_pointer(&data);
  else
    return 0;
}


static int
register_explode(explode_t *this, fts_symbol_t name)
{
  fts_atom_t data, k;

  fts_set_symbol( &k, name);

  if (fts_hashtable_get(&explode_table, &k, &data))
    return 0;
  else
    {
      fts_set_pointer( &data, this);
      fts_hashtable_put( &explode_table, &k, &data);
      return 1;
    }
}


static void
forget_explode(explode_t *this, fts_symbol_t name)
{
  if (name)
    {
      fts_atom_t k;

      fts_set_symbol( &k, name);
      fts_hashtable_remove( &explode_table, &k);
    }
}

static void
init_explode_register(void)
{
  fts_hashtable_init(&explode_table, 0, FTS_HASHTABLE_MEDIUM);
}

/****************************************************************************/
/*                                                                          */
/*                      Utility functions                                   */
/*                                                                          */
/****************************************************************************/


static void
explode_doappend(explode_t *this, long int time, long int pit, long int vel, long int dur, long int chan)
{
  evt_t *cur = this->current;

  if (this->data.evt && (!cur || cur->next))
    post("oops! lost my marbles\n");
  else
    {
      evt_t *e = (evt_t *) fts_heap_alloc(explode_evt_heap);

      if (time < 0)
	time = 0;

      e->time = this->rectime = time + this->rectime;
      e->pit = pit;
      e->vel = vel;
      e->dur = dur;
      e->chan = chan;
      e->next = 0;

      if (cur)
	cur->next = e;
      else
	this->data.evt = e;

      this->current = e;
    }
}

static void
explode_at(explode_t *this, long int n1, long int n2, long int n3)
{
  evt_t *e;

  for (e = this->data.evt; e; e = e->next)
    if (e->pit == n1 && e->vel == n2 && e->chan == n3)
      break;

  this->current = e;

  if (! e)
    post("explode: note not found\n");
}


static void
explode_stop(explode_t *this)
{
  hang_t *h, *nh;
  skip_t *s, *ns;
  
  this->mode = MINIT;

  this->set1 = 0;
  this->set2 = 0;
  this->set3 = 0;

  h = this->hang; 
  while (h)
    {
      nh = h->next;
      fts_heap_free((char *) h, explode_hang_heap);
      h = nh;
    }

  s = this->skip;
  while (s)
    {
      ns = s->next;
      fts_heap_free((char *) s, explode_skip_heap);
      s = ns;
    }

  this->skip = 0;
  this->hang = 0;
  this->nhang = 0;
  this->serial = ++explode_nextserial;
}

static void
explode_clear(explode_t *this)
{
  evt_t *e, *se;

  this->serial = 0;

  explode_stop(this);

  if(explode_editor_is_open(this))
    fts_client_send_message((fts_object_t *)this, sym_clean, 0, 0);

  e = this->data.evt;
  while ( e)
    {
      se = e->next;
      fts_heap_free((char *) e, explode_evt_heap);
      e = se;
    }

  this->data.evt = 0;
  this->current = 0;
}

/***************************************************************************
 *
 *  export to standard MIDI file
 *
 */


typedef struct _notestat_
{
  long time;
  int pitch;
  int status;
  int channel;
  struct _notestat_ *next;
} notestat_t;

#define NOTESTAT_OFF 0
#define NOTESTAT_ON 1

static void
notestat_init(notestat_t *notestat, int channel, int pitch)
{
  notestat->time = 0;
  notestat->pitch = pitch;
  notestat->status = NOTESTAT_OFF;
  notestat->channel = channel;
  notestat->next = 0;  
}

static void
notestat_insert(notestat_t **list, notestat_t *this, long time)
{
  this->time = time;

  if(*list == 0 || (*list)->time > time)
    {
      this->next = *list;
      *list = this;
    }
  else
    {
      notestat_t *elem = *list;
      notestat_t *next = elem->next;
      
      while(next && next->time <= time)
	{
	  elem = next;
	  next = next->next;
	}
      
      elem->next = this;
      this->next = next;
    }
}

static void
notestat_remove(notestat_t **list, notestat_t *this)
{
  if(*list == this)
    *list = this->next;
  else
    {
      notestat_t *elem = *list;
      notestat_t *next = elem->next;
      
      while(next && next != this)
	{
	  elem = next;
	  next = next->next;
	}
      
      elem->next = next->next;
      this->next = 0;
    }
}

static void
explode_export_midifile(explode_t *this, fts_symbol_t file_name)
{
  fts_midifile_t *file = fts_midifile_open_write(file_name);
 
  if(file)
    {
      notestat_t *noteoffs = 0; /* sequence of note status */
      notestat_t notestats[17][128]; /* matrix of note status (pitch x channel) */
      notestat_t *stat;
      evt_t *event;
      int i, j;

      /* init array of note status events */
      for(i=0; i<=16; i++)
	for(j=0; j<128; j++)
	  notestat_init(&notestats[i][j], i, j);
	  
      /* start writing the file */
      fts_midifile_write_header(file, 0, 1, 1000); /* format 0, 1 track, division = 1000 */
	  
      /* write track header */
      fts_midifile_write_track_begin(file);

      /* write tempo */
      fts_midifile_write_tempo(file, 1000000); /* 60 bpm */

      event = this->data.evt;
      while(event)
	{
	  double time = event->time;
	  long time_in_ticks = fts_midifile_time_to_ticks(file, time);
	  long off_time = event->time + event->dur;
	  int channel = (event->chan >= 1)? ((event->chan <= 16)? event->chan: 16): 1;
	  int pitch = event->pit % 128;
	  int velocity = event->vel % 128;
	  
	  /* write all pending note offs before the next event */
	  stat = noteoffs;
	  while(stat && stat->time <= time)
	    {
	      long off_time_in_ticks = fts_midifile_time_to_ticks(file, stat->time);

	      /* write note off */
	      fts_midifile_write_channel_message(file, off_time_in_ticks, midi_note, stat->channel, stat->pitch, 0); 

	      /* set note to off */
	      stat->status = NOTESTAT_OFF;

	      /* remove note off event from note off track */
	      notestat_remove(&noteoffs, stat);

	      stat = noteoffs;
	    }
	  
	  /* get status corresponding to current event */
	  stat = &notestats[channel][pitch];
	  
	  /* write event */
	  if(stat->status == NOTESTAT_ON) /* if overlapping note of same pitch */
	    {
	      /* write note off of previous note */
	      fts_midifile_write_channel_message(file, time_in_ticks, midi_note, channel, pitch, 0); 
	      
	      /* remove note off event from sequence */
	      notestat_remove(&noteoffs, stat);
	      
	      /* write new note on */
	      fts_midifile_write_channel_message(file, time_in_ticks, midi_note, channel, pitch, 0); 
	    }
	  else
	    {
	      /* write note on */
	      fts_midifile_write_channel_message(file, time_in_ticks, midi_note, channel, pitch, 0); 
	      
	      /* set note status to on */
	      stat->status = NOTESTAT_ON;
	    }
	  
	  /* add corresponding note to note off track */
	  notestat_insert(&noteoffs, stat, off_time);
	  
	  /* go to next event */
	  event = event->next;
	}  
	  
      /* write all pending note offs */
      stat = noteoffs;
      while(stat)
	{
	  long time_in_ticks = fts_midifile_time_to_ticks(file, stat->time);
	  
	  /* write note off */
	  fts_midifile_write_channel_message(file, time_in_ticks, midi_note, stat->channel, stat->pitch, 0); 
	  
	  /* set note to off */
	  stat->status = NOTESTAT_OFF;
	  
	  /* remove note off event from note off track */
	  notestat_remove(&noteoffs, stat);

	  stat = noteoffs; /* get next pending note off */
	}
      
      /* write track footer */
      fts_midifile_write_track_end(file);	  

      /* close midi file */
      fts_midifile_close(file);
    }
  else
    post("explode export: cannot open file %s\n", file_name);
}

/****************************************************************************/
/*                                                                          */
/*                   Score follower utility functions                       */
/*                                                                          */
/****************************************************************************/

static void
explode_hangon(explode_t *this, evt_t *e)
{
  if (this->nhang < MAXFOLLOWHANG)
    {
      hang_t *h = (hang_t *) fts_heap_alloc(explode_hang_heap);

      this->nhang++;
      h->next = this->hang;
      h->evt = e;
      this->hang = h;
    }
}

/* send out an event matched by follower */
static void
explode_sendevt(explode_t *this, evt_t *e, int noteon)	
{
  fts_outlet_int((fts_object_t *)this, 4, e->chan);
  fts_outlet_int((fts_object_t *)this, 3, e->dur);
  fts_outlet_int((fts_object_t *)this, 2, noteon ? e->vel : 0L);
  fts_outlet_int((fts_object_t *)this, 1, e->pit);
}

static void
explode_dofollow(explode_t *this, long int n)
{
  long tquit = 0x7fffffff;
  int count = 0, gotem = 0;
  skip_t newskip, *sp, *sp2;
  int eat = 0;
  evt_t *evtwas = this->current, *e;

  newskip.next = this->skip;

  /* check against skip array */
  for (sp = &newskip; (sp2 = sp->next);)
    {
      int drop = 0;
      long pit = sp2->evt->pit;

      /* if a skip is too old (score time skipped  >= FTIME) then kill it */
      if (this->matchscoretime - sp2->evt->time >= this->ftime)
	drop = 1;

      /* here's the payoff for the skip array:
	 if the pitch is in the array eat it. */
      if (pit == n || (this->oct && (pit == n+12 || pit == n-12)))
	{
	  /* if you haven't already sent one */
	  if (!eat)	
	    {
	      /* send the match */
	      explode_sendevt(this, sp2->evt, 1);

	      /* and add to hang list */
	      explode_hangon(this, sp2->evt);
	    }

	  drop = eat = 1;
	}

      if (drop)
	{
	  sp->next = sp2->next;
	  fts_heap_free((char *) sp2, explode_skip_heap);
	}
      else
	sp = sp2;
    }

  this->skip = newskip.next;

  if (eat || !evtwas)
    return;

  /* search forward in list for matching note */
  for (; ; this->current = e->next)
    {
      e = this->current;

      /* if at end of score no match */
      if (!e)
	{
	  this->current = evtwas;
	  return;
	}

      /* interested only in channel 1 */
      if (e->chan != 1)
	continue;

      /* if already ate all the anytime-skippable notes */
      if (gotem)	
	{
	  /* and if, past that, you ate ftime into the future */
	  if (e->time > tquit)
	    {
	      /* then there is no match. */
	      this->current = evtwas;
	      return;
	    }
	}

      /* break if got the good note */
      if (e->pit == n || (this->oct && (e->pit == n+12 || e->pit == n-12)))
	break;

      /* if you got NFWD notes already */
      if (!gotem && ++count > this->nfwd)
	{
	  gotem = 1;
	  tquit = e->time + this->ftime;
	}
    }

  /* If you break out here, it means you just matched x->current. */
  this->matchscoretime = this->current->time;

  /* put note in note-hang list for matching note-off later */
  explode_hangon(this, this->current);

  /* put skipped notes into skip list; output any notes in
     other channels we
     have jumped get output */
  for (e = evtwas; e != this->current; e = e->next)
    if (e->chan == 1) 
      {
	sp = (skip_t *) fts_heap_alloc(explode_skip_heap);
	sp->evt = e;
	sp->next = this->skip;
	this->skip = sp;
      }
    else
      explode_sendevt(this, e, 1);

  /* output the winning note */
  explode_sendevt(this, this->current, 1);

  /* and set pointer to the note after that */
  this->current = this->current->next;
}



/* note-off handler for score following */
static void
explode_fnoteoff(explode_t *this, long int n)
{
  hang_t *h, *h2;
  hang_t stub;

  stub.next = this->hang;

  for (h = &stub; (h2 = h->next); h = h2)
    {
      if (h2->evt->pit == n)
	{
	  evt_t *e = h2->evt;

	  explode_sendevt(this, e, 0L);
	  h->next = h2->next;
	  fts_heap_free((char *) h2, explode_hang_heap);
	  this->nhang--;
	  break;
	}
    }

  this->hang = stub.next;
}

/************************************************************
 *
 *  user methods
 *
 */

static void
explode_append_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  int i0 = 0;
  int i1 = 0;
  int i2 = 0;
  int i3 = 0;
  int i4 = 0;

  switch(ac)
    {
    default:
    case 5:
      if(fts_is_int(at + 4))
	i4 = fts_get_int(at + 4);
    case 4:
      if(fts_is_int(at + 3))
	i3 = fts_get_int(at + 3);
    case 3:
      if(fts_is_int(at + 2))
	i2 = fts_get_int(at + 2);
    case 2:
      if(fts_is_int(at + 1))
	i1 = fts_get_int(at + 1);
    case 1:
      if(fts_is_int(at + 0))
	i0 = fts_get_int(at + 0);
    case 0:
      break;
    }

  explode_doappend(this, i0, i1, i2, i3, i4);
}


static void
explode_stop_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_stop(this);
}


static void
explode_start_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_stop(this);

  this->set1 = 0;
  this->set2 = 0;
  this->set3 = 0;

  this->current = this->data.evt;

  if (this->current)
    {
      fts_outlet_int((fts_object_t *)this, 0, this->current->time);
      this->mode = MPLAY;
    }
  else
    post("explode: no sequence\n");
}


static void
explode_record_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_clear(this);
  this->mode = MRECORD;
  this->rectime = 0;
  this->serial = 0;
}

static void
explode_follow_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);
  evt_t *e;
  
  explode_stop(this);
  this->matchscoretime = 0;

  for (e = this->data.evt; e && n--; e = e->next)
    ;

  this->current = e;

  if (this->current)
    this->mode = MFOLLOW;
  else
    post("explode: follow: out of range\n");
}


static void
explode_followat_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  int i0 = 0;
  int i1 = 0;
  int i2 = 0;
  evt_t *e;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_int(at + 2))
	i2 = fts_get_int(at + 2);
    case 2:
      if(fts_is_int(at + 1))
	i1 = fts_get_int(at + 1);
    case 1:
      if(fts_is_int(at + 0))
	i0 = fts_get_int(at + 0);
    case 0:
      break;
    }

  explode_stop(this);
  this->matchscoretime = 0;

  explode_at(this, i0, i1, i2);

  e = this->current;

  if (e)
    this->current = e->next;

  if (this->current)
    this->mode = MFOLLOW;
}

static void
explode_startat_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  int i0 = 0;
  int i1 = 0;
  int i2 = 0;
  evt_t *e;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_int(at + 2))
	i2 = fts_get_int(at + 2);
    case 2:
      if(fts_is_int(at + 1))
	i1 = fts_get_int(at + 1);
    case 1:
      if(fts_is_int(at + 0))
	i0 = fts_get_int(at + 0);
    case 0:
      break;
    }

  explode_stop(this);
  explode_at(this, i0, i1, i2);

  if ((e = this->current) && e->next)
    {
      this->mode = MPLAY;
      fts_outlet_int((fts_object_t *)this, 0, e->next->time - e->time);
      this->current = e->next;
    }
}


static void
explode_nth_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  long int n = fts_get_int_arg(ac, at, 0, 0);
  evt_t *e;
  
  for (e = this->data.evt; e && n--; e = e->next)
    ;

  if (e)
    {
      explode_sendevt(this, e, 1);
      fts_outlet_int((fts_object_t *)this, 0, e->time);
    }
}

static void
explode_params_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_int(at + 2))
	this->oct = fts_get_int(at + 2);
    case 2:
      if(fts_is_int(at + 1))
      this->ftime = fts_get_int(at + 1);
    case 1:
      if(fts_is_int(at + 0))
	this->nfwd = fts_get_int(at + 0);
    case 0:
      break;
    }
}

static void
explode_clear_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_clear(this);
}

static void
explode_next_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  
  if (this->mode == MPLAY && this->current)
    {
      evt_t *e = this->current;

      fts_outlet_int((fts_object_t *)this, 4, e->chan);
      fts_outlet_int((fts_object_t *)this, 3, e->dur);
      fts_outlet_int((fts_object_t *)this, 2, e->vel);
      fts_outlet_int((fts_object_t *)this, 1, e->pit);

      this->current = e->next;

      if (this->current)
	fts_outlet_int((fts_object_t *)this, 0, this->current->time - e->time);
      else
	explode_stop(this);
    }
  else
    post("explode: next: not playing\n");
}


static void
explode_number_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  long int n = fts_get_number_int(at);
  
  if (n < 0)
    n = 0;

  if (this->mode == MRECORD)
    {
      if (this->set3)
	explode_doappend(this, n, this->n1, this->n2, this->n3, this->n4);
      else
	{
	  if (this->n2)	
	    {
	      /* if nonzero velocity */

	      hang_t *h = (hang_t *) fts_heap_alloc(explode_hang_heap);

	      /* add short note */

	      explode_doappend(this, n, this->n1, this->n2, 10L, this->n4);

	      /* mark it hanging */

	      h->next = this->hang;
	      h->evt = this->current;
	      this->hang = h;
	    }
	  else
	    {
	      hang_t *h, *h2;
	      hang_t stub;

	      stub.next = this->hang;
	      this->rectime += n;

	      for (h = &stub; (h2 = h->next); h = h2)
		{
		  if (h2->evt->pit == this->n1)
		    {
		      evt_t *e = h2->evt;

		      if ((e->dur = this->rectime - e->time) < 0)
			e->dur = 0;

		      h->next = h2->next;
		      fts_heap_free((char *) h2, explode_hang_heap);
		      break;
		    }
		}
	      this->hang = stub.next;
	    }
	}
    }
  else if (this->mode == MFOLLOW)
    {
      if (this->set2)
	{
	  if (this->n2)
	    explode_dofollow(this, this->n1);
	  else
	    explode_fnoteoff(this, this->n1);
	}
      else if (this->n2)
	explode_dofollow(this, this->n1);
    }
}

static void
explode_number_1_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n1 = fts_get_number_int(at);
  this->set1 = 1;
}

static void
explode_number_2_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n2 = fts_get_number_int(at);
  this->set2 = 1;
}

static void
explode_number_3_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n3 = fts_get_number_int(at);
  this->set3 = 1;
}

static void
explode_number_4_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n4 = fts_get_number_int(at);
}

static void
explode_list_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 5 && fts_is_number(&at[4]))
    explode_number_4_mth(o, winlet, s, 1, at + 4);

  if (ac >= 4 && fts_is_number(&at[3]))
    explode_number_3_mth(o, winlet, s, 1, at + 3);

  if (ac >= 3 && fts_is_number(&at[2]))
    explode_number_2_mth(o, winlet, s, 1, at + 2);

  if (ac >= 2 && fts_is_number(&at[1]))
    explode_number_1_mth(o, winlet, s, 1, at + 1);

  if (ac >= 1 && fts_is_number(&at[0]))
    explode_number_mth(o, winlet, s, 1, at);
}

static void
explode_export(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  fts_symbol_t file_name = fts_get_symbol_arg(ac, at, 0, 0);

  if(!file_name)
    {
      if(this->data.name)
	{
	  char s[1024];
	  snprintf(s, 1024, "%s.mid", this->data.name);
	  file_name = fts_new_symbol(s);
	}
      else
	file_name = fts_new_symbol("explode.mid");
    }
  
  explode_export_midifile(this, file_name);
}

/****************************************************
 *
 *  class
 *
 */

static void
explode_init_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 0, 0);

  this->n1 = 0;
  this->n2 = 0;
  this->n3 = 0;
  this->n4 = 1;

  this->set1 = 0;
  this->set2 = 0;
  this->set3 = 0;

  this->mode = MINIT;

  this->oct = 0;
  this->ftime = FTIME;
  this->nfwd = NFWD;
  this->skip = 0;
  this->hang = 0;
  this->serial = ++explode_nextserial;
  this->data.evt = 0;

  if (name)
    if (register_explode(this, name))
      this->data.name = name;
    else
      {
	post("explode: %s: named already exists\n", name);
	this->data.name = 0;
      }
  else
    this->data.name = 0;
}

static void
explode_delete_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_clear(this);
  forget_explode(this, this->data.name);

  /*  if(fts_object_has_id(o))
      fts_client_send_message(o, fts_s_destroyEditor, 0, 0);*/
}

static void 
explode_put_name_daemon(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  explode_t *this = (explode_t *)obj;

  if (this->data.name == 0)
    {
      fts_atom_t av[2];
      fts_symbol_t name;

      name = fts_get_symbol(value);

      if (register_explode(this, name))
	this->data.name = name;
      else
	{
	  post("explode: %s: named already exists\n", name);
	  this->data.name = 0;

	  return;
	}
      
      /* change its description */

      fts_set_symbol(&av[0], fts_new_symbol("explode"));
      fts_set_symbol(&av[1], name);
      
      fts_object_set_description(obj, 2, av);
    }
}

static void
explode_append_event(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  evt_t *cur = this->current;

  if (this->data.evt && (!cur || cur->next))
    post("oops! lost my marbles\n");
  else
    {
      evt_t *e = (evt_t *) fts_heap_alloc(explode_evt_heap);

      e->time = fts_get_int(&at[0]);
      e->pit  = fts_get_int(&at[1]);
      e->vel  = fts_get_int(&at[2]);
      e->dur  = fts_get_int(&at[3]);
      e->chan = fts_get_int(&at[4]);

      e->next = 0;
      this->rectime = e->time;

      if (cur)
	cur->next = e;
      else
	this->data.evt = e;

      this->current = e;
    }
}


static void
explode_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess;
  evt_t *e;

  /* dump restore message */
  fts_dumper_send(dumper, fts_s_restore, 0, 0);

  /* dump the events */
  for (e = this->data.evt ; e ; e = e->next)
    {
      /* get new dumper message */
      mess = fts_dumper_message_new(dumper, fts_s_append);

      /* put event */
      fts_message_append_int(mess, e->time);
      fts_message_append_int(mess, e->pit);
      fts_message_append_int(mess, e->vel);
      fts_message_append_int(mess, e->dur);
      fts_message_append_int(mess, e->chan);

      /* send dumper message */
      fts_dumper_message_send(dumper, mess);
    }

  /* dump stop message */
  fts_dumper_send(dumper, fts_s_stop, 0, 0);
}

static void explode_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *) o;
  FILE *file;
  int x, y, w, font_index;
  evt_t *e;
  fts_atom_t a;
  long prevtime;

  file = (FILE *)fts_get_pointer( at);

  fprintf( file, "#N explode");

  if ( this->data.name)
    fprintf( file, " %s", this->data.name); 

  fprintf( file, ";\n");

  fprintf( file, "#X restore;\n");

  prevtime = 0;
  for (e = this->data.evt ; e ; e = e->next)
    {
      fprintf( file, "#X %ld %ld %ld %ld %ld 1 0 0;\n", e->time-prevtime, e->pit, e->vel, e->dur, e->chan);
      prevtime = e->time;
    }

  fprintf( file, "#X stop;\n");

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P newobj %d %d %d %d explode", x, y, w, font_index);

  if ( this->data.name)
    fprintf( file, " %s", this->data.name); 

  fprintf( file, ";\n");
}

static void
explode_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
  fts_send_message( o, fts_s_upload, 0, 0);
}

static void
explode_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  explode_set_editor_close(this);
}
static void
explode_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  if(explode_editor_is_open(this))
    {
      explode_set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
    }
}
/*
 * Two fts_data_t functions to add and remove elements from a sequence.
 * No consistency checks in these two functions, we assume the UI
 * take care of it.
 */

static void
explode_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* Only argument, the zero based index of the event to suppress */
  explode_t *this = (explode_t *)o;
  int delete = fts_get_int(at);
  evt_t **pe, *e;			/* indirect precursor */

  /* Find the position */
  pe = &(this->data.evt);
  while (delete > 0)
    {
      pe = &( (*pe)->next);
      delete--;
    }

  /* Remove it from the list and free it */
  e = *pe;
  (*pe)  = (*pe)->next;
  fts_heap_free((char *) e, explode_evt_heap);
}

static void
explode_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* Arguments: 
   * the zero based index
   * the event paramter in order <time> <pit> <vel> <dur> <chan>
   *
   * The zero based index is used because there may be multiple
   * events at the same time, so the editor can use the index
   * as event id.
   */

  explode_t *this = (explode_t *)o;
  int add;
  evt_t **pe, *e;			/* indirect precursor */

  add = fts_get_int(&at[0]);	/* add index */

  e = (evt_t *) fts_heap_alloc(explode_evt_heap);

  e->time = fts_get_int(&at[1]);
  e->pit  = fts_get_int(&at[2]);
  e->vel  = fts_get_int(&at[3]);
  e->dur  = fts_get_int(&at[4]);
  e->chan = fts_get_int(&at[5]);

  /* Look for the good position */
  pe = &(this->data.evt);
  while (add > 0)
    {
      pe = &( (*pe)->next);
      add--;
    }

  /* Insert it */
  e->next = (*pe);
  (*pe)  = e;

  return;
}

static void
explode_change(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* Arguments: 
   * the zero based index
   * the event paramter in order <time> <pit> <vel> <dur> <chan>
   *
   * The zero based index is used because there may be multiple
   * events at the same time, so the editor can use the index
   * as event id.
   *
   * The function may need to move the event.
   */

  explode_t *this = (explode_t *)o;
  int change = fts_get_int(&at[0]);	/* change index */
  evt_t **pe, *e;			/* indirect precursor */

  /* First, found the event and remote it from the list */
  pe = &(this->data.evt);
  while (change > 0)
    {
      pe = &( (*pe)->next);
      change--;
    }

  e = *pe;

  /* Change the event  */
  e->time = fts_get_int(&at[1]);
  e->pit  = fts_get_int(&at[2]);
  e->vel  = fts_get_int(&at[3]);
  e->dur  = fts_get_int(&at[4]);
  e->chan = fts_get_int(&at[5]);

  return;
}

static void
explode_change_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* Arguments: 
   * the zero based index
   * the event paramter in order <time> <pit> <vel> <dur> <chan>
   *
   * The zero based index is used because there may be multiple
   * events at the same time, so the editor can use the index
   * as event id.
   *
   * The function may need to move the event.
   */

  explode_t *this = (explode_t *)o;
  int change = fts_get_int(&at[0]);	/* change index */
  evt_t **pe, *e;			/* indirect precursor */

  /* First, found the event and remote it from the list */
  pe = &(this->data.evt);
  while (change > 0)
    {
      pe = &( (*pe)->next);
      change--;
    }

  e = *pe;
  (*pe)  = (*pe)->next;

  /* Change the event  */
  e->time = fts_get_int(&at[1]);

  /* Find the correct new position */
  pe = &(this->data.evt); 
  while (*pe && ((*pe)->time <= e->time))
    pe = &( (*pe)->next);

  /* Insert it */
  e->next = (*pe);
  (*pe)  = e;

  return;
}

static void
explode_upload(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  evt_t *e;
  fts_atom_t args[5];

  if (this->data.name)
    {
      fts_set_symbol(&(args[0]), this->data.name);
      fts_client_send_message((fts_object_t *)this, sym_setName, 1, args);
    }

  fts_client_send_message((fts_object_t *)this, sym_loadStart, 0, 0);

  for ( e = this->data.evt; e; e = e->next)
    {
      fts_set_int( &(args[0]), e->time);
      fts_set_int( &(args[1]), e->pit);
      fts_set_int( &(args[2]), e->vel);
      fts_set_int( &(args[3]), e->dur);
      fts_set_int( &(args[4]), e->chan);
      
      fts_client_send_message((fts_object_t *)this, sym_loadAppend, 5, args);
    }
  fts_client_send_message((fts_object_t *)this, sym_loadEnd, 0, 0);
}

/****************************************************************************/
/*                                                                          */
/*                      Instantiation and config                            */
/*                                                                          */
/****************************************************************************/


#define MAXAPPENDARGS 16

static void
explode_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(explode_t), explode_init_mth, explode_delete_mth);
  
  /*fts_class_method_varargs(cl, fts_s_upload, explode_upload);*/
  /*fts_class_method_varargs(cl, fts_s_append, explode_append_event);*/
  fts_class_method_varargs(cl, fts_s_dump, explode_dump);
  fts_class_method_varargs(cl, fts_s_save_dotpat, explode_save_dotpat); 

  /* graphical editor */
  fts_class_method_varargs(cl, fts_s_openEditor, explode_open_editor);
  fts_class_method_varargs(cl, fts_s_destroyEditor, explode_destroy_editor);
  fts_class_method_varargs(cl, fts_s_closeEditor, explode_close_editor);
  
  fts_class_method_varargs(cl, sym_add_event, explode_add);
  fts_class_method_varargs(cl, sym_remove_event, explode_remove);
  fts_class_method_varargs(cl, sym_change_event, explode_change);
  fts_class_method_varargs(cl, sym_change_time, explode_change_time);

  fts_class_method_varargs(cl, fts_s_clear, explode_clear_mth);
  fts_class_method_varargs(cl, fts_s_start, explode_start_mth);
  fts_class_method_varargs(cl, fts_s_stop, explode_stop_mth);
  fts_class_method_varargs(cl, fts_new_symbol("record"), explode_record_mth); 
  fts_class_method_varargs(cl, fts_s_restore, explode_record_mth);

  fts_class_method_varargs(cl, fts_s_next, explode_next_mth);
  fts_class_method_varargs(cl, fts_new_symbol("nth"), explode_nth_mth);
  fts_class_method_varargs(cl, fts_s_append, explode_append_mth);

  fts_class_method_varargs(cl, fts_new_symbol("follow"), explode_follow_mth);
  fts_class_method_varargs(cl, fts_new_symbol("startat"), explode_startat_mth);
  fts_class_method_varargs(cl, fts_new_symbol("followat"), explode_followat_mth);
  fts_class_method_varargs(cl, fts_new_symbol("params"), explode_params_mth);

  /* export standard MIDI file */
  fts_class_method_varargs(cl, fts_s_export, explode_export);

  fts_class_inlet_number(cl, 0, explode_number_mth);
  fts_class_inlet_number(cl, 1, explode_number_1_mth);
  fts_class_inlet_number(cl, 2, explode_number_2_mth);
  fts_class_inlet_number(cl, 3, explode_number_3_mth);
  fts_class_inlet_number(cl, 4, explode_number_4_mth);
  fts_class_inlet_varargs(cl, 0, explode_list_mth);

  fts_class_outlet_int(cl, 0);
  fts_class_outlet_int(cl, 1);
  fts_class_outlet_int(cl, 2);
  fts_class_outlet_int(cl, 3);
  fts_class_outlet_int(cl, 4);

  /* add the rename daemon */
  fts_class_add_daemon(cl, obj_property_put, fts_s_name, explode_put_name_daemon);
}

void explay_config(void);

void
explode_config(void)
{
  explode_evt_heap = fts_heap_new(sizeof(evt_t));
  explode_skip_heap = fts_heap_new(sizeof(skip_t));
  explode_hang_heap = fts_heap_new(sizeof(hang_t));

  explode_symbol = fts_new_symbol("explode");
  sym_loadStart = fts_new_symbol("loadStart");
  sym_loadAppend = fts_new_symbol("loadAppend");
  sym_loadEnd = fts_new_symbol("loadEnd");
  sym_clean = fts_new_symbol("clean");
  sym_setName = fts_new_symbol("setName");
  sym_change_time = fts_new_symbol("change_time");
  sym_change_event = fts_new_symbol("change_event");
  sym_remove_event = fts_new_symbol("remove_event");
  sym_add_event = fts_new_symbol("add_event");

  fts_class_install(explode_symbol, explode_instantiate);

  init_explode_register();

  explay_config();
}
