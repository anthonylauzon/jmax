/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

FTS_API fts_class_t *fts_error_object_class;

FTS_API void fts_runtime_error_proxy_set(fts_object_t *obj);
FTS_API void fts_runtime_error_proxy_remove(fts_object_t *obj);

FTS_API void fts_set_error(const char *format, ...);
FTS_API fts_symbol_t fts_get_error(void);

FTS_API void fts_object_error(fts_object_t *obj, const char *format, ...);
