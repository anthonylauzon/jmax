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

#include <stdarg.h>

/**
 *
 * Printing and logging functions
 *
 *
 * @defgroup post post
 */

#if 0
/**
 * Print a message on a bytestream, a la printf.
 * This function take a va_list argument.
 *
 * This function do not call the va_end macro. Consequently,  the
 * value of  ap is undefined after the call. The application
 * should call va_end(ap) itself afterwards, as in:
 *
 * <pre>
  va_list ap;

  va_start( ap, format);
  fts_vspost( stream, format, ap);
  va_end( ap);
 * </pre>
 *
 * @fn void fts_vspost( fts_bytestream_t *stream, const char *format, va_list ap)
 * @param stream the bytestream on which the chars will be printed
 * @param format the format string (see printf)
 * @param ap the arguments
 * @ingroup post
 */
FTS_API void fts_vspost( fts_bytestream_t *stream, const char *format, va_list ap);
#endif

/**
 * Print a message on a bytestream, a la printf.
 * This function accepts a variable number of arguments
 *
 * @fn void fts_spost( fts_bytestream_t *stream, const char *format, ...)
 * @param stream the bytestream on which the chars will be printed
 * @param format the format string (see printf)
 * @ingroup post
 */
FTS_API void fts_spost( fts_bytestream_t *stream, const char *format, ...);

/**
 * Print an array of atoms on a bytestream
 *
 * @fn void fts_spost_atoms( fts_bytestream_t *stream, int ac, const fts_atom_t *at)
 * @param stream the bytestream on which the chars will be printed
 * @param ac the atoms count
 * @param at atoms to print
 * @ingroup post
 */
FTS_API void fts_spost_atoms( fts_bytestream_t *stream, int ac, const fts_atom_t *at);

/**
 * Print a log message.
 * Log messages are printed in a file, located in user's home directory
 * and named:
 * <ul>
 *   <li> .fts_log on Unix systems </li> 
 *   <li> fts_log.txt on Windows </li> 
 * </ul>
 *
 * @fn void fts_log( const char *format, ...)
 * @param format the format string (see printf)
 * @ingroup post
 */
FTS_API void fts_log( char *format, ...);

/**
 * Print an array of atoms as a log message.
 *
 * @fn void fts_log_atoms( int ac, const fts_atom_t *at)
 * @param ac the atoms count
 * @param at atoms to print
 * @ingroup post
 * @see fts_log
 */
FTS_API void fts_log_atoms( int ac, const fts_atom_t *at);

/*
 * Compatibility ???
 */
FTS_API void post( const char *format, ...);
FTS_API void post_atoms( int ac, const fts_atom_t *at);


