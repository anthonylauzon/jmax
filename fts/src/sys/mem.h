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

/* new allocation routines */

extern void *fts_malloc(unsigned int size);
extern void *fts_zalloc(unsigned int size);
extern void *fts_realloc(void *p, unsigned int size);
extern void fts_free(void *p);

struct fts_heap;
typedef struct fts_heap fts_heap_t;



extern fts_heap_t *fts_heap_new(unsigned int block_size);

extern void *fts_heap_alloc(fts_heap_t *p);
extern void *fts_heap_zalloc(fts_heap_t *p);
extern void  fts_heap_free(void *m, fts_heap_t *p);

extern void *fts_block_alloc(unsigned int size);
extern void *fts_block_zalloc(unsigned int size);
extern void  fts_block_free(void *p, unsigned int size);

#endif




