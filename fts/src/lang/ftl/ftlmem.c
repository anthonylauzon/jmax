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


/* Memory handling for object state
   New implementation: FTL objects are persistent w.r.t. dac start/stop,
   i.e. as persistent as a standard fts object; in this way, they can
   be created by the init method of a DSP object, and filled in the
   standard way when the data is available; this simplify a lot
   object coding and avoid duplicating the ftl obj inside the
   object structure.
 */

#include <string.h>
#include <fts/sys.h>
#include <fts/lang/mess.h>
#include <fts/lang/ftl.h>

/* 
   Handle table for FTL objects
 */

#define FTL_HANDLE_PER_PAGE  256


static struct ftl_data_handle_page
{
  struct ftl_data_handle handles[FTL_HANDLE_PER_PAGE];
  struct ftl_data_handle_page *next_handle_page;
} *ftl_data_handle_page_list = 0;


/* Memory heap static data */

#define FTL_MEM_BLOCK_SIZE (8192 * 4)
#define FTL_MEM_TOO_FAR    64

struct ftl_mem_block
{
  char mem[FTL_MEM_BLOCK_SIZE];
  unsigned int last_alignement_hole;
  unsigned int last_alignement_hole_size; /* must be initted to 0 */
  unsigned int next_free;
  
  struct ftl_mem_block *next_block;
};

/* forward declarations */

static struct ftl_mem_block *ftl_mem_new_block(void);
static  void ftl_mem_free_block(struct ftl_mem_block *p);
static void *ftl_mem_allocate(unsigned int size, struct ftl_mem_block **hp);
static int ftl_mem_ptr_in_heap(void *ptr, struct ftl_mem_block *p);

static int ftl_mem_is_relocating = 0;

/* the heap */

static struct ftl_mem_block *ftl_mem_block_list   = 0;


/* functions handling handles */

/* Handles tables are never freed; a free handle is marked by a null
   pointer in its ptr field.
  */

static struct ftl_data_handle *
ftl_get_new_handle(void)
{
  struct ftl_data_handle_page *p;
  int i;

  p = ftl_data_handle_page_list;

  while (p)
    {
      /* look in the current page */

      for (i = 0; i < FTL_HANDLE_PER_PAGE; i++)
	if (p->handles[i].state == ftl_handle_free)
	  return &(p->handles[i]);

      /* not found, look in the next one */

      p = p->next_handle_page;
    }

  /* handle not found: allocate a new*/

  p = (struct ftl_data_handle_page *) fts_malloc(sizeof(struct ftl_data_handle_page));

  for (i = 0; i < FTL_HANDLE_PER_PAGE; i++)
    p->handles[i].state = ftl_handle_free;

  p->next_handle_page = ftl_data_handle_page_list;
  ftl_data_handle_page_list = p;

  return &p->handles[0];
}


/* reset all the non free handle to the allocated state,
   after a copy
*/

static void
ftl_reset_handle_table(void)
{
  struct ftl_data_handle_page *p;
  int i;

  p = ftl_data_handle_page_list;

  while (p)
    {
      /* look in the current page */

      for (i = 0; i < FTL_HANDLE_PER_PAGE; i++)
	if (p->handles[i].state != ftl_handle_free)
	  p->handles[i].state = ftl_handle_allocated;

      /* not found, look in the next one */

      p = p->next_handle_page;
    }
}




/* User level functions */

ftl_data_t
ftl_data_alloc(unsigned int size)
{
  ftl_data_t h;

  /* First, get a new handle, and initialize it */

  h = ftl_get_new_handle();

  h->size = size;		/* store the original, unaligned memory size here !!*/

  /* Second, allocate memory, from the correct heap
     
   */

  h->ptr = ftl_mem_allocate(size, &ftl_mem_block_list);

  if (ftl_mem_is_relocating == 0)
    h->state = ftl_handle_allocated;
  else
    h->state = ftl_handle_copied;

  return h;
}


void
ftl_data_free(ftl_data_t obj)
{
  struct ftl_data_handle *p = obj;

  p->state = ftl_handle_free;
}

/* DSP code generation and memory reorganization interface  

   When the DSP compiler is generating code, we switch heap,
   and every time a fts_put_ftl_data is called (that become
   a function), we relocate the object in the calling order inside
   the new heap (trying to compacting alignement holes, if possible),
   and put in the atom list the pointer to the new heap.


   To simplify coding, the relocation is made by reallocation and copy.
   This also guarantee a smooth integration with existing code that
   allocate the ftl object inside the put.

   When the DSP code is completed, the old heap is freed; this guarantee
   to free automatically the memory allocated by Objects that have
   been freed.
*/

/* an 0 value in the old list guarantee that there is no code generation
   process going on
*/

static struct ftl_mem_block *ftl_mem_old_block_list = 0;


void
ftl_mem_start_memory_relocation(void)
{
  /* move the heap aside, and prepare reallocating and
     copying all the objects
     */

  ftl_mem_old_block_list = ftl_mem_block_list;
  ftl_mem_block_list = 0;

  ftl_mem_is_relocating = 1;
}


void
ftl_mem_end_memory_relocation(void)
{
  struct ftl_mem_block *p;


  /* relocate all the active handles that
     have not been relocated yet
     */

  {
    struct ftl_data_handle_page *p;
    int i;

    p = ftl_data_handle_page_list;

    while (p)
      {
	for (i = 0; i < FTL_HANDLE_PER_PAGE; i++)
	  if (p->handles[i].state == ftl_handle_allocated)
	    {
	      /* if the pointer is in the old heap, relocate */
	      void *mp;

	      mp = ftl_mem_allocate(p->handles[i].size, &ftl_mem_block_list);
	      memcpy(mp, p->handles[i].ptr, p->handles[i].size);
	      p->handles[i].ptr = mp;
	      p->handles[i].state = ftl_handle_copied;
	    }

	p = p->next_handle_page;
      }
  }

  /* stop the relocation, now */

  ftl_mem_is_relocating = 0;

  /* free the old heap (this act also as a garbage collector flip) */

  while (ftl_mem_old_block_list)
    {
      p = ftl_mem_old_block_list;
      ftl_mem_old_block_list = ftl_mem_old_block_list->next_block;

      ftl_mem_free_block(p);
    }

  /* reset the handle status */

  ftl_reset_handle_table();

  /* print statistics (TMP) */

#ifdef FTL_MEM_DEBUG
  {
    int total_size = 0;
    int blocks = 0;

    struct ftl_mem_block *p;

    p = ftl_mem_block_list;

    while(p)
      {
	total_size += p->next_free;
	blocks++;
	p = p->next_block;
      }

    fprintf(stderr, "FTL MEM allocated %d bytes, in %d blocks\n", total_size, blocks);
  }
#endif
}


void *
ftl_data_relocate_and_get(struct ftl_data_handle *h)
{
  if (ftl_mem_is_relocating == 0)
    {
      /* if no generation is going on, just return the handle pointer (??ERROR??) */
      
      return h->ptr;
    }
  else if (h->state == ftl_handle_allocated)
    {
      /* if the pointer is in the old heap, relocate */
      void *p;

      p = ftl_mem_allocate(h->size, &ftl_mem_block_list);
      memcpy(p, h->ptr, h->size);
      h->ptr = p;
      h->state = ftl_handle_copied;
      return p;
    }
  else
    {
      /* else, the pointer is already in the new heap, keep it */

      return h->ptr;
    }
}


/* Low level memory allocation.

   The new heap try to automatically compacting alignement 
   holes; the idea is that the heap keep track of the last
   alignement hole, and if the request match its size, and
   the hole is not too far from the current pointer, the
   hole is used for the new pointer. "too far" means not farer
   than a constant, that reasonably guarantee that the cache
   line is still loaded or the same as the current cache line.

   This politics guarantee, with the current object set,
   (alignement to double and float), that all the holes are
   filled if needed not "too far" from their position.

   The heap keep a free list of memory pages, so reallocating
   a page is not too expensive.
  */

/* new and free have a free list */

static struct ftl_mem_block *ftl_mem_free_block_list = 0;

static struct ftl_mem_block *
ftl_mem_new_block(void)
{
  struct ftl_mem_block *p;

  if (ftl_mem_free_block_list)
    {
      p = ftl_mem_free_block_list;
      ftl_mem_free_block_list = p->next_block;
    }
  else
    p = fts_malloc(sizeof(struct ftl_mem_block));

  p->last_alignement_hole_size = 0;
  p->next_free = 0;
  p->next_block = 0;

  return p;
}


static  void
ftl_mem_free_block(struct ftl_mem_block *p)
{
  p->next_block = ftl_mem_free_block_list;
  ftl_mem_free_block_list = p;
}
 
/* this function is shitty; but supported only  for compatibility
   with existing code, will be dropped soon.
*/

static int
ftl_mem_ptr_in_heap(void *ptr, struct ftl_mem_block *p)
{
  while (p)
    {
      /* look in the current page */

      if ((((unsigned long) &(p->mem[0])) <= (unsigned long) ptr) && 
	  ((unsigned long) ptr < ((unsigned long) &(p->mem[FTL_MEM_BLOCK_SIZE]))))
	return 1;

      p = p->next_block;
    }

  return 0;
}
    
static void *
ftl_mem_allocate_aligned(int size, struct ftl_mem_block *p)
{
  /* allocate size bytes in the current block (there is room),
     keeping alignement, and filling alignement hole info if needed.
    */

  char *dp;
  
  /* if size == sizeof(float), do not check for alignement */

  if (size == sizeof(float))
    {
      dp = &(p->mem[p->next_free]);
      p->next_free += size;
    }
  else
    {

      /* Compute the data pointer, and check if it aligned.
	 The alignement is done at sizeof(double)
	 Any alignement hole not yet used is lost.
	 I.e. the algorithm consider only one pending alignement hole.
	 */

      dp = &(p->mem[p->next_free]);

      if (((unsigned long) dp) % sizeof(double))
	{
	  p->last_alignement_hole_size = sizeof(double) - ((unsigned long) dp) % sizeof(double);
	  p->last_alignement_hole = p->next_free;
	  dp += p->last_alignement_hole_size;
	  p->next_free += (size + p->last_alignement_hole_size);
	}
      else
	p->next_free += size;
    }

  return dp;
}


static void *
ftl_mem_allocate(unsigned int size, struct ftl_mem_block **hp)
{
  unsigned int new_size;
  struct ftl_mem_block *p = *hp;

  if (size > FTL_MEM_BLOCK_SIZE)
    return 0;			/* block too big, should give an error here  */

  /* first, if the size is not multiple of sizeof(float),
     we realign it; we don't handle char sized alignement holes
     that shouldn't exists, anyway.
   */

  if (size % sizeof(float))
    new_size = size + sizeof(float) - size % sizeof(float);
  else
    new_size = size;

  /* blocks are filled and pushed in the list; i.e. only 
     the first block is guaranteed to have memory free,
     the others are all filled up.

   */

  /* if the heap is already allocated, we look at the first block */

  if (p)
    {
      /* First, check if there is a not too far alignement hole 
	 that we can resuse
	 */

      if ((p->last_alignement_hole_size == new_size) &&
	  ((p->next_free - p->last_alignement_hole_size) <= FTL_MEM_TOO_FAR))
	{
	  /* yes, use the alignement hole */

	  p->last_alignement_hole_size = 0;

	  return &(p->mem[p->last_alignement_hole]);
	}


      /* Second, check if there is memory available at the end of the current
	 block; update the last alignement hole data if needed
	 */

      if ((FTL_MEM_BLOCK_SIZE - p->next_free) > new_size)
	{
	  /* yes, allocate memory in the current block;
	     use a subfunction */

	  return ftl_mem_allocate_aligned(new_size, p);

	}
    }

  /* no heap, or not enough memory available, allocate a new block, and use its memory */

  p = ftl_mem_new_block();
  p->next_block = *hp;
  *hp = p;

  return ftl_mem_allocate_aligned(new_size, p);
}
