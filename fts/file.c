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

#include <fts/ftsnew.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_IO_H
#include <io.h>
#endif

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

/* global search path */
static fts_symbol_t fts_search_path = 0;

/* global project path */
static fts_symbol_t fts_project_dir = 0;


/*******************************************************
 *
 *  files module
 *
 */

extern void fts_soundfile_format_init(void);

static void
fts_files_init(void)
{
  fts_soundfile_format_init();
}

fts_module_t fts_files_module = {"FTS Files", "FTS File handling", fts_files_init, 0, 0};



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

static int 
file_exists( const char *filename)
{
  struct stat statbuf;

  return ( stat( filename, &statbuf) == 0) && (statbuf.st_mode & S_IFREG);
}

int 
fts_file_search_in_path( const char *filename, const char *search_path, char *full_path)
{
  if (fts_path_is_absolute(filename))
    {
      strcpy( full_path, filename);

      return file_exists( full_path);
    }

  while ( (search_path = splitpath( search_path, full_path, fts_path_separator)) )
    {
      strcat( full_path, "/");
      strcat( full_path, filename);

      if (file_exists( full_path))
	  return 1;
    }

  return 0;
}

int 
fts_file_get_read_path(const char *path, char *full_path)
{
  if (fts_path_is_absolute(path))
    {
      if (file_exists(path))
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
	  else if (fts_get_project_dir())
	    strcpy(buf, fts_symbol_name(fts_get_project_dir()));
	  else
	    begin = 0; /* invalid directory */

	  if (begin)
	    {
	      strncat(buf, begin, end - begin);
	      strcat(buf, "/");
	      strcat(buf, path);
	  
	      /* look for the file */
	      
	      if (file_exists(buf))
		{
		  if (full_path)
		    strcpy(full_path, buf);
		  
		  return 1;
		}
	    }
	}
      
      /* look in project directory itself */
      
      if (fts_get_project_dir())
	{
	  char buf[1024];

	  strcpy(buf, fts_symbol_name(fts_get_project_dir()));
	  strcat(buf, "/");
	  strcat(buf, path);
	  
	  if (file_exists(buf))
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
	  if (fts_get_project_dir())
	    {
	      strcpy(full_path, fts_symbol_name(fts_get_project_dir()));
	      strcat(full_path, "/");
	    }
	  else
	    full_path[0] = '\0';
	  
	  strcat(full_path, path);
	}
    }
}


static int
fts_do_file_open(const char *path, const char *mode)
{
  if (*mode == 'w')
#ifdef SGI
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
#else
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
#endif
  else
    return open(path, O_RDONLY);
}

int fts_file_open(const char *name,  const char *mode)
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
	return -1;
    }
}

int 
fts_file_close(int fd)
{
  return close(fd);
}

int 
fts_file_is_text( fts_symbol_t file_name)
{
  char full_path[1024];
  int fd, n, i;
  char buff[256];

  if (!fts_file_get_read_path( fts_symbol_name( file_name), full_path))
     return 0;

  if ( (fd = open( full_path, O_RDONLY)) < 0)
    return 0;

  if ( (n = read( fd, buff, 256)) < 256)
    {
      close( fd);
      return 0;
    }

  for ( i = 0; i < n; i++)
    {
      if ( !isgraph(buff[i]) && !isspace(buff[i]))
	return 0;
    }

  close( fd);

  return 1;
}


/*******************************************************
 *
 *  functions for directory handling
 *
 */

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

void
fts_set_project_dir(fts_symbol_t project_dir)
{
  fts_project_dir = project_dir;
}

fts_symbol_t
fts_get_project_dir(void)
{
  if (fts_project_dir)
    return fts_project_dir;
  else
    {
      char buf[1024];

      return fts_new_symbol(getcwd(buf, 1024));
    }
}

