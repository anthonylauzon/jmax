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

#define OPS_LIMIT 256*1024
#define BLOCK_CACHE_SIZE 16

#include <stdio.h>		/* HW specific file, can include stdio.h */
#ifdef SGI
#include <malloc.h>
#endif

#define SMEM_START_MARKER 0xaa55aa55
#define SMEM_END_MARKER   0xaa5555aa


struct mem_header
{
  unsigned long marker;
  const char *filename;
  int   line;
  unsigned long size;
  struct mem_header *next;
  unsigned long marker2;
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


static void
check_integrity(char *p, const char *msg)
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

  fprintf(stderr, "%s %s\nOriginally allocated file %s line %d\n", err, msg, 
	  ((struct mem_header *)p)->filename,  ((struct mem_header *)p)->line);

  fprintf(stderr, " - Trapping !!!!\n");
  fflush(stderr);

  {
    volatile char c;
    c = *((volatile char *) 0);
  }
}

void
check_integrity_all(void)
{
  struct mem_header *p;

  for (p = first_block; p; p = p->next)
    check_integrity((char *)p, "Checking global integrity");
}



typedef enum { malloced, freed, invalid} mem_status_t;

static  struct
{
  mem_status_t status;
  char *p;
  unsigned long size;
} malloc_table[OPS_LIMIT];

static  struct
{
  char *p;
  unsigned long size;
} block_cache[BLOCK_CACHE_SIZE];

static unsigned int malloc_table_fill = 0;
static unsigned int block_cache_fill = 0;
static unsigned int block_cache_insert = 0;

static int 
blocks_overlap(char *p1, unsigned long size1, char *p2, unsigned long size2)
{
  return  ((p1 <= p2 && p2 <= p1 + size1) ||
	   (p2 <= p1 && p1 <= p2 + size2));
}

static int 
pointer_included(char *p1, char *p2, unsigned long size2)
{
  return  (p2 <= p1 && p1 <= p2 + size2);
}

static int 
block_included(char *p1, unsigned long size1, char *p2, unsigned long size2)
{
  return  ((p2 <= p1 && p1 <= p2 + size2) &&
	   (p2 <= p1 + size1 && p1 + size1 <= p2 + size2));
}

static void
register_check_malloc(char *p, unsigned long size)
{
  unsigned int i;
  int idx = -1;			/* index where to insert the new entry */
  static int warning_given = 0; /* malloc table full warning given */

  /* Testing & registering a malloc.

     1- put to invalid all the free blocks 
        with a not null intersection.
     2- signal an inconsistency when overlap
        with a already malloced block.
     3- add the entry to the table
     */

  for (i = 0; i < malloc_table_fill; i++)
    switch (malloc_table[i].status)
      {
      case invalid:
	/* found and invalid cell, keep it for later
	   storage */
	idx = i;
	break;
      case malloced:
	if (blocks_overlap(malloc_table[i].p, malloc_table[i].size, p, size))
	  {
	    /* TWICE MALLOC'ed ERROR !!!! */
	    fprintf(stderr, "Malloc'ed block overlap with already malloc'ed memory !!!\n");
	    fprintf(stderr, "Trapping !!!!\n");
	    fflush(stderr);

	    {
	      volatile char c;
	      c = *((volatile char *) 0);
	    }
	  }
	break;
      case freed:
	if (blocks_overlap(malloc_table[i].p, malloc_table[i].size, p, size))
	  {
	    malloc_table[i].status = invalid;
	    idx = i;
	  }
	break;
      }

  if (idx == -1)
    {
      /* we haven't found an invalid entry to reuse,
	 try to grow the (static) table by incrementing
	 the fill pointer */
  
      if (malloc_table_fill < OPS_LIMIT)
	{
	  idx = malloc_table_fill;
	  malloc_table_fill++;
	}
      else
	{
	  /* No more room on the table, give the warning
	     and don't register the malloc */
	  
	  if (! warning_given)
	    {
	      fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	      fprintf(stderr, "!!! SMEM LOOSING INFORMATION, GROW SMEM OPS_LIMIT FACTOR !!!!\n");
	      fprintf(stderr, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

	      warning_given = 1;
	    }

	  return;
	}
    }

  /* register the malloc in the found entry */

  malloc_table[idx].status = malloced;
  malloc_table[idx].p      =  p;
  malloc_table[idx].size   = size;

  /* Invalid the block cache */

  block_cache_fill = 0;
  block_cache_insert = 0;
}



static void
register_check_free(char *p)
{
  unsigned int i;
  int found = 0;		/* 1 if we found the block to free */

  /* Testing & registering a free.

     1- look for exactly the same block in malloc status,
        and change to freed.
     2- Verifiy that there are no other malloc area partially
        overlapped with the freed pointer.
     3- Verifiy that there are no other freed overlapped
        with the freed pointer.
     4- If does not found the malloced block, give an error !
     */

  for (i = 0; i < malloc_table_fill; i++)
    switch (malloc_table[i].status)
      {
      case invalid:
	break;
      case malloced:
	if (malloc_table[i].p == p)
	  {
	    /* Block to free found */
	    malloc_table[i].status = freed;
	    found = 1;
	  }
	else if (pointer_included(p, malloc_table[i].p, malloc_table[i].size))
	  {
	    /* ERROR: Freeing a partial malloc block */
	    fprintf(stderr, "Freeing a part of a Malloc'ed !!!\n");
	    fprintf(stderr, "Trapping !!!!\n");
	    fflush(stderr);

	    {
	      volatile char c;
	      c = *((volatile char *) 0);
	    }
	  }
	break;
      case freed:
	if (pointer_included(p, malloc_table[i].p, malloc_table[i].size))
	  {
	    /* ERROR: Freeing  a already freed block !!! */

	    fprintf(stderr, "Freeing freed memory !!!\n");
	    fprintf(stderr, "Trapping !!!!\n");
	    fflush(stderr);

	    {
	      volatile char c;
	      c = *((volatile char *) 0);
	    }
	  }
	break;
      }

  if (! found)
    {
      /* ERROR: Freeing  a not malloced block  */

      fprintf(stderr, "Freeing not malloc'ed !!!\n");
      fprintf(stderr, "Trapping !!!!\n");
      fflush(stderr);

      {
	volatile char c;
	c = *((volatile char *) 0);
      }
    }

  /* Invalid the block cache */

  block_cache_fill = 0;
  block_cache_insert = 0;
}

void
fts_check_block(char *p, unsigned long size, const char *msg)
{
  unsigned int i;
  int found = 0;		/* 1 when we found an including malloced block */

  /* Looking in the block test cache */

  for (i = 0; i <  block_cache_fill; i++)
    if (block_cache[i].p == p && block_cache[i].size == size)
      return;

  /* Testing a in use memory block

     1- check if it overlap with a freed block.
     2- check if it exists a malloced block that include the check block.
     3- to give more info, also check if there is a malloced block overlapping,
        but not included, with the block.
     */

  for (i = 0; i < malloc_table_fill; i++)
    switch (malloc_table[i].status)
      {
      case invalid:
	break;
      case malloced:
	if (block_included(p, size, malloc_table[i].p, malloc_table[i].size))
	  found = 1;
	else if (blocks_overlap(malloc_table[i].p, malloc_table[i].size, p, size))
	  {
	    /* bad overlap ERROR !!!! */
	    fprintf(stderr, "In use block %s overlap with, but is not included in, a malloced block !!!\n", msg);
	    fprintf(stderr, "Trapping !!!!\n");
	    fflush(stderr);

	    {
	      volatile char c;
	      c = *((volatile char *) 0);
	    }
	  }
	break;
      case freed:
	if (blocks_overlap(malloc_table[i].p, malloc_table[i].size, p, size))
	  {
	    /* overlap with free ERROR !!!! */
	    fprintf(stderr, "In use block %s overlap with a freed block !!!\n", msg);
	    fprintf(stderr, "Trapping !!!!\n");
	    fflush(stderr);

	    {
	      volatile char c;
	      c = *((volatile char *) 0);
	    }
	  }
	break;
      }

  if (! found)
    {
      /* ERROR: Using  a not malloced block  */

      fprintf(stderr, "In use block %s not malloc'ed !!!\n", msg);
      fprintf(stderr, "Trapping !!!!\n");
      fflush(stderr);

      {
	volatile char c;
	c = *((volatile char *) 0);
      }
    }

  /* Add the block to the cache */

  if (block_cache_fill < BLOCK_CACHE_SIZE)
    {
      block_cache[block_cache_fill].p = p;
      block_cache[block_cache_fill].size = size;
      block_cache_fill++;
    }
  else
    {
      block_cache[block_cache_insert].p = p;
      block_cache[block_cache_insert].size = size;
      block_cache_insert = (block_cache_insert + 1) % BLOCK_CACHE_SIZE;
    }

}


void
fts_check_pointer(void *p, char *msg)
{
  /* check_integrity_all(); */
  fts_check_block(((char *)p) , 1, msg);
}


/* Call this on a malloced block only !! */

void
fts_check_malloc(void *p, char *msg)
{
  if (p)
    {
      check_integrity((char *)p  - sizeof(struct mem_header), msg);
      fts_check_block(((char *)p) - sizeof(struct mem_header), 1, msg);
    }

  /* check_integrity_all(); */
}


char *
fts_safe_malloc(unsigned long size, const char *filename, int line)
{
  char *p;

  /* check_integrity_all(); */

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

  /* check_integrity_all(); */

  return p + sizeof(struct mem_header);
}

void
fts_safe_free(char *p)
{
  /* check_integrity_all(); */

  remove_block(p - sizeof(struct mem_header));
  register_check_free(p - sizeof(struct mem_header));
  free(p - sizeof(struct mem_header));

  /* check_integrity_all(); */
}



char *
fts_safe_realloc(void *pv, int size, const char *filename, int line)
{
  char *p = (char *)pv;

  /* check_integrity_all(); */

  /* align the size */

  if (size % sizeof(unsigned long))
    size = ((size / sizeof(unsigned long)) + 1) * sizeof(unsigned long);

  register_check_free(p - sizeof(struct mem_header)); 
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

  /* check_integrity_all(); */


  return p + sizeof(struct mem_header);
}


