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
   This file define the update generation scheduling mechanism for objects.

   NEW version for FTS 2.0.

   In FTS 2.0, the flow control is gone; updates are sent directly as
   property values; we allow update-per-ticks updates per ticks, where
   the update-per-ticks parameter can be set with a UCS command (for the moment),
   and default to 100.

   The unidirectional flow control is gone because we estimate that
   on a UNIX machine, a more "statistical" control may be more
   meaningfull; also, the whole determinism of the flow control make
   little or no sense compared to the complete undeterminism of the
   load given by a control patch.

   The update scheduler use the property change registration mechanism:
   when scheduled, it get the first N property change in the change fifo,
   and send them to the client.

   The scheduler is started by an alarm; the first property change start
   the alarm with a small time (actually 0) to have the maximal reactivity,
   and then the alarm is automatically rearmed iff there are elements in the fifo.
   In this way for burst of events with a period larger than the alarm we answer
   in real-time.
   */

/* #define UPDATE_TRACE  */

#include "sys.h"
#include "lang.h"
#include "runtime/devices.h"
#include "runtime/sched.h"
#include "runtime/time.h"
#include "runtime/client/protocol.h"
#include "runtime/client/client.h"
#include "runtime/client/outgoing.h"
#include "runtime/client/updates.h"

/* Default values are for 400 updates per seconds,
 */

static int fts_updates_per_ticks = 40;
static int fts_update_period = 10; /* every how many milliseconds we do an update */

static fts_alarm_t update_alarm;

struct changes
{
  fts_symbol_t property;
  fts_object_t *obj;
  struct changes *next;
};

static fts_heap_t *changes_heap;
static struct changes *changes_queue_head = 0;


static void fts_client_send_property(fts_object_t *obj, fts_symbol_t name)
{
  fts_atom_t a;

  if (obj && name)
    {
      fts_object_get_prop(obj, name, &a);

      if (fts_is_data(&a))
	{
	  /* If the property value is an fts_data, we
	     export the data needed  */

	  fts_data_t *d;
	  
	  d = fts_get_data(&a);

	  if (! fts_data_is_exported(d))
	    fts_data_export(d);
	}
      else if (fts_is_object(&a))
	{
	  /* If the property is an fts_object and is not 
	     uploaded, upload it; note that this will shortly
	     be unified with fts_data model */

	  fts_object_t *obj;

	  obj = fts_get_object(&a);

	  if (!fts_object_has_id(obj))
	    fts_client_upload_object(obj);
	}

      /* If the value is void, send a null value only
	 if the property is a special registered property;
	 for the moment, only fts_s_data; it should be done
	 better */

      if (fts_is_void(&a))
	{
	  if (name == fts_s_data)
	    fts_set_data(&a, (fts_data_t *) 0);
	  else
	    return;
	}

#ifdef UPDATE_TRACE 
      {
	fprintf(stderr, "Sending property %s value " , fts_symbol_name(name));
	fprintf_atoms(stderr, 1, &a);
	fprintf(stderr, " for object ");
	fprintf_object(stderr, obj);
	fprintf(stderr, "\n");
      }
#endif

      fts_client_start_msg(CLIENTPROP_CODE);
      fts_client_add_object(obj);
      fts_client_add_symbol(name);
      fts_client_add_atoms(1, &a);
      fts_client_done_msg();
    }
}


static void update_group_start(void)
{
  fts_client_start_msg(UPDATE_GROUP_START_CODE);
  fts_client_done_msg();
}


static void update_group_end(void)
{
  fts_client_start_msg(UPDATE_GROUP_END_CODE);
  fts_client_done_msg();
}


static void fts_client_updates_sched(fts_alarm_t *alarm, void *arg)
{
  fts_object_t *obj;
  fts_symbol_t property;
  int update_count;

  if (changes_queue_head)
    {
      update_group_start();

      for (update_count = 0;
	   (update_count < fts_updates_per_ticks) && changes_queue_head ;
	   update_count++)
	{
	  struct changes *p;

	  p = changes_queue_head;
	  changes_queue_head = p->next;

	  fts_client_send_property(p->obj, p->property);
	  fts_heap_free(p, changes_heap);
	}

      update_group_end();

      fts_alarm_set_delay(&update_alarm, fts_update_period);
      fts_alarm_arm(&update_alarm);
    }
}

/* Sync empty the change fifo; this is used to introduce a sequence
   point between the client and the server, with the sync protocol,
   under request from the server; it do not touch the alarm.
   */

void fts_client_updates_sync(void)
{
  fts_object_t *obj;
  fts_symbol_t property;

  if (changes_queue_head)
    {
      update_group_start();

      while (changes_queue_head)
	{
	  struct changes *p;

	  p = changes_queue_head;
	  changes_queue_head = p->next;

	  /* Note that fts_client_send_property may
	     recursively call this function; this
	     is why changes_queue_head *must* be updated
	     before the call */

	  fts_client_send_property(p->obj, p->property);
	  fts_heap_free(p, changes_heap);
	}

      update_group_end();
    }
}


void fts_updates_set_updates_per_ticks(int upt)
{
  if (upt > 0)
    fts_updates_per_ticks = upt;
}

void fts_updates_set_update_period(int upt)
{
  if (upt > 0)
    fts_update_period = upt;
}



/* This function send a property value asynchroniously to the client,
   when some bandwith is available; please note that the function
   fts_client_send_property can be used to send the property
   sychroniusly, in  contexts like uploading */


void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property)
{
  struct changes *p;
  struct changes *last = 0;

  /* check if the object is already in the evsched list */

  for (p = changes_queue_head; p; last = p, p = p->next)
    if ((p->obj == obj) && p->property == property)
      return;

  /* 
     Here, if last is not null, is the last element of the list;
     if it is null, there are no element in the list.
   */

  p = (struct changes *)fts_heap_alloc(changes_heap);

  p->property = property;
  p->obj = obj;
  p->next = 0;

  /* add the new queue element to the end of the list */

  if (last)
    last->next = p;
  else
    changes_queue_head = p;

  /* If the update timer is not armed, arm it with a zero period,
     so it will fire immediately */

  if (! fts_alarm_is_armed(&update_alarm))
    {
      fts_alarm_set_delay(&update_alarm, 0);
      fts_alarm_arm(&update_alarm);
    }
}



/* By using the fts_object_ui_property_changed function, an object declare
   that the changed property is a User Interface related matter, and this
   allow optimization like not registering the property change if the corresponding
   patcher is not open, or if there are no editors open for this object ...
   (the latter is to be implemented).
 */


void fts_object_ui_property_changed(fts_object_t *obj, fts_symbol_t property)
{
  if (fts_object_patcher_is_open(obj))
      fts_object_property_changed(obj, property);
}


/* To reset to signal that an object  shouldn't be considered changed anymore.
   It is called before deleting an object, to keep the change list
   consistent.
*/

void fts_object_reset_changed(fts_object_t *obj)
{
  struct changes **pp;		/* indirect precursor  */
  struct changes *p;		/* found element */

  pp = &changes_queue_head;

  while (*pp)
    {
      if ((*pp)->obj == obj)
	{
	  p = (*pp);

	  (*pp) = (*pp)->next;

	  fts_heap_free((char *)p, changes_heap);
	}
      else
	pp = &((*pp)->next);
    }
}


void fts_client_updates_init(void)
{
  changes_heap = fts_heap_new(sizeof(struct changes));
  fts_alarm_init(&update_alarm, 0, fts_client_updates_sched, 0);
}












