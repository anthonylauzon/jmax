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

/* 
 * Memory allocators
 *
 * fts_malloc, fts_realloc, fts_zalloc and fts_free
 * are just wrappers on top of the corresponding libc functions.
 * fts_malloc retries allocation in case of error: if the platform
 * provides memory locking for real-time processes, the memory
 * allocation fails as soon as the physical memory is exhausted.
 * fts_malloc unlocks then the memory and retries, in order to provide
 * a safe degradation mode.
 *
 * The heap allocator allocates chunks of fixed size from a free list.
 * Two heaps of the same chunks size are merged.
 * 
 * If you define HELP_PURIFY, all the memory allocation function
 *  will just map to malloc and free, in all cases, simplifing 
 *  catching memory errors with purify.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <fts/fts.h>
#include <ftsprivate/platform.h>

/***********************************************************************
 *
 * Memory allocation 
 *
 */

void *fts_zalloc( unsigned int size)
{
  void *p;

  if (size <= 0)
    return 0;

  p = fts_malloc(size);

  if (p)
    memset(p, 0, size);

  return p;
}


void *fts_malloc(unsigned int size)
{
  void *p;

  if (size <= 0)
    return 0;

  p = malloc(size);

  if (p == 0)
    {
      if (fts_unlock_memory())
	{
	  p = malloc(size);
	  
	  if (p == 0)
	    fts_log( "Out of virtual memory");
	  else
	    {
	      post("Cannot allocate more physical memory, switching to non real-time mode\n");
	      post("See your maxlkmem parameter (man systune)");
	    }
	}
      else
	fts_log( "Out of memory");
    }

  return p;
}


void *fts_realloc( void *p, unsigned int size)
{
  void *r;

  r = realloc(p, size);

  if ( size != 0 && r == 0)
    {
      if (fts_unlock_memory())
	{
	  r = realloc(p, size);
	  
	  if (r == 0)
	    fts_log( "Out of virtual memory");
	  else
	    {
	      post("Cannot allocate more physical memory, switching to non real-time mode\n");
	      post("See your maxlkmem parameter (man systune)");
	    }
	}
      else
	fts_log( "Out of memory");
    }

  return r;
}

void fts_free(void *p)
{
  free(p);
}


/***********************************************************************
 *
 * Heap allocator
 *
 */

#define SHARED_HEAP_MAX_SIZE 256
#define FREE_TO_USED_RATIO    5

struct _fts_heap_t {
  char *free_list;
  unsigned int current_block_group;
  unsigned int block_size;
  unsigned int reserved_blocks;
};

static fts_heap_t *fts_heaps[SHARED_HEAP_MAX_SIZE / sizeof(long)];

static void fts_heap_grow(fts_heap_t *p)
{
  char *mem;
  unsigned int i;

  mem = fts_malloc(p->current_block_group * p->block_size);
  
  for (i = 0; i < (p->current_block_group - 1); i++)
    *((char **) (mem + i * p->block_size)) = mem + (i + 1) * p->block_size;

  *((char **)(mem + (p->current_block_group - 1) * p->block_size)) = p->free_list;
  p->free_list = mem;

  /* Compute next block_size so to have an average of 
     FREE_TO_USED_RATIO free memory to used memory in the heap,
     in case of continous grow */

  p->reserved_blocks += p->current_block_group;
  
  if (p->reserved_blocks / FREE_TO_USED_RATIO > p->current_block_group / 2)
    p->current_block_group = (p->reserved_blocks / FREE_TO_USED_RATIO) * 2;
}


fts_heap_t *fts_heap_new(unsigned int block_size)
{
  if (block_size > SHARED_HEAP_MAX_SIZE)
    {
      /* Unshared heap */

      fts_heap_t *p;

      p = (fts_heap_t *) fts_malloc(sizeof(fts_heap_t));
      p->free_list = 0;
      p->block_size = ( block_size > sizeof(char **) ? block_size : sizeof(char **));
      p->current_block_group = 64;
      p->reserved_blocks = 0;

      return p;
    }
  else
    {
      /* shared heap */

      int idx;

      idx = (block_size / sizeof(long)) - 1;

      if (fts_heaps[idx])
	return fts_heaps[idx];
      else
	{
	  fts_heap_t *p;

	  p = (fts_heap_t *) fts_malloc(sizeof(fts_heap_t));
	  p->free_list = 0;
	  p->block_size = (idx + 1) * sizeof(long);
	  p->current_block_group = 64;
	  fts_heaps[idx] = p;
	  p->reserved_blocks = 0;

	  return p;
	}
    }
}

void *fts_heap_alloc( fts_heap_t *p)
{
#ifdef HELP_PURIFY
  return fts_malloc( p->block_size);
#else
  char *m;

  if (! p->free_list)
    fts_heap_grow(p);

  m = p->free_list;

  p->free_list = *((char **) p->free_list);

  return m;
#endif
}

void *fts_heap_zalloc( fts_heap_t *heap)
{
#ifdef HELP_PURIFY
  return fts_zalloc( heap->block_size);
#else
  char *p;

  p = fts_heap_alloc(heap);

  if (p)
    memset(p, 0, heap->block_size);

  return p;
#endif
}


void fts_heap_free( void *p, fts_heap_t *heap)
{
#ifdef HELP_PURIFY
  fts_free( p);
#else
  *((char **)p) = heap->free_list;
  heap->free_list = p;
#endif
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
