/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

/*
  Argument doctor; expand an "argument i" object to "const $args[i]",
  where i should be an integer.
 */

#include <fts/fts.h>

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
    
