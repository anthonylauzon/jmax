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

/* #define HELP_PURIFY 
#include "smem.h"  */

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

static void fts_block_init(void);

void
mem_init(void)
{
  fts_block_init();
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
}



void
fts_heap_init(fts_heap_t *p, int block_size, int block_group)
{
  /* initialize to an empty heap */

  p->free_list = 0;
  p->block_size = ( block_size > sizeof(char **) ? block_size : sizeof(char **));
  p->current_block_group = (block_group ? block_group : 512);
}


fts_heap_t *
fts_heap_new(int block_size, int block_group)
{
  fts_heap_t *p;

  p = (fts_heap_t *) fts_malloc(sizeof(fts_heap_t));
  fts_heap_init(p, block_size, block_group);

  return p;
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


#define MAX_SIZE 64

fts_heap_t mess_heaps[MAX_SIZE / sizeof(long)];

static void
fts_block_init(void)
{
  int i;

  for (i = 0; i < (MAX_SIZE / sizeof(long)); i++)
    fts_heap_init(mess_heaps + i, (i + 1) * sizeof(long), 32);
}


char *
fts_block_alloc(int size)
{
#ifdef HELP_PURIFY
  return fts_malloc(size);
#else
  if (size > MAX_SIZE)
    return fts_malloc(size);
  else
    return fts_heap_alloc(&(mess_heaps[(size / sizeof(long)) - 1]));
#endif
}


char *
fts_block_zalloc(int size)
{
#ifdef HELP_PURIFY
  return fts_zalloc(size);
#else
  if (size > MAX_SIZE)
    return fts_zalloc(size);
  else
    return fts_heap_zalloc(&(mess_heaps[(size / sizeof(long)) - 1]));
#endif
}


void
fts_block_free(char *p, int size)
{
#ifdef HELP_PURIFY
  fts_free(p);
#else
  if (size > MAX_SIZE)
    fts_free(p);
  else
    fts_heap_free(p, &(mess_heaps[(size / sizeof(long)) - 1]));
#endif
}

