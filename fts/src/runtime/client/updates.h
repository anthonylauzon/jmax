/* 
   This file define the event generation scheduling mechanism for objects.
*/

/* Should this be a Tile ??? */

#ifndef _UPDATES_H_
#define _UPDATES_H_

/* functions */

extern void fts_updates_set_updates_per_ticks(int upt);
extern void fts_updates_set_update_period(int upt);
extern void fts_updates_sync();	/* private */

#endif
