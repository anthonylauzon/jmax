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


/*
 * Handling of looking up for new templates.
 * 
 * We handle a table of direct declarations (name --> file)
 * and a table of search directory.
 *
 * The table of direct declarations actually contains a template
 * declaration object, including the file name, and possibly 
 * a cache of the declaration text.
 *
 * There is only one group of search path (at least, for the moment)
 * and when an template is found, a new declaration is dynamically
 * created, this to avoid searching again; also, in the short future,
 * the template text can be cached under some condition (size of the
 * text, global cache size limits, ...)
 *
 * Later, the fts_template will also include a table of pointers
 * to all the instances, to allow dynamic redefinition
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ftsprivate/errobj.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/package.h>
#include <ftsprivate/template.h>

/*  #define TEMPLATE_DEBUG  */

/*
 * globals variable
 */
static fts_heap_t *template_heap;

static void fts_template_recompute_instances(fts_template_t *template);
static fts_template_t *fts_template_for_file(fts_symbol_t filename);

static fts_object_t* 
fts_make_template_instance(fts_template_t *template, fts_patcher_t *patcher, int ac, const fts_atom_t *at);

/***********************************************************************
 *
 *  Template object functions
 *
 */
fts_template_t* 
fts_template_new(fts_symbol_t name, fts_symbol_t filename, fts_symbol_t original_filename)
{
  fts_template_t *template;

  /* Make the template */
  template = (fts_template_t *) fts_heap_alloc(template_heap);

  template->name = name;
  template->filename = filename;
  template->original_filename = original_filename;
  template->instances = 0;
  template->package = 0;

#ifdef TEMPLATE_DEBUG 
  {
    const char* file = (filename)? filename : (original_filename)? original_filename : "unknown"; 
    fts_log( "[template]: New template %s, file %s\n", name, file); /* @@@ */
  }
#endif

  return template;
}

void 
fts_template_redefine(fts_template_t *template, fts_symbol_t filename)
{
  template->filename = filename;
  fts_template_recompute_instances(template);
}

/*
 * The real template loader: load the template, looking in the
 * declaration and path table
 *
 */
static fts_object_t*
fts_make_template_instance(fts_template_t *template, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;

  fts_package_push(template->package);

  obj = fts_binary_file_load( template->filename, (fts_object_t *) patcher, ac, at);

  fts_package_pop(template->package);
    
  /* flag the patcher as template, and set the template */
  if (obj) 
    fts_patcher_set_template((fts_patcher_t *)obj, template);
  
  return obj;
}


static void 
fts_template_recompute_instances(fts_template_t *template)
{
  fts_list_t* list;

#ifdef TEMPLATE_DEBUG 
  fts_log("Recomputing instances of template %s\n", template->name); /* @@@ */
#endif

  list = template->instances;
  template->instances = NULL;

  while (list) {
    fts_object_t *object = fts_get_object( fts_list_get(list));

    fts_object_recompute(object);
    
    list = fts_list_next(list);
  }

#ifdef TEMPLATE_DEBUG 
  fts_log("Done.\n");
#endif
}


void 
fts_template_add_instance(fts_template_t *template, fts_object_t *object)
{
  fts_atom_t a[1];

  fts_set_object(a, object);
  template->instances = fts_list_prepend(template->instances, a);
       
#ifdef TEMPLATE_DEBUG 
  fts_log("Adding instance to template %s : \n", template->filename); /* @@@ */
#endif
}

void 
fts_template_remove_instance(fts_template_t *template, fts_object_t *object)
{
  fts_atom_t a[1];

  fts_set_object(a, object);
  template->instances = fts_list_remove(template->instances, a);

#ifdef TEMPLATE_DEBUG 
  fts_log("Removing instance from template %s : \n", template->filename); 
#endif
}

/***********************************************************************
 *
 *  Global template functions
 *
 */

fts_object_t*
fts_template_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_template_t *template;
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  pkg = fts_get_current_package();
  template = fts_package_get_declared_template(pkg, fts_get_symbol(&at[0]));

  if (template)
    return fts_make_template_instance(template, patcher, ac, at);
  
  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &pkg_iter);

  while ( fts_iterator_has_more( &pkg_iter)) 
    {
      fts_iterator_next( &pkg_iter, &pkg_name);
      pkg = fts_package_get(fts_get_symbol(&pkg_name));
      
      if (pkg == NULL)
	continue;
      
      template = fts_package_get_declared_template(pkg, fts_get_symbol(&at[0]));

      if (template)
	return fts_make_template_instance(template, patcher, ac, at);
    }
  
  return 0;
}

fts_object_t*
fts_template_new_search(fts_patcher_t *patcher,	int ac, const fts_atom_t *at)
{
  fts_template_t *template;
  fts_package_t *pkg;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  pkg = fts_get_current_package();
  template = fts_package_get_template_in_path(pkg, fts_get_symbol(&at[0]));

  if (template)
    return fts_make_template_instance(template, patcher, ac, at);
  
  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &pkg_iter);

  while ( fts_iterator_has_more( &pkg_iter)) 
    {
      fts_iterator_next( &pkg_iter, &pkg_name);
      pkg = fts_package_get(fts_get_symbol(&pkg_name));
      
      if (pkg == NULL)
	continue;
      
      template = fts_package_get_template_in_path(pkg, fts_get_symbol(&at[0]));

      if (template)
	return fts_make_template_instance(template, patcher, ac, at);
    }
  
  return 0;
}


/* redefine the template corresponding to a given file */
void 
fts_template_file_modified(fts_symbol_t filename)
{
  fts_package_t* pkg;
  fts_atom_t pkg_atom;
  fts_iterator_t pkg_iter;
  char buf[MAXPATHLEN];
  fts_template_t *template;

  /* resolve the links in the path, so that we have a unique name 
     for the file */
  realpath( filename, buf);
  filename = fts_new_symbol_copy(buf);

#ifdef TEMPLATE_DEBUG 
  fts_log("File %s modified.\n", filename);
#endif

  fts_get_packages( &pkg_iter);
  
  while ( fts_iterator_has_more( &pkg_iter)) {

    fts_iterator_next( &pkg_iter, &pkg_atom);
    pkg = fts_get_pointer(&pkg_atom);

    template = fts_package_get_template_from_file(pkg, filename);

    if (template) {
#ifdef TEMPLATE_DEBUG 
      fts_log("Then Redefining Instances.\n");
#endif
      fts_template_recompute_instances(template);
    }
  }
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_template_init()
{
  template_heap = fts_heap_new(sizeof(fts_template_t));
}

