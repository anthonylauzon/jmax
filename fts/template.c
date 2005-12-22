/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <string.h>
#include <stdlib.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <sys/stat.h>
#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <ftsprivate/errobj.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/patcherobject.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/package.h>
#include <ftsprivate/template.h>

static fts_heap_t *template_heap;

static fts_hashtable_t template_file_to_load;
static fts_hashtable_t template_file_loaded;

static void fts_template_recompute_instances(fts_template_t *template);

fts_template_t *fts_template_new(fts_symbol_t name, fts_symbol_t filename, fts_symbol_t original_filename)
{
  fts_template_t *template;
  fts_atom_t key;
  fts_atom_t value;

  template = (fts_template_t *) fts_heap_alloc(template_heap);

  template->name = name;
  template->filename = filename;
  template->original_filename = original_filename;
  template->instances = 0;
  template->package = 0;

  /* add the new template in hashtable */
  fts_set_symbol(&key, filename);
  fts_set_pointer(&value, template);
  fts_hashtable_put(&template_file_loaded, &key, &value);

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
  fts_patcher_t *instance = 0;
  fts_atom_t key;
  fts_atom_t value;

  fts_package_push(template->package);

  /* add filename in file to load hashtable */
  fts_set_symbol(&key, template->filename);
  fts_set_symbol(&value, template->filename);
  if (1 == fts_hashtable_put(&template_file_to_load, &key, &value))
    {
      /* cyclic dependency in template definition */
      fts_log("[template] cyclic definition for template %s \n", template->name);
      fts_post("[template] cyclic definition for template %s \n", template->name);
    }
  else
    {
      fts_file_load( template->filename, (fts_object_t *)patcher, ac, at, (fts_object_t **)&instance);
    }
  
  /* remove filename in file to load */
  if (0 == fts_hashtable_remove(&template_file_to_load, &key))
    {
      /* not reachable case */
      fts_log("[template] file to load %s wasn't in file_to_load hashtable \n", template->filename);
    }

  fts_package_pop(template->package);
  
  if (instance)
    {
      fts_template_add_instance( template, (fts_object_t *)instance);

      fts_patcher_set_template( instance, template);

      if(ac > 0)
	{
	  fts_atom_t va;
   
	  /* define the "args" name */
	  /* instance->args is already created by patcher constructor */
   	  fts_tuple_set(instance->args, ac, at);
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
  fts_atom_t key;
  fts_atom_t value;
  fts_template_t* template;
  
  fts_set_symbol(&key, filename);
  /* look into hashtable to know if this file is associated to a template */
  if (1 == fts_hashtable_get(&template_file_loaded, &key, &value))
    {
      /* get template poijter and recompute instances */
      template = fts_get_pointer(&value);
      if (template)
	{
	  /* template can't be null here but it could be better to check */
	  fts_template_recompute_instances(template);
	}
      else
	{
	  /* if we are here it's really bad news ..... */
	  fts_log("[template] a null template is associated with file %s\n",filename);
	  fts_post("[template] a null template is associated with file %s\n",filename);
	}
    }
}


/***********************************************************************
 *
 * Initialization
 *
 */

FTS_MODULE_INIT(template)
{
  template_heap = fts_heap_new(sizeof(fts_template_t));
  fts_hashtable_init(&template_file_to_load, FTS_HASHTABLE_MEDIUM);
  fts_hashtable_init(&template_file_loaded, FTS_HASHTABLE_MEDIUM);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
