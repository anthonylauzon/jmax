/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _FTS_PRIVATE_OUTLET_H_
#define _FTS_PRIVATE_OUTLET_H_

/* The object stack; used for fpe handling, debug and who know what else in the future */
#define DO_OBJECT_STACK

#ifdef DO_OBJECT_STACK

#define FTS_OBJSTACK_SIZE 8*1024
#define FTS_CALL_DEPTH 16*1024

extern int fts_objstack_top;
extern fts_object_t *fts_objstack[FTS_OBJSTACK_SIZE];

#define FTS_REACHED_MAX_CALL_DEPTH() (fts_objstack_top >= FTS_CALL_DEPTH) 

#define FTS_OBJSTACK_PUSH(obj) { \
  if (fts_objstack_top < FTS_OBJSTACK_SIZE)  \
    fts_objstack[fts_objstack_top++] = (obj); \
      else \
        fts_objstack_top++; \
}

#define FTS_OBJSTACK_POP(obj) (fts_objstack_top--)

#define fts_get_current_object() (\
  ((fts_objstack_top > 0) && (fts_objstack_top <= FTS_OBJSTACK_SIZE)) ? \
  fts_objstack[fts_objstack_top - 1] : \
  (fts_object_t *)0)

#else

#define FTS_REACHED_MAX_CALL_DEPTH() (0) 
#define FTS_OBJSTACK_PUSH(obj)
#define FTS_OBJSTACK_POP(obj)
#define fts_get_current_object() (0)

#endif

#endif
