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

void
fts_timebase_entry_reset(fts_timebase_entry_t *entry)
{
  fts_object_release(entry->object);
  fts_atom_void(&entry->atom);
}

void
fts_timebase_entry_set(fts_timebase_entry_t *entry, fts_object_t *object, fts_method_t method, const fts_atom_t *atom, double time)
{
  entry->time = time;
  entry->object = object;
  entry->method = method;

  if(atom)
    fts_atom_assign(&entry->atom, atom);
}

static void
fts_timebase_entry_init(fts_timebase_entry_t *entry, fts_object_t *object, fts_method_t method, const fts_atom_t *atom, double time)
{
  entry->time = time;
  entry->object = object;
  entry->method = method;
  fts_set_void(&entry->atom);

  /* claim object */
  fts_object_refer(object);

  /* claim atom if object */
  if(atom)
    fts_atom_assign(&entry->atom, atom);
}

static void
fts_timebase_entry_free(fts_timebase_entry_t *entry)
{
  fts_timebase_entry_reset(entry);
  fts_heap_free(entry, timebase_entry_heap);
}


/****************************************************************************
 *
 *  timebase reset utilities
 *
 */

static void
timebase_insert_entry(fts_timebase_t *timebase, fts_timebase_entry_t *entry)
{
  fts_timebase_entry_t **p = &timebase->entries;
  
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
      fts_timebase_entry_t *freeme = timebase->entries;
      
      /* remove the entry from the list */
      timebase->entries = timebase->entries->next;
      
      /* free entry */
      fts_timebase_entry_free(freeme);
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
	  fts_timebase_entry_t *entry = slave->entries;
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
      fts_timebase_entry_t *entry = timebase->entries;
      
      /* remove the entry from the list */
      timebase->entries = timebase->entries->next;

      /* set logical time */
      timebase->time = entry->time;
      
      /* call the function */
      entry->method(entry->object, 0, 0, 1, &entry->atom);
      
      /* free entry */
      fts_timebase_entry_free(entry);
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
  fts_timebase_entry_t *entry = fts_heap_alloc(timebase_entry_heap);

  if(delay > 0.0)
    fts_timebase_entry_init(entry, object, method, atom, timebase->time + delay);
  else
    fts_timebase_entry_init(entry, object, method, atom, timebase->time);
    
  timebase_insert_entry(timebase, entry);
}

void
fts_timebase_remove_object(fts_timebase_t *timebase, fts_object_t *object)
{
  fts_timebase_entry_t **p = &timebase->entries;
  
  while(*p)
    {
      if ((*p)->object == object)
	{
	  fts_timebase_entry_t *freeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  
	  /* free entry */
	  fts_timebase_entry_free(freeme);
	}
      else
	p = &((*p)->next);
    }
}

void
fts_timebase_flush_object(fts_timebase_t *timebase, fts_object_t *object)
{
  fts_timebase_entry_t **p = &timebase->entries;
  
  while(*p)
    {
      if ((*p)->object == object)
	{
	  fts_timebase_entry_t *freeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  
	  /* call the function */
	  freeme->method(freeme->object, 0, 0, 1, &freeme->atom);

	  /* free entry */
	  fts_timebase_entry_free(freeme);
	}
      else
	p = &((*p)->next);
    }
}

/***************************************************
 *
 *  timebase fifo
 *
 */
void
fts_timebase_fifo_init(fts_timebase_fifo_t *fifo, fts_timebase_t *timebase, int size)
{
  int bytes = sizeof(fts_timebase_entry_t *) * size;
  fts_timebase_entry_t **entries;
  int i;

  fts_fifo_init(&fifo->data, fts_malloc(bytes), bytes);
  entries = (fts_timebase_entry_t **)fts_fifo_get_buffer(&fifo->data);

  for(i=0; i<size; i++)
    entries[i] = fts_heap_zalloc(timebase_entry_heap);

  fifo->timebase = timebase;
  fifo->delta = 0.0;
  fifo->size = size;
}

void
fts_timebase_fifo_destroy(fts_timebase_fifo_t *fifo, int size)
{
  fts_timebase_entry_t **entries = (fts_timebase_entry_t **)fts_fifo_get_buffer(&fifo->data);
  int i;
  
  for(i=0; i<size; i++)
    fts_heap_free(entries, timebase_entry_heap);

  fts_free((void *)fifo->data.buffer);
}

/* read next fifo entry into time base (returns pointer to atom of newly allocated entry) */
fts_timebase_entry_t *
fts_timebase_fifo_next(fts_timebase_fifo_t *fifo)
{
  if(fts_fifo_read_level(&fifo->data) >= sizeof(fts_timebase_entry_t *)) {
    fts_timebase_entry_t **entry_ptr = (fts_timebase_entry_t **)fts_fifo_write_pointer(&fifo->data);
    fts_timebase_entry_t *entry = *entry_ptr;
    double time = entry->time;
    double now = fts_timebase_get_time(fifo->timebase);

    /* time == 0.0 means: send now */
    if(time != 0.0) {
      /* adjust delta time on very first fifo entry */
      if(fifo->delta == 0.0)
        fifo->delta = time - now;

      /* translate event time */
      time -= fifo->delta;

      /* adjust delta time */
      if(time < now) {
        time = now;
        fifo->delta = entry->time - now;
      }

      /* set entry time and insert to timebase */
      entry->time = time;
      timebase_insert_entry(fifo->timebase, entry);

      /* create new entry and put into fifo */
      *entry_ptr = fts_heap_zalloc(timebase_entry_heap);
    } else {
      fts_object_t *object = entry->object;
      fts_method_t method = entry->method;
      fts_atom_t *atom = &entry->atom;

      /* call method right away */
      entry->method(entry->object, 0, 0, 1, &entry->atom);

      /* release object and atom */
      fts_timebase_entry_reset(entry);
    }
    
    return entry;
  }

  return NULL;
}

fts_timebase_entry_t *
fts_timebase_fifo_get_entry(fts_timebase_fifo_t *fifo)
{
  if(fts_fifo_write_level(&fifo->data) >= sizeof(fts_timebase_entry_t *))
    return *((fts_timebase_entry_t **)fts_fifo_write_pointer(&fifo->data));
  else
    return NULL;
}

void
fts_timebase_fifo_incr(fts_timebase_fifo_t *fifo)
{
  fts_fifo_incr_write(&fifo->data, sizeof(fts_timebase_entry_t *));
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
  timebase_entry_heap = fts_heap_new(sizeof(fts_timebase_entry_t));
}

