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

#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_

/* init function */

extern void fts_connections_init(void);

/* Connections */

extern fts_connection_t *fts_connection_new(int id, fts_object_t *, int woutlet, fts_object_t *, int winlet);
extern void fts_connection_delete(fts_connection_t *conn);

extern void fts_object_move_connections(fts_object_t *old, fts_object_t *new, int doclient);
extern void fts_object_trim_inlets_connections(fts_object_t *obj, int inlets);
extern void fts_object_trim_outlets_connections(fts_object_t *obj, int outlets);

#define fts_connection_get_id(c)     ((c)->id)

/* Debug print */

extern void fprintf_connection(FILE *f, fts_connection_t *conn);


#endif

