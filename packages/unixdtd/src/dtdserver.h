/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 */

/*
 * This file's authors: Francois Dechelle.
 */

#ifndef _FTS_DTDSERVER_H_
#define _FTS_DTDSERVER_H_

#include "dtdfifo.h"

typedef struct _dtdserver_t dtdserver_t;

extern dtdserver_t *dtdserver_get_default_instance( void);

extern void dtdserver_add_object( dtdserver_t *server, void *object);
extern void dtdserver_remove_object( dtdserver_t *server, void *object);

extern dtdfifo_t *dtdserver_read( dtdserver_t *server, const char *filename, int n_channels);
extern dtdfifo_t *dtdserver_write( dtdserver_t *server, const char *filename, int n_channels);

extern void dtdserver_close( dtdserver_t *server, dtdfifo_t *fifo);

extern void dtdserver_stop( void);

#endif
