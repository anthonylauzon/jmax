/* 
   This file define the event generation scheduling mechanism for objects.
*/

/* Should this be a Tile ??? */

#ifndef _UPDATES_H_
#define _UPDATES_H_

/* functions */

extern void fts_updates_set_updates_per_ticks(int upt);
extern void fts_updates_set_update_period(int upt);
extern void fts_client_updates_sync(void);
extern void fts_client_send_property(fts_object_t *obj, fts_symbol_t name);

#endif
