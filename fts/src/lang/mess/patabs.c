/*
 * Handling of old .abs/.pat abstractions.
 * 
 * We handle a table of direct declarations (name --> file)
 * and a table of search directory.
 *
 * The table of direct declarations actually contains a abstraction
 * declaration object, including the file name, and possibly 
 * a cache of the declaration text.
 *
 * There is only one group of search path (at least, for the moment)
 * and when an abstraction is found, a new declaration is dynamically
 * created, this to avoid searching again; also, in the short future,
 * the abstraction text can be cached under some condition (size of the
 * text, global cache size limits, ...)
 *
 * Later, the fts_abstraction will also include a table of pointers
 * to all the instances, to allow dynamic redefinition
 */

#include <stdio.h>		/* for error reporting, temp. */
#include <string.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/messP.h"


typedef struct fts_abstraction
{
  fts_symbol_t name;
  fts_symbol_t filename;
  
} fts_abstraction_t;
 
static fts_hash_table_t abstraction_table;
#define INIT_SEARCH_PATH_SIZE 16
static fts_symbol_t *search_path_table;
static int search_path_size;
static int search_path_fill;


void fts_abstraction_init()
{
  fts_hash_table_init(&abstraction_table);

  search_path_size  = INIT_SEARCH_PATH_SIZE;
  search_path_table = (fts_symbol_t *) fts_malloc(search_path_size * sizeof(fts_symbol_t *));
  search_path_fill  = 0;
}


void fts_abstraction_declare(fts_symbol_t name, fts_symbol_t filename)
{
  fts_abstraction_t *abs;
  void *d;

  /* If the declaration existed already, remove it first */

  if (fts_hash_table_lookup(&abstraction_table, name, &d))
    fts_hash_table_remove(&abstraction_table, name);

  abs = (fts_abstraction_t *) fts_malloc(sizeof(fts_abstraction_t));

  abs->name = name;
  abs->filename = filename;

  fts_hash_table_insert(&abstraction_table, name, (void *)abs);
}


void fts_abstraction_declare_path(fts_symbol_t path)
{
  if (search_path_fill >= search_path_size)
    {
      /* realloc the path table */
      fts_symbol_t *new_table;
      int i;
      
      search_path_size = 2 * search_path_size;
      new_table = (fts_symbol_t *) fts_malloc(search_path_size * sizeof(fts_symbol_t *));

      for (i = 0; i < search_path_fill ; i++)
	new_table[i] = search_path_table[i];
      
      fts_free(search_path_table);
      search_path_table = new_table;
    }

  search_path_table[search_path_fill++] = path;
}


static FILE *fts_abstraction_find_declared_file(fts_symbol_t name)
{
  FILE *file;
  void *d;

  /* First, look in the abstraction declaration table */

  if (fts_hash_table_lookup(&abstraction_table, name, &d))
    {
      fts_abstraction_t *abs = (fts_abstraction_t *) d;

      file = fopen(fts_symbol_name(abs->filename), "r");

      /* Here, we should handle differently declarations that are
	 caches, and declarations that are user declaration;
	 a failing cache is not a failure, just look again in the
	 path (the abstraction moved); a user declaration failure
	 is an error !!
	 */

      if (file)
	return file;
    }
  
  return 0;
}



static FILE *fts_abstraction_find_path_file(fts_symbol_t name)
{
  char buf[1024];
  int i;
  FILE *file;

  /*
   * Look in the search path, either with or without ".pat" or ".abs"
   * extensions; declare the abstraction if found !!
   * Problem: in this way, an already path loaded abstraction overwrite
   * a C object dynamically loaded after the first abstraction instantiation.
   * this path thing should just have a private cache.
   */

  for (i = 0; i < search_path_fill ; i++)
    {
      fts_symbol_t filename = search_path_table[i];
	  
      /* Try Nature */

      sprintf(buf, "%s/%s", fts_symbol_name(filename), fts_symbol_name(name));

      file = fopen(buf, "r");

      if (! file)
	{
	  /* Try .abs */

	  sprintf(buf, "%s/%s.abs", fts_symbol_name(filename), fts_symbol_name(name));

	  file = fopen(buf, "r");
	      
	  if (! file)
	    {
	      /* Try .pat */

	      sprintf(buf, "%s/%s.pat", fts_symbol_name(filename), fts_symbol_name(name));
	      file = fopen(buf, "r");
	    }
	}

      if (file)
	{
	  /* found, declare it and return */

	  fts_abstraction_declare(name, fts_new_symbol_copy(buf));

	  return file;
	}
    }

  /* Not found anywhere, return null */

  return 0;
}


/*
 * The real abstraction loader: load the abstraction, looking in the
 * declaration and path table
 *
 */

static fts_object_t *fts_make_abstraction(FILE *file, fts_patcher_t *patcher, int id, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_symbol_t name;
  char name_buf[1024];
  char *p;
  int i;
  fts_patlex_t *in; 
  fts_atom_t a;
  fts_atom_t description[4];

  fts_set_symbol(&description[0], fts_s_patcher);
  fts_set_symbol(&description[1], fts_new_symbol("unnamed"));
  fts_set_int(&description[2], 0);
  fts_set_int(&description[3], 0);

  obj = fts_object_new((fts_patcher_t *)patcher, id, 4, description);

  /* Change the description in the object */

  fts_object_set_description(obj, ac, at);

  /* flag the patcher as abstraction */

  fts_patcher_set_abstraction((fts_patcher_t *)obj);

  /* get the lexer */

  in = fts_patlex_open_file(file, ac - 1, at + 1);

  fts_patparse_parse_patlex(obj, in);

  fts_patcher_reassign_inlets_outlets_name((fts_patcher_t *) obj, fts_get_symbol(&at[0]));

  fts_patlex_close(in);

  return obj;
}


fts_object_t *fts_abstraction_new_declared(fts_patcher_t *patcher, int id, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_symbol_t name;
  char name_buf[1024];
  char *p;
  FILE *file;

  strcpy(name_buf, fts_symbol_name(fts_get_symbol(&at[0])));

  p = strrchr(name_buf, '.');

  if (p && ((! strcmp(p, ".abs")) || (! strcmp(p, ".pat"))))
    {
      /* .pat or .abs Extension used, generate a new name symbol */

      *p = '\0';
      name = fts_new_symbol_copy(name_buf);
    }
  else
    {
      /* No extension used, use the provided symbol directly */

      name = fts_get_symbol(&at[0]);
    }

  file = fts_abstraction_find_declared_file(name);

  if (file)
    return fts_make_abstraction(file, patcher, id, ac, at);
  else
    return 0;
}


fts_object_t *fts_abstraction_new_search(fts_patcher_t *patcher, int id, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_symbol_t name;
  char name_buf[1024];
  char *p;
  FILE *file;

  strcpy(name_buf, fts_symbol_name(fts_get_symbol(&at[0])));

  p = strrchr(name_buf, '.');

  if (p && ((! strcmp(p, ".abs")) || (! strcmp(p, ".pat"))))
    {
      /* .pat or .abs Extension used, generate a new name symbol */

      *p = '\0';
      name = fts_new_symbol_copy(name_buf);
    }
  else
    {
      /* No extension used, use the provided symbol directly */

      name = fts_get_symbol(&at[0]);
    }

  file = fts_abstraction_find_path_file(name);

  if (file)
    return fts_make_abstraction(file, patcher, id, ac, at);
  else
    return 0;
}
  
