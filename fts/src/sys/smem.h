/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
#define HELP_PURIFY

extern char *fts_safe_malloc(unsigned long size, const char *filename, int line);
extern void fts_safe_free(void  *pv, const char *filename, int line);
extern char *fts_safe_realloc(void *pv, int size, const char *filename, int line);

#define real_malloc(size, filename, line)        fts_safe_malloc(size, filename, line)
#define real_free(size, filename, line)          fts_safe_free(size, filename, line)
#define real_realloc(pnt, size, filename, line)  fts_safe_realloc(pnt, size, filename, line)









