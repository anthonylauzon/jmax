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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_OBJECT_H_
#define _FTS_PRIVATE_OBJECT_H_

#define fts_object_set_id(o, i) ((o)->client_id = (i))

#define fts_object_set_definition(o, d) ((o)->definition = (d))
#define fts_object_get_definition(o) ((o)->definition)

#define fts_object_is_error(o) (fts_object_get_class(o) == fts_error_object_class)

#define fts_object_is_abstraction(o) (fts_object_is_patcher((o)) && fts_patcher_is_abstraction((fts_patcher_t *) (o)))
#define fts_object_is_standard_patcher(o) (fts_object_is_patcher((o)) && fts_patcher_is_standard((fts_patcher_t *) (o)))
#define fts_object_is_template(o) (fts_object_is_patcher((o)) && fts_patcher_is_template((fts_patcher_t *) (o)))

#define fts_object_has_patcher_data(o) ((o)->patcher_data != NULL)

#define fts_object_get_next_in_patcher(o) ((o)->patcher_data->next_in_patcher)
#define fts_object_set_next_in_patcher(o, x) ((o)->patcher_data->next_in_patcher = (x))

#define fts_object_get_inlet_connections(o, i) ((o)->patcher_data->in_conn[(i)])
#define fts_object_get_outlet_connections(o, i) ((o)->patcher_data->out_conn[(i)])

extern fts_object_t *fts_object_create_in_patcher(fts_class_t *cl, fts_patcher_t *patcher, int ac, const fts_atom_t *at);

extern fts_object_patcher_data_t *fts_object_get_patcher_data(fts_object_t *obj);
extern void fts_object_remove_patcher_data(fts_object_t *obj);

extern void fts_object_unpatch(fts_object_t *obj);

extern void fts_object_add_binding(fts_object_t *obj, fts_definition_t *def);

/* support for redefinition */
extern fts_object_t *fts_object_recompute(fts_object_t *old);
extern fts_object_t *fts_object_redefine(fts_object_t *old, int ac, const fts_atom_t *at);

#endif
