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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_EXPRESSIONS_H_
#define _FTS_EXPRESSIONS_H_

FTS_API void fts_expressions_init(void);

struct fts_expression_state;
typedef struct fts_expression_state fts_expression_state_t;

struct fts_expression_assignement;
typedef struct fts_expression_assignement fts_expression_assignement_t;

FTS_API fts_expression_state_t *
fts_expression_eval(fts_patcher_t *scope, int expr_size, const fts_atom_t *expr, int result_size, fts_atom_t *result);

FTS_API void fts_expression_state_free(fts_expression_state_t *e);

FTS_API int fts_expression_get_result_count(fts_expression_state_t *e);

typedef int (* fts_expression_fun_t)(int ac, const fts_atom_t *at, fts_atom_t *result) ;
void fts_expression_declare_fun(fts_symbol_t name, fts_expression_fun_t f);

/* Error codes must be negative numbers */
#define FTS_EXPRESSION_OK 0
#define FTS_EXPRESSION_SYNTAX_ERROR -1
#define FTS_EXPRESSION_UNDEFINED_VARIABLE -2
#define FTS_EXPRESSION_OP_TYPE_ERROR -3
#define FTS_EXPRESSION_ERROR_OBJECT_REFERENCE -4
#define FTS_EXPRESSION_UNDEFINED_FUNCTION -5
#define FTS_EXPRESSION_ARRAY_ACCESS_ERROR -6

FTS_API int fts_expression_get_status(fts_expression_state_t *e);
FTS_API const char *fts_expression_get_msg(fts_expression_state_t *e);
FTS_API const char *fts_expression_get_err_arg(fts_expression_state_t *e);
FTS_API void fts_expression_add_variables_user(fts_expression_state_t *e, fts_object_t *obj);

FTS_API int fts_expression_map_to_assignements(fts_expression_state_t *e, 
					      void (* f)(fts_symbol_t name, fts_atom_t *value, void *data), void *data);

#endif





