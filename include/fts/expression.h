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

typedef struct _fts_expression_t fts_expression_t;

FTS_API fts_status_t fts_expression_new( int ac, const fts_atom_t *at, fts_expression_t **pexp);
FTS_API void fts_expression_delete( fts_expression_t *exp);

FTS_API void fts_expression_clear( fts_expression_t *exp);
FTS_API fts_status_t fts_expression_set( fts_expression_t *exp, int ac, const fts_atom_t *at);

FTS_API int fts_expression_get_env_count( fts_expression_t *exp);

FTS_API void fts_expression_add_variables_user( fts_expression_t *exp, fts_patcher_t *scope, fts_object_t *obj);

typedef fts_status_t (*fts_expression_callback_t)( int ac, const fts_atom_t *at, void *data);

FTS_API fts_status_t fts_expression_reduce( fts_expression_t *exp, fts_patcher_t *scope, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data);
FTS_API fts_status_t fts_expression_evaluate( fts_expression_t *exp, fts_hashtable_t *locals, fts_hashtable_t *globals, int env_ac, const fts_atom_t *env_at, fts_expression_callback_t callback, void *data);

FTS_API int fts_expression_is_valid( fts_expression_t *exp);
