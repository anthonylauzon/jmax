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

#include <fts/fts.h>
#include <ftsconfig.h>

#include <string.h> 

#if HAVE_DIRECT_H
#include <direct.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ftsprivate/package.h>

static fts_package_t* fts_project = NULL;
static fts_symbol_t sym_project = NULL;
static char *loading_project_dir = NULL;

static void
project_cut_name(const char* filename, char *name)
{
  int n = strlen(filename);
  int i_begin, i_end;
  int i = n - 1;

  while(i > 0 && filename[i] != '.' && filename[i] != '/')
    i--;

  if(i == 0)
    {
      i_end = n;
      i_begin = 0;
    }
  else if(filename[i] == '/')
    {
      i_end = n;
      i_begin = i + 1;      
    }
  else
    {
      i_end = i;

      while(i > 0 && filename[i] != '/')
	i--;

      i_begin = i + 1;
    }

  snprintf(name, i_end - i_begin + 1, "%s", filename + i_begin);
}

fts_package_t * 
fts_project_open(const char* filename)
{
  fts_symbol_t sym;
  char name[256];
  char *fnm;

  if (fts_project != NULL) {
    fts_project_close();
  }
  
  if (sym_project == NULL) {
    sym_project = fts_new_symbol("project");
  }

  fts_log("[project]: Opening project %s\n", filename);
  
  fnm = strcpy( fts_malloc( strlen( filename) + 1), filename);
  loading_project_dir = fts_dirname( fnm);

  project_cut_name(filename, name);
  sym = fts_new_symbol(name);

  if(sym == fts_new_symbol("config"))
    post("open default project\n");
  else
    post("open project: %s\n", sym);

  fts_project = fts_package_load_from_file(sym, filename);

  loading_project_dir = NULL;

  /* make the project the current package context */
  fts_package_push(fts_project);

  return fts_project;
}

int 
fts_project_close(void)
{
  if (fts_project == NULL) {
    post("Error: can't close non-existing project");
    return -1;
  }
  
  /* the current package must be the project. it's an error
     otherwise. */
  if (fts_get_current_package() != fts_project) {
    post("Error: can't close project when the project is not the current context");
    return -1;
  }

  /* pop the project of the package context stack */
  fts_package_pop(fts_project);
  
  fts_package_delete(fts_project);
  fts_project = NULL; 

  return 0;
}

void
fts_project_set(fts_package_t* p)
{
  if (fts_project != NULL) {
    fts_project_close();
  }

  fts_project = p;

  /* make the project the current package context */
  fts_package_push(fts_project);  
}

fts_package_t* 
fts_project_get(void)
{
  return fts_project;
}

fts_symbol_t
fts_project_get_dir(void)
{
  if ((fts_project != NULL ) && (fts_package_get_dir(fts_project) != NULL)) {
    return fts_package_get_dir(fts_project);
  } else 
    if( loading_project_dir != NULL)
	return loading_project_dir;
    else
      {
	char buf[1024];
	
	return fts_new_symbol(getcwd(buf, 1024));
      }
}
