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
 */

#ifndef _FTS_MEM_H
#define _FTS_MEM_H
/* 
   This file include all the memory allocation tools provided by FTS.
*/

/* Standard allocation routines */
FTS_API void *fts_malloc(unsigned int size);
FTS_API void *fts_calloc(unsigned int size);
FTS_API void *fts_realloc(void *p, unsigned int size);
FTS_API void fts_free(void *p);

/* Compatibility */
#define fts_zalloc fts_calloc

/* Heap allocator */
typedef struct _fts_heap_t fts_heap_t;

FTS_API fts_heap_t *fts_heap_new(unsigned int block_size);

FTS_API void *fts_heap_alloc( fts_heap_t *heap);
FTS_API void *fts_heap_calloc( fts_heap_t *heap);
FTS_API void  fts_heap_free( void *p, fts_heap_t *heap);

/* Compatibility */
#define fts_heap_zalloc fts_heap_calloc


#endif




