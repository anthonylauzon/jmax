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

#include <fts/fts.h>

static fts_object_t *
send_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];

  if(ac == 2 && fts_is_symbol(at + 1))
    {
      fts_set_symbol(a + 0, fts_s_outlet);
      a[1] = at[1];
      fts_object_new_to_patcher(patcher, 2, a, &obj);

      return obj;
    }
  
  return 0;
}

static fts_object_t *
receive_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];

  if(ac == 2 && fts_is_symbol(at + 1))
    {
      fts_set_symbol(a + 0, fts_s_inlet);
      a[1] = at[1];
      fts_object_new_to_patcher(patcher, 2, a, &obj);

      return obj;
    }
  
  return 0;
}

void
ispwcomp_config(void)
{
  fts_register_object_doctor(fts_new_symbol("send"), send_doctor);
  fts_register_object_doctor(fts_new_symbol("s"), send_doctor);
  fts_register_object_doctor(fts_new_symbol("receive"), receive_doctor);
  fts_register_object_doctor(fts_new_symbol("r"), receive_doctor);
}

