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

#ifndef _FTS_PLATFORM_H_
#define _FTS_PLATFORM_H_

#ifdef WIN32

/** Registry access functions. FTS uses the registry to set and get
    parameters until the work on the FTS configuration is
    completed. */
FTS_API int fts_get_regvalue_string(char* name, char* buf, int len);
FTS_API int fts_set_regvalue_string(char* name, const char* value);
FTS_API int fts_get_regvalue_int(char* name, int* value);
FTS_API int fts_set_regvalue_int(char* name, int value);

#endif

#endif
