/* Time handling primitives.

   This module introduce a number of primitive abstractions for 
   handling of time in FTS abstractions.

   
*/

#include <math.h>
#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"
#include "runtime/time.h"

#define TIME_DEBUG

#ifdef TIME_DEBUG
#include <stdio.h>
static void fts_alarm_describe(char *msg, fts_clock_t *clock);
#endif

/* hack to make the old fashioned understand aswell */

#ifndef SGI
#define trunc(f) floor(f)
#define fmodf(f,g) fmod((f),(g))
#endif

/******************************************************************************/
/*                                                                            */
/*                        Module declaration                                  */
/*                                                                            */
/******************************************************************************/

static void fts_alarm_sched(void);
static void fts_time_init(void);
static void fts_all_clocks_restart(void);
static void fts_time_restart(void);

fts_module_t fts_time_module = {"Time", "Generic Time handling", fts_time_init, fts_time_restart, 0};

static void
fts_time_init(void)
{
  fts_sched_declare(fts_alarm_sched, provide, fts_new_symbol("control"), "fts_alarm_poll"); 
}


static void
fts_time_restart(void)
{
  fts_all_clocks_restart();
}


/******************************************************************************/
/*                                                                            */
/*                        Clock                                               */
/*                                                                            */
/******************************************************************************/

/* 
   A clock in fts is a source of time.

   All the abstraction defined below (timers, alarms and time gates) refer to 
   a given clock in order to compute time; different clock can give different 
   time; examples of clocks are the scheduler time, the MIDI time code, hardware
   timers and so on.

   In the current "polling" oriented architecture, a clock is actually represented by 
   a float keeping the time count; the time unit and representation is actually not important,
   at the condition that is an isomorphism on the float time for what regard the comparison
   and add/subtract operations. (may be in the future the implementation will allow
   the definition of user implementation of time operators, so to allow more general
   representations of time, like DD:HH:MM:SS, one for byte.

   Clocks are kept in a table, and represented by name, where a name is a symbol.

   The only services provided at this level are name registration, and access (private
   to this file).

   In order to create a clock source, you have to specify:

   2- A pointer to a float, that is incremented by the physical clock.
      
   3- the symbol name to associate to the clock source.

   The implementation keep and generalize the concept of "logical time"
   introduced in FTS by Miller: in general, a clock time is incremented 
   of some amount for each tick; for example, the millisecond time can be   
   incremented of 1.4 ms in each tick at 44.1 Khz; the absolute time returned
   or kept by a clock while scheduling an alarm is anyway the time declared
   by the alarm object, so that cascade alarm settings, like in the metro
   object, give an accurate result, instead of accumulate errors.

   The system define the concept of "default" clock; any call to the below
   function with a NULL pointer instead of the clock name, access to
   a default clock, specified at configuration time with the 
   fts_set_default_clock function.

   A clock can be undefined: undefining a clock *do not* free the clock
   object, neither destroy all the objects referring to the clock, but
   just set a flag that prevent it's scheduling; if the user redefine
   a clock with the same name, this clock is reused, recasted to the
   new clock word; this allow editing. for example, patches with
   clocks, like deleting and reinstalling a clock, without 
   loosing the link with existing alarms and so on; a better solution
   would be using a reference count GC for clocks, but after all we
   don't care about loosing some memory during editing, if we gain memory
   for final applications.

   Note anyway that undefining and redefining a clock can have weird
   temporary effects on object using the clock; the transition is
   not guaranteed to be smooth, depend on the value of the clock.
   For the alarms, the effect is the same as with a clock value change
   (at the redefinition).

   The system support the concept of "protected clock", clocks
   that cannot be redefined or freed; they are the system clock.
   */

static fts_clock_t *clock_table = 0;

/* clock define return the defined clock; if the
   clock existed but was disabled, it is enabled.
   if it existed, it is simply returned.
   If clock is NULL, the clock is created, but left disabled.
*/

static fts_clock_t *default_clock = 0;

/* DA AGGIUNGERE la creazione del clock disabilitato (funzione statuc make_clocK).
   e modifica dello scheduler per non schedulare i clock non abilitati.

*/

/* create a new one, do not check for duplicate names,
 the clock is always disabled */

static fts_clock_t *
make_clock(fts_symbol_t clock_name)
{
  fts_clock_t **p;		/* indirect pre-cursor */

  p = &clock_table;

  while (*p)
    p = &((*p)->next);

  *p = (fts_clock_t *) fts_malloc(sizeof(fts_clock_t));

  (*p)->name         = clock_name;
  (*p)->real_time    = 0;
  (*p)->logical_time = 0.0;	
  (*p)->alarm_list   = 0;
  (*p)->future_alarm_list = 0;
  (*p)->next   = 0;
  (*p)->enabled      = 0;
  (*p)->protected    = 0;

  return *p;
}

static void
fts_all_clocks_restart(void)
{
  fts_clock_t *clock;

  for (clock = clock_table; clock; clock = clock->next)
    {
      fts_alarm_t *alarm;

      alarm = clock->alarm_list;

      while (alarm)
	{
	  fts_alarm_unarm(alarm);
	  alarm = clock->alarm_list;
	}

      if (! clock->protected)
	clock->enabled = 0;
    }
}


static fts_clock_t *
get_clock(fts_symbol_t clock_name)
{
  if (clock_name)
    {
      fts_clock_t *p;

      for (p = clock_table; p; p = p->next)
	if (p->name == clock_name)
	  return p;

      return (fts_clock_t *)0;
    }
  else
    return default_clock;
}

static fts_clock_t *
get_or_make_clock(fts_symbol_t clock_name)
{
  fts_clock_t *cl;

  cl = get_clock(clock_name);

  if (cl)
    return cl;
  else
    return make_clock(clock_name);
}


void
fts_clock_define(fts_symbol_t name, double *clock)
{
  fts_clock_t *cl;

  cl = get_clock(name);

  if (cl && cl->protected)
    return;

  if (cl)
    {
      if (! cl->enabled)
	{
	  /* reuse old clock if disabled, leaving the current logical_time */

	  cl->enabled   = 1;
	  cl->real_time = clock;
	}
    }
  else
    {
      cl = make_clock(name);

      cl->real_time    = clock;

      /* leave it disabled if we don't have the real time word defined */

      cl->enabled      = (clock ?  1 : 0);
    }
}


void
fts_clock_define_protected(fts_symbol_t name, double *clock)
{
  fts_clock_t *cl;

  cl = get_clock(name);

  if (cl)
    {
      /* the protected clock exists already, just return, don't modify it */

      return;
    }
  else
    {
      cl = make_clock(name);

      cl->real_time    = clock;

      /* leave it disabled if we don't have the real time word defined */

      cl->enabled      = (clock ?  1 : 0);
      cl->protected    = 1;
    }
}

void
fts_clock_undefine(fts_symbol_t clock_name)
{
  if (clock_name)
    {
      fts_clock_t *p;

      /* note that protected clocks (enabled == 2) are not undefined*/

      for (p = clock_table; p; p = p->next)
	if ((p->name == clock_name) && p->enabled == 1)
	  p->enabled = 0;
    }
}



/* clock_exists give as non existing undefined clocks */

int
fts_clock_exists(fts_symbol_t clock_name)
{
  if (clock_name)
    {
      fts_clock_t *p;

      for (p = clock_table; p; p = p->next)
	if (p->name == clock_name)
	  if (p->enabled)
	    return 1;

      return 0;
    }
  else
    return 1;	/* default clock exists !!! */
}


void
fts_set_default_clock(fts_symbol_t clock_name)
{
  default_clock = get_clock(clock_name);
}


/* undocumented call to find out the counter */

double *
fts_clock_get_real_time_p(fts_symbol_t clock_name)
{
  fts_clock_t *cl;

  cl = get_clock(clock_name);

  if (cl && cl->enabled)
    return cl->real_time;
  else
    return 0;
}


/*
  unarm all the pending alarms in each clock, and
  then disable all the non protected clocks.
 */


/* call to reset the clock consistency after the time moved backward
   should be called to get a consistent logical time.
 */

static void
fts_clock_reset(fts_clock_t *clock)
{
  fts_alarm_t *alarm;

  /* Move the logical time */

  clock->logical_time = *(clock->real_time);

  /* update the cyclic alarms  */

  /* Wrong code: in this way 'p' can move down in the
     list, so the work can be redone multiple times for
     multiple timers */

  for (alarm = clock->alarm_list; alarm ; alarm = alarm->next) 
    if (alarm->cycle)
      {
	fts_alarm_unarm(alarm);

	/* Update the cycle_counter */

	alarm->cycle_counter = trunc(clock->logical_time / alarm->cycle) + 1;
	alarm->when = clock->logical_time + alarm->cycle;

	fts_alarm_arm(alarm);
      }

  /* note the test on stricly less than */

  clock->future_alarm_list = clock->alarm_list;
  while (clock->future_alarm_list && clock->future_alarm_list->when < *(clock->real_time))
    clock->future_alarm_list = clock->future_alarm_list->next;
}

/******************************************************************************/
/*                                                                            */
/*                        Absolute time access                                */
/*                                                                            */
/******************************************************************************/

/* logical time */

double
fts_clock_get_time(fts_symbol_t clock_name)
{
  fts_clock_t *cl;

  cl = get_clock(clock_name);

  if (cl->logical_time > *(cl->real_time))
    fts_clock_reset(cl);

  if (cl && cl->enabled)
    return cl->logical_time;
  else
    return 0;
}


/******************************************************************************/
/*                                                                            */
/*                        Alarms                                              */
/*                                                                            */
/******************************************************************************/


/* the alarm object; should this go in time.h, or should the structure be hided ? */


fts_alarm_t *
fts_alarm_new(fts_symbol_t clock_name, void (* fun)(fts_alarm_t *, void *), void *arg)
{
  fts_alarm_t *alarm;

  alarm = (fts_alarm_t *) fts_malloc(sizeof(fts_alarm_t));

  alarm->clock = get_or_make_clock(clock_name);
  alarm->next = 0;
  alarm->prev = 0;
  alarm->active = 0;
  alarm->when   = 0.0;
  alarm->cycle  = 0.0;
  alarm->phase  = 0.0;
  alarm->cycle_counter = 0.0;
  alarm->arg  = arg;
  alarm->fun  = fun;

  return alarm;
}


void
fts_alarm_free(fts_alarm_t *alarm)
{
  fts_alarm_unarm(alarm);

  fts_free(alarm);
}


void
fts_alarm_init(fts_alarm_t *alarm, fts_symbol_t clock_name, void (* fun)(fts_alarm_t *, void *), void *arg)
{
  alarm->clock = get_or_make_clock(clock_name);
  alarm->next = 0;
  alarm->prev = 0;
  alarm->active = 0;
  alarm->when = 0;
  alarm->cycle  = 0.0;
  alarm->phase  = 0.0;
  alarm->cycle_counter = 0.0;
  alarm->fun  = fun;
  alarm->arg  = arg;
}

/* 
   Set_time set the time of the alarm; if the alarm is armed, is left
   armed, otherwise is *not* armed; you should arm it with the fts_alarm_arm
   function.

   _arm put the alarm in the alarm queue without changing the planned time.
   This is used when the time of the alarm is computed in a context different
   from the alarm activation context.

   Calling _arm before calling _set_time is illegal, and can produce
   unknown  results !
   */

void
fts_alarm_set_time(fts_alarm_t *alarm, double when)
{
  if (alarm->active)
    {
      fts_alarm_unarm(alarm);
      alarm->when = when;
      fts_alarm_arm(alarm);
      return;
    }
  else
    {
      /* Update the alarm structure */

      alarm->when = when;
    }
}


void
fts_alarm_set_delay(fts_alarm_t *alarm, double delay)
{
  fts_alarm_set_time(alarm, alarm->clock->logical_time + delay);
}


/*
   Changing the cycle of an existing alarm will take
   effect after immediately
   cycle of 0 is illegal, but not tested
*/

void
fts_alarm_set_cycle(fts_alarm_t *alarm, double cycle)
{
  int was_armed;
  fts_clock_t *clock;

  clock = alarm->clock;

  if (alarm->active)
    {
      was_armed = 1;
      fts_alarm_unarm(alarm);
    }
  else
    was_armed = 0;

  /* set the cycle_counter from the current logical time */

  alarm->cycle = cycle;
  alarm->cycle_counter = trunc(clock->logical_time / alarm->cycle) + 1;
  alarm->phase = fmodf(clock->logical_time, alarm->cycle);
  alarm->when = clock->logical_time + alarm->cycle;

  if (was_armed)
    fts_alarm_arm(alarm);
}


#ifdef TIME_DEBUG
static void
fts_alarm_describe(char *msg, fts_clock_t *clock)
{
  fts_alarm_t *alarm;

  fprintf(stderr, "%s: Alarm queue for clock %s (time %lf)\n", msg,
	  fts_symbol_name(clock->name), clock->logical_time);
  
  for (alarm = clock->alarm_list; alarm; alarm = alarm->next)
    {
      if (alarm->active)
	fprintf(stderr, "\tACTIVE\n");
      else
	fprintf(stderr, "\tINACTIVE\n");	
      
      fprintf(stderr, "\twhen\t%lf\n", alarm->when);
      fprintf(stderr, "\tcycle\t%lf\n", alarm->cycle);
      fprintf(stderr, "\tphase\t%lf\n", alarm->phase);
      fprintf(stderr, "\tcycles\t%lf\n", alarm->cycle_counter);
    }
}
#endif

void
fts_alarm_arm(fts_alarm_t *alarm)
{
  fts_alarm_t *p;
  fts_clock_t *clock;

  clock = alarm->clock;

  /* If active, ignore */

  if (alarm->active)
    {
      return;
    }

  /* set the active flag */

  alarm->active = 1;

  /* Small optimization: if an alarm is in the future, look in the
     list starting from future_alarm_list, instead that from the beginning.
     In case where many "at" are around, this may gain quite a lot of time ! */

  if (clock->alarm_list)
    {
      if (alarm->when < clock->logical_time)
	p = clock->alarm_list;
      else
	{
	  if (clock->future_alarm_list)
	    p = clock->future_alarm_list;
	  else
	    {
	      /* Since this is the only alarm in the future, insert at the end */

	      alarm->next = 0;
	      alarm->prev = clock->last_alarm;

	      clock->last_alarm->next = alarm;
	      clock->last_alarm = alarm;
	      clock->future_alarm_list = alarm;

	      alarm->active = 1;
	      return;
	    }
	}
      
      /* lokk for the good insertion point */

      while (p && p->when <= alarm->when)
	p = p->next;

      if (p)
	{
	  if (p == clock->alarm_list)
	    {
	      /* insert at the beginning */

	      alarm->next = p;
	      alarm->prev = 0;

	      p->prev = alarm;
	      clock->alarm_list = alarm;
	    }
	  else
	    {
	      /* p point to next in list */

	      alarm->next = p;
	      alarm->prev = p->prev;
	      
	      if (p->prev)
		p->prev->next = alarm;

	      p->prev = alarm;
	    }

	  /* Move the future_alarm_list pointer to this alarm
	     if in future, and if the future_alarm_list is empty,
	     or the next alarm is the beginning of the future_alarm_list
	     */

	  if (alarm->when >= clock->logical_time)
	    if (clock->future_alarm_list)
	      {
		if (clock->future_alarm_list == alarm->next)
		  clock->future_alarm_list = alarm;
	      }
	    else
	      clock->future_alarm_list = alarm;
	}
      else
	{
	  /* p is zero, insert at the end */
	  
	  alarm->next = 0;
	  alarm->prev = clock->last_alarm;

	  clock->last_alarm->next = alarm;
	  clock->last_alarm = alarm;

	  /* Do not need to move the future pointer,
	     If we add to the future, the future pointer
	     is not null, so it is ok.
	     If we are adding to the past, the future pointer
	     is not concerned anyway.
	     */
	}
    }
  else
    {
      /* this is the first alarm !! */

      clock->alarm_list = alarm;
      clock->last_alarm = alarm;

      alarm->next = 0;
      alarm->prev = 0;

      if (alarm->when >= clock->logical_time)
	clock->future_alarm_list = alarm;
    }
}


void
fts_alarm_unarm(fts_alarm_t *alarm)
{
  fts_clock_t *clock;

  /* If not active, return */

  if (! alarm->active)
    return;

  clock = alarm->clock;

  /* update the future_alarm_list pointer if needed */

  if (clock->future_alarm_list == alarm)
    clock->future_alarm_list = alarm->next;

  /* remove the alarm from the list */

  if (alarm->next)
    alarm->next->prev = alarm->prev;
  else
    clock->last_alarm = alarm->prev;

  if (alarm->prev)
    alarm->prev->next = alarm->next;
  else
    clock->alarm_list = alarm->next;

  /* clean the alarm structure and return*/

  alarm->active = 0;
}

/* predicate */

int
fts_alarm_is_in_future(fts_alarm_t *alarm)
{
  return (alarm->when > alarm->clock->logical_time);
}

int
fts_alarm_is_armed(fts_alarm_t *alarm)
{
  return alarm->active;
}


/* alarm level scheduling: installed by the module initialization function;
   note that a fired alarm is *not* unarmed; this is to allow
   for multiple firing of the same alarm in case the clock is not monotone;
   (as in the object "at 3000 dsp_ms", for example.
   User callback should unarm the alarm if needed.
 */


static void
fts_alarm_sched(void)	
{
  fts_clock_t *clock;

  /* For all the clock, look in the alarm list */

  for (clock = clock_table; clock; clock = clock->next)
    if (clock->enabled)
      {
	double real_time;

	/* Read the real time */

	real_time = *(clock->real_time);

	/* First, check if the clock moved back in time w.r.t. logical time.
	   if it did, reschedule all cyclic alarms, and 
	   find the good position for the   future_alarm_list pointer */

	if (clock->logical_time > real_time)
	  fts_clock_reset(clock);

	/* Take the next alarm, if can be triggered set the logical
	   time to its trigger time and call it; do not pull it out
	   from the list !!! */
      
	while (clock->future_alarm_list && clock->future_alarm_list->when <= real_time)
	  {
	    fts_alarm_t *p;

	    p = clock->future_alarm_list;

	    /* Set the logical time */

	    clock->logical_time = p->when;

	    /* Then, if it is a cyclic and active alarm, recompute the delay;
	       The timer function can, if desired, free the alarm, so we have to do it now, before
	       the callback calls; if the callback move around the timer, it will be repositioned
	       by the relative functions.
	     */

	    if (p->cycle && p->active)
	      {
		/* The following assumptions are true here:
		   - the alarm is active
		   - the alarm will be in the future in next cycle
		   */

		p->cycle_counter += 1.0;
		p->when = p->cycle_counter * p->cycle + p->phase;

		if ((! p->next) || (p->when <= p->next->when))
		  {
		    /* Do nothing, we are already in the good position */
		  }
		else
		  {
		    fts_alarm_t *ins;

		    /* Move forward the future alarm pointer, and reinsert 
		       the cycle alarm in the good position */

		    clock->future_alarm_list = p->next;

		    /* Store the insertion pointer */

		    ins = p->next;

		    /* remove the alarm from the list */

		    p->next->prev = p->prev;

		    if (p->prev)
		      p->prev->next = p->next;
		    else
		      clock->alarm_list = p->next;
		    
		    /* Insert in the correct position */

		    while (ins && ins->when <= p->when)
		      ins = ins->next;

		    if (ins)
		      {
			/* INsert in the list */

			p->next = ins;
			p->prev = ins->prev;

			if (ins->prev)
			  ins->prev->next = p;
			else
			  clock->alarm_list = p;

			ins->prev = p;
		      }
		    else
		      {
			/* Add to the end */

			p->next = 0;
			p->prev = clock->last_alarm;

			clock->last_alarm->next = p;
			clock->last_alarm = p;
		      }
		  }

	      }
	    else
	      {
		/* the alarm is not cyclic, so we can move forward the future alarm pointer:
		   now, because the callback can modify the list itself
		   and the pointer */

		clock->future_alarm_list = clock->future_alarm_list->next;
	      }

	    /* Call the function */

	    (*p->fun)(p, p->arg);
	  }
      
	/* Finally, set the logical time equal to the physical (real) time for the clock  */

	clock->logical_time = real_time;
      }
}


/******************************************************************************/
/*                                                                            */
/*                        Timers                                              */
/*                                                                            */
/******************************************************************************/

/* Timers use logical_time, in order to give results compatibles with
   the alarm (if used in a alarm callback, the results will be consistent
   with the alarm idea of time) 

   It work exactly like a stopwatch.
   
*/



fts_timer_t *
fts_timer_new(fts_symbol_t clock_name)
{
  fts_timer_t *p;

  p = (fts_timer_t *) fts_malloc(sizeof(fts_timer_t));

  p->clock = get_or_make_clock(clock_name);
  p->running = 0;
  p->elapsed_time = 0.0;

  return p;
}

void
fts_timer_free(fts_timer_t *timer)
{
  fts_free(timer);
}


void
fts_timer_init(fts_timer_t *timer, fts_symbol_t clock_name)
{
  timer->clock = get_or_make_clock(clock_name);
  timer->running = 0;
  timer->elapsed_time = 0.0;
}

void
fts_timer_start(fts_timer_t *timer)
{
  if (! timer->running)
    {
      timer->running = 1;
      timer->start_time = timer->clock->logical_time;
    }
}

/*  zero can be called with the timer running or stopped */

void
fts_timer_zero(fts_timer_t *timer)
{
  timer->elapsed_time = 0.0;
  timer->start_time = timer->clock->logical_time;
}

void
fts_timer_stop(fts_timer_t *timer)
{
  if (timer->running)
    {
      timer->running = 0;
      timer->elapsed_time = timer->elapsed_time + (timer->clock->logical_time - timer->start_time);
    }
}


double
fts_timer_elapsed_time(fts_timer_t *timer)
{
  if (timer->running)
    return timer->elapsed_time + (timer->clock->logical_time - timer->start_time);
  else
    return timer->elapsed_time;
}

/******************************************************************************/
/*                                                                            */
/*                        Time Gate                                           */
/*                                                                            */
/******************************************************************************/

/* 
   Time gate.

   The model is that of a gated time: when this function is 
   called, if the gate is open it close the timer gate for the given interval, and
   return 1; if the gate is closed, just return 0
*/ 


fts_time_gate_t *
fts_time_gate_new(fts_symbol_t clock_name)
{
  fts_time_gate_t *p;

  p = (fts_time_gate_t *) fts_malloc(sizeof(fts_time_gate_t));

  p->clock = get_or_make_clock(clock_name);
  p->open_time = p->clock->logical_time;

  return p;
}



void
fts_time_gate_free(fts_time_gate_t *gate)
{
  fts_free(gate);
}

/* Do nothing if already initialized */

void
fts_time_gate_init(fts_time_gate_t *gate, fts_symbol_t clock_name)
{
  gate->clock = get_or_make_clock(clock_name);
  gate->open_time = gate->clock->logical_time;
}

int
fts_time_gate_close(fts_time_gate_t *gate, double interval)
{

  if (gate->clock->logical_time >= gate->open_time)
    {
      gate->open_time = gate->clock->logical_time + interval;
      return 1;
    }
  else
    return 0;
}  

