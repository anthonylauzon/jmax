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

#ifndef _FTS_FILE_H_
#define _FTS_FILE_H_

/**
 * Search for a file in a path.
 *
 * A path is a list of directories separated by ':'
 *
 * If file is found, the full path will be returned in <code>full_path</code>. It is under
 * the responsability of the caller to pass a buffer which will not overflow.
 * 
 * @fn void fts_file_search_in_path( const path *filename, const char *search_path, char *full_path)
 * @param filename     the name of the file to be searched
 * @param search_path  the list of directories to be searched in
 * @param full_path    the full path of result, if file is found
 *
 * @return 1 if file is found, 0 if not
 */
FTS_API int fts_file_search_in_path( const char *filename, const char *search_path, char *full_path);


FTS_API void fts_file_get_write_path(const char *path, char *full_path);

/* Utility to find a given file in the
   a given column separated path list.
   If the path list is a null pointer,
   use the deafault path list .

   Return 1 if the file was found, 0 otherwise.
   If  the pathname pointer is not null, we copy the file pathname if
   found.
   */

FTS_API int fts_file_get_read_path(const char *name, char *pathname);

FTS_API int fts_file_open(const char *name, const char *mode);
FTS_API int fts_file_close(int fd);

FTS_API int fts_file_is_text( fts_symbol_t file_name);

/*
 *  Directory paths
 */

FTS_API void fts_set_search_path(fts_symbol_t search_path);
FTS_API fts_symbol_t fts_get_search_path(void);

FTS_API void fts_set_project_dir(fts_symbol_t project_dir);
FTS_API fts_symbol_t fts_get_project_dir(void);

#endif
