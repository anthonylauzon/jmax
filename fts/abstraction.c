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


#include <string.h>
#include <sys/stat.h>

#include <fts/fts.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/patparser.h>
#include <ftsprivate/package.h>

struct fts_abstraction
{
  fts_symbol_t name;
  fts_symbol_t filename;
  fts_symbol_t original_filename;
};

static fts_object_t *
fts_make_abstraction_instance(fts_abstraction_t *abstraction, fts_patcher_t *patcher, int ac, const fts_atom_t *at);


static fts_heap_t *abstraction_heap;



fts_abstraction_t* 
fts_abstraction_new(fts_symbol_t name, fts_symbol_t filename, fts_symbol_t original_filename)
{
  fts_abstraction_t *abstraction;

  abstraction = (fts_abstraction_t *) fts_heap_alloc(abstraction_heap);
  if (abstraction == NULL) {
    return NULL;
  }

  abstraction->name = name;
  abstraction->filename = filename;
  abstraction->original_filename = original_filename;
  
  return abstraction;
}

fts_symbol_t 
fts_abstraction_get_filename(fts_abstraction_t *abstraction)
{
  return abstraction->filename;
}

void
fts_abstraction_set_filename(fts_abstraction_t *abstraction, fts_symbol_t filename)
{
  abstraction->filename = filename;
}

fts_symbol_t 
fts_abstraction_get_original_filename(fts_abstraction_t *abstraction)
{
  return abstraction->original_filename;
}


fts_object_t* 
fts_abstraction_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_abstraction_t *abstraction;
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  pkg = fts_get_current_package();
  abstraction = fts_package_get_declared_abstraction(pkg, fts_get_symbol(&at[0]));

  if (abstraction)
    return fts_make_abstraction_instance(abstraction, patcher, ac, at);
  
  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &pkg_iter);

  while (fts_iterator_has_more( &pkg_iter)) 
    {
      fts_iterator_next( &pkg_iter, &pkg_name);
      pkg = fts_package_get(fts_get_symbol(&pkg_name));
      
      if (pkg == NULL)
	continue;
      
      abstraction = fts_package_get_declared_abstraction(pkg, fts_get_symbol(&at[0]));

      if (abstraction)
	return fts_make_abstraction_instance(abstraction, patcher, ac, at);
    }
  
  return 0;
}


fts_object_t*
fts_abstraction_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_abstraction_t *abstraction;
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  pkg = fts_get_current_package();
  abstraction = fts_package_get_abstraction_in_path(pkg, fts_get_symbol(&at[0]));

  if (abstraction)
    return fts_make_abstraction_instance(abstraction, patcher, ac, at);
  
  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &pkg_iter);

  while ( fts_iterator_has_more( &pkg_iter)) 
    {
      fts_iterator_next( &pkg_iter, &pkg_name);
      pkg = fts_package_get(fts_get_symbol(&pkg_name));
      
      if (pkg == NULL)
	continue;
      
      abstraction = fts_package_get_abstraction_in_path(pkg, fts_get_symbol(&at[0]));

      if (abstraction)
	return fts_make_abstraction_instance(abstraction, patcher, ac, at);
    }
  
  return 0;
}


/*
 * The real abstraction loader: load the abstraction, looking in the
 * declaration and path table
 *
 */

static fts_object_t *
fts_make_abstraction_instance(fts_abstraction_t *abstraction, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_patlex_t *in; 
  fts_atom_t description[1];

  fts_set_symbol(&description[0], fts_s_patcher);
  fts_object_new_to_patcher((fts_patcher_t *)patcher, 1, description, &obj);

  /* flag the patcher as abstraction */
  fts_patcher_set_abstraction((fts_patcher_t *)obj);

  /* get the lexer */

  in = fts_patlex_open(fts_symbol_name(abstraction->filename), ac - 1, at + 1);
  if (in == NULL) {
    fts_object_set_error(obj, "failed to open the abstraction file");
    return obj;
  }

  fts_patparse_parse_patlex(obj, in);

  fts_patcher_reassign_inlets_outlets((fts_patcher_t *) obj);

  /* Add the template like variables in order to support direct
     .abs to template translation */

  {
    fts_atom_t rat[256];
    int i;

    fts_set_symbol(&rat[0], fts_s_patcher);

    for (i = 0; (i < ac) && (i < 256) ; i++)
      rat[i + 1] = at[i];

    fts_patcher_redefine((fts_patcher_t *)obj, ac + 1, rat);
  }

  fts_patlex_close(in);

  return obj;
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_abstraction_init()
{
  abstraction_heap = fts_heap_new(sizeof(fts_abstraction_t));
}
