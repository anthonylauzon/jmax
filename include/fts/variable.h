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
 */

typedef struct fts_definition fts_definition_t;

FTS_API void fts_name_set_value(fts_patcher_t *patcher, fts_symbol_t name, const fts_atom_t *value);

FTS_API void fts_name_add_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj);
FTS_API void fts_name_remove_listener(fts_patcher_t *patcher, fts_symbol_t name, fts_object_t *obj);

FTS_API fts_atom_t *fts_name_get_value(fts_patcher_t *patcher, fts_symbol_t name);

FTS_API fts_symbol_t fts_name_get_unused(fts_patcher_t *patcher, fts_symbol_t name);

FTS_API void fts_name_method( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

