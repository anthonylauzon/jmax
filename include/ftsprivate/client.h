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
 */

#ifndef _FTS_PRIVATE_CLIENT_H_
#define _FTS_PRIVATE_CLIENT_H_

typedef struct _client_t client_t;

#define FTS_CLIENT_DEFAULT_PORT 2023

#define MAX_CLIENTS (1<<(32-OBJECT_ID_BITS))

/**
 * Load a patcher from file
 *
 * @fn fts_patcher_t *fts_client_load_patcher( fts_symbol_t file_name, fts_object_t *parent, int client_id)
 * @param file_name the file name 
 * @param parent the parent object 
 * @param client_id the client id 
 *
 * @ingroup client
 */
extern fts_patcher_t *fts_client_load_patcher( fts_symbol_t file_name, int client_id);
extern client_t *object_get_client( fts_object_t *obj);

#endif






