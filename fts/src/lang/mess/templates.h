#ifndef _TEMPLATES_H_
#define _TEMPLATES_H_

extern void fts_template_init();

extern void fts_template_declare(fts_symbol_t name, fts_symbol_t filename);
extern void fts_template_declare_path(fts_symbol_t path);

extern fts_object_t *fts_template_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at,
					       fts_expression_state_t *e);
extern fts_object_t *fts_template_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at,
					       fts_expression_state_t *e);

#endif
