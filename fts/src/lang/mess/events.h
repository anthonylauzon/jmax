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
#ifndef _EVENTS_H_
#define _EVENTS_H_

/* Event/Error handling API */

extern void fts_event_add_handler(fts_object_t *handler);
extern void fts_event_remove_handler(fts_object_t *handler);
extern void fts_event_push_handler(fts_object_t *handler);
extern void fts_event_pop_handler(fts_object_t *handler);
extern void fts_event_add_default_handler(fts_object_t *handler);

extern void fts_events_init(void);

#define fts_error  fts_event

extern void fts_event(fts_symbol_t s, int ac, const fts_atom_t *at);

#endif
