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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


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

#include <string.h>
#include <sys/stat.h>

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/messP.h"

enum abstraction_mode {fts_abstraction_cache, fts_abstraction_declaration};

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


static void fts_abstraction_add_declaration(fts_symbol_t name, fts_symbol_t filename)
{
  fts_abstraction_t *abs;
  fts_atom_t a;

  /* If the declaration existed already, remove it first */

  if (fts_hash_table_lookup(&abstraction_table, name, &a))
    fts_hash_table_remove(&abstraction_table, name);

  abs = (fts_abstraction_t *) fts_malloc(sizeof(fts_abstraction_t));

  abs->name = name;
  abs->filename = filename;

  fts_set_ptr(&a, abs);
  fts_hash_table_insert(&abstraction_table, name, &a);
}


void fts_abstraction_declare(fts_symbol_t name, fts_symbol_t filename)
{
  fts_abstraction_add_declaration(name, filename);
  fts_recompute_errors();
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

  /* Try to fix some error object */

  fts_recompute_errors();
}


static FILE *fts_abstraction_find_declared_file(fts_symbol_t name)
{
  FILE *file;
  fts_atom_t a;

  /* First, look in the abstraction declaration table */

  if (fts_hash_table_lookup(&abstraction_table, name, &a))
    {
      fts_abstraction_t *abs = (fts_abstraction_t *) fts_get_ptr(&a);

      file = fopen(fts_symbol_name(abs->filename), "r");

      /* Here, we should handle differently declarations that are
	 caches, and declarations that are user declaration;
	 a failing cache is not a failure, just look again in the
	 path (the abstraction moved); a user declaration failure
	 is an error !!
	 */

      return file;
    }
  
  return 0;
}



static FILE *fts_abstraction_find_path_file(fts_symbol_t name)
{
  int i;
  FILE *file;
  struct stat statbuf;

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
      char *extensions[] = { "", ".abs", ".pat" };
      char buf[1024];
      int k;

      file = 0;

      for ( k = 0; k < sizeof( extensions)/sizeof( char *); k++)
	{
	  sprintf(buf, "%s/%s%s", fts_symbol_name(filename), fts_symbol_name(name), extensions[k]);

	  /* If the file is there and it is a regular file and not a directory , open it */
	  if ((stat(buf, &statbuf) == 0) && (statbuf.st_mode & S_IFREG))
	    {
	      file = fopen( buf, "r");
	      break;
	    }
	}

      if (file)
	{
	  /* found, declare it and return */

	  fts_abstraction_add_declaration(name, fts_new_symbol_copy(buf));

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

static fts_object_t *fts_make_abstraction(FILE *file, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_patlex_t *in; 
  fts_atom_t description[1];

  fts_set_symbol(&description[0], fts_s_patcher);
  fts_object_new((fts_patcher_t *)patcher, 1, description, &obj);

  /* flag the patcher as abstraction */

  fts_patcher_set_abstraction((fts_patcher_t *)obj);

  /* get the lexer */

  in = fts_patlex_open_file(file, ac - 1, at + 1);

  fts_patparse_parse_patlex(obj, in);

  fts_patcher_reassign_inlets_outlets((fts_patcher_t *) obj);

  /* Add the template like variables in order to support direct
     .abs to template traslation */

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

static fts_symbol_t get_name_without_extension( fts_symbol_t name)
{
  char *pname, *pdot;

  pname = fts_symbol_name( name);
  pdot = strrchr( pname, '.');

  if ( pdot && ( !strcmp( pdot, ".abs") || !strcmp( pdot, ".pat")) )
    {
      char buff[1024];

      /* .pat or .abs Extension used, generate a new name symbol */

      strcpy( buff, pname);

      buff[ pdot - pname]  = '\0';

      return fts_new_symbol_copy( buff);
    }

  return name;
}


fts_object_t *fts_abstraction_new_declared(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  FILE *file;

  name = get_name_without_extension( fts_get_symbol( at));
  file = fts_abstraction_find_declared_file( name);

  if (file)
    return fts_make_abstraction(file, patcher, ac, at);
  else
    return 0;
}


fts_object_t *fts_abstraction_new_search(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_symbol_t name;
  FILE *file;

  name = get_name_without_extension( fts_get_symbol( at));
  file = fts_abstraction_find_path_file(name);

  if (file)
    return fts_make_abstraction(file, patcher, ac, at);
  else
    return 0;
}
