#include "sys.h"
#include "lang/mess.h"
#include "lang/ftl.h"

void
ftl_memory_declaration_init( ftl_memory_declaration *decl, int size)
{
  decl->size = size;
  decl->address = 0;
}

ftl_memory_declaration *
ftl_memory_declaration_new( int size )
{
  ftl_memory_declaration *decl;

  decl = (ftl_memory_declaration *) fts_malloc(sizeof( ftl_memory_declaration));
  if (decl)
    ftl_memory_declaration_init( decl, size);
  return decl;
}



void
ftl_function_declaration_init( ftl_function_declaration *decl, ftl_wrapper_t wrapper)
{
  decl->wrapper = wrapper;
}

ftl_function_declaration *
ftl_function_declaration_new( ftl_wrapper_t wrapper)
{
  ftl_function_declaration *decl;

  decl = (ftl_function_declaration *) fts_malloc(sizeof( ftl_function_declaration));
  if (decl)
    ftl_function_declaration_init( decl, wrapper);
  return decl;
}



void
ftl_function_declaration_free( ftl_function_declaration *decl)
{
  fts_free(decl);
}


