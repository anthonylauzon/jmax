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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "sys.h"
#include "lang.h"
#include "runtime/files/files.h"
#include "runtime/files/directories.h"

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

/* for the moment, we emulate the 0.26 sematic,
   but we really need something stronger
*/

/* Utility to find a given file in the
   a given column separated path list.
   If the path list is a null pointer,
   use the deafault path list .

   Return 1 if the file was found, 0 otherwise.
   If  the pathname pointer is not null, we copy the file pathname if
   found.
   */

static int file_exists(const char *path)
{
  struct stat statbuf;

  return (stat(path, &statbuf) == 0) && (statbuf.st_mode & S_IFREG);
}

int fts_file_get_read_path(const char *path, char *full_path)
{
  if (path[0] == '/')
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
	  
	  if ((end = strchr(begin, ':')) || (end = strchr(begin, ','))) /* path followed by separator */
	    next = end + 1; /* skip seperator */
	  else
	    {
	      end = begin + strlen(begin); /* last path in list */
	      next = 0; /* end of string */
	    }	  
	  
	  if (begin[0] == '/') /* absolute default path */
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
      if (path[0] == '/')
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

int fts_file_close(int fd)
{
  return close(fd);
}

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



