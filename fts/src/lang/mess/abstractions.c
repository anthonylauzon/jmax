/*
  This file include the compatibilty code
  with the good old .abs abstractions; of course
  it use the pat parser included in the two files
  patparser.c and patlex.c
  */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

#include <stdio.h>

fts_object_t *fts_abstraction_new(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  char buf[1024];
  fts_symbol_t name;

  name = fts_get_symbol(&at[0]);

  fprintf(stderr, "Called importPatcher %s\n", fts_symbol_name(name)); /* @@@@ */  

  /* Find the file (HACK, to be substituted with the real thing) */

  sprintf(buf, "/u/worksta/dececco/tmp/abs/%s.abs", fts_symbol_name(name));

  /* Once find the file */

  return importAbstraction((fts_object_t *) patcher, buf, ac, at);
}
