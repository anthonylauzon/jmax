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
#define fts_stack_clear(S) ((S)->top = -1)

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
#define fts_stack_push(S,T,V) (((S)->top+1 >= (S)->alloc) ? __fts_stack_realloc((S)) : 0, ((T*)(S)->buffer)[++(S)->top] = (V))

/* This function is not to be called directly */
FTS_API int __fts_stack_realloc( fts_stack_t *b);

/**
 * Pop n elements off the stack.<BR>
 *
 * @fn void fts_stack_pop( b, n)
 * @param b the stack
 * @param n the number of elements to pop
 * @ingroup stack
 */
#define fts_stack_pop(B,N) ((B)->top -= (N))

/**
 * Get the content of a stack
 * 
 * @fn void *fts_stack_base( b)
 * @param b the stack
 * @return a pointer to the current content of the stack
 * @ingroup stack
 */
#define fts_stack_base(S) ((S)->buffer)

/**
 * Get the top of a stack
 * 
 * @fn int fts_stack_top( b)
 * @param b the stack
 * @return the stack top
 * @ingroup stack
 */
#define fts_stack_top(S) ((S)->top)

/**
 * Get the size of a stack (number of elements)
 * 
 * @fn int fts_stack_size( b)
 * @param b the stack
 * @return the stack size
 * @ingroup stack
 */
#define fts_stack_size(S) ((S)->top + 1)

#define fts_stack_init_int(s) __fts_stack_init(s, sizeof(int))
#define fts_stack_push_int(s, o) fts_stack_push(s, int, o)
#define fts_stack_top_int(s) (((s)->top >= 0)? (((int *)fts_stack_base(s))[(s)->top]): NULL)
#define fts_stack_get_int(s, i) (((i) >= 0 && (i) <= (s)->top)? (((int *)fts_stack_base(s))[(i)]): NULL)

#define fts_stack_init_object(s) __fts_stack_init(s, sizeof(fts_object_t *))
#define fts_stack_push_object(s, o) fts_stack_push(s, fts_object_t *, o)
#define fts_stack_top_object(s) (((s)->top >= 0)? (((fts_object_t **)fts_stack_base(s))[(s)->top]): NULL)
#define fts_stack_get_object(s, i) (((i) >= 0 && (i) <= (s)->top)? (((fts_object_t **)fts_stack_base(s))[(i)]): NULL)

