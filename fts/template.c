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

/* #define TEMPLATE_DEBUG */

#include "ftsconfig.h"

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

#include <fts/fts.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/vm.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/loader.h>

/* Template */

enum template_mode {fts_template_filename_cached, fts_template_declaration, fts_template_binary_cached};

struct fts_template
{
  fts_symbol_t name;
  fts_symbol_t filename;
  fts_objectset_t *instances;
  struct {
    unsigned char *program;
    fts_symbol_t *symbol_table;
  } binary;
  enum template_mode mode;
};

static void fts_template_recompute_instances(fts_template_t *template);

static fts_hashtable_t template_table;
#define INIT_TEMPLATE_SEARCH_PATH_SIZE 16
static fts_symbol_t *template_search_path_table;
static int template_search_path_size;
static int template_search_path_fill;
static fts_heap_t *template_heap;

static void fts_template_register_filename(fts_symbol_t name, fts_symbol_t filename, enum template_mode mode)
{
  char buf[MAXPATHLEN];
  fts_template_t *template;
  fts_atom_t d, k;

  /* resolve the links in the path, so that we have a unique name 
     for the file */

  realpath(fts_symbol_name(filename), buf);
  filename = fts_new_symbol_copy(buf);

  /* Make the template */
     
  template = (fts_template_t *) fts_heap_alloc(template_heap);

  template->name = name;
  template->filename = filename;
  template->instances = 0;
  template->mode = mode;

  fts_set_symbol( &k, name);
  fts_set_ptr(&d, template);
  fts_hashtable_put(&template_table, &k, &d);

#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Registered template %s file %s mode %s\n",
	  fts_symbol_name(name), fts_symbol_name(filename),
	  ( mode == fts_template_declaration ? "declaration" : "cache")); /* @@@@ */
#endif
}

void fts_template_register_binary(fts_symbol_t name, unsigned char *program, fts_symbol_t *symbol_table)
{
  fts_template_t *template;
  fts_atom_t d, k;

  /* Make the template */
     
  template = (fts_template_t *) fts_heap_alloc(template_heap);

  template->name = name;
  template->instances = 0;
  template->binary.program = program;
  template->binary.symbol_table = symbol_table;

  template->mode = fts_template_binary_cached;

  fts_set_symbol( &k, name);
  fts_set_ptr(&d, template);
  fts_hashtable_put(&template_table, &k, &d);

#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Registered binary template %s\n", fts_symbol_name(name)); /* @@@@ */
#endif
}


static void fts_template_redefine(fts_template_t *template, fts_symbol_t filename)
{
  template->filename = filename;
  fts_template_recompute_instances(template);
}


static fts_template_t *fts_template_find(fts_symbol_t name)
{
  fts_atom_t a, k;

  fts_set_symbol( &k, name);
  if (fts_hashtable_get(&template_table, &k, &a))
    return (fts_template_t *) fts_get_ptr(&a);
  else
    return 0;
}


static void fts_template_recompute_instances(fts_template_t *template)
{
#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Recomputing instances of template %s\n", 
	  fts_symbol_name(template->name)); /* @@@ */
#endif

  if (template->instances)
    {
      fts_objectset_t *old_instances;
      fts_iterator_t iterator;

      old_instances = template->instances;

      template->instances = (fts_objectset_t *)fts_malloc( sizeof( fts_objectset_t));
      fts_objectset_init( template->instances);

      fts_objectset_get_objects( old_instances, &iterator);

      while ( fts_iterator_has_more( &iterator))
	{
	  fts_object_t *object;
	  fts_atom_t a;

	  fts_iterator_next( &iterator, &a);
	  object = fts_get_object( &a);

#ifdef TEMPLATE_DEBUG 
	  fprintf(stderr, "Recomputing instance:"); /* @@@ */
	  fprintf_object(stderr, object);
	  fprintf(stderr, "\n");
#endif

	  fts_object_recompute(object);
	}

      fts_objectset_destroy(old_instances);
      fts_free( old_instances);
    }
#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Done.\n");
#endif
}


void fts_template_declare(fts_symbol_t name, fts_symbol_t filename)
{
  fts_template_t *template;

  /* If the declaration existed already, remove it first */

  template = fts_template_find(name);

  if (template)
    {
      /* change the template definition, and redefine all the instances */
      
      fts_template_redefine(template, filename);
    }
  else
    {
      /* Register the template */

      fts_template_register_filename(name, filename, fts_template_declaration);
    }

  /* And give a try to error objects also */

  fts_recompute_errors();
}


void fts_template_declare_path(fts_symbol_t path)
{
  if (template_search_path_fill >= template_search_path_size)
    {
      /* realloc the path table */
      fts_symbol_t *new_table;
      int i;
      
      template_search_path_size = 2 * template_search_path_size;
      new_table = (fts_symbol_t *) fts_malloc(template_search_path_size * sizeof(fts_symbol_t *));

      for (i = 0; i < template_search_path_fill ; i++)
	new_table[i] = template_search_path_table[i];
      
      fts_free(template_search_path_table);
      template_search_path_table = new_table;
    }

  template_search_path_table[template_search_path_fill++] = path;

  /* And give a try to error objects also */

  fts_recompute_errors();
}



static void fts_template_find_in_path_and_cache(fts_symbol_t name)
{
  char buf[MAXPATHLEN];
  int i;
  FILE *file;

  /*
   * Look in the search path, with a mandatory ".tpl" extension.
   * cache the template if found !!
   * Problem: in this way, an already path loaded template overwrite
   * a C object dynamically loaded after the first template instantiation.
   * this path thing should just have a private cache.: But, the 
   * declaration should actually become the template cache, and redefinition
   * structure, so it should actually be the dynamic definition of a class
   * to clean up the template declaration if there an over-riding is wished.
   */

  for (i = 0; i < template_search_path_fill ; i++)
    {
      struct stat statbuf;
      fts_symbol_t filename = template_search_path_table[i];
	  
      /* it should use stat here !!! */

      sprintf(buf, "%s/%s.jmax", fts_symbol_name(filename), fts_symbol_name(name));

      if ((stat(buf, &statbuf) == 0) && (statbuf.st_mode & S_IFREG))
	fts_template_register_filename(name, fts_new_symbol_copy(buf), fts_template_filename_cached);
    }
}


void fts_template_add_instance(fts_template_t *template, fts_object_t *object)
{
#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Adding instance to template %s : ", fts_symbol_name(template->filename)); /* @@@ */
  fprintf_object(stderr, object);
  fprintf(stderr, "\n");
#endif

  if (template->instances == 0)
    {
      template->instances = (fts_objectset_t *)fts_malloc( sizeof( fts_objectset_t));
      fts_objectset_init( template->instances);
    }
  
  fts_objectset_add(template->instances, object);
}

void fts_template_remove_instance(fts_template_t *template, fts_object_t *object)
{
#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Removing instance to template %s : ", fts_symbol_name(template->filename)); /* @@@ */
  fprintf_object(stderr, object);
  fprintf(stderr, "\n");
#endif

  if (template->instances != 0)
    fts_objectset_remove(template->instances, object);
}


/*
 * The real template loader: load the template, looking in the
 * declaration and path table
 *
 */

static fts_object_t *fts_make_template_instance(fts_template_t *template,
						fts_patcher_t *patcher,
						int ac, const fts_atom_t *at,
						fts_expression_state_t *e)
{
  fts_object_t *obj;

  if (template->mode == fts_template_binary_cached)
    obj = fts_run_mess_vm( (fts_object_t *)patcher, template->binary.program, template->binary.symbol_table, ac, at, e);
  else
    obj = fts_binary_file_load(fts_symbol_name(template->filename), (fts_object_t *) patcher, ac, at, e);
  
  /* flag the patcher as template, and set the template */

  if (obj)
    fts_patcher_set_template((fts_patcher_t *)obj, template);

  return obj;
}


fts_object_t *fts_template_new_declared(fts_patcher_t *patcher,
					int ac, const fts_atom_t *at,
					fts_expression_state_t *e)
{
  fts_template_t *template;

  template = fts_template_find(fts_get_symbol(&at[0]));

  if (template 
      && ((template->mode ==  fts_template_declaration) || (template->mode ==  fts_template_binary_cached)))
    return fts_make_template_instance(template, patcher, ac, at, e);
  else
    return 0;
}


fts_object_t *fts_template_new_search(fts_patcher_t *patcher,
				      int ac, const fts_atom_t *at,
				      fts_expression_state_t *e)
{
  fts_template_t *template;

  /* First, test the cache */

  template = fts_template_find(fts_get_symbol(&at[0]));

  if (! template)
    {
      fts_template_find_in_path_and_cache(fts_get_symbol(&at[0]));
      template = fts_template_find(fts_get_symbol(&at[0]));
    }

  if (template && (template->mode == fts_template_filename_cached))
    return fts_make_template_instance(template, patcher, ac, at, e);
  else
    return 0;
}
  

/* Redefinition support */

static fts_template_t *fts_template_for_file(fts_symbol_t filename)
{
  fts_iterator_t hit;

  fts_hashtable_get_values( &template_table, &hit);

  while ( fts_iterator_has_more( &hit))
    {
      fts_atom_t v;
      fts_template_t *template;

      fts_iterator_next( &hit, &v);

      template = (fts_template_t *) fts_get_ptr( &v);

      if (template->filename == filename)
	return template;
    }

  return 0;
}

/* redefine the template corresponding to a given file */

void fts_template_file_modified(fts_symbol_t filename)
{
  char buf[MAXPATHLEN];
  fts_template_t *template;

  /* resolve the links in the path, so that we have a unique name 
     for the file */

  realpath(fts_symbol_name(filename), buf);
  filename = fts_new_symbol_copy(buf);

#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "File %s modified.\n", fts_symbol_name(filename));
#endif

  template = fts_template_for_file(filename);

  if (template)
    {
#ifdef TEMPLATE_DEBUG 
      fprintf(stderr, "Then Redefining Instances.\n");
#endif
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
  fts_hashtable_init(&template_table, 0, FTS_HASHTABLE_MEDIUM);
  template_heap = fts_heap_new(sizeof(fts_template_t));

  template_search_path_size  = INIT_TEMPLATE_SEARCH_PATH_SIZE;
  template_search_path_table = (fts_symbol_t *) fts_malloc(template_search_path_size * sizeof(fts_symbol_t *));
  template_search_path_fill  = 0;
}
