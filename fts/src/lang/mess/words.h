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

#ifndef _FTS_WORDS_H_
#define _FTS_WORDS_H_

/*
 * Macros that deals with words.
 * 
 * the fts_word_t is defined in mess_types.h
 * An fts_word_t is an union of the different basic types used in FTS.
 * It do not include type tagging (see atoms for that).
 *
 * We introduce "int" as a type name, also if the implementation
 * is done with long, to be more uniform; the old naming is kept,
 * anyway.
 */


#define fts_word_set_symbol(ap, x)      ((ap)->fts_symbol  = (x))
#define fts_word_set_string(ap, x)      ((ap)->fts_str  = (x))
#define fts_word_set_ptr(ap, x)         ((ap)->fts_ptr   = (x))
#define fts_word_set_fun(ap, x)         ((ap)->fts_fun   = (x))
#define fts_word_set_long(ap, x)        ((ap)->fts_int  = (x))
#define fts_word_set_int(ap, x)         ((ap)->fts_int  = (x))
#define fts_word_set_float(ap, x)       ((ap)->fts_float = (x))
#define fts_word_set_object(ap, x)      ((ap)->fts_obj = (x))
#define fts_word_set_connection(ap, x)  ((ap)->fts_connection = (x))
#define fts_word_set_data(ap, x)        ((ap)->fts_data = (x))

#define fts_word_get_symbol(ap)         ((ap)->fts_symbol)
#define fts_word_get_string(ap)         ((ap)->fts_str)
#define fts_word_get_ptr(ap)            ((ap)->fts_ptr)
#define fts_word_get_fun(ap)            ((ap)->fts_fun)
#define fts_word_get_long(ap)           ((ap)->fts_int)
#define fts_word_get_int(ap)            ((ap)->fts_int)
#define fts_word_get_float(ap)          ((ap)->fts_float)
#define fts_word_get_object(ap)         ((ap)->fts_obj)
#define fts_word_get_connection(ap)     ((ap)->fts_connection)
#define fts_word_get_data(ap)           ((ap)->fts_data)

#endif

