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
 *  - fts_clock
 *  - fts_alarm
 *  - fts_timer
 *
 */
#include <fts/fts.h>
#include <math.h>

static fts_heap_t *alarms_heap = 0;
static fts_heap_t *timers_heap = 0;

/****************************************************************************
 *
 *  clocks
 *
 */
void 
fts_clock_init(fts_clock_t *clock)
{
  clock->time = 0.0;
  clock->alarms = 0;
}

void 
fts_clock_reset(fts_clock_t *clock)
{
  clock->time = 0;

  /* fire all current alarms */
  while(clock->alarms)
    {
      /* remove the alarm from the list */
      clock->alarms = clock->alarms->next;
    }
}

void 
fts_clock_advance(fts_clock_t *clock, double time)
{
  /* fire all current alarms */
  while(clock->alarms && clock->alarms->time <= time)
    {
      fts_alarm_t *p = clock->alarms;
      
      /* set logical time */
      clock->time = p->time;
      
      /* remove the alarm from the list */
      clock->alarms = clock->alarms->next;
      
      /* call the function */
      p->active = 0;
      (*p->fun)(p, p->arg);
    }
  
  /* set logical time equal given time */
  clock->time = time;
}

/****************************************************************************
 *
 *  alarms
 *
 */
void
fts_alarm_init(fts_alarm_t *alarm, fts_clock_t *clock, void (* fun)(fts_alarm_t *, void *), void *arg)
{
  if(clock)
    alarm->clock = clock;
  else
    alarm->clock = fts_sched_get_clock();

  alarm->next = 0;
  alarm->active = 0;
  alarm->time = 0.0;
  alarm->fun = fun;
  alarm->arg = arg;
}

fts_alarm_t *
fts_alarm_new(fts_clock_t *clock, void (* fun)(fts_alarm_t *, void *), void *arg)
{
  fts_alarm_t *alarm = (fts_alarm_t *) fts_heap_alloc(alarms_heap);

  fts_alarm_init(alarm, clock, fun, arg);

  return alarm;
}


void
fts_alarm_free(fts_alarm_t *alarm)
{
  fts_alarm_reset(alarm);

  fts_heap_free(alarm, alarms_heap);
}

void
fts_alarm_set(fts_alarm_t *alarm, double time)
{
  fts_clock_t *clock = alarm->clock;

  if(time >= clock->time)
    {
      fts_alarm_t **p = &clock->alarms;

      /* insert alarm to ordered list */
      while(*p && (time >= (*p)->time))
	p = &((*p)->next);
      
      alarm->next = (*p);
      *p = alarm;
     
      /* set time */
      alarm->time = time;
      
      /* set active */
      alarm->active = 1;
    }
  else
    {
      /* insert at beginning of list */
      alarm->next = clock->alarms;
      clock->alarms = alarm;
     
      /* set time */
      alarm->time = clock->time;
      
      /* set active */
      alarm->active = 1;
    }    
}

/* remove alarm from the alarm list of its clock */
void
fts_alarm_reset(fts_alarm_t *alarm)
{
  fts_alarm_t **p;

  if(alarm->active)
    {
      /* search alarm in list */
      for (p=&(alarm->clock->alarms); *p; p=&((*p)->next))
	{
	  if ((*p) == alarm)
	    {
	      /* set inactive */
	      (*p)->active = 0;

	      /* remove from list */
	      *p = alarm->next;

	      return;
	    }
	}
    }
}

void
fts_alarm_set_time(fts_alarm_t *alarm, double time)
{
  if(alarm->active)
    {
      fts_alarm_reset(alarm);

      fts_alarm_set(alarm, time);

      return;
    }
  else
    fts_alarm_set(alarm, time);
}

void 
fts_alarm_set_delay(fts_alarm_t *alarm, double delay)
{
  fts_alarm_set_time(alarm, alarm->clock->time + delay);
}

/*****************************************************************
 *
 *  timers
 *
 */

void
fts_timer_init(fts_timer_t *timer, fts_clock_t *clock)
{
  if(clock)
    timer->clock = clock;
  else
    timer->clock = fts_sched_get_clock();

  timer->running = 0;
  timer->time = 0.0;
}

fts_timer_t *
fts_timer_new(fts_clock_t *clock)
{
  fts_timer_t *timer = (fts_timer_t *) fts_heap_alloc(timers_heap);

  fts_timer_init(timer, clock);

  return timer;
}

void
fts_timer_free(fts_timer_t *timer)
{
  fts_heap_free(timer, timers_heap);
}

void
fts_timer_start(fts_timer_t *timer)
{
  if(!timer->running)
    {
      timer->running = 1;
      timer->start = timer->clock->time;
    }
}

void
fts_timer_reset(fts_timer_t *timer)
{
  timer->time = 0.0;
  timer->start = timer->clock->time;
}

void
fts_timer_stop(fts_timer_t *timer)
{
  if(timer->running)
    {
      timer->running = 0;
      timer->time = timer->time + (timer->clock->time - timer->start);
    }
}


/***************************************************
 *
 *     Module initialization
 *
 */

void fts_kernel_time_init(void)
{
  alarms_heap = fts_heap_new(sizeof(fts_alarm_t));
  timers_heap = fts_heap_new(sizeof(fts_timer_t));
}


