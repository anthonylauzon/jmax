#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

extern void fts_expressions_init(void);

typedef struct fts_epression_state fts_expression_state_t;
struct fts_epression_state;

extern fts_expression_state_t *fts_expression_eval(fts_object_t *object,
						   int expr_size, const fts_atom_t *expr,
						   int result_size, fts_atom_t *result);

extern int fts_expression_get_count(fts_expression_state_t *e);

#define  FTS_EXPRESSION_OK           0
#define  FTS_EXPRESSION_SYNTAX_ERROR 1
#define  FTS_EXPRESSION_UNDEFINED_VARIABLE 2
#define  FTS_EXPRESSION_OP_TYPE_ERROR     3

extern int  fts_expression_get_status(fts_expression_state_t *e);
extern void fts_expression_assign_properties(fts_expression_state_t *e, fts_object_t *obj);

#endif
