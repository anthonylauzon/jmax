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
   Support for user modules startup messages.
   */


#include "sys.h"


static fts_welcome_t *welcome_list = 0;

void
fts_add_welcome(fts_welcome_t *w)
{
  if (welcome_list)
    {
      fts_welcome_t *p;

      for (p = welcome_list; p->next; p = p->next)
	;
      p->next = w;
    }
  else
    welcome_list = w;
}

/* fts_welcome_apply:
   hw or module dependent welcome should be moved in the proper
   module init function */

void
fts_welcome_apply(void (* f)(const char *))
{
  fts_welcome_t *p;

  for (p = welcome_list; p; p = p->next)
    (* f)(p->msg);
}



