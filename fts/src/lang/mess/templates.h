/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _TEMPLATES_H_
#define _TEMPLATES_H_

extern void fts_template_init(void);

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
