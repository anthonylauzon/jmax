#ifndef _NAMING_H_
#define _NAMING_H_


/* get patchers full name (returns pointer to local string!) */
extern char *fts_patcher_get_full_name_string(fts_patcher_t *p);

extern fts_object_t *fts_get_object_by_name(fts_symbol_t name);
extern void fts_register_named_object(fts_object_t *obj, fts_symbol_t name);
extern void fts_unregister_named_object(fts_object_t *obj, fts_symbol_t name);


#endif
