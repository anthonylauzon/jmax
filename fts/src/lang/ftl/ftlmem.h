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

/* 

   New interface, compatible with a multi-thread pipelined implementation
   of DSP.
   It is completely structure oriented, you cannot require a block of
   data without defining a type.

typedef ..... ftl_data_t;

   macros:

extern void *ftl_data_get_ptr(ftl_data_t handle)
 
extern ftl_data_t ftl_data_new(TYPE);
extern void ftl_data_copy(TYPE, PTR, void *src)
extern void ftl_data_recopy(TYPE, PTR, void *src)
extern void ftl_data_set(TYPE, PTR, FIELD, ptr_to_value)
extern void ftl_data_get(TYPE, PTR, FIELD, ptr_to_value)

Don't use this one, is for system use:

extern ftl_data_t ftl_data_free_all();

It also define a fts_set_ftl_data on atoms to pass it to the
ftl assembler.

*/

struct ftl_data_handle
{
  void *ptr;			/* pointer to the real memory */
  enum {ftl_handle_free, ftl_handle_copied, ftl_handle_allocated} state;
  int   size;			/* object size */
};

typedef struct ftl_data_handle *ftl_data_t;

extern ftl_data_t ftl_data_alloc(unsigned int size);
extern void ftl_data_free(ftl_data_t obj);

#define ftl_data_get_ptr(OBJ) (OBJ)->ptr

#define ftl_data_new(TYPE) ftl_data_alloc(sizeof(TYPE))

#define ftl_data_copy(TYPE, OBJ, SRC) \
        do \
        {         \
          if (OBJ) \
              *((TYPE *) ((OBJ)->ptr)) = *((TYPE *) (SRC)); \
         } while(0)

#define ftl_data_recopy(TYPE, OBJ, SRC) \
        do \
        {         \
          if (OBJ) \
              *((TYPE *) (SRC)) = *((TYPE *) ((OBJ)->ptr)); \
         } while(0)

#define ftl_data_set(TYPE, OBJ, FIELD, SRC)  \
        do \
        {         \
          if (OBJ) \
	    ( (TYPE *) ((OBJ)->ptr) )->FIELD = *(SRC);  \
         } while(0)

#define ftl_data_get(TYPE, OBJ, FIELD, DEST)  \
        do \
        {         \
          if (OBJ) \
	     *(DEST) = ( (TYPE *) ((OBJ)->ptr) )->FIELD;  \
         } while(0)


/* this macro is to simulate a specialized "atom set" function
to cope with the type ftl_data_name_t, that are object names
but are more than symbols.
*/


#define fts_set_ftl_data(A, H)   fts_set_ptr((A), ftl_data_relocate_and_get(H))


/* Private definition for the dsp module: don't use */

extern void ftl_mem_start_memory_relocation(void);
extern void ftl_mem_end_memory_relocation(void);
extern void *ftl_data_relocate_and_get(struct ftl_data_handle *h);
