#ifndef _MEM_H
#define _MEM_H
/* 
   This file include all the memory allocation 
   tools provided by FTS.

   A unique memory initizialization function is provided; should be called
   as soon as possible at the FTS kernel startup.
*/

extern void  mem_init(void);

/* new allocation routines */

extern void *fts_malloc(int size);
extern void *fts_zalloc(int size);
extern void *fts_realloc(void *p, int size);
extern void fts_free(void *p);

struct fts_heap;
typedef struct fts_heap fts_heap_t;



extern fts_heap_t *fts_heap_new(int block_size);

extern char *fts_heap_alloc(fts_heap_t *p);
extern char *fts_heap_zalloc(fts_heap_t *p);
extern void  fts_heap_free(char *m, fts_heap_t *p);

extern char *fts_block_alloc(int size);
extern char *fts_block_zalloc(int size);
extern void  fts_block_free(char *p, int size);

#endif




