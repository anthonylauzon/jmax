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
/* Scheduler 

   The New Fts Scheduler it is a static scheduler, that sort the functions to call
   in each tick based on static constraint given at the function installation time.

   The constraint are of type require/provide; the "things" (called "state")
   required/provided are represented    by symbols.

   A  number of states are predefined by the kernel functions, but other can be defined.
   by the user.

   The order rule is the following: 

   f1 < f2 means is executed before; 
   Provide(f) is the set of state the symbol provide
   Require(f) is the set of state the symbol require. 
   included_in, union, intersection are the set operator

   f1, f2, ... fn  < f  ==> Require(f) included_in (Provide(f1) union Provide(f2) union ... Provide(fn)) 

   So note that this is a sufficent condition, but not necessary (??).

   The compilation algoritm is based on this rule: it handle a list of provided state,
   and loop in the function table to find a function that may be executed at with the current
   state; it peek up the first one, and continue, until there no more functions.

   It is not very efficent, but we don't really care, because it is executed only
   once at start up time.

   An variation is: a function can declare that it provide a state for the next tick; 
   they simply added to initial state of the compiler.

   It include now the scheduler time generation.
*/

#include "sys.h"
#include "lang.h"
#include "runtime/sched.h"

/* Global variables:

 */


/* The two clock variables */

static double schedtime_ms_clock = 0.0;
static double schedtime_tick_clock = 0.0;

/* The scheduler tick length in millisecond;
   set by a sampling rate parameter listener
   */

static float tick_length = ((MAXVS * 1000) / 44100.0); /* set the default scheduler tick */


/* the pause length; it say how often (in scheduler ticks) 
   we need to give the processor control back to the OS,
   in order to don't overload the machine.

   Default to 3 hundreds of ticks (around 400 msec, with 44.1Khz/64 samples per ticks)
   */

static int fts_pause_period = 12;

/* forward declarations */

static fts_status_t fts_sched_set_scheduler(int argc, const fts_atom_t *argv);
static void fts_sched_init(void); /* init the scheduler, before the task declarations */
void fts_sched_compile(void); /* compile the scheduler function list  */

static void fts_sched_set_sampling_rate(void *listener, fts_symbol_t name, const fts_atom_t *value);

static void fts_sched_describe_failure(void);

/* Params.h */

#define SCHED_NUM_OF_DECLARATIONS   32
#define SCHED_MAX_NUM_OF_STATES    128
#define SCHED_MAX_NUM_OF_FUNS      256


/*****************************************************************************/
/*                                                                           */
/*          Scheduler Module declaration                                     */
/*                                                                           */
/*****************************************************************************/
/* module declaration */

static void fts_sched_init(void);

fts_module_t fts_sched_module = {"sched", "The FTS static scheduler", fts_sched_init};


/*****************************************************************************/
/*                                                                           */
/*           Scheduler Compiler                                              */
/*                                                                           */
/*****************************************************************************/



/* Schedule compiler */

static struct declaration
{
  void                  (* f)(void);
  enum declaration_verb  mode;
  fts_symbol_t           state;
  const char            *name;		/* Temp ?? */
} *declaration_table;

static int declaration_count, declaration_alloc;

static void
fts_sched_init(void)
{
  /* will be freed by the fts_sched_compile_table function */

  declaration_alloc = SCHED_NUM_OF_DECLARATIONS;
  declaration_table = (struct declaration *)fts_malloc(declaration_alloc * sizeof(struct declaration));
  declaration_count = 0;

  /* Install the timer */

  fts_clock_define_protected(fts_new_symbol("msec"), &schedtime_ms_clock);
  fts_clock_define_protected(fts_new_symbol("tick"), &schedtime_tick_clock);

  /* Install the "ms" timer as default */

  fts_set_default_clock(fts_new_symbol("msec"));

  /* Install the listener for the sampling rate parameter;
   * the actual listener is a null pointer, no state to pass to the handler
   * and we will not deinstall the listener.
   */

  fts_param_add_listener(fts_s_sampling_rate, 0, fts_sched_set_sampling_rate);
}

void
fts_sched_declare(void (* fun)(void), enum declaration_verb decl, fts_symbol_t state, const char *name)
{
  if (declaration_count >= declaration_alloc)
    {
      declaration_alloc += SCHED_NUM_OF_DECLARATIONS;
      declaration_table = (struct declaration *) fts_realloc(declaration_table, declaration_alloc * sizeof(struct declaration));
    }

  declaration_table[declaration_count].f     = fun;
  declaration_table[declaration_count].mode  = decl;
  declaration_table[declaration_count].state = state;
  declaration_table[declaration_count].name  = name;
  declaration_count++;
}


static void (* sched_table[SCHED_MAX_NUM_OF_FUNS])(void);
static const char *sched_table_names[SCHED_MAX_NUM_OF_FUNS];
static  int sched_fun_count = 0;

/* A function is executable iff:

   1- all the function require exists in the current all state_list.
   2- there are no open (not scheduled) declarations that provide a 
      state this function require.

      i.e. not only the require must have been already provided, but *all* the require of 
      this type must be already provided
*/

static int
sched_is_provided(fts_symbol_t state, int state_size, fts_symbol_t *state_list)
{
  int i;
		
  /* search the require in the state_list */

  for (i = 0; i < state_size; i++)
    if (state == state_list[i])
      return 1;

  return 0;
}
	      

static int
sched_is_executable(void (* f)(void), int decl_size, struct declaration *table, int state_size, fts_symbol_t *state_list)
{
  int i;

  for (i = 0; i < decl_size; i++)
    {
      if (table[i].f == f)
	{
	  switch (table[i].mode)
	    {
	    case provide:
	      break;
	    case require:
	      {
		int j;
		
		/* search the require in the state_list */

		if (! sched_is_provided(table[i].state, state_size, state_list))
		  return 0;

		/* now search the require in the open declarations */

		for (j = 0; j < decl_size; j++)
		  if (table[j].f && (table[j].mode == provide) && (table[i].state == table[j].state))
		    return 0;
	      }
	      break;
	    case freerun:
	      return 1;
	    }
	}
    }

  return 1;
}


static void
sched_add_provides(void (* f)(void), int decl_size, struct declaration *table, int *state_size, fts_symbol_t *state_list)
{
  int i;

  for (i = 0; i < decl_size; i++)
    if ((table[i].f == f) && (table[i].mode == provide))
      if (! sched_is_provided(table[i].state, *state_size, state_list)) 
	state_list[(*state_size)++] = table[i].state;
}


static void
sched_delete_declarations(void (* f)(void), int decl_size, struct declaration *table)
{
  int i;

  for (i = 0; i < decl_size; i++)
    if (table[i].f == f)
      table[i].f = 0; 
}


void
fts_sched_compile(void)
{
  fts_symbol_t state_list[SCHED_MAX_NUM_OF_STATES];
  int state_count = 0;
  int empty_declaration_table = 0;
  int found_a_function = 1;

  /* while the table is not empty, look for a declaration
     that correspond to a function that can be executed;
     if it can, add their provide to the state_list,
     and delete the declaration from the table (fun = 0) */

  while (! empty_declaration_table && found_a_function)
    {
      int i;

      empty_declaration_table = 1;
      found_a_function = 0;

      for (i = 0; i < declaration_count; i++)
	{
	  void (* candidate_fun)(void);

	  candidate_fun = declaration_table[i].f;

	  if (candidate_fun)
	    {
	      empty_declaration_table = 0;

	      if (sched_is_executable(candidate_fun, declaration_count,
				      declaration_table, state_count, state_list))
		{
		  if (sched_fun_count >= SCHED_MAX_NUM_OF_FUNS)
		    {
		      /* FATAL !! should be an event, and have a nicer exit */
		      fprintf(stderr,"Out of scheduler function, configuration error\n");
		      exit(0);	
		    }

		  found_a_function = 1;
		  sched_table[sched_fun_count] = candidate_fun;
		  sched_table_names[sched_fun_count] = declaration_table[i].name;
		  sched_fun_count++;

		  sched_add_provides(candidate_fun, declaration_count, declaration_table, &state_count, state_list);
		  sched_delete_declarations(candidate_fun, declaration_count, declaration_table);

		  /* restart from the beginning */
		  break;
		}
	    }
	}
    } 

  if (!empty_declaration_table)
    {
      /* FATAL !! should be an event, and have a nicer exit */

      fprintf(stderr,"Inconsistent scheduler declarations , configuration error\n");
      fts_sched_describe_failure();

      exit(0);	
    }

  /* finally, free the declaration table */

  fts_free(declaration_table);

}

/*****************************************************************************/
/*                                                                           */
/*          Run Time Scheduler                                               */
/*                                                                           */
/*****************************************************************************/

/* A new scheduler; the actual scheduler can be changed 
   by the user with UCS commands; we do not support user scheduler
   functions at the moment, but only a predefined set.

   Existing schedulers are: a free run scheduler, a profile instrumented
   scheduler, and a fixed number of ticks scheduler.
*/

/* Scheduler status: suspended means that the user requested
   a scheduler function change
*/


static enum {running, halted, dead} fts_running_status;

void
fts_halt(void)
{
  fts_running_status = halted;
}

void
fts_restart(void)
{
  fts_modules_restart();
}

void
fts_sched_run(void)
{
  int tick_counter = 0;
  fts_running_status = running;

  while (fts_running_status == running)
    {
      int i;

      for (i = 0; i < sched_fun_count; i++)
	(*sched_table[i])();

      schedtime_tick_clock += 1.0;
      schedtime_ms_clock =  schedtime_tick_clock * tick_length;

      tick_counter++;

      if (tick_counter > fts_pause_period)
	{
	  fts_pause(); /* call the OS dependent pause function */
	  tick_counter = 0;
	}
    }
}


/*****************************************************************************/
/*                                                                           */
/*          Describe/Debug functions                                         */
/*                                                                           */
/*****************************************************************************/

/* Debug/describe function */

void
fts_sched_describe_order(void)
{
  int i;

  fprintf(stderr,"Scheduling order:\n");

  for (i = 0; i < sched_fun_count; i++)
    fprintf(stderr, "  %d: %s\n", i, sched_table_names[i]); 
}


void
fts_sched_describe_declarations(void)
{
  int i;

  fprintf(stderr, "%d Declarations:\n", declaration_count);

  for (i = 0; i < declaration_count; i++)
    {
      char *s;

      switch (declaration_table[i].mode)
	{
	case require:
	  s = "require";
	  break;
	case provide:
	  s = "provide";
	  break;
	case freerun:
	  s = "freerun";
	  break;
	default:
	  s = "(unknown declaration)";
	  break;
	}

      if ( declaration_table[i].state)
	fprintf(stderr,"  %s %s %s\n", declaration_table[i].name, s,
		fts_symbol_name(declaration_table[i].state));
      else
	fprintf(stderr,"  %s %s\n", declaration_table[i].name, s);
    }
}

static void
fts_sched_describe_failure(void)
{
  int i;

  fprintf(stderr, "%d Declarations:\n", declaration_count);

  for (i = 0; i < declaration_count; i++)
    {
      char *s;

      switch (declaration_table[i].mode)
	{
	case require:
	  s = "require";
	  break;
	case provide:
	  s = "provide";
	  break;
	case freerun:
	  s = "freerun";
	  break;
	default:
	  s = "<unknown declaration>";
	  break;
	}

      if ( declaration_table[i].f)
	{
	  if ( declaration_table[i].state)
	    fprintf(stderr,"  %s %s %s unresolved\n", declaration_table[i].name, s, fts_symbol_name(declaration_table[i].state));
	  else
	    fprintf(stderr,"  %s %s unresolved\n", declaration_table[i].name, s);
	}
      else
	{
	  if ( declaration_table[i].state)
	    fprintf(stderr,"  %s %s %s resolved\n", declaration_table[i].name, s, fts_symbol_name(declaration_table[i].state));
	  else
	    fprintf(stderr,"  %s %s resolved\n", declaration_table[i].name, s);
	}
    }
}


/* Tick length handling */

float fts_sched_get_tick_length(void)
{
  return tick_length;
}


static void
fts_sched_set_sampling_rate(void *listener, fts_symbol_t name, const fts_atom_t *value)
{
  if (fts_is_number(value))
    {
      float f;

      f = fts_get_number_float(value);
      tick_length =  ((MAXVS * 1000) / f); /* set the scheduler tick */
    }
}


/* Pause handling  */

void fts_sched_set_pause(int p)
{
  fts_pause_period = p;
}

