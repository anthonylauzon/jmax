#ifndef _PATPARSER_H_
#define _PATPARSER_H_

extern fts_object_t *importPatcher(fts_object_t *patcher, const char *inputFile);

extern fts_object_t *importAbstraction(fts_object_t *parent, const char *inputFile,
				       int env_argc, const fts_atom_t *env_argv);
#endif
