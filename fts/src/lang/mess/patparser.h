#ifndef _PATPARSER_H_
#define _PATPARSER_H_

#include <stdio.h>

extern fts_object_t *fts_load_dotpat_patcher(fts_object_t *patcher, const char *inputFile);
extern fts_object_t *fts_abstraction_new(fts_patcher_t *patcher, int ac, const fts_atom_t *at);

#endif
