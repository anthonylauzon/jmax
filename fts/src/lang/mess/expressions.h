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
#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

extern void fts_expressions_init(void);

struct fts_expression_state;
typedef struct fts_expression_state fts_expression_state_t;

struct fts_expression_assignement;
typedef struct fts_expression_assignement fts_expression_assignement_t;

extern fts_expression_state_t *fts_expression_eval(fts_patcher_t *scope,
						   int expr_size, const fts_atom_t *expr,
						   int result_size, fts_atom_t *result);

extern void fts_expression_state_free(fts_expression_state_t *e);

extern int fts_expression_get_count(fts_expression_state_t *e);

typedef int (* fts_expression_fun_t)(int ac, const fts_atom_t *at, fts_atom_t *result) ;

void fts_expression_declare_fun(fts_symbol_t name, fts_expression_fun_t f);

/* Error codes must be negative numbers */

#define  FTS_EXPRESSION_OK                      0
#define  FTS_EXPRESSION_SYNTAX_ERROR           -1
#define  FTS_EXPRESSION_UNDEFINED_VARIABLE     -2
#define  FTS_EXPRESSION_OP_TYPE_ERROR          -3
#define  FTS_EXPRESSION_ERROR_OBJECT_REFERENCE -4
#define  FTS_EXPRESSION_UNDEFINED_FUNCTION     -5
#define  FTS_EXPRESSION_ARRAY_ACCESS_ERROR     -6


extern int  fts_expression_get_status(fts_expression_state_t *e);
const char *fts_expression_get_msg(fts_expression_state_t *e);
extern const char *fts_expression_get_err_arg(fts_expression_state_t *e);
extern void fts_expression_add_variables_user(fts_expression_state_t *e, fts_object_t *obj);

extern int fts_expression_map_to_assignements(fts_expression_state_t *e, 
				       void (* f)(fts_symbol_t name, fts_atom_t *value, void *data),
				       void *data);

#endif


