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

#ifndef _FTS_PRIVATE_PLATFORM_H_
#define _FTS_PRIVATE_PLATFORM_H_

extern fts_symbol_t fts_get_default_root_directory( void);

extern fts_symbol_t fts_get_user_project( int create);
extern fts_symbol_t fts_get_system_project( void);

extern fts_symbol_t fts_get_user_configuration( int create);
extern fts_symbol_t fts_get_system_configuration(void);

extern fts_status_t fts_load_library( const char *filename, const char *symbol);

extern int fts_unlock_memory( void);

extern void fts_platform_init( void);

#endif
