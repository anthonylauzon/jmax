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

#ifndef _FTS_SMEM_H
#define _FTS_SMEM_H

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

#endif
