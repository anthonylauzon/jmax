/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_VARIABLE_H_
#define _FTS_PRIVATE_VARIABLE_H_

typedef struct fts_definition_listener fts_definition_listener_t;

struct fts_definition
{
  fts_symbol_t name;
  fts_atom_t value;
  struct fts_definition *global;
  fts_definition_listener_t *listeners;
};

#define fts_definition_get_name(d) ((d)->name)

#define fts_definition_get_value(d) (&(d)->value)
#define fts_definition_set_value(d, x) ((d)->value = (*x))

#define fts_definition_is_global(d) ((d)->global != NULL)

#define fts_definition_get_listeners(d) (&(d)->listeners)
extern void fts_definition_add_listener(fts_definition_t *def, fts_object_t *obj);
extern void fts_definition_remove_listener(fts_definition_t *def, fts_object_t *obj);

/* get definition by name from given patcher */
extern fts_definition_t *fts_definition_get(fts_patcher_t *patcher, fts_symbol_t name);

/* set value and call listeners */
extern void fts_definition_update(fts_definition_t *def, const fts_atom_t *a);

/* set scope */
extern void fts_definition_set_global(fts_definition_t *def);
extern void fts_definition_set_local(fts_definition_t *def);

#endif
