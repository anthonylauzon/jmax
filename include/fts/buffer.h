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

#ifndef _FTS_BUFFER_H_
#define _FTS_BUFFER_H_

/**
 * Buffer
 *
 * The FTS buffer is a growable array of elements, each elements being of the same 
 * size.
 *
 * @defgroup buffer buffer
 */

/**
 * @name The FTS buffer structure
 */
/*@{*/

/**
 * The FTS buffer.
 *
 * @typedef fts_buffer_t
 *
 * @ingroup buffer
 */

typedef struct _fts_buffer_t {
  void *buffer;
  int fill;
  int len;
  int element_size;
} fts_buffer_t;

/*@}*/

/**
 * Initializes a buffer<BR>
 * This is a C macro that is called with the C type of the buffer element
 * as second argument, as in <CODE>fts_buffer_init( &buff1, double)</CODE>
 *
 * @fn void fts_buffer_init( b, t);
 * @param b the buffer
 * @param t a C type that is the type of the element
 * @ingroup buffer
 */
#define fts_buffer_init(B,T) __fts_buffer_init(B, sizeof(T))

/* This function is not to be called directly */
FTS_API void __fts_buffer_init( fts_buffer_t *b, int element_size);

/**
 * Deinitializes a buffer
 *
 * @fn void fts_buffer_destroy( fts_buffer_t *b);
 * @param b the buffer
 * @ingroup buffer
 */
FTS_API void fts_buffer_destroy( fts_buffer_t *buffer);

/**
 * Clears the content of the buffer
 *
 * @fn void fts_buffer_clear( fts_buffer_t *b);
 * @param b the buffer
 * @ingroup buffer
 */
FTS_API void fts_buffer_clear( fts_buffer_t *b);

/**
 * Appends an element to the end of the buffer.<BR>
 * This is a C macro that is called with the C type of the buffer element
 * as second argument, as in <CODE>fts_buffer_append( &buff1, char, '\0')</CODE>
 *
 * @fn void fts_buffer_append( b, t, v)
 * @param b the buffer
 * @param t a C type that is the type of the element
 * @param v the value to append
 * @ingroup buffer
 */
#define fts_buffer_append(B,T,V) (((B)->fill+1 >= (B)->len) ? __fts_buffer_realloc((B)) : 0, ((T*)(B)->buffer)[(B)->fill++] = (V))

/* This function is not to be called directly */
FTS_API int __fts_buffer_realloc( fts_buffer_t *b);

/**
 * Get the content of a buffer
 * 
 * @fn void *fts_buffer_get_ptr( b)
 * @param b the buffer
 * @return a pointer to the current content of the buffer
 * @ingroup buffer
 */
#define fts_buffer_get_ptr(B) ((B)->buffer)

/**
 * Get the number of elements in a buffer
 * 
 * @fn int fts_buffer_get_length( b)
 * @param b the buffer
 * @return the number of elements
 * @ingroup array
 */
#define fts_buffer_get_length(B) ((B)->fill)

#endif
