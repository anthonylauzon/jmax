#ifndef _PATCHERDATA_H_
#define _PATCHERDATA_H_

extern fts_patcher_data_t *fts_patcher_data_new(fts_patcher_t *patcher);
extern void fts_patcher_data_free( fts_patcher_data_t *data);
extern void fts_patcher_data_add_object(fts_patcher_data_t *d, fts_object_t *obj);
extern void fts_patcher_data_add_connection(fts_patcher_data_t *d, fts_connection_t *obj);
extern void fts_patcher_data_remove_object(fts_patcher_data_t *d, fts_object_t *obj);
extern void fts_patcher_data_remove_connection(fts_patcher_data_t *d, fts_connection_t *obj);
extern void fts_patcher_data_redefine(fts_patcher_data_t *d);

#endif
