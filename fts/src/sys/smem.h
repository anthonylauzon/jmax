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

*/

#define SAFE_MEM

extern char *fts_safe_malloc(unsigned long size, const char *filename, int line);
extern void fts_safe_free(char *p);
extern char *fts_safe_realloc(void *pv, int size, const char *filename, int line);

extern void check_integrity(char *p, const char *msg);
extern void fts_check_pointer(void *p, char *msg);
extern void fts_check_malloc(void *p, char *msg);
extern void fts_check_block(char *p, unsigned long size, const char *msg);

#define real_malloc(size)   fts_safe_malloc(size, __FILE__, __LINE__)
#define real_free    fts_safe_free
#define real_realloc(pnt, size)  fts_safe_realloc(pnt, size, __FILE__, __LINE__)









