#ifndef _FTSDATAID_H_
#define _FTSDATAID_H_

extern void fts_data_id_put( int id, fts_data_t *d);
extern fts_data_t *fts_data_id_get( int id);
extern void fts_data_id_remove( int id, fts_data_t *d);

#endif

