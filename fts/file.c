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

#include "ftsconfig.h"

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

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

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


int 
fts_file_exists( const char *filename)
{
  struct stat statbuf;
  return stat( filename, &statbuf) == 0;
}

int 
fts_is_file(const char *name)
{
  struct stat statbuf;
  return ( stat( name, &statbuf) == 0) && (statbuf.st_mode & S_IFREG);
}

int 
fts_is_directory(const char *name)
{
  struct stat statbuf;
  return ( stat( name, &statbuf) == 0) && (statbuf.st_mode & S_IFDIR);
}

void 
fts_dirname(const char *name, char* buf, int size)
{
  int len = strlen(name);
  
  snprintf(buf, size, "%s", name);
  while (--len >= 0) {
    if (buf[len] == fts_file_separator) {
      buf[len] = 0;
      break;
    }
  }
}

int 
fts_file_correct_separators( char *filename)
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

static FILE*
fts_do_file_open(const char *path, const char *mode)
{
  if (*mode == 'w')
    return fopen(path, "wb");
  else
    return fopen(path, "rb");
}

FILE* 
fts_file_open(const char *name,  const char *mode)
{
  char path[1024];

  if (*mode == 'w')
    {
      fts_file_get_write_path(name, path);

      return fts_do_file_open(path,  mode);
    }
  else
    {
      if (fts_file_get_read_path(name, path))
	return fts_do_file_open(path, mode); /* @@@ */
      else
	return NULL;
    }
}

int 
fts_file_close(FILE* fd)
{
  return fclose(fd);
}

int 
fts_file_is_text( fts_symbol_t file_name)
{
  char full_path[1024];
  int n, i;
  char buff[256];
  FILE* fd;

  if (!fts_file_get_read_path( fts_symbol_name( file_name), full_path))
     return 0;

  if ( (fd = fopen( full_path, "rb")) == NULL)
    return 0;

  if ( (n = fread( buff, 1, 256, fd)) < 256)
    {
      fclose( fd);
      return 0;
    }

  for ( i = 0; i < n; i++)
    {
      if ( !isgraph(buff[i]) && !isspace(buff[i]))
	return 0;
    }

  fclose( fd);

  return 1;
}

char*
fts_make_absolute_path(const char* parent, const char* file, char* buf, int len)
{
  char path[MAXPATHLEN];

  if (!fts_path_is_absolute(file) && (parent != NULL)) {
    snprintf(path, len, "%s%c%s", parent, fts_file_separator, file);
  } else {
    snprintf(path, len, "%s", file);
  }

  /* correct possible separators */
  fts_file_correct_separators(path);

  /* try to resolve symbolic links */
  if (realpath(path, buf) == NULL) {
    snprintf(buf, len, "%s", path);      
  } 

  return buf;
}

int 
fts_find_file(fts_list_t* paths, const char* filename, char* buf, int len)
{
  while (paths) {
    fts_make_absolute_path(fts_symbol_name(fts_get_symbol(fts_list_get(paths))), filename, buf, len);
    if (fts_file_exists(buf)) {
      return 1;
    }
    paths = fts_list_next(paths);
  }

  buf[0] = 0;
  return 0;
}

/**************************************************************/
/**************************************************************/
/**************** old stuff, should disappear *****************/
/**************************************************************/
/**************************************************************/

/* global search path */
static fts_symbol_t fts_search_path = 0;

static const char *
splitpath( const char *path, char *result, char sep)
{
  if ( *path == '\0')
    return 0;

  while ( *path != sep && *path != '\0')
    {
      *result++ = *path++;
    }

  *result = '\0';

  return ( *path != '\0') ? path+1 : path;
}

/*
   The default search path is set with an UCS command,
   but also from the user interface.
 */
void
fts_set_search_path(fts_symbol_t search_path)
{
  fts_search_path = search_path;
}

fts_symbol_t 
fts_get_search_path()
{
  return fts_search_path;
}

int 
fts_file_search_in_path( const char *filename, const char *search_path, char *full_path)
{
  if (fts_path_is_absolute(filename))
    {
      strcpy( full_path, filename);

      return fts_file_exists( full_path);
    }

  while ( (search_path = splitpath( search_path, full_path, fts_path_separator)) )
    {
      strcat( full_path, "/");
      strcat( full_path, filename);

      if (fts_file_exists( full_path))
	  return 1;
    }

  return 0;
}

int 
fts_file_get_read_path(const char *path, char *full_path)
{
  if (fts_path_is_absolute(path))
    {
      if (fts_file_exists(path))
	{
	  strcpy(full_path, path);	  
	  return 1;
	}
    }
  else
    {
      const char *begin, *end, *next;

      next = fts_symbol_name(fts_get_search_path());
      
      while (next)
	{
	  char buf[1024];
	  
	  begin = next;
	  
	  if ((end = strchr(begin, fts_path_separator)) || (end = strchr(begin, ','))) /* path followed by separator */
	    next = end + 1; /* skip seperator */
	  else
	    {
	      end = begin + strlen(begin); /* last path in list */
	      next = 0; /* end of string */
	    }	  
	  
	  if (fts_path_is_absolute(begin)) /* absolute default path */
	    buf[0] = '\0';
	  else if (fts_project_get_dir())
	    strcpy(buf, fts_symbol_name(fts_project_get_dir()));
	  else
	    begin = 0; /* invalid directory */

	  if (begin)
	    {
	      strncat(buf, begin, end - begin);
	      strcat(buf, "/");
	      strcat(buf, path);
	  
	      /* look for the file */
	      
	      if (fts_file_exists(buf))
		{
		  if (full_path)
		    strcpy(full_path, buf);
		  
		  return 1;
		}
	    }
	}
      
      /* look in project directory itself */
      
      if (fts_project_get_dir())
	{
	  char buf[1024];

	  strcpy(buf, fts_symbol_name(fts_project_get_dir()));
	  strcat(buf, "/");
	  strcat(buf, path);
	  
	  if (fts_file_exists(buf))
	    {
	      if (full_path)
		strcpy(full_path, buf);
	      
	      return 1;
	    }
	}
    }

  return 0;
}

void fts_file_get_write_path(const char *path, char *full_path)
{
  if (full_path)
    {
      if (fts_path_is_absolute(path))
	strcpy(full_path, path); /* path is absolute (just copied) */
      else
	{
	  if (fts_project_get_dir())
	    {
	      strcpy(full_path, fts_symbol_name(fts_project_get_dir()));
	      strcat(full_path, "/");
	    }
	  else
	    full_path[0] = '\0';
	  
	  strcat(full_path, path);
	}
    }
}
