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

/* Safe memory library.

   To link only during debugging of bugs difficult to trace.

   It do the following thing:

   1- verify that all malloc return distinct block of memory.
   2- verify that all the free are called on malloc'ed and not yet freed memory.
   3- Providy a way to verify that a pointer, or an array are actually in malloced
      not yet freed memory.

   Now is slow, will become faster if needed.

   Since it cannot use malloc, it use static memory for its
   housekeeping; the result is that there is a compile time defined limit
   to the number of malloc/free for any session (the OPS_LIMIT below).

   Added begin and end marker, to check for block consistency.


   MOST OF THE CHECKS are being disabled, because purify made them
   useless; anyway, we still keep the structure to perform memory
   statistics.
*/

#include <stdio.h>
#include <malloc.h>

/* Uncomment the following define to get a print for each malloc/free operation.
   Usefull to understand what's going on; only for small tests, of course */

/* #define LOG_CALLS */

/* Uncomment the following define to get smem to trap (core dump) for each error found */

#define TRAP_ON_ERROR

#define OPS_LIMIT 512*1024
#define BLOCK_CACHE_SIZE 16

#include "smem.h"

#define SMEM_START_MARKER 0xaa55aa55
#define SMEM_END_MARKER   0xaa5555aaL


struct mem_header
{
  unsigned int marker;
  const char *filename;
  int   line;
  unsigned long size;
  struct mem_header *next;
  unsigned int  marker2;
};

static struct mem_header *first_block = 0;

struct mem_trailer
{
  unsigned long marker;
};


static void
add_block(char *block)
{
  struct mem_header *p = (struct mem_header *)block;

  p->next = first_block;
  first_block = p;
}

static void
remove_block(char *block)
{
  struct mem_header **pp;	/* indirect precursor */
  struct mem_header  *p = (struct mem_header *)block;

  for (pp = &first_block; *pp; pp = &((*pp)->next))
    if (*pp == p)
      {
	*pp = (*pp)->next;
	return;
      }
}

static void fts_check_block(char *p, const char *msg)
{
  const char *err;

  if ((((struct mem_header *)p)->marker != SMEM_START_MARKER) &&
      (((struct mem_header *)p)->marker2 != SMEM_START_MARKER))
    err = "Block integrity error: data structure corrupted before the block";
  else if (((struct mem_trailer *)(p + sizeof(struct mem_header) + ((struct mem_header *)p)->size))->marker
	   != SMEM_END_MARKER)
    err = "Block integrity error: data structure corrupted after the block";
  else
    return;

  fprintf(stderr, "%s %s\n", err, msg);
  fprintf(stderr, "Originally allocated file %s line %d\n", 
	  ((struct mem_header *)p)->filename,  ((struct mem_header *)p)->line);

#ifdef TRAP_ON_ERROR
  fprintf(stderr, " - Trapping !!!!\n");
  fflush(stderr);

  {
    volatile char c;
    c = *((volatile char *) 0);
  }
#endif
}


void fts_check_pointer(void *p, const char *msg)
{
  fts_check_block(((char *)p  - sizeof(struct mem_header)), msg);
}

void fts_describe_pointer(const char *msg, void *pv, FILE *f)
{
  if (pv)
    {
      char *p = ((char *)pv  - sizeof(struct mem_header));

      fprintf(f, "%s: Pointer %lx Originally allocated file %s line %d\n", msg, (unsigned long) pv,
	      ((struct mem_header *)p)->filename,  ((struct mem_header *)p)->line);

      fts_check_pointer(pv, msg);
    }
  else
    fprintf(f, "%s: Null pointer\n", msg);
}


typedef enum { malloced, invalid} mem_status_t;

static  struct
{
  mem_status_t status;
  char *p;
} malloc_table[OPS_LIMIT];

static unsigned int malloc_table_fill = 0;

static int filled_table_full = 1;

static int smem_error = 0; /* malloc table full warning given */

static void smem_show_error(void)
{
  if (! smem_error)
    {
      fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      fprintf(stderr, "!!! SMEM LOOSING INFORMATION, GROW SMEM OPS_LIMIT FACTOR !!!!\n");
      fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      smem_error = 1;
    }
}

static void
register_check_malloc(char *p, unsigned long size)
{
  unsigned int i;
  int idx = -1;			/* index where to insert the new entry */

  if (smem_error)
    return;

  if (filled_table_full)
    {
      /* The table have no holes, so just add at the end */

      if (malloc_table_fill < OPS_LIMIT)
	{
	  idx = malloc_table_fill;
	  malloc_table_fill++;
	}
      else
	{
	  smem_show_error();
	  return;
	}
    }
  else
    {
      /* The table may have holes, so look for them */
  
      for (i = 0; i < malloc_table_fill; i++)
	switch (malloc_table[i].status)
	  {
	  case invalid:
	    /* found and invalid cell, keep it for later
	       storage */
	    idx = i;
	    break;
	  case malloced:
	    break;
	  }

      if (idx == -1)
	{
	  /* we haven't found an invalid entry to reuse,
	     so the filled table is full; register the fact,
	     and use grow the table */

	  filled_table_full = 1;

	  if (malloc_table_fill < OPS_LIMIT)
	    {
	      idx = malloc_table_fill;
	      malloc_table_fill++;
	    }
	  else
	    {
	      smem_show_error();
	      return;
	    }
	}
    }

  /* register the malloc in the found entry */

  malloc_table[idx].status = malloced;
  malloc_table[idx].p      =  p;
}



static void
register_check_free(char *p, const char *filename, int line)
{
  unsigned int i;
  int found = 0;		/* 1 if we found the block to free */

  /* Testing & registering a free. */

  for (i = 0; i < malloc_table_fill; i++)
    switch (malloc_table[i].status)
      {
      case invalid:
	break;
      case malloced:
	if (malloc_table[i].p == p)
	  {
	    /* Block to free found */
	    malloc_table[i].status = invalid;
	    found = 1;

	    if (i == (malloc_table_fill - 1))
	      malloc_table_fill--;
	    else
	      filled_table_full = 0;
	  }
	break;
      }

  if (! found)
    {
      /* ERROR: Freeing  a not malloced block  */

      fprintf(stderr, "In file %s line %d: freeing not malloc'ed memory !!!\n", filename, line);

#ifdef TRAP_ON_ERROR
      fprintf(stderr, "Trapping !!!!\n");
      fflush(stderr);

      {
	volatile char c;
	c = *((volatile char *) 0);
      }
#endif
    }
}



char *fts_safe_malloc(unsigned long size, const char *filename, int line)
{
  char *p;

  /* align the size */

  if (size % sizeof(unsigned long))
    size = ((size / sizeof(unsigned long)) + 1) * sizeof(unsigned long);

  p = (char *)malloc(size + sizeof(struct mem_header) + sizeof(struct mem_trailer));

  ((struct mem_header *)p)->marker   = SMEM_START_MARKER;
  ((struct mem_header *)p)->marker2  = SMEM_START_MARKER;
  ((struct mem_header *)p)->size     = size;
  ((struct mem_header *)p)->filename = filename;
  ((struct mem_header *)p)->line     = line;
  ((struct mem_trailer *)(p + sizeof(struct mem_header) + size))->marker = SMEM_END_MARKER;

  add_block(p);

  register_check_malloc(p, size + sizeof(struct mem_header) + sizeof(struct mem_trailer));

#ifdef LOG_CALLS
  fprintf(stderr, "Malloc %lx at %s:%d\n", (unsigned long) p + sizeof(struct mem_header), filename, line);
#endif

  return p + sizeof(struct mem_header);
}


void fts_safe_free(void *pv, const char *filename, int line)
{
  char *p = (char *)pv;

#ifdef LOG_CALLS
  fprintf(stderr, "Free %lx at %s:%d\n", (unsigned long) p, filename, line);
#endif

  fts_check_block((char *)p  - sizeof(struct mem_header), "freeing block");
  remove_block(p - sizeof(struct mem_header));
  register_check_free(p - sizeof(struct mem_header), filename, line);
  free(p - sizeof(struct mem_header));
}


char *fts_safe_realloc(void *pv, int size, const char *filename, int line)
{
  char *p = (char *)pv;

  fts_check_block((char *)p  - sizeof(struct mem_header), "reallocating block");

  /* align the size */

  if (size % sizeof(unsigned long))
    size = ((size / sizeof(unsigned long)) + 1) * sizeof(unsigned long);

  register_check_free(p - sizeof(struct mem_header), filename, line); 
  remove_block(p - sizeof(struct mem_header));

  p = (char *)realloc(p - sizeof(struct mem_header),
		      size + sizeof(struct mem_header) + sizeof(struct mem_trailer));

  ((struct mem_header *)p)->marker = SMEM_START_MARKER;
  ((struct mem_header *)p)->marker2 = SMEM_START_MARKER;
  ((struct mem_header *)p)->size   = size;
  ((struct mem_header *)p)->filename = filename;
  ((struct mem_header *)p)->line     = line;
  ((struct mem_trailer *)(p + sizeof(struct mem_header) + size))->marker = SMEM_END_MARKER;

  add_block(p);

  register_check_malloc(p, size + sizeof(struct mem_header) + sizeof(struct mem_trailer));

#ifdef LOG_CALLS
  fprintf(stderr, "Realloc %lx at %s:%d\n", (unsigned long) p + sizeof(struct mem_header), filename, line);
#endif

  return p + sizeof(struct mem_header);
}


