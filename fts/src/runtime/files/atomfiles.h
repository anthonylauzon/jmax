#ifndef _ATOMFILES_H_
#define _ATOMFILES_H_

struct _fts_atom_file_t_;
typedef struct _fts_atom_file_t_ fts_atom_file_t;

extern fts_atom_file_t *fts_atom_file_open(const char *name, fts_symbol_t dir, const char *mode);
extern void fts_atom_file_close(fts_atom_file_t *f);

extern int fts_atom_file_read(fts_atom_file_t *f, fts_atom_t *at);
extern int fts_atom_file_write(fts_atom_file_t *f, const fts_atom_t *at, char separator);

#endif
