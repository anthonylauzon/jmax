#ifndef _COMPATIBILITY_H_
#define _COMPATIBILITY_H_

fts_object_t *fts_get_receive_by_name(fts_symbol_t name);
void          fts_set_receive_by_name(fts_symbol_t name, fts_object_t *obj);
void          fts_remove_receive_by_name(fts_symbol_t name);
void fts_mess_compatibility_init(void);

#endif
