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

/* Changes handling */

extern void fts_object_property_changed(fts_object_t *obj, fts_symbol_t property);
extern void fts_object_ui_property_changed(fts_object_t *obj, fts_symbol_t property);
extern void fts_object_reset_changed(fts_object_t *obj);

#endif
