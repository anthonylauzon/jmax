/* 
   The at class; at an absolute time, output a bang.
   Nothing else; the absolute time can be changed by means of
   the int message on the inlet; setting a past time will 
   trigger the bang immediately.
   
   A stop message reset the at, and disable the bang completely.

   A second optional argument set the clock the time is relative to.
   default to ms.
*/

#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  fts_alarm_t alarm;
} at_t;


void
at_tick(fts_alarm_t *alarm, void *o)
{
  /* Do not unarm the alarm; keep it good if time go
     backward (can happen, everything can happen in FTS :->) */

  fts_outlet_bang((fts_object_t *)o, 0);
}

static void
at_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  at_t *x = (at_t *)o;

  fts_alarm_unarm(&x->alarm);
}


static void
at_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  at_t *x = (at_t *)o;
  double n = fts_get_double_arg(ac, at, 0, 0);

  fts_alarm_set_time(&x->alarm, n);
  fts_alarm_arm(&x->alarm);
}


/* Installed as int method for inlet 1 */

static void
at_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  at_t *x = (at_t *)o;
  fts_symbol_t clock;
  double n;

  clock = fts_get_symbol_arg(ac, at, 1, 0);

  if(ac > 1 && fts_is_symbol(at + 1))
    {
      clock = fts_get_symbol(at + 1);
      n = fts_get_double_arg(ac, at, 2, 0);
    }
  else
    {
      n = fts_get_double_arg(ac, at, 1, 0);    
    }

  if (clock)
    {
      if (! fts_clock_exists(clock))
	post("at: warning: clock %s does not exists, yet\n", fts_symbol_name(clock));

      fts_alarm_init(&x->alarm, clock, at_tick, x);
    }
  else
    fts_alarm_init(&x->alarm, 0, at_tick, x);

  fts_alarm_set_time(&x->alarm, n);
  fts_alarm_arm(&x->alarm);
}

static void
at_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  at_t *x = (at_t *)o;

  fts_alarm_unarm(&x->alarm);
}

static fts_status_t
at_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(at_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, at_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, at_delete, 0, 0);

  /* At args */

  fts_method_define(cl, 0, fts_new_symbol("stop"), at_stop, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, at_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, at_number, 1, a);

  /* Type the outlet */

  fts_outlet_type_define(cl, 0,	fts_s_bang, 0, 0);

  return fts_Success;
}


void
at_config(void)
{
  fts_metaclass_create(fts_new_symbol("at"),at_instantiate, fts_always_equiv);
}

