/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

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

/* Used to switch to fts_object_property_changed */

extern void fts_client_send_property(fts_object_t *obj, fts_symbol_t name);

#endif
