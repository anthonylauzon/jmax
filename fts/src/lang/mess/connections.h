/*
 *                      Copyright (c) 1995 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.3 $ IRCAM $Date: 1998/10/12 17:13:15 $
 *
 *  Eric Viara for Ircam, January 1995
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

