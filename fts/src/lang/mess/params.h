#ifndef _PARAMS_H_
#define _PARAMS_H_

typedef void (* fts_param_listener_fun_t) (void *listener, fts_symbol_t name,
					   const fts_atom_t *value);


extern void fts_param_set(fts_symbol_t name, const fts_atom_t *value);
extern const fts_atom_t *fts_param_get(fts_symbol_t name);
extern void fts_param_add_listener(fts_symbol_t name, void *listener,
				   fts_param_listener_fun_t listener_fun);
extern void fts_param_remove_listener(void *listener);

/* Convenience function for getting scalar parameter values */

extern float fts_param_get_float(fts_symbol_t name, float default_value);
extern int fts_param_get_int(fts_symbol_t name, int default_value);
extern fts_symbol_t fts_param_get_symbol(fts_symbol_t name, fts_symbol_t default_value);

#endif
