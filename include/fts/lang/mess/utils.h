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

#ifndef _FTS_MESS_UTILS_H_
#define _FTS_MESS_UTILS_H_

/* data atoms */
#define fts_data_atom_get_type(atom) ((fts_is_data(&atom))? (fts_data_get_class_name(fts_get_data(&atom))): (fts_get_type(&atom)))
FTS_API void fts_data_atom_set(fts_atom_t *atom, fts_atom_t new);
FTS_API void fts_data_atom_void(fts_atom_t *atom);

/* data -> atom list */
FTS_API int fts_data_get_size(fts_data_t *data);
FTS_API int fts_data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at);

/* object utils */
FTS_API void fts_method_define_data(fts_class_t *class, int winlet, fts_method_t fun);
FTS_API void fts_outlet_data(fts_object_t *o, int woutlet, fts_data_t *data);
#define fts_outlet_data_atom(o, woutlet, atom) do{fts_outlet_send(o, woutlet, fts_data_atom_get_type(atom), 1, &atom);}while(0)

#endif

