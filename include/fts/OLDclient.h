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

#ifndef _FTS_OLDCLIENT_H_
#define _FTS_OLDCLIENT_H_

FTS_API void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property);
FTS_API void fts_object_ui_property_changed(fts_object_t *obj, fts_symbol_t property);

FTS_API void fts_client_start_clientmess(void);
FTS_API void fts_client_add_int(int value);
FTS_API void fts_client_add_float(float value);
FTS_API void fts_client_add_symbol(fts_symbol_t s);
FTS_API void fts_client_add_object(fts_object_t *obj);
FTS_API void fts_client_add_atoms(int ac, const fts_atom_t *args);
FTS_API void fts_client_done_msg(void);

#endif
