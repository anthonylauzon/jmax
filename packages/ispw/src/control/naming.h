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

#ifndef _NAMING_H_
#define _NAMING_H_

extern fts_object_t *ispw_get_object_by_name(fts_symbol_t name);
extern void ispw_register_named_object(fts_object_t *obj, fts_symbol_t name);
extern void ispw_unregister_named_object(fts_object_t *obj, fts_symbol_t name);
extern int ispw_named_object_exists(fts_symbol_t name);
extern void ispw_named_object_send(fts_symbol_t name, fts_symbol_t s, int argc, const fts_atom_t *argv);

#endif
