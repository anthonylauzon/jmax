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



#include "lists.h"

extern void list_config(void);
extern void listarith_config(void);
extern void listsum_config(void);
extern void listelement_config(void);
extern void listhead_config(void);
extern void listjoin_config(void);
extern void listsize_config(void);
extern void listreverse_config(void);
extern void listsublist_config(void);
extern void listtail_config(void);

void
lists_config(void)
{
  list_config();
  listarith_config();
  listsum_config();
  listelement_config();
  listhead_config();
  listjoin_config();
  listsize_config();
  listreverse_config();
  listsublist_config();
  listtail_config();
}
