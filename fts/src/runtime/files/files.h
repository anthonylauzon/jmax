#ifndef _FILES_H_
#define _FILES_H_

extern void fts_file_get_write_path(const char *path, char *full_path);
extern int fts_file_find(const char *name, const char *pathlist, char *pathname);

extern int fts_file_open(const char *name, fts_symbol_t dir, const char *mode);
extern int fts_file_close(int fd);

#endif
