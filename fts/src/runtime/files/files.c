#include "sys.h"
#include "lang.h"
#include "runtime/files/files.h"
#include "runtime/files/directories.h"

#include <string.h>
#include <fcntl.h>

#ifdef SGI
#include <sys/stat.h>
#endif

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

static int
file_exists(const char *path)
{
  int fd;

  fd = open(path, O_RDONLY);
  
  if (fd >= 0)
    {
      close(fd);
      return 1;
    }
  else
    return 0;
}

int
fts_file_find(const char *path, const char *path_list, char *full_path)
{
  if(path[0] == '/')
    {
      if(file_exists(path))
	{
	  strcpy(full_path, path);	  
	  return 1;
	}
    }
  else
    {
      const char *begin, *end, *next;

      if (!path_list)
	next = fts_symbol_name(fts_get_default_search_path());
      else
	next = path_list;
      
      while(next)
	{
	  char buf[1024];
	  
	  begin = next;
	  
	  if((end = strchr(begin, ':')) || (end = strchr(begin, ','))) /* path followed by separator */
	    next = end + 1; /* skip seperator */
	  else
	    {
	      end = begin + strlen(begin); /* last path in list */
	      next = 0; /* end of string */
	    }	  
	  
	  if(begin[0] == '/') /* absolute default path */
	    buf[0] = '\0';
	  else if(fts_get_project_dir())
	    strcpy(buf, fts_symbol_name(fts_get_project_dir()));
	  else
	    begin = 0; /* invalid directory */

	  if(begin)
	    {
	      strncat(buf, begin, end - begin);
	      strcat(buf, "/");
	      strcat(buf, path);
	  
	      /* look for the file */
	      
	      if(file_exists(buf))
		{
		  if(full_path)
		    strcpy(full_path, buf);
		  
		  return 1;
		}
	    }
	}
      
      /* look in project directory itself */
      
      if(fts_get_project_dir())
	{
	  char buf[1024];

	  strcpy(buf, fts_symbol_name(fts_get_project_dir()));
	  strcat(buf, "/");
	  strcat(buf, path);
	  
	  if(file_exists(buf))
	    {
	      if(full_path)
		strcpy(full_path, buf);
	      
	      return 1;
	    }
	}
    }

  return 0;
}

void
fts_file_get_write_path(const char *path, char *full_path)
{
  if(full_path)
    {
      if(path[0] == '/')
	strcpy(full_path, path); /* path is absolute (just copied) */
      else
	{
	  if(fts_get_project_dir())
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

static int
fts_file_open_with_path(fts_symbol_t path, fts_symbol_t dir, const char *name, const char *mode)
{
  char buf[1024];
  int fd;
  const char *p, *end;

  p = fts_symbol_name(path);

  while (1)
    {
      /* finding the end of the current path */
      
      if (strchr(p, ':'))
	end = strchr(p,':');
      else  if (strchr(p, ','))
	end = strchr(p,',');
      else
	end = p + strlen(p);

      /* composing the file name */

      if ((*p != '/') && dir)
	{
	  strcpy(buf, fts_symbol_name(dir));
	  strcat(buf, "/");
	  strncat(buf, p, end - p);
	  strcat(buf, "/");
	  strcat(buf, name);
	}
      else
	{
	  buf[0] = '\0';
	  strncat(buf, p, end - p); /* use strncat instead of strcpy because the
				       former guarantee null termination, and not the
				       latter !!!!!!!*/
	  strcat(buf, "/");
	  strcat(buf, name);
	}

      /* look for the file */

      fd = fts_do_file_open(buf, mode);

      if (fd >= 0)
	return fd;

      /* test for loop end, and failure */
      
      if (*end)
	p = end + 1;
      else
	return -1;	/* loop end, nothing found */
    }
}

int
fts_file_open(const char *name, fts_symbol_t dir, const char *mode)
{
  int fd = -1;

  if (name[0] == '/')
    {
      return fts_do_file_open(name, mode);
    }
  else
    {
      /* The algorithm is the following: 
	 we look for the file first in the directory
	 specified, if any; than, we use the path;
	 if we have the directory, relative component
	 of the path are interpreted relative to the
	 directory, if specified, otherwise are ignored.
       */

      if (dir)
	{
	  char buf[1024];

	  sprintf(buf, "%s/%s", fts_symbol_name(dir), name);

	  fd = fts_do_file_open(buf, mode);

	  if (fd >= 0)
	    return fd;
	}
      
      /* No directory found, and the name is not an absolute path
	 If we have a use the search_path, if set, or otherwise use
	 the default path
	 */

      if (fts_get_search_path())
	fd = fts_file_open_with_path(fts_get_search_path(), dir, name, mode);

      if (fd >= 0)
	return fd;

      if (fts_get_default_search_path())
	fd = fts_file_open_with_path(fts_get_default_search_path(), dir, name, mode);

      return fd;
    }
}

int
fts_file_close(int fd)
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

