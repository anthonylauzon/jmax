#ifndef _NAMING_H_
#define _NAMING_H_


extern fts_object_t *fts_get_object_by_name(fts_symbol_t name);
extern void fts_register_named_object(fts_object_t *obj, fts_symbol_t name);
extern void fts_unregister_named_object(fts_object_t *obj, fts_symbol_t name);


#endif
