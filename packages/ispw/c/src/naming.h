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

#ifndef _NAMING_H_
#define _NAMING_H_

#include "ispw.h"

ISPW_API fts_object_t *ispw_get_object_by_name(fts_symbol_t name);
ISPW_API void ispw_register_named_object(fts_object_t *obj, fts_symbol_t name);
ISPW_API void ispw_unregister_named_object(fts_object_t *obj, fts_symbol_t name);

ISPW_API fts_object_t *ispw_get_target(fts_patcher_t *scope, fts_symbol_t name);
ISPW_API void ispw_target_send(fts_object_t *target, fts_symbol_t s, int ac, const fts_atom_t *at);

#endif
