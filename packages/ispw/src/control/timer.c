/* "timer"  bang to save current time &
   bang rite corner to send elapsed time message 

   Don't call timer_t this object, it conflict with
   system types in many platforms.

*/


#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_timer_t timer;
} timer_obj_t;

/* store time-now on default bang message */

static void
timer_zero(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_obj_t *x = (timer_obj_t *)o;

  fts_timer_zero(&x->timer);
}

static void
timer_continue(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_obj_t *x = (timer_obj_t *)o;

  fts_timer_start(&x->timer);
}

static void
timer_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_obj_t *x = (timer_obj_t *)o;

  fts_timer_stop(&x->timer);
}



static void
timer_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_obj_t *x = (timer_obj_t *)o;

  fts_timer_zero(&x->timer);
  fts_timer_start(&x->timer);
}

/* bang message into rite corner sends fix out  */

static void
timer_send_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_obj_t *x = (timer_obj_t *)o;

  fts_outlet_float(o, 0, fts_timer_elapsed_time(&x->timer));
}

static void
timer_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  timer_obj_t *x = (timer_obj_t *)o;
  fts_symbol_t clock = fts_get_symbol_arg(ac, at, 1, 0);

  fts_timer_init(&x->timer, clock);
  fts_timer_zero(&x->timer);
  fts_timer_start(&x->timer);
}


static fts_status_t
timer_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[10];

  /* initialize the class */

  fts_class_init(cl, sizeof(timer_obj_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, timer_init, 2, a, 1);

  /* user methods */

  fts_method_define(cl, 0, fts_s_bang, timer_zero, 0, 0);

  /* Messages added, keeping compatibility with the old timer:
     zero: as bang, set the timer to zero.
     pause: stop the timer, don't change the time.
     continue: start the timer from the current time.
     stop: stop the timer, put the time to zero.
     start: start the timer from zero.
     time: as bang on the right outlet.
     */

  fts_method_define(cl, 0, fts_new_symbol("zero"),  timer_zero, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("start"), timer_start, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("stop"),  timer_stop, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("continue"), timer_continue, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("time"),  timer_send_time, 0, 0);

  fts_method_define(cl, 1, fts_s_bang, timer_send_time, 0, 0);

  /*  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, timer_send_time, 1, a); */

  /* Type the outlet */

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}

void
timer_config(void)
{
  fts_metaclass_create(fts_new_symbol("timer"),timer_instantiate, fts_always_equiv);
}
