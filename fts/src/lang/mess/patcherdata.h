/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _PATCHERDATA_H_
#define _PATCHERDATA_H_

extern fts_patcher_data_t *fts_patcher_data_new(fts_patcher_t *patcher);
extern void fts_patcher_data_free( fts_patcher_data_t *data);
extern void fts_patcher_data_add_object(fts_patcher_data_t *d, fts_object_t *obj);
extern void fts_patcher_data_add_connection(fts_patcher_data_t *d, fts_connection_t *obj);
extern void fts_patcher_data_remove_object(fts_patcher_data_t *d, fts_object_t *obj);
extern void fts_patcher_data_remove_connection(fts_patcher_data_t *d, fts_connection_t *obj);
extern void fts_patcher_data_redefine(fts_patcher_data_t *d);
extern void fts_patcher_data_blip(fts_data_t *d, const char *msg);

#endif
