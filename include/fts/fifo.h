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


/**
 * Fifo
 *
 * The FTS fifo is a fifo of fixed size that maintains a read and a write 
 * pointer inside a buffer of elements.
 * 
 * The buffer and the buffer size are determined at initialization time.
 *
 * A fifo can be shared between 2 threads or 2 processes (via shared
 * memory) as long as there is only one reader and one write: the read 
 * (resp. write) pointer is changed only by the reader (resp. writer) process.
 *
 * The fifo structure does not provide mutexes because of this restriction.
 * If needed, the mutexes must be put around the accesses to the fifo.
 *
 * @defgroup fifo fifo
 */

/**
 * @name The FTS fifo structure
 */
/*@{*/

/**
 * The FTS fifo.
 *
 * @typedef fts_fifo_t
 *
 * @ingroup fifo
 */

typedef struct _fts_fifo_t {
  volatile int read_index;
  volatile int write_index;
  volatile void *buffer;
  int size;
} fts_fifo_t;

#define fts_fifo_get_buffer(f) ((f)->buffer)
#define fts_fifo_set_buffer(f, b) ((f)->buffer = (b))

/*@}*/

/**
 * Initializes a fifo
 *
 * @fn void fts_fifo_init( fts_fifo_t *fifo, void *buffer, int size)
 * @param fifo the fifo
 * @param buffer the buffer used for storing elements
 * @param size the buffer size
 * @ingroup fifo
 */
FTS_API void fts_fifo_init( fts_fifo_t *fifo, void *buffer, int size);

/**
 * Move fifo content to new buffer and set pointers to new buffer
 *
 * @fn void fts_fifo_reinit( fts_fifo_t *fifo, void *buffer, int size)
 * @param fifo the fifo
 * @param buffer new buffer (if NULL only resets read and write pointer to 0)
 * @param size new buffer size 
 * @warning function is NOT thread save
 * @ingroup fifo
 */
FTS_API void fts_fifo_reinit( fts_fifo_t *fifo, void *buffer, int size);

/**
 * Get the fifo read pointer
 * Note: the returned pointer may point to a non-valid element if
 * the fifo is empty
 *
 * @fn void *fts_fifo_read_pointer( fts_fifo_t *fifo)
 * @param fifo the fifo
 * @return a pointer to the first available element for reading
 * @ingroup fifo
 */
#define fts_fifo_read_pointer(F) ((void *)(((volatile char *)((F)->buffer)) + (F)->read_index))

/**
 * Get the fifo write pointer
 * Note: the returned pointer may point to a non-valid element if
 * the fifo is full
 *
 * @fn void *fts_fifo_write_pointer( fts_fifo_t *fifo)
 * @param fifo the fifo
 * @return a pointer to the first available element for writing
 * @ingroup fifo
 */
#define fts_fifo_write_pointer(F) ((void *)(((volatile char *)((F)->buffer)) + (F)->write_index))

/**
 * Get the fifo read level
 * Note: the read level is returned as a number of <strong>bytes</strong>.
 *
 * @fn int fts_fifo_read_level( fts_fifo_t *fifo)
 * @param fifo the fifo
 * @return the number of elements available for reading
 * @ingroup fifo
 */
FTS_API int fts_fifo_read_level( fts_fifo_t *fifo);

/**
 * Get the fifo write level
 * Note: the write level is returned as a number of <strong>bytes</strong>.
 *
 * @fn int fts_fifo_write_level( fts_fifo_t *fifo)
 * @param fifo the fifo
 * @return the number of elements available for writing
 * @ingroup fifo
 */
FTS_API int fts_fifo_write_level( fts_fifo_t *fifo);

/**
 * Increments the fifo read pointer
 * Note: the read increment is given as a number of <strong>bytes</strong>.
 *
 * @fn int fts_fifo_incr_read( fts_fifo_t *fifo, int incr)
 * @param fifo the fifo
 * @param incr the increment to the read pointer
 * @ingroup fifo
 */
FTS_API void fts_fifo_incr_read( fts_fifo_t *fifo, int incr);

/**
 * Increments the fifo write pointer
 * Note: the write increment is given as a number of <strong>bytes</strong>.
 *
 * @fn int fts_fifo_incr_write( fts_fifo_t *fifo, int incr)
 * @param fifo the fifo
 * @param incr the increment to the write pointer
 * @ingroup fifo
 */
FTS_API void fts_fifo_incr_write( fts_fifo_t *fifo, int incr);

