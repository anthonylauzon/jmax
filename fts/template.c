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

static fts_heap_t *template_heap;

static void fts_template_recompute_instances(fts_template_t *template);

fts_template_t *fts_template_new(fts_symbol_t name, fts_symbol_t filename, fts_symbol_t original_filename)
{
  fts_template_t *template;

  template = (fts_template_t *) fts_heap_alloc(template_heap);

  template->name = name;
  template->filename = filename;
  template->original_filename = original_filename;
  template->instances = 0;
  template->package = 0;

  return template;
}

void 
fts_template_redefine(fts_template_t *template, fts_symbol_t filename)
{
  template->filename = filename;
  fts_template_recompute_instances(template);
}

fts_object_t *
fts_template_make_instance(fts_template_t *template, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_patcher_t *instance;

  fts_package_push(template->package);

  instance = (fts_patcher_t *)fts_binary_file_load( template->filename, (fts_object_t *)patcher, ac, at);

  fts_package_pop(template->package);
    
  if (instance)
    {
      fts_template_add_instance( template, (fts_object_t *)instance);
      fts_patcher_set_template( instance, template);

      if(ac > 0)
	{
	  fts_atom_t va;
    
	  /* define the "args" name */
	  instance->args = (fts_tuple_t *)fts_object_create(fts_tuple_class, ac, at);
	  fts_object_refer(instance->args);

	  fts_set_object( &va, (fts_object_t *)instance->args);
	  fts_name_set_value( instance, fts_s_args, &va);
	}
    }

  return (fts_object_t *)instance;
}


static void 
fts_template_recompute_instances(fts_template_t *template)
{
  fts_list_t* list;

  list = template->instances;
  template->instances = NULL;

  while (list) {
    fts_object_t *object = fts_get_object( fts_list_get(list));

    fts_object_recompute(object);
    
    list = fts_list_next(list);
  }
}


void 
fts_template_add_instance(fts_template_t *template, fts_object_t *object)
{
  fts_atom_t a[1];

  fts_set_object(a, object);
  template->instances = fts_list_prepend(template->instances, a);
}

void 
fts_template_remove_instance(fts_template_t *template, fts_object_t *object)
{
  fts_atom_t a[1];

  fts_set_object(a, object);
  template->instances = fts_list_remove(template->instances, a);
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
  filename = fts_new_symbol(buf);

  fts_get_packages( &pkg_iter);
  
  while ( fts_iterator_has_more( &pkg_iter)) {

    fts_iterator_next( &pkg_iter, &pkg_atom);
    pkg = fts_get_pointer(&pkg_atom);

    template = fts_package_get_template_from_file(pkg, filename);

    if (template)
      fts_template_recompute_instances(template);
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

