/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
/* correlated random generator  <quick and rather dirty>
 * from the original code by (?) Zack Settel
 */

#include "fts.h"

#define MAXVAL	0x7FFF

typedef struct 
{
  fts_object_t ob;
  long reg;
  float scaler;
  long range;
} drunk_t;


/*  output a value  */

static void
drunk_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  extern int rand(void);
  drunk_t *x = (drunk_t *)o;
  int offset, newval;

  offset = 1 + (int)(x->scaler * (0x7FFF & rand()));

  if (x->reg > x->range)
    x->reg = x->range;
  else if (x->reg < 0)
    x->reg = 0;

  offset = (rand() & 512) ? -offset : offset;
  newval = offset + x->reg;

  if (newval < 0 || newval > x->range)
    newval = x->reg - offset;

  x->reg = newval;

  fts_outlet_int(o, 0, newval);
}


/*   change correlation   */

static void
drunk_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n < 0)
    n = 0;
  else if (n > x->range)
    n = x->range;

  x->reg = n;

  fts_outlet_int(o, 0, n);
}

/*  change range -- msp  */

static void
drunk_number_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  if (n <= 0)
    n = 1;

  x->range = n;

  if (x->reg >= n)
    x->reg = n-1;
}


/*   change correlation   */

static void
drunk_number_2(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  n = (n > x->range / 2L) ? (x->range / 2L) : n;
  x->scaler = (float)n / MAXVAL;
}


/* Method for list */
static void
drunk_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if ((ac >= 3) && fts_is_number(&at[2]))
    drunk_number(o, 2, s, 1, at + 2);

  if ((ac >= 2) && fts_is_number(&at[1]))
    drunk_number(o, 1, s, 1, at + 1);

  if ((ac >= 1) && fts_is_number(&at[0]))
    drunk_number(o, 0, s, 1, at);
}


/*   change current value (position in drunk walk)  */

static void
drunk_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long n = (long) fts_get_int_arg(ac, at, 0, 0);

  x->reg = n % x->range;
}


static void
drunk_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *x = (drunk_t *)o;
  long int range       = (long) fts_get_int_arg(ac, at, 1, 0);
  long int correlation = (long) fts_get_int_arg(ac, at, 2, 0);

  if (range <= 0)
    range = 128;

  if (correlation <= 0)
    correlation = 1;

  correlation = (correlation > range / 2L) ? (range / 2L) : correlation;
  x->scaler = (float)correlation / MAXVAL;
  x->reg = range / 2L;
  x->range = range;
}


static fts_status_t
drunk_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(drunk_t), 3, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, drunk_init, 3, a, 1);

  /* Drunk args */

  fts_method_define(cl, 0, fts_s_bang, drunk_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("set"), drunk_set, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int,   drunk_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, drunk_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, drunk_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int,   drunk_number_1, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 1, fts_s_float, drunk_number_1, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 2, fts_s_int,   drunk_number_2, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 2, fts_s_float, drunk_number_2, 1, a);

  /* Type the outlet */

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0,	fts_s_int, 1, a);

  return fts_Success;
}

void
drunk_config(void)
{
  fts_class_install(fts_new_symbol("drunk"),drunk_instantiate);
}

