#ifndef _PATABS_H_
#define _PATABS_H_

extern void fts_abstraction_init(void);

extern void fts_abstraction_declare(fts_symbol_t name, fts_symbol_t filename);
extern void fts_abstraction_declare_path(fts_symbol_t path);

extern fts_object_t *fts_abstraction_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at);
extern fts_object_t *fts_abstraction_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

#endif
