#ifndef _DIRECTORIES_H_
#define _DIRECTORIES_H_


extern void fts_set_search_path(fts_symbol_t search_path);
extern fts_symbol_t fts_get_search_path(void);

extern void fts_set_project_dir(fts_symbol_t project_dir);
extern fts_symbol_t fts_get_project_dir(void);

#endif
