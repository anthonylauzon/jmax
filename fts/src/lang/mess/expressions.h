#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

extern void fts_expressions_init(void);
extern int fts_expression_eval(fts_object_t *object,
			       int expr_size, const fts_atom_t *expr,
			       int result_size, fts_atom_t *result);

#endif
