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
/*
  Argument doctor; expand an "argument i" object to "const $args[i]",
  where i should be an integer.
 */

#include "fts.h"

static fts_object_t *argument_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if ((ac == 2) && fts_is_int(&(at[1])))
    {
      fts_object_t *obj;
      fts_atom_t a[6];

      fts_set_symbol(&a[0], fts_new_symbol("const"));
      fts_set_symbol(&a[1], fts_s_dollar);
      fts_set_symbol(&a[2], fts_s_args);
      fts_set_symbol(&a[3], fts_s_open_sqpar);
      a[4] = at[1];
      fts_set_symbol(&a[5], fts_s_closed_sqpar);

      obj = fts_object_new(patcher, 6, a);
      fts_object_reset_description(obj);
      return obj;
    }
  else
    return 0;
}


void argument_init()
{
  fts_register_object_doctor(fts_new_symbol("argument"), argument_doctor);
}
    
