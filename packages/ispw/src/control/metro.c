#include "fts.h"

typedef struct 
{
  fts_object_t ob;
  double cycle;
  long run;
  int  changed;
  fts_alarm_t alarm;
} metro_t;


static void
metro_tick(fts_alarm_t *alarm, void *o)
{
  metro_t *x = (metro_t *)o;

  fts_outlet_bang((fts_object_t *)o, 0);

  if (x->changed)
    {
      fts_alarm_set_cycle(&x->alarm, x->cycle);
      x->changed = 0;
    }
}


static void
metro_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;

  x->run = 1;
  x->changed = 0;
  fts_alarm_set_cycle(&x->alarm, x->cycle);
  fts_alarm_arm(&x->alarm);
  fts_outlet_bang((fts_object_t *)o, 0);
}


static void
metro_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;

  x->run = 0;
  fts_alarm_unarm(&x->alarm);
}


static void
metro_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n)
    {
      x->run = 1;
      x->changed = 0;
      fts_alarm_set_cycle(&x->alarm, x->cycle);
      fts_alarm_arm(&x->alarm);
      fts_outlet_bang((fts_object_t *)o, 0);
    }
  else
    {
      x->run = 0;
      fts_alarm_unarm(&x->alarm);
    }
}


/* Installed as int method for inlet 1 */

static void
metro_set_metro(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;
  double n = fts_get_double_arg(ac, at, 0, 0);

  if (n <= 0.0)
    n = 5.0;

  if (n != x->cycle)
    {
      x->changed = 1;
      x->cycle = n;
    }
}


static void
metro_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;

  fts_alarm_unarm(&x->alarm);
}

static void
metro_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  metro_t *x = (metro_t *)o;
  fts_symbol_t clock = 0;
  float n;

  if(ac > 1 && fts_is_symbol(at + 1))
    {
      clock = fts_get_symbol(at + 1);
      n = (float) fts_get_float_arg(ac, at, 2, 0.0f);
    }
  else
    {
      n = (float) fts_get_float_arg(ac, at, 1, 0.0f);    
    }

  if (clock)
    {
      if (! fts_clock_exists(clock))
	post("metro: warning: clock %s does not exists, yet\n", fts_symbol_name(clock));

      fts_alarm_init(&x->alarm, clock, metro_tick, x);
    }
  else
    fts_alarm_init(&x->alarm, 0, metro_tick, x);

  x->changed = 0;

  if (n <= 0.0)
    x->cycle = 5.0;
  else
    x->cycle = n;
}


static fts_status_t
metro_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(metro_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;

  a[1] = fts_s_anything;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, metro_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, metro_delete, 0, 0);

  /* Metro args */

  fts_method_define(cl, 0, fts_s_bang, metro_bang, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("stop"), metro_stop, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, metro_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, metro_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, metro_set_metro, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, metro_set_metro, 1, a);

  /* Type the outlet */

  fts_outlet_type_define(cl, 0,	fts_s_bang, 0, 0);

  return fts_Success;
}


void
metro_config(void)
{
  fts_metaclass_create(fts_new_symbol("metro"),metro_instantiate, fts_always_equiv);
}

