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
#ifndef _FTSDATAID_H_
#define _FTSDATAID_H_

extern void fts_data_id_put( int id, fts_data_t *d);
extern fts_data_t *fts_data_id_get( int id);
extern void fts_data_id_remove( int id, fts_data_t *d);

#endif

