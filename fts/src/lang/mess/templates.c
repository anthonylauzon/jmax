/*
 * Handling of looking up for new templates; the handling it is probabily
 * temporary.
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

#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/datalib.h"
#include "lang/mess/messP.h"

/* Forward declarations */



/* Template */

enum template_mode {fts_template_cache, fts_template_declaration};

struct fts_template
{
  fts_symbol_t name;
  fts_symbol_t filename;
  fts_object_set_t *instances;
  enum template_mode mode;
};

static void fts_template_recompute_instances(fts_template_t *template);

static fts_hash_table_t template_table;
#define INIT_TEMPLATE_SEARCH_PATH_SIZE 16
static fts_symbol_t *template_search_path_table;
static int template_search_path_size;
static int template_search_path_fill;
static fts_heap_t *template_heap;

void fts_template_init()
{
  fts_hash_table_init(&template_table);
  template_heap = fts_heap_new(sizeof(fts_template_t));

  template_search_path_size  = INIT_TEMPLATE_SEARCH_PATH_SIZE;
  template_search_path_table = (fts_symbol_t *) fts_malloc(template_search_path_size * sizeof(fts_symbol_t *));
  template_search_path_fill  = 0;
}

static void fts_template_register(fts_symbol_t name, fts_symbol_t filename, enum template_mode mode)
{
  char buf[MAXPATHLEN];
  fts_template_t *template;
  fts_atom_t d;

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

  fts_set_ptr(&d, template);
  fts_hash_table_insert(&template_table, name, &d);

#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Registered template %s file %s mode %s\n",
	  fts_symbol_name(name), fts_symbol_name(filename),
	  ( mode == fts_template_declaration ? "declaration" : "cache")); /* @@@@ */
#endif
}


static void fts_template_redefine(fts_template_t *template, fts_symbol_t filename)
{
  template->filename = filename;
  fts_template_recompute_instances(template);
}


static fts_template_t *fts_template_find(fts_symbol_t name)
{
  fts_atom_t a;

  if (fts_hash_table_lookup(&template_table, name, &a))
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
      fts_object_set_t *old_instances;
      fts_object_set_iterator_t *iterator;

      old_instances = template->instances;

      template->instances = fts_object_set_new();

      iterator = fts_object_set_iterator_new(old_instances);

      while (! fts_object_set_iterator_end(iterator))
	{
	  fts_object_t *object;

	  object = fts_object_set_iterator_current(iterator);

#ifdef TEMPLATE_DEBUG 
	  fprintf(stderr, "Recomputing instance:"); /* @@@ */
	  fprintf_object(stderr, object);
	  fprintf(stderr, "\n");
#endif

	  fts_object_recompute(object);

	  fts_object_set_iterator_next(iterator);
	}

      fts_object_set_iterator_free(iterator);
      fts_object_set_delete(old_instances);
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

      fts_template_register(name, filename, fts_template_declaration);
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
	fts_template_register(name, fts_new_symbol_copy(buf), fts_template_cache);
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
    template->instances = fts_object_set_new();
  
  fts_object_set_add(template->instances, object);
}

void fts_template_remove_instance(fts_template_t *template, fts_object_t *object)
{
#ifdef TEMPLATE_DEBUG 
  fprintf(stderr, "Removing instance to template %s : ", fts_symbol_name(template->filename)); /* @@@ */
  fprintf_object(stderr, object);
  fprintf(stderr, "\n");
#endif

  if (template->instances != 0)
    fts_object_set_remove(template->instances, object);
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

  if (template && (template->mode ==  fts_template_declaration))
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

  if (template && (template->mode == fts_template_cache))
    return fts_make_template_instance(template, patcher, ac, at, e);
  else
    return 0;
}
  

/* Redefinition support */

static fts_template_t *fts_template_for_file(fts_symbol_t filename)
{
  fts_hash_table_iterator_t hit;

  fts_hash_table_iterator_init(&hit, &template_table);

  for (; (!   fts_hash_table_iterator_end(&hit)); fts_hash_table_iterator_next(&hit))
    {
      fts_template_t *template = (fts_template_t *) fts_get_ptr(fts_hash_table_iterator_current_data(&hit));

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



