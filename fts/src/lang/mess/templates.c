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

#include <string.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/messP.h"


typedef struct fts_template
{
  fts_symbol_t name;
  fts_symbol_t filename;
  
} fts_template_t;
 
static fts_hash_table_t template_table;
#define INIT_TEMPLATE_SEARCH_PATH_SIZE 16
static fts_symbol_t *template_search_path_table;
static int template_search_path_size;
static int template_search_path_fill;


void fts_template_init()
{
  fts_hash_table_init(&template_table);

  template_search_path_size  = INIT_TEMPLATE_SEARCH_PATH_SIZE;
  template_search_path_table = (fts_symbol_t *) fts_malloc(template_search_path_size * sizeof(fts_symbol_t *));
  template_search_path_fill  = 0;
}


void fts_template_declare(fts_symbol_t name, fts_symbol_t filename)
{
  fts_template_t *template;
  fts_atom_t d;

  /* If the declaration existed already, remove it first */

  if (fts_hash_table_lookup(&template_table, name, &d))
    fts_hash_table_remove(&template_table, name);

  template = (fts_template_t *) fts_malloc(sizeof(fts_template_t));

  template->name = name;
  template->filename = filename;
  fts_set_ptr(&d, template);
  fts_hash_table_insert(&template_table, name, &d);
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
}


static fts_symbol_t fts_template_find_declared_file(fts_symbol_t name)
{
  FILE *file;
  fts_atom_t d;

  /* First, look in the template declaration table */

  if (fts_hash_table_lookup(&template_table, name, &d))
    {
      fts_template_t *template = (fts_template_t *) fts_get_ptr(&d);

      /* it should use stat here !!! */

      file = fopen(fts_symbol_name(template->filename), "r");

      /* Here, we should handle differently declarations that are
	 caches, and declarations that are user declaration;
	 a failing cache is not a failure, just look again in the
	 path (the template moved); a user declaration failure
	 is an error !!
	 */

      if (file)
	{
	  fclose(file);

	  return template->filename;
	}
    }
  
  return 0;
}



static fts_symbol_t fts_template_find_path_file(fts_symbol_t name)
{
  char buf[1024];
  int i;
  FILE *file;

  /*
   * Look in the search path, with a mandatory ".tpl" extension.
   * declare the template if found !!
   * Problem: in this way, an already path loaded template overwrite
   * a C object dynamically loaded after the first template instantiation.
   * this path thing should just have a private cache.: But, the 
   * declaration should actually become the template cache, and redefinition
   * structure, so it should actually be the dynamic definition of a class
   * to clean up the template declaration if there an over-riding is wished.
   */

  for (i = 0; i < template_search_path_fill ; i++)
    {
      fts_symbol_t filename = template_search_path_table[i];
	  
      /* it should use stat here !!! */

      sprintf(buf, "%s/%s.tpl", fts_symbol_name(filename), fts_symbol_name(name));

      file = fopen(buf, "r");

      if (file)
	{
	  fts_symbol_t s;


	  /* found, declare it and return */

	  fclose(file);
	  s  = fts_new_symbol_copy(buf);
	  fts_template_declare(name, s);

	  return s;
	}
    }

  /* Not found anywhere, return null */

  return 0;
}


/*
 * The real template loader: load the template, looking in the
 * declaration and path table
 *
 */

static fts_object_t *fts_make_template(fts_symbol_t file,
				       fts_patcher_t *patcher,
				       int ac, const fts_atom_t *at,
				       fts_expression_state_t *e)
{
  fts_object_t *obj;

  obj = fts_binary_file_load(fts_symbol_name(file), (fts_object_t *) patcher, ac, at, e);

  /* flag the patcher as template */

  fts_patcher_set_template((fts_patcher_t *)obj);

  return obj;
}


fts_object_t *fts_template_new_declared(fts_patcher_t *patcher,
					int ac, const fts_atom_t *at,
					fts_expression_state_t *e)
{
  fts_object_t *obj;
  fts_symbol_t file;

  file = fts_template_find_declared_file(fts_get_symbol(&at[0]));

  if (file)
    return fts_make_template(file, patcher, ac, at, e);
  else
    return 0;
}


fts_object_t *fts_template_new_search(fts_patcher_t *patcher,
				      int ac, const fts_atom_t *at,
				      fts_expression_state_t *e)
{
  fts_object_t *obj;
  fts_symbol_t file;

  file = fts_template_find_path_file(fts_get_symbol(&at[0]));

  if (file)
    return fts_make_template(file, patcher, ac, at, e);
  else
    return 0;
}
  
