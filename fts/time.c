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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/***************************************************************************
 *
 *  FTS time
 *
 */
#include <fts/fts.h>
#include <ftsprivate/time.h>
#include <math.h>

/* the FTS master timebase */
static fts_timebase_t *main_timebase = 0;

static fts_heap_t *timebase_entry_heap = 0;


/****************************************************************************
 *
 *  timebase scheduling entry
 *
 */

typedef struct _timebase_entry_
{
  double time; /* when to trigger this entry */
  fts_object_t *object;
  fts_method_t method; /* entry callback method */
  fts_atom_t atom; /* entry callback argument */
  struct _timebase_entry_ *next; /* next entry in timebase */
} timebase_entry_t;


static timebase_entry_t *
timebase_entry_new(fts_object_t *object, fts_method_t method, const fts_atom_t *atom, double time)
{
  timebase_entry_t *entry = fts_heap_alloc(timebase_entry_heap);

  entry->time = time;
  entry->object = object;
  entry->method = method;
  fts_set_void(&entry->atom);

  /* claim object */
  fts_object_refer(object);

  /* claim atom if object */
  if(atom)
    fts_atom_assign(&entry->atom, atom);

  return entry;
}

static void
timebase_entry_free(timebase_entry_t *entry)
{
  fts_object_release(entry->object);
  fts_atom_assign( &entry->atom, fts_null);
  fts_heap_free(entry, timebase_entry_heap);
}


/****************************************************************************
 *
 *  timebase reset utilities
 *
 */

static void
timebase_insert_entry(fts_timebase_t *timebase, timebase_entry_t *entry)
{
  timebase_entry_t **p = &timebase->entries;
  
  /* place in ordered list */
  while(*p && (entry->time >= (*p)->time))
    p = &((*p)->next);
  
  /* insert to list */
  entry->next = (*p);
  *p = entry;
}

static void
timebase_remove_entries(fts_timebase_t *timebase)
{
  /* remove all entries */
  while(timebase->entries)
    {
      timebase_entry_t *freeme = timebase->entries;
      
      /* remove the entry from the list */
      timebase->entries = timebase->entries->next;
      
      /* free entry */
      timebase_entry_free(freeme);
    }
}

static void 
timebase_remove_slaves(fts_timebase_t *timebase)
{
  /* remove slaves form list and set their master to 0 */
  while(timebase->slaves)
    {
      /* get first slave in the list */
      fts_timebase_t *slave = timebase->slaves;

      /* remove slave from list */
      timebase->slaves = slave->next;

      /* reset master */
      slave->master = 0;
      slave->origin = 0;

      /* release slave */
      fts_object_release(slave);
    }
}
  

/****************************************************************************
 *
 *  timebase API
 *
 */
void 
fts_timebase_init(fts_timebase_t *timebase)
{
  fts_class_t *class = fts_object_get_class((fts_object_t *)timebase);
  fts_method_t tick = fts_class_get_method(class, fts_SystemInlet, fts_s_timebase_tick);
  fts_method_t locate = fts_class_get_method(class, fts_SystemInlet, fts_s_timebase_locate);

  timebase->time = 0.0;
  timebase->step = 0.0;

  /* time scheduling entries */
  timebase->entries = 0;

  /* for master */
  timebase->slaves = 0;

  /* for slave */
  timebase->master = 0;
  timebase->origin = 0;

  timebase->tick = tick;
  timebase->locate = locate;

  timebase->next = 0;
}

void 
fts_timebase_reset(fts_timebase_t *timebase)
{
  timebase->time = 0.0;

  timebase_remove_entries(timebase);
  timebase_remove_slaves(timebase);
}

/***************************************************
 *
 *  derived timebases (slaves)
 *
 */

void
fts_timebase_add_slave(fts_timebase_t *timebase, fts_timebase_t *slave)
{
  /* timebase is inactive (never true for master!) and has a tick method */
  if(!slave->master && slave->tick)
    {
      /* insert slave to list of slaves of timebase */
      slave->next = timebase->slaves;
      timebase->slaves = slave;

      /* claim slave */
      fts_object_refer(slave);

      /* set timebase as master of slave */
      slave->master = timebase;
      slave->origin = timebase->origin;
    }
}

void
fts_timebase_remove_slave(fts_timebase_t *timebase, fts_timebase_t *slave)
{
  if(slave->master)
    {
      fts_timebase_t **p;
      
      /* search slave in list */
      for (p=&timebase->slaves; *p; p=&(*p)->next)
	{
	  if (*p == slave)
	    {
	      /* remove from list */
	      *p = (*p)->next;
	      
	      return;
	    }
	}
      
      /* reset master */
      slave->master = 0;
      slave->origin = 0;

      /* release slave */
      fts_object_release(slave);
    }
}

void
fts_timebase_advance_slaves(fts_timebase_t *timebase)
{
  fts_timebase_t *slave = timebase->slaves;

  /* call timebase slaves */
  while(slave)
    {
      fts_timebase_t *next = slave->next;

      /* send tick */
      if(slave->tick)
	slave->tick((fts_object_t *)slave, 0, 0, 0, 0);
      
      /* reschedule all current entries to origin */
      while(slave->entries && slave->entries->time < slave->time + slave->step)
	{
	  timebase_entry_t *entry = slave->entries;
	  double retime = slave->origin->time + (entry->time - slave->time) * slave->origin->step / slave->step;
	  
	  /* remove the entry from the list */
	  slave->entries = slave->entries->next;
	  
	  /* set time and schedule in origin */
	  entry->time = retime;
	  timebase_insert_entry(slave, entry);
	}
      
      slave->time += slave->step;
      
      slave = next;
    }
}

void 
fts_timebase_advance(fts_timebase_t *timebase)
{
  double time = timebase->time + timebase->step;

  timebase->tick_time = timebase->time;
  fts_timebase_advance_slaves(timebase);

  /* fire all current entries */	  
  while(timebase->entries && timebase->entries->time < time)
    {
      timebase_entry_t *entry = timebase->entries;
      
      /* remove the entry from the list */
      timebase->entries = timebase->entries->next;

      /* set logical time */
      timebase->time = entry->time;
      
      /* call the function */
      entry->method(entry->object, 0, 0, 1, &entry->atom);
      
      /* free entry */
      timebase_entry_free(entry);
    }

  timebase->time = time;
}

void
fts_timebase_locate(fts_timebase_t *timebase)
{
  fts_timebase_t *slave = timebase->slaves;

  /* remove all scheduled entries */
  timebase_remove_entries(timebase);

  /* call slaves */
  while(slave)
    {
      fts_timebase_t *next = slave->next;

      if(slave->locate)
	slave->locate((fts_object_t *)slave, 0, 0, 0, 0);
      else
	{
	  fts_timebase_locate(slave);
	  fts_timebase_remove_slave(timebase, slave);
	}

      slave = next;
    }
}

/***************************************************
 *
 *  timebase method scheduling
 *
 */

void 
fts_timebase_add_call(fts_timebase_t *timebase, fts_object_t *object, fts_method_t method, const fts_atom_t *atom, double delay)
{
  timebase_entry_t *entry;

  if(delay > 0.0)
    entry = timebase_entry_new(object, method, atom, timebase->time + delay);
  else
    entry = timebase_entry_new(object, method, atom, timebase->time);
    
  timebase_insert_entry(timebase, entry);
}

void
fts_timebase_remove_object(fts_timebase_t *timebase, fts_object_t *object)
{
  timebase_entry_t **p = &timebase->entries;
  
  while(*p)
    {
      if ((*p)->object == object)
	{
	  timebase_entry_t *freeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  
	  /* free entry */
	  timebase_entry_free(freeme);
	}
      else
	p = &((*p)->next);
    }
}

void
fts_timebase_flush_object(fts_timebase_t *timebase, fts_object_t *object)
{
  timebase_entry_t **p = &timebase->entries;
  
  while(*p)
    {
      if ((*p)->object == object)
	{
	  timebase_entry_t *freeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  
	  /* call the function */
	  freeme->method(freeme->object, 0, 0, 1, &freeme->atom);

	  /* free entry */
	  timebase_entry_free(freeme);
	}
      else
	p = &((*p)->next);
    }
}

/***************************************************
 *
 *  time module
 *
 */

void
fts_set_timebase(fts_timebase_t *timebase)
{
  main_timebase = timebase;
}

fts_timebase_t *
fts_get_timebase(void)
{
  return main_timebase;
}

double 
fts_get_time(void)
{
  return main_timebase->time;
}

void 
fts_kernel_time_init(void)
{
  timebase_entry_heap = fts_heap_new(sizeof(timebase_entry_t));
}
