#ifndef _EVENTS_H_
#define _EVENTS_H_

/* Event/Error handling API */

extern void fts_event_add_handler(fts_object_t *handler);
extern void fts_event_remove_handler(fts_object_t *handler);
extern void fts_event_push_handler(fts_object_t *handler);
extern void fts_event_pop_handler(fts_object_t *handler);

extern void fts_event_add_default_handler(fts_object_t *handler);

extern void fts_event(fts_symbol_t s, int ac, const fts_atom_t *at);
extern void fts_error(fts_symbol_t s, int ac, const fts_atom_t *at);

/* private  */

extern void fts_error_init(void);

#endif
