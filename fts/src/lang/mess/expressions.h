#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

extern void fts_expressions_init(void);

typedef struct fts_expression_state fts_expression_state_t;
struct fts_expression_state;

extern fts_expression_state_t *fts_expression_eval(fts_object_t *object,
						   int expr_size, const fts_atom_t *expr,
						   int result_size, fts_atom_t *result);

extern int fts_expression_get_count(fts_expression_state_t *e);

/* Error codes must be negative numbers */

#define  FTS_EXPRESSION_OK                      0
#define  FTS_EXPRESSION_SYNTAX_ERROR           -1
#define  FTS_EXPRESSION_UNDEFINED_VARIABLE     -2
#define  FTS_EXPRESSION_OP_TYPE_ERROR          -3
#define  FTS_EXPRESSION_ERROR_OBJECT_REFERENCE -4

extern int  fts_expression_get_status(fts_expression_state_t *e);
extern void fts_expression_add_variables_user(fts_expression_state_t *e, fts_object_t *obj);


extern int fts_expression_map_to_assignements(fts_expression_state_t *e,
					       void (* f)(fts_symbol_t name, fts_atom_t *value, void *data),
					       void *data);
#endif
