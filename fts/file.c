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
 */

/*
 * This file contains utility functions for file access and file location 
 *
 */

#include <fts/fts.h>
#include <ftsprivate/package.h>
#include <ftsconfig.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_WINDOWS_H
#include <windows.h>
#endif
#if HAVE_DIRECT_H
#include <direct.h>
#endif

#ifdef WIN32

char fts_path_separator = ';';
char fts_file_separator = '\\';

#define fts_path_is_absolute(_p) \
 ((_p[0] == '/') || \
  ((_p[1] == ':') && (_p[2] == '/')) || \
  ((_p[1] == ':') && (_p[2] == '\\')))

#else

char fts_path_separator = ':';
char fts_file_separator = '/';
#define fts_path_is_absolute(_p) (_p[0] == '/')

#endif


int fts_file_exists( const char *filename)
{
  struct stat statbuf;

  return stat( filename, &statbuf) == 0;
}

int fts_is_file(const char *name)
{
  struct stat statbuf;

  return ( stat( name, &statbuf) == 0) && (statbuf.st_mode & S_IFREG);
}

int fts_is_directory(const char *name)
{
  struct stat statbuf;

  return ( stat( name, &statbuf) == 0) && (statbuf.st_mode & S_IFDIR);
}

char *fts_dirname( char *name)
{
  char *end;

  if ((end = strrchr( name, fts_file_separator)) != NULL)
    *end = '\0';

  return name;
}

static int fts_file_correct_separators( char *filename)
{
#ifdef WIN32
  int i, len, r = 0;

  len = strlen(filename);
  for (i = 0; i < len; i++) {
    if (filename[i] == '/') {
      filename[i] = '\\';
      r = 1;
    }
  }
  return r;
#else
  return 0;
#endif
}

char *fts_make_absolute_path(const char* parent, const char* file, char* buf, int len)
{
  char path[MAXPATHLEN];

  if (!fts_path_is_absolute(file))
    {
      if (parent != NULL)
	snprintf(path, len, "%s%c%s", parent, fts_file_separator, file);
      else 
	{
	  char cwd[MAXPATHLEN];

	  getcwd( cwd, MAXPATHLEN);
	  snprintf( path, len, "%s%c%s", cwd, fts_file_separator, file);      
	}
    } 
  else
    snprintf( path, len, "%s", file);


  /* correct possible separators */
  fts_file_correct_separators(path);

  /* try to resolve symbolic links */
  if (realpath(path, buf) == NULL)
    snprintf(buf, len, "%s", path);      

  return buf;
}

/*  fts_find_file_aux
 *
 * Takes 3 arguments: root, path, filename. root and path can be
 * null. filename cannot be null. it tests if following files exists:
 *
 *    root/path/filename      (root!=null, path!=null)
 *    root/filename           (root!=null, path==null)
 *    path/filename           (root==null, path!=null)
 *    filename                (root==null, path==null)
*/        
static int 
fts_find_file_aux(const char* root, const char* path, const char* filename, char* buf, int len)
{
  char newroot[MAXPATHLEN];

  if (root != NULL) {
    if (path != NULL) {
      fts_make_absolute_path(root, path, newroot, MAXPATHLEN);
      fts_make_absolute_path(newroot, filename, buf, len);
    } else {
      snprintf(newroot, MAXPATHLEN, "%s", root);
      fts_make_absolute_path(newroot, filename, buf, len);
    }
  } else {
    if (path != NULL) {
      snprintf(newroot, MAXPATHLEN, "%s", path);
      fts_make_absolute_path(newroot, filename, buf, len);
    } else {
      snprintf(buf, len, "%s", filename);      
    }
  }

  return fts_file_exists(buf);
}

int fts_file_find_in_path(const char* root, fts_list_t* paths, const char* filename, char* buf, int len)
{
  const char* path;

  if ((filename == NULL) || (strlen(filename) == 0)) {
    return 0;
  }

  while (paths) {
    path = fts_get_symbol(fts_list_get(paths));
    if (fts_find_file_aux(root, path, filename, buf, len)) {
      return 1;
    }
    paths = fts_list_next(paths);
  }

  if ((root != NULL) && fts_find_file_aux(root, NULL, filename, buf, len)) {
    return 1;
  }

  return 0;
}

char *fts_file_find( const char *filename, char *buf, int len)
{
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  pkg = fts_project_get();
  if (fts_package_get_data_file( pkg, filename, buf, len))
    return buf;
  
  /* ask the required packages of the current package */
  fts_package_get_required_packages( pkg, &pkg_iter);

  while ( fts_iterator_has_more( &pkg_iter)) 
    {
      fts_iterator_next( &pkg_iter, &pkg_name);
      pkg = fts_package_get(fts_get_symbol(&pkg_name));
      
      if (pkg == NULL)
	continue;
      
      if (fts_package_get_data_file( pkg, filename, buf, len))
	return buf;
    }

  return NULL;
}

