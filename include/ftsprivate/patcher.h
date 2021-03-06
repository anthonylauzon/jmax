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

#ifndef _FTS_PRIVATE_PATCHER_H_
#define _FTS_PRIVATE_PATCHER_H_

extern fts_class_t *patcher_class;

#define fts_object_is_patcher(o) (fts_object_get_class(o) == patcher_class)

#define fts_patcher_get_definitions(p) ((p)->definitions)
#define fts_patcher_set_definitions(p, d) ((p)->definitions = (d))

#define fts_patcher_set_standard(p)      ((p)->type = fts_p_standard)
#define fts_patcher_set_abstraction(p)   ((p)->type = fts_p_abstraction)

extern void fts_patcher_set_template(fts_patcher_t *patcher, fts_template_t *template_definition);

#define fts_patcher_get_template(p) ((p)->template_definition)

#define fts_patcher_get_file_name(p) ((p)->file_name)
#define fts_patcher_set_file_name(p, n) ((p)->file_name = (n))

#define fts_patcher_is_standard(p)       ((p)->type == fts_p_standard)
#define fts_patcher_is_abstraction(p)    ((p)->type == fts_p_abstraction)
#define fts_patcher_is_template(p)       ((p)->type == fts_p_template)

#define fts_patcher_set_scope(p) ((p)->scope = 1)
#define fts_patcher_is_scope(p) ((p)->scope != 0)

#define fts_patcher_get_args(p) ((p)->args)

#define fts_patcher_set_save_id(p, i) ((p)->save_id = (i))
#define fts_patcher_get_save_id(p, i) ((p)->save_id)

extern void fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj);
extern void fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj);
extern int  fts_patcher_get_objects_count( fts_patcher_t *this);

extern void fts_patcher_order_inoutlets_regarding_position(fts_patcher_t *this);
extern fts_patcher_t *fts_patcher_get_by_file_name( fts_symbol_t file_name);

#endif
