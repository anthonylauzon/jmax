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

      /* object: const $args[<value>] */

      fts_set_symbol(&a[0], fts_new_symbol("const"));
      fts_set_symbol(&a[1], fts_s_dollar);
      fts_set_symbol(&a[2], fts_s_args);
      fts_set_symbol(&a[3], fts_s_open_sqpar);
      a[4] = at[1];
      fts_set_symbol(&a[5], fts_s_closed_sqpar);

      obj = fts_eval_object_description(patcher, 6, a);
      fts_object_reset_description(obj);
      return obj;
    }
  else if ((ac >= 3) && fts_is_int(&(at[1])))
    {
      int i;
      fts_object_t *obj;
      fts_atom_t a[512];

      /* object: const ( _getElement($args, <value2>, ( <values> )) )  */

      fts_set_symbol(&a[0], fts_new_symbol("const"));
      fts_set_symbol(&a[1], fts_s_open_par);
      fts_set_symbol(&a[2], fts_new_symbol("_getElement"));
      fts_set_symbol(&a[3], fts_s_open_par);
      fts_set_symbol(&a[4], fts_s_dollar);
      fts_set_symbol(&a[5], fts_s_args);
      fts_set_symbol(&a[6], fts_s_comma);
      a[7] = at[1];
      fts_set_symbol(&a[8], fts_s_comma);

      fts_set_symbol(&a[9], fts_s_open_par);

      for (i = 0; i < ac - 2; i++)
	a[i + 10] = at[i + 2];

      fts_set_symbol(&a[10 + (ac - 2)], fts_s_closed_par);
      fts_set_symbol(&a[11 + (ac - 2)], fts_s_closed_par);
      fts_set_symbol(&a[12 + (ac - 2)], fts_s_closed_par);

      obj = fts_eval_object_description(patcher, 12 + (ac - 2), a);
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
    
