#ifndef _FILES_H_
#define _FILES_H_

extern int fts_file_find(const char *name, const char *pathlist, char *pathname);
extern int fts_file_open(const char *name, fts_symbol_t dir, const char *mode);
extern int fts_file_close(int fd);

extern void gensampname(char *buf, const char *base, long int n);

extern void          fts_set_default_directory(fts_symbol_t dir);
extern fts_symbol_t fts_get_default_directory(void);
extern void          fts_set_default_search_path(fts_symbol_t search_path);
extern fts_symbol_t fts_get_default_search_path();
extern void          fts_set_search_path(fts_symbol_t search_path);
extern fts_symbol_t fts_get_search_path();

struct _fts_atom_file_t_;
typedef struct _fts_atom_file_t_ fts_atom_file_t;

extern fts_atom_file_t *fts_atom_file_open(const char *name, fts_symbol_t dir, const char *mode);
extern void             fts_atom_file_close(fts_atom_file_t *f);

extern int fts_atom_file_read(fts_atom_file_t *f, fts_atom_t *at);
extern int fts_atom_file_write(fts_atom_file_t *f, const fts_atom_t *at, char separator);
#endif
