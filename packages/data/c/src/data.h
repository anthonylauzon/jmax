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
 */
#ifndef _DATA_H
#define _DATA_H

#include <fts/fts.h>

#ifdef WIN32
#if defined(DATA_EXPORTS)
#define DATA_API __declspec(dllexport)
#else
#define DATA_API __declspec(dllimport)
#endif
#else
#define DATA_API extern
#endif

typedef struct _data_object
{
  fts_object_t o;
  fts_symbol_t keep;
} data_object_t;

#define data_object_set_keep(o, k) ((o)->keep = (k))
#define data_object_get_keep(o) ((o)->keep)

DATA_API void data_object_daemon_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);
DATA_API void data_object_daemon_get_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);
DATA_API void data_object_set_dirty(fts_object_t *o);

DATA_API void
data_config(void);

#endif /* _DATA_H */
