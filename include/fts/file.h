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


FTS_API int fts_file_exists(const char *name);

FTS_API int fts_is_file(const char *name);

FTS_API int fts_is_directory(const char *name);

FTS_API char *fts_dirname( char *name);

/** Creates the absolute file path. The result is copied in the buf
 *  argument. 
 *
 * @fn char* fts_make_absolute_path(const char* parent, const char* file, char* buf, int len)
 * @param parent the parent directory of the file 
 * @param file the name of the file
 * @param buf the user allocated buffer which will contain the full path
 * @param len the length of the buffer 
 * @returns the pointer to buf. 
 */
FTS_API char* fts_make_absolute_path(const char* parent, const char* file, char* buf, int len);

/** 
 * Find a file name in paths using first the project paths and then its required packages.
 *
 * @fn char *fts_file_find( const char *filenname, char *buf, int len)
 * @param filename the name of the file
 * @param buf the user allocated buffer which will contain the full path
 * @param len the length of the buffer 
 * @returns 0 if the file was not found, 1 if the directory was found 
 */
FTS_API char *fts_file_find( const char *filename, char *buf, int len);


FTS_API char fts_file_separator;
FTS_API char fts_path_separator;

