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

#ifndef _FTS_PATABS_H_
#define _FTS_PATABS_H_

FTS_API void fts_abstraction_init(void);

FTS_API void fts_abstraction_declare(fts_symbol_t name, fts_symbol_t filename);
FTS_API void fts_abstraction_declare_path(fts_symbol_t path);

FTS_API fts_object_t *fts_abstraction_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
FTS_API fts_object_t *fts_abstraction_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

#endif