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
#include "fts.h"
#include "explode.h"

/* explode data function keys */

#define EXPLODE_LOAD_START    1
#define EXPLODE_LOAD_APPEND   2
#define EXPLODE_LOAD_END      3
#define EXPLODE_CLEAN         4
#define EXPLODE_APPEND        5
#define EXPLODE_REMOTE_ADD    6
#define EXPLODE_REMOTE_REMOVE 7
#define EXPLODE_REMOTE_CHANGE 8
#define EXPLODE_REMOTE_CHANGE_TIME 9
#define EXPLODE_REMOTE_NAME   10

static long explode_nextserial;

static fts_data_class_t *explode_data_class = 0;

static fts_heap_t *explode_evt_heap;
static fts_heap_t *explode_hang_heap;
static fts_heap_t *explode_skip_heap;

/****************************************************************************/
/*                                                                          */
/*                      Naming handling                                     */
/*                                                                          */
/****************************************************************************/

/* if names are duplicated, the explode is not registered, and
   cannot be accessed by name; the internal symbol is set to zero
   Question: should the explode share data like table ?
*/

static fts_hash_table_t explode_table;

explode_t *
explode_get_by_name(fts_symbol_t name)
{
  fts_atom_t data;

  if (fts_hash_table_lookup(&explode_table, name, &data))
    return (explode_t *) fts_get_ptr(&data);
  else
    return 0;
}


static int
register_explode(explode_t *this, fts_symbol_t name)
{
  fts_atom_t data;

  if (fts_hash_table_lookup(&explode_table, name, &data))
    return 0;
  else
    {
      fts_set_ptr(&data, this);
      fts_hash_table_insert(&explode_table, name, &data);
      return 1;
    }
}


static void
forget_explode(explode_t *this, fts_symbol_t name)
{
  if (name)
    {
      fts_hash_table_remove(&explode_table, name);
    }
}

static void
init_explode_register(void)
{
  fts_hash_table_init(&explode_table);


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

      if (fts_data_is_exported((fts_data_t *) &(this->data)))
	{
	  fts_atom_t args[5];

	  fts_set_long( &(args[0]), e->time);
	  fts_set_long( &(args[1]), e->pit);
	  fts_set_long( &(args[2]), e->vel);
	  fts_set_long( &(args[3]), e->dur);
	  fts_set_long( &(args[4]), e->chan);
	  
	  fts_data_remote_call((fts_data_t *) &(this->data), EXPLODE_APPEND, 5, args);
	}
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

  if (fts_data_is_exported((fts_data_t *) &(this->data)))
    fts_data_remote_call((fts_data_t *) &(this->data), EXPLODE_CLEAN, 0, 0);

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



/****************************************************************************/
/*                                                                          */
/*                      User methods: Event handling & connect              */
/*                                                                          */
/****************************************************************************/



/* "append" method, inlet 0, */

static void
explode_append_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_doappend(this,
		   fts_get_long(&at[0]),
		   fts_get_long(&at[1]),
		   fts_get_long(&at[2]),
		   fts_get_long(&at[3]),
		   fts_get_long(&at[4]));
}


/* "stop" method, inlet 0; also called around  */

static void
explode_stop_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_stop(this);
}


/* "start" method, inlet 0 */

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


/* "record" method, inlet 0 */

static void
explode_record_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_clear(this);
  this->mode = MRECORD;
  this->rectime = 0;
  this->serial = 0;
}

/* "follow" method, inlet 0 */

static void
explode_follow_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  long int n = fts_get_long_arg(ac, at, 0, 0);
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


/* "followat" method, inlet 0 */

static void
explode_followat_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  evt_t *e;

  explode_stop(this);
  this->matchscoretime = 0;

  explode_at(this, fts_get_long(at + 0), fts_get_long(at + 1), fts_get_long(at + 2));

  e = this->current;

  if (e)
    this->current = e->next;

  if (this->current)
    this->mode = MFOLLOW;
}

/* "startat" method, inlet 0 */

static void
explode_startat_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  evt_t *e;

  explode_stop(this);
  explode_at(this, fts_get_long(at + 0), fts_get_long(at + 1), fts_get_long(at + 2));

  if ((e = this->current) && e->next)
    {
      this->mode = MPLAY;
      fts_outlet_int((fts_object_t *)this, 0, e->next->time - e->time);
      this->current = e->next;
    }
}


/* "nth" method, inlet 0 */

static void
explode_nth_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  long int n = fts_get_long_arg(ac, at, 0, 0);
  evt_t *e;
  
  for (e = this->data.evt; e && n--; e = e->next)
    ;

  if (e)
    {
      explode_sendevt(this, e, 1);
      fts_outlet_int((fts_object_t *)this, 0, e->time);
    }
}

/* "params" method, inlet 0 ??? */

static void
explode_params_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->nfwd  = fts_get_long(at + 0);
  this->ftime = fts_get_long(at + 1);
  this->oct   = fts_get_long(at + 2);
}

/* "clear" methods, inlet 0.
   Also called around, should be splitted in a function plus a method */

static void
explode_clear_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  explode_clear(this);
}

/* "next" method, inlet 0 */

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


/* int/float method, inlet 0 */

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

/* int/float method, inlet 1 */

static void
explode_number_1_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n1 = fts_get_number_int(at);
  this->set1 = 1;
}

/* int/float method, inlet 2 */

static void
explode_number_2_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n2 = fts_get_number_int(at);
  this->set2 = 1;
}

/* int/float method, inlet 3 */

static void
explode_number_3_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n3 = fts_get_number_int(at);
  this->set3 = 1;
}

/* int/float method, inlet 4 */

static void
explode_number_4_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  this->n4 = fts_get_number_int(at);
}

/* list method, inlet 0 */

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


/****************************************************************************/
/*                                                                          */
/*                      System methods: @@@@@@@@                            */
/*                                                                          */
/****************************************************************************/



/****************************************************************************/
/*                                                                          */
/*                      System methods: init, delete                        */
/*                                                                          */
/****************************************************************************/

/* init method, system inlet */

static void
explode_init_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  fts_symbol_t name = fts_get_symbol_arg(ac, at, 1, 0);

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
	post("explode: %s: named already exists\n", fts_symbol_name(name));
	this->data.name = 0;
      }
  else
    this->data.name = 0;

  fts_data_init( (fts_data_t *) &(this->data), explode_data_class);
}

/* delete method, system inlet */

static void
explode_delete_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;

  fts_data_delete((fts_data_t *) &(this->data));
  explode_clear(this);
  forget_explode(this, this->data.name);
}


/* Daemon of the "name" property; you can change the name of
   an explode only if the name was not set before */

static void explode_put_name_daemon(fts_daemon_action_t action,
			     fts_object_t *obj,
			     fts_symbol_t property,
			     fts_atom_t *value)
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
	  post("explode: %s: named already exists\n", fts_symbol_name(name));
	  this->data.name = 0;

	  return;
	}
      
      /* change its description */

      fts_set_symbol(&av[0], fts_new_symbol("explode"));
      fts_set_symbol(&av[1], name);
      
      fts_object_set_description(obj, 2, av);
    }
}

/****************************************************************************/
/*                                                                          */
/*                      Bmax Saving and reloading                           */
/*                                                                          */
/****************************************************************************/

/* We have special methods for bmax saving and loading; it work more or less
   as the .pat saving, but the time is saved as absolute, i.e. the precise
   content of the event structure is directly saved and restored.

   We use the restore message and the stop message (on system inlet) to initialize
   and end the loading, and a special append message on the system inlet to 
   append an event
   */


static void
explode_append_from_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  evt_t *cur = this->current;

  if (this->data.evt && (!cur || cur->next))
    post("oops! lost my marbles\n");
  else
    {
      evt_t *e = (evt_t *) fts_heap_alloc(explode_evt_heap);

      e->time = fts_get_long(&at[0]);
      e->pit  = fts_get_long(&at[1]);
      e->vel  = fts_get_long(&at[2]);
      e->dur  = fts_get_long(&at[3]);
      e->chan = fts_get_long(&at[4]);

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
explode_save_bmax(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  explode_t *this = (explode_t *)o;
  fts_bmax_file_t *f = (fts_bmax_file_t *) fts_get_ptr(at);
  evt_t *e;

  /* Code the restore message */

  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_restore, 0);

  /* Loop on all the events */

  for (e = this->data.evt ; e ; e = e->next)
    {
      /* Push directly the append argument in reverse order
	 e->chan,  e->dur, e->vel, e->pit, e->time 
	 */

      fts_bmax_code_push_int(f, e->chan);
      fts_bmax_code_push_int(f, e->dur);
      fts_bmax_code_push_int(f, e->vel);
      fts_bmax_code_push_int(f, e->pit);
      fts_bmax_code_push_int(f, e->time);

      /* Code the message */

      fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_append, 5);

      /* Then, pop the arguments */

      fts_bmax_code_pop_args(f, 5);
    }

  /* Code the stop message */

  fts_bmax_code_obj_mess(f, fts_SystemInlet, fts_s_stop, 0);
}

/*
 * Two fts_data_t functions to add and remove elements from a sequence.
 * No consistency checks in these two functions, we assume the UI
 * take care of it.
 */

static void explode_remote_remove( fts_data_t *d, int ac, const fts_atom_t *at)
{
  /* Only argument, the zero based index of the event to suppress */

  explode_data_t *data = (explode_data_t *)d;
  int delete = fts_get_int(at);
  evt_t **pe, *e;			/* indirect precursor */

  /* Find the position */

  pe = &(data->evt);
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

static void explode_remote_add( fts_data_t *d, int ac, const fts_atom_t *at)
{
  /* Arguments: 
   * the zero based index
   * the event paramter in order <time> <pit> <vel> <dur> <chan>
   *
   * The zero based index is used because there may be multiple
   * events at the same time, so the editor can use the index
   * as event id.
   */

  explode_data_t *data = (explode_data_t *)d;
  int add;
  evt_t **pe, *e;			/* indirect precursor */

  add  = fts_get_long(&at[0]);	/* add index */

  e = (evt_t *) fts_heap_alloc(explode_evt_heap);

  e->time = fts_get_long(&at[1]);
  e->pit  = fts_get_long(&at[2]);
  e->vel  = fts_get_long(&at[3]);
  e->dur  = fts_get_long(&at[4]);
  e->chan = fts_get_long(&at[5]);

  /* Look for the good position */

  pe = &(data->evt);
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


static void explode_remote_change( fts_data_t *d, int ac, const fts_atom_t *at)
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

  explode_data_t *data = (explode_data_t *)d;
  int change = fts_get_long(&at[0]);	/* change index */
  evt_t **pe, *e;			/* indirect precursor */

  /* First, found the event and remote it from the list */

  pe = &(data->evt);
  while (change > 0)
    {
      pe = &( (*pe)->next);
      change--;
    }

  e = *pe;

  /* Change the event  */

  e->time = fts_get_long(&at[1]);
  e->pit  = fts_get_long(&at[2]);
  e->vel  = fts_get_long(&at[3]);
  e->dur  = fts_get_long(&at[4]);
  e->chan = fts_get_long(&at[5]);

  return;
}

static void explode_remote_change_time( fts_data_t *d, int ac, const fts_atom_t *at)
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

  explode_data_t *data = (explode_data_t *)d;
  int change = fts_get_long(&at[0]);	/* change index */
  evt_t **pe, *e;			/* indirect precursor */

  /* First, found the event and remote it from the list */

  pe = &(data->evt);
  while (change > 0)
    {
      pe = &( (*pe)->next);
      change--;
    }

  e = *pe;
  (*pe)  = (*pe)->next;

  /* Change the event  */

  e->time = fts_get_long(&at[1]);
  /*e->pit  = fts_get_long(&at[2]);
    e->vel  = fts_get_long(&at[3]);
    e->dur  = fts_get_long(&at[4]);
    e->chan = fts_get_long(&at[5]);*/

  /* Find the correct new position */

  pe = &(data->evt); 
  while (*pe && ((*pe)->time <= e->time))/*ho messo l'uguale*/
    pe = &( (*pe)->next);

  /* Insert it */

  e->next = (*pe);
  (*pe)  = e;

  return;
}

/* Note that the explode data should actually be a different type
   defined in a different file, and used by explode (hint: two explode
   sharing the same sequence); for now, the two are almost merged
   and the data object type is embedded in the object type */

/* Data Export function */

static void explode_data_export_fun(fts_data_t *d)
{
  explode_data_t *data = (explode_data_t *)d;
  evt_t *e;
  fts_atom_t args[5];

  if (data->name)
    {
      fts_data_start_remote_call((fts_data_t *) data, EXPLODE_REMOTE_NAME);
      fts_client_mess_add_symbol(data->name);
      fts_data_end_remote_call();
    }

  fts_data_remote_call(d, EXPLODE_LOAD_START, 0, 0);

  for ( e = data->evt; e; e = e->next)
    {
      fts_set_long( &(args[0]), e->time);
      fts_set_long( &(args[1]), e->pit);
      fts_set_long( &(args[2]), e->vel);
      fts_set_long( &(args[3]), e->dur);
      fts_set_long( &(args[4]), e->chan);

      fts_data_remote_call((fts_data_t *) data, EXPLODE_LOAD_APPEND, 5, args);
    }

  fts_data_remote_call(d, EXPLODE_LOAD_END, 0, 0);
}

/* Daemon for getting the property "data".
   Note that we return a pointer to the data; 
   if the request come from the client, it will be the
   kernel to handle the export of the data, not the explode
   object.
 */

static void
explode_get_data(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  explode_t *this = (explode_t *)obj;

  fts_set_data(value, (fts_data_t *) &(this->data));
}


/****************************************************************************/
/*                                                                          */
/*                      Instantiation and config                            */
/*                                                                          */
/****************************************************************************/



static fts_status_t
explode_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];

  /* initialize the class */

  fts_class_init(cl, sizeof(explode_t), 5, 5, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, explode_init_mth, 2, a, 1);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, explode_delete_mth, 0, 0);

  /* Bmax related methods */

  fts_method_define(cl, fts_SystemInlet, fts_s_restore, explode_record_mth, 0, 0); 
  fts_method_define(cl, fts_SystemInlet, fts_s_stop, explode_stop_mth, 0, 0);

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  a[4] = fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_append, explode_append_from_bmax, 5, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_save_bmax, explode_save_bmax, 1, a);

  /* Explode number methods */

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, explode_number_mth, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, explode_number_mth, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, explode_number_1_mth, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, explode_number_1_mth, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int, explode_number_2_mth, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, explode_number_2_mth, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 3, fts_s_int, explode_number_3_mth, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 3, fts_s_float, explode_number_3_mth, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 4, fts_s_int, explode_number_4_mth, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 4, fts_s_float, explode_number_4_mth, 1, a);

  /* lists */

  fts_method_define_varargs(cl, 0, fts_s_list, explode_list_mth);

  /* Other methods */


  fts_method_define(cl, 0, fts_new_symbol("next"), explode_next_mth, 0, 0);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("nth"), explode_nth_mth, 1, a, 0);

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  a[4] = fts_s_int;
  fts_method_define(cl, 0, fts_s_append, explode_append_mth, 5, a);

  fts_method_define(cl, 0, fts_s_clear, explode_clear_mth, 0, 0);
  fts_method_define(cl, 0, fts_s_start, explode_start_mth, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("record"), explode_record_mth, 0, 0); 
  fts_method_define(cl, 0, fts_s_stop, explode_stop_mth, 0, 0);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("follow"), explode_follow_mth, 1, a, 0);

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("startat"), explode_startat_mth, 3, a);

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("followat"), explode_followat_mth, 3, a);

  a[0] = fts_s_int;
  a[1] = fts_s_int;
  a[2] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("params"), explode_params_mth, 3, a);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 1,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 2,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 3,	fts_s_int, 1, a);
  fts_outlet_type_define(cl, 4,	fts_s_int, 1, a);

  /* Add the rename daemon */

  fts_class_add_daemon(cl, obj_property_put, fts_s_name, explode_put_name_daemon);

  /* daemon for data property */

  fts_class_add_daemon(cl, obj_property_get, fts_s_data, explode_get_data);

  return fts_Success;
}

void
explode_config(void)
{
  explode_evt_heap = fts_heap_new(sizeof(evt_t));
  explode_skip_heap = fts_heap_new(sizeof(skip_t));
  explode_hang_heap = fts_heap_new(sizeof(hang_t));

  fts_class_install( fts_new_symbol( "explode"),explode_instantiate);

  explode_data_class = fts_data_class_new( fts_new_symbol( "explode_data"));
  fts_data_class_define_export_function(explode_data_class, explode_data_export_fun);
  fts_data_class_define_function(explode_data_class, EXPLODE_REMOTE_ADD, explode_remote_add);
  fts_data_class_define_function(explode_data_class, EXPLODE_REMOTE_REMOVE, explode_remote_remove);
  fts_data_class_define_function(explode_data_class, EXPLODE_REMOTE_CHANGE, explode_remote_change);
  fts_data_class_define_function(explode_data_class, EXPLODE_REMOTE_CHANGE_TIME, explode_remote_change_time);
  init_explode_register();
}
