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
#include <fts/project.h>
#include <ftsprivate/package.h>
#include <ftsconfig.h>

#if HAVE_DIRECT_H
#include <direct.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static fts_package_t* fts_project = NULL;
static fts_symbol_t sym_project = NULL;

fts_package_t* 
fts_project_open(const char* filename)
{
  if (fts_project != NULL) {
    fts_project_close();
  }
  
  if (sym_project == NULL) {
    sym_project = fts_new_symbol("project");
  }

  fts_log("[project]: Opening project %s\n", filename);
  
  fts_project = fts_package_load_from_file(sym_project, filename);

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
  } else {
    char buf[1024];
    
    return fts_new_symbol_copy(getcwd(buf, 1024));
  }
}

fts_symbol_t
fts_project_get_data_file(fts_symbol_t filename)
{
  fts_symbol_t path;
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  if (fts_project == NULL ) {
    return filename;
  }

  /* first try to find the file in the current project */
  path = fts_package_get_data_file(fts_project, filename);
  if (path != NULL) {
    return path;
  }

  /* if it's not in the project, try to find it in the required
     packages */
  fts_package_get_required_packages(fts_project, &pkg_iter);

  while (fts_iterator_has_more(&pkg_iter)) {
    
    fts_iterator_next(&pkg_iter, &pkg_name);
    pkg = fts_package_get(fts_get_symbol(&pkg_name));
    
    if (pkg == NULL) {
      continue;
    }

    path = fts_package_get_data_file(pkg, filename);
    
    if (path != NULL) {
      return path;
    }
  }

  return filename;
}
