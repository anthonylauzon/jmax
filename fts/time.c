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

/*
 * This files provides the time related objects:
 *
 *  - fts_timebase
 *  - fts_timer
 *  - fts_alarm
 *
 */
#include <fts/fts.h>
#include <ftsprivate/time.h>
#include <math.h>

static fts_heap_t *timer_heap = 0;
static fts_heap_t *alarm_heap = 0;
static fts_timebase_t *main_timebase = 0;

/****************************************************************************
 *
 *  alarm
 *
 */

static alarm_t *
alarm_new(fts_timer_t *timer, double time, fts_atom_t *atom)
{
  alarm_t *alarm = fts_heap_alloc(alarm_heap);

  alarm->timer = timer;
  alarm->time = time;
  fts_set_void(&alarm->atom);

  if(atom)
    fts_atom_assign(&alarm->atom, atom);

  return alarm;
}

static void
alarm_free(alarm_t *alarm)
{
  fts_atom_void(&alarm->atom);
  fts_heap_free(alarm, alarm_heap);
}

/****************************************************************************
 *
 *  timebase alarm utilities
 *
 */

static void
timebase_insert_alarm(fts_timebase_t *timebase, alarm_t *alarm)
{
  alarm_t **p = &timebase->alarms;
  
  /* search in ordered list */
  while(*p && (alarm->time >= (*p)->time))
    p = &((*p)->next);
  
  /* insert to list */
  alarm->next = (*p);
  *p = alarm;
}

static void
timebase_remove_alarms(fts_timebase_t *timebase)
{
  /* remove all alarms */
  while(timebase->alarms)
    {
      alarm_t *freeme = timebase->alarms;
      fts_timer_t *timer = freeme->timer;
      
      /* remove the alarm from the list */
      timebase->alarms = timebase->alarms->next;
      timer->n_alarms--;
      
      /* free alarm */
      alarm_free(freeme);
    }
}

static void 
timebase_fire_alarms(fts_timebase_t *timebase, double time)
{
  /* fire all current alarms */	  
  while(timebase->alarms && timebase->alarms->time < time)
    {
      alarm_t *fireme = timebase->alarms;
      fts_timer_t *timer = fireme->timer;
      
      /* set logical time */
      timebase->time = fireme->time;
      
      /* remove the alarm from the list */
      timebase->alarms = timebase->alarms->next;
      timer->n_alarms--;
      
      /* call the function */
      timer->alarm(timer->object, 0, 0, 1, &fireme->atom);

      /* free alarm */
      alarm_free(fireme);
    }
}


/****************************************************************************
 *
 *  timebase timer utilities
 *
 */

static void
timebase_add_timer(fts_timer_t **list, fts_timer_t *timer)
{
  /* insert to timer list */
  timer->next = *list;
  *list = timer;
}

static void
timebase_remove_timer(fts_timer_t **list, fts_timer_t *timer)
{
  fts_timer_t **p;

  for (p=list; *p; p=&(*p)->next)
    {
      if (*p == timer)
	{
	  /* remove from timer list */
	  *p = (*p)->next;

	  return;
	}
    }
}

static void
timer_activate(fts_timer_t *timer)
{
  fts_timebase_t *timebase = timer->timebase;
  
  if(timer->locate)
    timebase_remove_timer(&timebase->locate, timer);
  else
    timebase_remove_timer(&timebase->silent, timer);
  
  timebase_add_timer(&timebase->active, timer);
  timer->active = 1;
}

static void
timer_desactivate(fts_timer_t *timer)
{
  fts_timebase_t *timebase = timer->timebase;

  timebase_remove_timer(&timebase->active, timer);
  timer->active = 0;

  if(timer->locate)
    timebase_add_timer(&timebase->locate, timer);
  else
    timebase_add_timer(&timebase->silent, timer);
}

static void
timer_remove_alarms(fts_timer_t *timer)
{
  fts_timebase_t *timebase = timer->timebase;
  alarm_t **p = &timebase->alarms;
  
  while(*p)
    {
      if ((*p)->timer == timer)
	{
	  alarm_t *freeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  timer->n_alarms--;
	  
	  /* free alarm */
	  alarm_free(freeme);

	  if(timer->n_alarms == 0)
	    return;
	}
      
      p = &((*p)->next);
    }
}

static void
timer_flush_alarms(fts_timer_t *timer)
{
  fts_timebase_t *timebase = timer->timebase;
  alarm_t **p = &timebase->alarms;
  
  while(*p)
    {
      if ((*p)->timer == timer)
	{
	  alarm_t *firenfreeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  timer->n_alarms--;
	  
	  /* call the function */
	  timer->alarm(timer->object, 0, 0, 1, &firenfreeme->atom);

	  /* free alarm */
	  alarm_free(firenfreeme);

	  if(timer->n_alarms == 0)
	    return;
	}
      
      p = &((*p)->next);
    }
}

static void 
timebase_call_active_timers(fts_timebase_t *timebase)
{
  fts_timer_t *timer;

  for(timer=timebase->active; timer; timer=timer->next)
    {
      if(timer->tick)
	timer->tick(timer->object, 0, 0, 0, 0);
    }
}

static void 
timebase_desactivate_timers(fts_timebase_t *timebase)
{
  /* remove active timers */
  while(timebase->active)
    {
      /* get first timer in the list */
      fts_timer_t *timer = timebase->active;

      /* set inactive */
      timer->active = 0;

      /* remove from list of active timers */
      timebase->active = timer->next;

      /* add to one of the silent lists */
      if(timer->locate)
	timebase_add_timer(&timebase->locate, timer);
      else
	timebase_add_timer(&timebase->silent, timer);
    }
}
  
/****************************************************************************
 *
 *  timebase API
 *
 */
void 
fts_timebase_init(fts_timebase_t *timebase, double step)
{
  timebase->time = 0.0;
  timebase->step = step;

  timebase->silent = 0;
  timebase->locate = 0;
  timebase->active = 0;

  timebase->alarms = 0;
}

void 
fts_timebase_reset(fts_timebase_t *timebase)
{
  timebase->time = 0.0;

  timebase_remove_alarms(timebase);
  timebase_desactivate_timers(timebase);
}

void 
fts_timebase_tick(fts_timebase_t *timebase)
{
  double time = timebase->time + timebase->step;

  timebase_call_active_timers(timebase);
  timebase_fire_alarms(timebase, time);

  timebase->time = time;
}

void
fts_timebase_set_time(fts_timebase_t *timebase, double time)
{
  fts_timer_t *timer;

  /* remove all pending alarms */
  timebase_remove_alarms(timebase);

  /* call or desactivate active timers */
  for(timer=timebase->active; timer; timer=timer->next)
    {
      if(timer->locate)
	timer->locate(timer->object, 0, 0, 0, 0);
      else
	timer_desactivate(timer);
    }

  /* call silent locatable timers */
  for(timer=timebase->locate; timer; timer=timer->next)
    timer->locate(timer->object, 0, 0, 0, 0);
}

double
fts_timebase_get_time(fts_timebase_t *timebase)
{
  return timebase->time;
}


/****************************************************************************
 *
 *  timer
 *
 */
fts_timer_t *
fts_timer_new(fts_object_t *object, fts_timebase_t *timebase)
{ 
  fts_timer_t *timer = fts_heap_alloc(timer_heap);
  fts_class_t *class = fts_object_get_class(object);
  fts_method_t tick = fts_class_get_method(class, fts_SystemInlet, fts_s_timer_tick);
  fts_method_t alarm = fts_class_get_method(class, fts_SystemInlet, fts_s_timer_alarm);
  fts_method_t locate = fts_class_get_method(class, fts_SystemInlet, fts_s_timer_locate);
  
  if(timebase)
    timer->timebase = timebase;
  else
    timer->timebase = main_timebase;
  
  timer->object = object;

  timer->tick = tick;
  timer->alarm = alarm;
  timer->locate = locate;

  timer->active = 0;
  timer->n_alarms = 0.0;
  timer->next = 0;

  if(locate)
    timebase_add_timer(&timer->timebase->locate, timer);
  else
    timebase_add_timer(&timer->timebase->silent, timer);    

  return timer;
}

void
fts_timer_reset(fts_timer_t *timer)
{
  if(timer->n_alarms > 0)
    timer_remove_alarms(timer);

  if(timer->active)
    timer_desactivate(timer);
}

void
fts_timer_flush(fts_timer_t *timer)
{
  if(timer->n_alarms > 0)
    timer_flush_alarms(timer);
}

void
fts_timer_delete(fts_timer_t *timer)
{
  fts_timer_reset(timer);
  fts_heap_free(timer, timer_heap);
}

void 
fts_timer_activate(fts_timer_t *timer)
{
  if(timer->tick)
    timer_activate(timer);
}

void
fts_timer_set_alarm(fts_timer_t *timer, double time, fts_atom_t *atom)
{
  fts_timebase_t *timebase = timer->timebase;

  if(timer->alarm && time >= timebase->time)
    {
      alarm_t *alarm = alarm_new(timer, time, atom);

      timebase_insert_alarm(timebase, alarm);
      timer->n_alarms++;
    }
}

void 
fts_timer_set_delay(fts_timer_t *timer, double delay, fts_atom_t *atom)
{
  fts_timebase_t *timebase = timer->timebase;
  double time = timebase->time + delay;
      
  if(timer->alarm && time >= timebase->time)
    {
      alarm_t *alarm = alarm_new(timer, time, atom);

      timebase_insert_alarm(timebase, alarm);
      timer->n_alarms++;
    }
}

/***************************************************
 *
 *  time module
 *
 */

void
fts_time_set_timebase(fts_timebase_t *timebase)
{
  main_timebase = timebase;
}

double 
fts_get_time(void)
{
  return main_timebase->time;
}

void 
fts_kernel_time_init(void)
{
  timer_heap = fts_heap_new(sizeof(fts_timer_t));
  alarm_heap = fts_heap_new(sizeof(alarm_t));
}
