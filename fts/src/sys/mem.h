/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _MEM_H
#define _MEM_H
/* 
   This file include all the memory allocation 
   tools provided by FTS.

   A unique memory initizialization function is provided; should be called
   as soon as possible at the FTS kernel startup.
*/

extern void  mem_init(void);

/* allocation routines */

#define fts_malloc(size)        fts_do_malloc((size), __FILE__, __LINE__)
#define fts_zalloc(size)        fts_do_zalloc((size), __FILE__, __LINE__)
#define fts_realloc(pnt, size)  fts_do_realloc((pnt), (size), __FILE__, __LINE__)
#define fts_free(pnt)           fts_do_free((pnt),  __FILE__, __LINE__)

extern void *fts_do_malloc(unsigned int size, const char *filename, int line);
extern void *fts_do_zalloc(unsigned int size, const char *filename, int line);
extern void *fts_do_realloc(void *p, unsigned int size, const char *filename, int line);
extern void fts_do_free(void *p, const char *filename, int line);

struct fts_heap;
typedef struct fts_heap fts_heap_t;

extern fts_heap_t *fts_heap_new(unsigned int block_size);

#define fts_heap_alloc(heap)    fts_do_heap_alloc((heap), __FILE__, __LINE__)
#define fts_heap_zalloc(heap)   fts_do_heap_zalloc((heap), __FILE__, __LINE__)
#define fts_heap_free(m, heap)  fts_do_heap_free((m), (heap),  __FILE__, __LINE__)

extern void *fts_do_heap_alloc(fts_heap_t *p, const char *filename, int line);
extern void *fts_do_heap_zalloc(fts_heap_t *p, const char *filename, int line);
extern void  fts_do_heap_free(void *m, fts_heap_t *p, const char *filename, int line);

#define fts_block_alloc(size)   fts_do_block_alloc((size), __FILE__, __LINE__)
#define fts_block_zalloc(size)  fts_do_block_zalloc((size), __FILE__, __LINE__)
#define fts_block_free(p, size) fts_do_block_free((p), (size),  __FILE__, __LINE__)

extern void *fts_do_block_alloc(unsigned int size, const char *filename, int line);
extern void *fts_do_block_zalloc(unsigned int size, const char *filename, int line);
extern void  fts_do_block_free(void *p, unsigned int size, const char *filename, int line);

/* Exists only with smem.h */

extern void fts_check_pointer(void *p, const char *msg);
extern void fts_describe_pointer(const char *msg, void *pv, FILE *f);

#endif




