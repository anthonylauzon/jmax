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

#ifndef _FTS_PRIVATE_TEMPLATE_H_
#define _FTS_PRIVATE_TEMPLATE_H_

extern void fts_template_init(void);

extern void fts_template_register_binary(fts_symbol_t name, unsigned char *program, fts_symbol_t symbol_table[]);

extern void fts_template_add_instance(fts_template_t *template, fts_object_t *object);
extern void fts_template_remove_instance(fts_template_t *template, fts_object_t *object);

extern void fts_template_declare(fts_symbol_t name, fts_symbol_t filename);
extern void fts_template_declare_path(fts_symbol_t path);

extern fts_object_t *fts_template_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at,
					       fts_expression_state_t *e);
extern fts_object_t *fts_template_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at,
					       fts_expression_state_t *e);

extern void fts_template_file_modified(fts_symbol_t filename);

#endif
