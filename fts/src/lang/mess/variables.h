#ifndef _VARIABLES_H_
#define _VARIABLES_H_

/* FTS Variable handling */


/* Storing variables evaluted in expressions for future created objects dependencies */

typedef struct fts_var_ref
{
  fts_variable_t *var;
  struct fts_var_ref *next;
} fts_var_ref_t;


/* Variables */

extern fts_atom_t *fts_variable_get_value(fts_object_t *obj, fts_symbol_t name);
extern fts_status_t fts_patcher_variable_define(fts_patcher_t *patcher, fts_symbol_t name, fts_atom_t *value);
extern fts_status_t fts_variable_bind_to_object(fts_symbol_t name, fts_object_t *obj);
extern void fts_variable_unbind_to_object(fts_object_t *obj);
extern void fts_patcher_remove_variables(fts_patcher_t *patcher);
extern void fts_variable_remove_all(fts_object_t *owner);

#endif
