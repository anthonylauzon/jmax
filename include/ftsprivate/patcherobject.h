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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_PATCHER_OBJECT_H_
#define _FTS_PRIVATE_PATCHER_OBJECT_H_

#define fts_object_is_error(o) (fts_object_get_class(o) == fts_error_object_class)

#define fts_object_is_abstraction(o) (fts_object_is_patcher((o)) && fts_patcher_is_abstraction((fts_patcher_t *) (o)))
#define fts_object_is_standard_patcher(o) (fts_object_is_patcher((o)) && fts_patcher_is_standard((fts_patcher_t *) (o)))
#define fts_object_is_template(o) (fts_object_is_patcher((o)) && fts_patcher_is_template((fts_patcher_t *) (o)))
#define fts_object_get_definition(o) (((o)->context != NULL)? (((fts_object_patcher_data_t *)(o)->context)->definition): NULL)

#define fts_object_has_patcher_data(o) ((o)->context != NULL)

#define fts_object_get_next_in_patcher(o) (((fts_object_patcher_data_t *)(o)->context)->next_in_patcher)
#define fts_object_set_next_in_patcher(o, x) (((fts_object_patcher_data_t *)(o)->context)->next_in_patcher = (x))

#define fts_object_get_inlet_connections(o, i) (((fts_object_patcher_data_t *)(o)->context)->in_conn[(i)])
#define fts_object_get_outlet_connections(o, i) (((fts_object_patcher_data_t *)(o)->context)->out_conn[(i)])

extern fts_object_t *fts_object_create_in_patcher(fts_class_t *cl, fts_patcher_t *patcher, int ac, const fts_atom_t *at);

extern fts_object_patcher_data_t *fts_object_get_patcher_data(fts_object_t *obj);
extern void fts_object_remove_patcher_data(fts_object_t *obj);

/* object properties */
extern void fts_object_put_prop(fts_object_t *obj, fts_symbol_t property, const fts_atom_t *value);
extern void fts_object_get_prop(fts_object_t *obj, fts_symbol_t property, fts_atom_t *value);
extern void fts_object_remove_prop(fts_object_t *obj, fts_symbol_t property);

/* support for redefinition */
extern fts_object_t *fts_object_recompute(fts_object_t *old);
extern fts_object_t *fts_object_redefine(fts_object_t *old, int ac, const fts_atom_t *at);

/* name of object in patcher */
extern void fts_patcher_object_set_name(fts_object_t *obj, fts_symbol_t sym, int global);
extern fts_symbol_t fts_patcher_object_get_name(fts_object_t *obj);
extern int fts_patcher_object_is_global(fts_object_t *obj);

extern void fts_patcher_object_add_binding(fts_object_t *obj, fts_definition_t *def);

/* persistence */
extern void fts_patcher_object_set_state_persistence(fts_object_t *obj, int persistence);
extern void fts_patcher_object_set_state_dirty(fts_object_t *obj);
extern void fts_patcher_object_set_dirty(fts_object_t *obj);
extern int fts_patcher_object_is_persistent(fts_object_t *obj);

#endif
