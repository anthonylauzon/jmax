/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#ifndef _FTS_MEM_H
#define _FTS_MEM_H
/* 
   This file include all the memory allocation 
   tools provided by FTS.
*/

#include <stdio.h>

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




