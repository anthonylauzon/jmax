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
   This file define the event generation scheduling mechanism for objects.
*/

/* Should this be a Tile ??? */

#ifndef _UPDATES_H_
#define _UPDATES_H_

/* functions */

extern void fts_updates_set_updates_per_ticks(int upt);
extern void fts_updates_set_update_period(int upt);
extern void fts_client_updates_sync(void);

/* Changes handling */

extern void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property);
extern void fts_object_ui_property_changed(fts_object_t *obj, fts_symbol_t property);
extern void fts_object_reset_changed(fts_object_t *obj);

#endif
