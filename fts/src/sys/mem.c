/* 
   This file include all the memory allocation 
   tools provided by FTS.

   A unique memory initizialization function is provided; should be called
   as soon as possible at the FTS kernel startup.

   Also, the memory allocation package do not print error messages,
   it just return NULL (or a error status ?); it is the application
   responsability to return the correct error code.

   The heap system should merge two heaps of the same size.

   If you define HELP_PURIFY, all the memory allocation function
   will just map to malloc and free, in all cases, simplifing 
   catching memory errors with purify.

   If you include smem.h, the smem.c library is used instead
   of the standard malloc; use help_purify and this one to try
   to catch memory problems without purify
*/

/* #ifdef DEBUG */
#define HELP_PURIFY 
/* #endif */

/* #include "smem.h"  */

#include <stdlib.h>
#include "sys.h"

#ifndef SAFE_MEM
#define real_malloc  malloc
#define real_free    free
#define real_realloc realloc
#endif

/******************************************************************************/
/*                                                                            */
/*          Memory Initialization                                             */
/*                                                                            */
/******************************************************************************/

static void fts_heaps_init(void);

void
mem_init(void)
{
  fts_heaps_init();
}

/******************************************************************************/
/*                                                                            */
/*          The real official FTS memory manager                              */
/*                                                                            */
/******************************************************************************/

/* statistic routines */

void *
fts_zalloc(int size)
{
  if (size <= 0)
    return 0;
  else
    {
      void *p;

      p = fts_malloc(size);

      if (p)
	memset(p, 0, size);

      return p;
    }
}


void *
fts_malloc(int size)
{
  return real_malloc(size);
}


void *
fts_realloc(void *p, int size)
{
  return real_realloc(p, size);
}

void
fts_free(void *p)
{
  real_free(p);
}

/******************************************************************************/
/*                                                                            */
/*          The real official FTS Block Heap manager                          */
/*                                                                            */
/******************************************************************************/


#define SHARED_HEAP_MAX_SIZE 256
#define FREE_TO_USED_RATIO    5

struct fts_heap
{
  char *free_list;
  int current_block_group;
  int block_size;
  int reserved_blocks;
};

static fts_heap_t *fts_heaps[SHARED_HEAP_MAX_SIZE / sizeof(long)];

static void
fts_heaps_init(void)
{
  int i;

  for (i = 0; i < (SHARED_HEAP_MAX_SIZE / sizeof(long)); i++)
    fts_heaps[i] = 0;
}



static void
fts_heap_grow(fts_heap_t *p)
{
  char *mem;
  int i;

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



fts_heap_t *
fts_heap_new(int block_size)
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


char *
fts_heap_alloc(fts_heap_t *p)
{
#ifdef HELP_PURIFY
  return fts_malloc(p->block_size);
#else
  char *m;

  if (! p->free_list)
    fts_heap_grow(p);

  m = p->free_list;

  p->free_list = *((char **) p->free_list);

  return m;
#endif
}


char *
fts_heap_zalloc(fts_heap_t *heap)
{
#ifdef HELP_PURIFY
  return fts_zalloc(heap->block_size);
#else
  char *p;

  p = fts_heap_alloc(heap);

  if (p)
    memset(p, 0, heap->block_size);

  return p;
#endif
}


void
fts_heap_free(char *m, fts_heap_t *p)
{
#ifdef HELP_PURIFY
  fts_free((void *)m);
#else
  *((char **)m) = p->free_list;
  p->free_list = m;
#endif
}


/* 

   Heap handling dedicated to the message system.

   It use an array of heaps, of small dimensions,
   and define an fts_block_alloc and fts_block_free
   function, that dispatch to those heaps or to 
   the main fts alloc function.

 */



char *
fts_block_alloc(int size)
{
#ifdef HELP_PURIFY
  return fts_malloc(size);
#else
  if (size > SHARED_HEAP_MAX_SIZE)
    return fts_malloc(size);
  else
    {
      int idx;

      idx = (size / sizeof(long)) - 1;

      if (! fts_heaps[idx])
	fts_heaps[idx] = fts_heap_new((idx + 1) * sizeof(long));

      return fts_heap_alloc(fts_heaps[idx]);
    }
#endif
}


char *
fts_block_zalloc(int size)
{
#ifdef HELP_PURIFY
  return fts_zalloc(size);
#else
  if (size > SHARED_HEAP_MAX_SIZE)
    return fts_zalloc(size);
  else
    {
      int idx;

      idx = (size / sizeof(long)) - 1;

      if (! fts_heaps[idx])
	fts_heaps[idx] = fts_heap_new((idx + 1) * sizeof(long));

      return fts_heap_zalloc(fts_heaps[idx]);
    }
#endif
}


void
fts_block_free(char *p, int size)
{
#ifdef HELP_PURIFY
  fts_free(p);
#else
  if (size > SHARED_HEAP_MAX_SIZE)
    fts_free(p);
  else
    {
      int idx;

      idx = (size / sizeof(long)) - 1;

      if (! fts_heaps[idx])
	return;			/* error !!! */

      fts_heap_free(p, fts_heaps[idx]);
    }
#endif
}





