#include "sys.h"
#include "lang.h"
#include "runtime/files/files.h"


static fts_symbol_t fts_search_path = 0;
static fts_symbol_t fts_project_dir = 0;

/*
   The default one is set with an UCS command,
   but also from the user interface.
 */

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

