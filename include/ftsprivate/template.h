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

struct fts_template {
  fts_symbol_t name;
  fts_symbol_t filename;
  fts_symbol_t original_filename;
  fts_list_t *instances;
  fts_package_t *package;
};

#define fts_template_get_filename(t) ((t)->filename)
#define fts_template_set_filename(t, n) ((t)->filename = (n))

#define fts_template_get_original_filename(t) ((t)->original_filename)

#define fts_template_get_package(t) ((t)->package)
#define fts_template_set_package(t, p) ((t)->package = (p))

extern fts_template_t* fts_template_new(fts_symbol_t name, fts_symbol_t filename, fts_symbol_t original_filename);
extern void fts_template_redefine(fts_template_t *template, fts_symbol_t filename);

extern void fts_template_add_instance(fts_template_t *template, fts_object_t *object);
extern void fts_template_remove_instance(fts_template_t *template, fts_object_t *object);

extern void fts_template_declare(fts_symbol_t name, fts_symbol_t filename);
extern void fts_template_declare_path(fts_symbol_t path);

extern fts_object_t *fts_template_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
extern fts_object_t *fts_template_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

extern void fts_template_file_modified(fts_symbol_t filename);

#endif
