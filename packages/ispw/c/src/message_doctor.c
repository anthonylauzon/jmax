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
  Messbox doctor.

  Fix messbox boxes created with as argument the content of the messbox.
  Message box like that are present in some binary and tcl patches
  around, saved before 20/5/1998.
 */

#include <fts/fts.h>

static fts_object_t *messbox_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  if (ac >= 1)
    {
      fts_object_t *obj;

      fts_object_new_to_patcher(patcher, 1, at, &obj);

      if (ac > 1)
	{
	  fts_send_message(obj, fts_SystemInlet, fts_s_clear, 0, 0);
	  fts_send_message(obj, fts_SystemInlet, fts_s_append, ac - 1, at + 1);
	}

      return obj;
    }
  else
    return 0;
}


void messbox_doctor_init(void)
{
  /*fts_register_object_doctor(fts_new_symbol("messbox"), messbox_doctor);*/
}

