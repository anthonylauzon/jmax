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

static fts_package_t* fts_project = NULL;
static fts_symbol_t sym_project = NULL;

fts_package_t* 
fts_open_project(char* filename)
{
  if (fts_project != NULL) {
    fts_close_project();
  }

  if (sym_project == NULL) {
    sym_project = fts_new_symbol("_project_");
  }
  
  fts_project = new_fts_package(sym_project);
  if (fts_project == NULL) {
    return NULL;
  }
  
  if (filename != NULL) {
    /* FIXME: explicitely set the definition file and diretory */
  }

  /* make the project the current package context */
  fts_push_package(fts_project);

  return fts_project;
}

int 
fts_close_project(void)
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
  fts_pop_package();
  
  delete_fts_package(fts_project);
  fts_project = NULL; 

  return 0;
}

fts_package_t* 
fts_get_project(void)
{
  return fts_project;
}
