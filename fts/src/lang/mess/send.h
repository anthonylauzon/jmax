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

#ifndef _SEND_H_
#define _SEND_H_

extern fts_metaclass_t *fts_send_metaclass;
extern fts_metaclass_t *fts_receive_metaclass;

#define fts_object_is_send(o) (fts_object_get_class_name(o) == fts_send_metaclass)
#define fts_object_is_receive(o) ((o)->cl->mcl == fts_receive_metaclass)

extern int fts_send_message_to_receives(fts_symbol_t name, fts_symbol_t selector, int ac, const fts_atom_t *at);
extern int fts_receive_exists(fts_symbol_t name);

extern fts_object_t *fts_send_get_first_receive(fts_object_t *send);
extern fts_object_t *fts_receive_get_next_receive(fts_object_t *receive);

#endif
