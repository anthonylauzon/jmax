#ifndef _SAVER_H
#define _SAVER_H

typedef struct fts_bmax_file
{
  int fd;
  fts_binary_file_header_t header; 
  fts_symbol_t *symbol_table;
  int symbol_table_size;
  int symbol_table_fill;
} fts_bmax_file_t;



extern fts_bmax_file_t *fts_open_bmax_file_for_writing(const char *name);
extern void fts_close_bmax_file(fts_bmax_file_t *f);
extern void fts_bmax_code_new_patcher(fts_bmax_file_t *f, fts_object_t *obj);

#endif
