#include "sys.h"
#include "lang.h"
#include "runtime/files/files.h"

/*
   temporary, should be substituted by something more complex,
   flexible, and easy to use; essentially, it should not
   involve the MAX cooperation.
*/

static fts_symbol_t default_directory = 0;
static fts_symbol_t fts_default_search_path = 0;
static fts_symbol_t fts_search_path = 0;
static fts_symbol_t fts_project_dir = 0;

void
fts_set_default_directory(fts_symbol_t dir)
{
  default_directory = dir;
}

fts_symbol_t 
fts_get_default_directory()
{
  return default_directory;
}

/*
   The default one is set with an UCS command,
   but also from the user interface.
 */

void
fts_set_default_search_path(fts_symbol_t search_path)
{
  fts_default_search_path = search_path;
}

fts_symbol_t 
fts_get_default_search_path()
{
  return fts_default_search_path;
}

void
fts_set_search_path(fts_symbol_t search_path)
{
  fts_search_path = search_path;
}

fts_symbol_t 
fts_get_search_path()
{
  return fts_search_path;
}

void
fts_set_project_dir(fts_symbol_t project_dir)
{
  fts_project_dir = project_dir;
}

fts_symbol_t
fts_get_project_dir(void)
{
  return fts_project_dir;
}

