
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.1 $ IRCAM $Date: 1997/12/08 16:52:34 $
 *
 */

#include "fts.h"

/*------------------------- change class -------------------------------------*/

typedef struct {
  fts_object_t o;
  long r_state;
} change_t;

static void
change_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((change_t *)o)->r_state = (long) fts_get_number_arg(ac, at, 1, 0);
}

static void
change_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  long n;
  if ((n = fts_get_number(at)) != ((change_t *)o)->r_state)
    {
      fts_outlet_send(o, 0, fts_s_int, ac, at);
      ((change_t *)o)->r_state = n;
    }
}

static void
change_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ((change_t *)o)->r_state = fts_get_long_arg(ac, at, 0, 0);
}

static fts_status_t
change_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(change_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, change_init, 2, a, 1);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, change_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, change_int, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_set, change_set, 1, a);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
change_config(void)
{
  fts_metaclass_create(fts_new_symbol("change"),change_instantiate, fts_always_equiv);
}

