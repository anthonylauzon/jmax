#ifndef _NAMING_H_
#define _NAMING_H_


/* Only for compatibility with the ISPW Object set; don't use for new objects. */



extern fts_object_t *fts_get_object_by_name(fts_symbol_t name);
extern void fts_register_named_object(fts_object_t *obj, fts_symbol_t name);
extern void fts_unregister_named_object(fts_object_t *obj, fts_symbol_t name);
extern int fts_named_object_exists(fts_symbol_t name);
extern void fts_named_object_send(fts_symbol_t name, fts_symbol_t s, int argc, const fts_atom_t *argv);

#endif
