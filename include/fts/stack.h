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
 */

#ifndef _FTS_STACK_H_
#define _FTS_STACK_H_

/**
 * Stack
 *
 * The FTS stack is a growable array of elements, each elements being of the same 
 * size.
 *
 * Elements are added on top of the stack.
 *
 * @defgroup stack stack
 */

/**
 * @name The FTS stack structure
 */
/*@{*/

/**
 * The FTS stack.
 *
 * @typedef fts_stack_t
 *
 * @ingroup stack
 */

typedef struct _fts_stack_t {
  void *buffer;
  int top;
  int alloc;
  int element_size;
} fts_stack_t;

/*@}*/

/**
 * Initializes a stack<BR>
 * This is a C macro that is called with the C type of the stack element
 * as second argument, as in <CODE>fts_stack_init( &buff1, double)</CODE>
 *
 * @fn void fts_stack_init( b, t);
 * @param b the stack
 * @param t a C type that is the type of the element
 * @ingroup stack
 */
#define fts_stack_init(B,T) __fts_stack_init(B, sizeof(T))

/* This function is not to be called directly */
FTS_API void __fts_stack_init( fts_stack_t *b, int element_size);

/**
 * Deinitializes a stack
 *
 * @fn void fts_stack_destroy( fts_stack_t *b);
 * @param b the stack
 * @ingroup stack
 */
FTS_API void fts_stack_destroy( fts_stack_t *stack);

/**
 * Clears the content of the stack
 *
 * @fn void fts_stack_clear( fts_stack_t *b);
 * @param b the stack
 * @ingroup stack
 */
FTS_API void fts_stack_clear( fts_stack_t *b);

/**
 * Push an element on the stack.<BR>
 * This is a C macro that is called with the C type of the stack element
 * as second argument, as in <CODE>fts_stack_push( &buff1, char, '\0')</CODE>
 *
 * @fn void fts_stack_push( b, t, v)
 * @param b the stack
 * @param t a C type that is the type of the element
 * @param v the value to push
 * @ingroup stack
 */
#define fts_stack_push(B,T,V) (((B)->top+1 >= (B)->alloc) ? __fts_stack_realloc((B)) : 0, ((T*)(B)->buffer)[(B)->top++] = (V))

/* This function is not to be called directly */
FTS_API int __fts_stack_realloc( fts_stack_t *b);

/**
 * Get the content of a stack
 * 
 * @fn void *fts_stack_get_base( b)
 * @param b the stack
 * @return a pointer to the current content of the stack
 * @ingroup stack
 */
#define fts_stack_get_base(B) ((B)->buffer)

/**
 * Get the number of elements in a stack
 * 
 * @fn int fts_stack_get_top( b)
 * @param b the stack
 * @return the number of elements
 * @ingroup array
 */
#define fts_stack_get_top(B) ((B)->top)

#endif
