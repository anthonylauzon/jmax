#ifndef _FTSTIME_H
#define _FTSTIME_H

/* Time handling module.


*/

/* module declaration */

extern fts_module_t fts_time_module;

/* Clock */

typedef struct _fts_clock
{
  fts_symbol_t name;		/* the clock name */

  double *real_time;		/* the pointer to the real time source */
  double logical_time;		/* the logical time (see above) */

  struct _fts_alarm *alarm_list; /* alarm scheduling list, past and future */
  struct _fts_alarm *last_alarm; /* last object in the alarm scheduling list (for loop optimization) */
  struct _fts_alarm *future_alarm_list; /* alarm scheduling list, only future;
					 actually, an intermediate pointer in the previous list */

  int enabled;			/* 1 if enabled, 0 if we don't know it yet or was undefined */
  int protected;	        /* 1 if protected, 0 otherwise; protected clocks cannot be redefined */
  struct _fts_clock *next; /* next in the clock list */
} fts_clock_t;


extern void  fts_clock_define(fts_symbol_t name, double *clock);
extern void  fts_clock_define_protected(fts_symbol_t name, double *clock);
extern void  fts_clock_undefine(fts_symbol_t clock_name);

extern void  fts_set_default_clock(fts_symbol_t clock_name); 
extern int   fts_clock_exists(fts_symbol_t clock_name);
extern double *fts_clock_get_real_time_p(fts_symbol_t clock_name); /* undocumented ?? */
extern double fts_clock_get_time(fts_symbol_t clock_name);


/* alarm: call the function when the alarm expire, with the given argument

   Can reset the alarm, and delay it, or move to another absolute time
 */


typedef struct _fts_alarm
{
  double when;			/* when to trigger this alarm */

  void (* fun)(struct _fts_alarm *, void *);		/* the function to call when the alarm trigger */
  void *arg;			/* the argument to pass to the function */

  struct _fts_alarm *next;	/* next alarm for the same clock */
  struct _fts_alarm *prev;	/* prev alarm for the same clock (or 0 if the first) */

  int active;			/* non zero when inserted in a list */

  /* cycled alarms */

  double cycle;			/* the cycle size */
  double phase;			/* the phase with respect to absolute time 0.0 */
  double cycle_counter;		/* the number of cycles done after absoute time 0.0 */
  
  fts_clock_t *clock;		/* a pointer to the clock to use with this alarm */
} fts_alarm_t;

/* Dynamic creation Or static creation */

extern fts_alarm_t *fts_alarm_new(fts_symbol_t clock_name, void (* fun)(fts_alarm_t *, void *), void *arg);
extern void       fts_alarm_free(fts_alarm_t *alarm);
extern void       fts_alarm_init(fts_alarm_t *alarm, fts_symbol_t clock_name, void (* fun)(fts_alarm_t *, void *), void *arg);

/* Set_time just set the time, but do not put the alarm in the alarm queue;
   Set delay set the time as delay to the current time.

   _arm put the alarm in the alarm queue without changing the planned time.

   _unarm take the alarm away from the queue.
*/

extern void fts_alarm_set_time(fts_alarm_t *alarm,  double when);
extern void fts_alarm_set_delay(fts_alarm_t *alarm, double when);
extern void fts_alarm_set_cycle(fts_alarm_t *alarm, double cycle);

extern void fts_alarm_arm(fts_alarm_t *alarm);
extern void fts_alarm_unarm(fts_alarm_t *alarm);	

/* Predicates: it is guaranteed that for a freshly allocated or initted alarm
   fts_alarm_is_in_future return false.
*/

extern int  fts_alarm_is_in_future(fts_alarm_t *alarm);
extern int  fts_alarm_is_armed(fts_alarm_t *alarm);

/* timer  */

typedef struct _fts_timer
{
  int running;			/* 1 if running, zero otherwise */
  double elapsed_time;
  double start_time;
  fts_clock_t *clock;
} fts_timer_t;


extern fts_timer_t *fts_timer_new(fts_symbol_t clock_name);
extern void       fts_timer_free(fts_timer_t *timer);
extern void       fts_timer_init(fts_timer_t *timer, fts_symbol_t clock_name);

extern void fts_timer_start(fts_timer_t *timer);	
extern void fts_timer_zero(fts_timer_t *timer);	
extern void fts_timer_stop(fts_timer_t *timer);	
extern double fts_timer_elapsed_time(fts_timer_t *timer);	

/* 
   Time gate.

   The model is that of a gated time: when this function is 
   called, if the gate is open it close the timer gate for the given interval, and
   return 1; if the gate is closed, just return 0
*/ 

typedef struct _fts_time_gate
{
  fts_clock_t *clock;

  double open_time;
} fts_time_gate_t;

extern fts_time_gate_t *fts_time_gate_new(fts_symbol_t clock_name);
extern void       fts_time_gate_free(fts_time_gate_t *time_gate);
extern void       fts_time_gate_init(fts_time_gate_t *timer, fts_symbol_t clock_name);

extern int fts_time_gate_close(fts_time_gate_t *gate, double interval);

#endif


