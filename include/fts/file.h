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

FTS_API int fts_file_exists(const char *name);

FTS_API int fts_is_file(const char *name);

FTS_API int fts_is_directory(const char *name);

/**
 *
 * @return 0 if the filename remained unchanged, 1 if it was changed
 */
FTS_API int fts_file_correct_separators( char *filename);

FTS_API FILE* fts_file_open(const char *name, const char *mode);

FTS_API int fts_file_close(FILE* fd);

FTS_API int fts_file_is_text( fts_symbol_t file_name);

/*
 *  Directory paths
 */

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
 * Find a file on a set of paths.
 *
 * @fn int fts_find_file(fts_list_t* paths, const char *filename, char* buf, int len)
 * @param paths the list of search paths
 * @param filename the name of the file
 * @param buf the user allocated buffer which will contain the full path
 * @param len the length of the buffer 
 * @returns 0 if the file was not found, 1 if the directory was found 
 */
FTS_API int fts_find_file(fts_list_t* paths, const char *filename, char* buf, int len);

/** 
 * Find a directory on a set of paths.
 *
 * @fn int fts_find_directory(fts_list_t* paths, const char *dirname, char* buf, int len)
 * @param paths the list of search paths
 * @param dirname the name of the directory
 * @param buf the user allocated buffer which will contain the full path
 * @param len the length of the buffer 
 * @returns 0 if the directory was not found, 1 if the directory was found 
 */
FTS_API int fts_find_directory(fts_list_t* paths, const char *dirname, char* buf, int len);

FTS_API char fts_file_separator;
FTS_API char fts_path_separator;


/********************************************************************/
/********************************************************************/
/*********************** will disappear *****************************/
/********************************************************************/
/********************************************************************/


FTS_API void fts_set_search_path(fts_symbol_t search_path);
FTS_API fts_symbol_t fts_get_search_path(void);


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


FTS_API void fts_set_project_dir(fts_symbol_t project_dir);
FTS_API fts_symbol_t fts_get_project_dir(void);

#endif
