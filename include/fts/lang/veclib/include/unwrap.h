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

/* unwrap.h */

#include <fts/lang/mess.h>

/*
 *  Unwrapping of arguments of different types for FTL callable functions
 */

#define UNWRAP_VEC(idx, t_vec, vec)\
 t_vec *vec = (t_vec *)fts_word_get_ptr(argv + idx);

#define UNWRAP_PTR(idx, t_ptr, ptr)\
 t_ptr *ptr = (t_ptr *)fts_word_get_ptr(argv + idx);

#define UNWRAP_SCL(idx, t_scl, scl)\
 t_scl scl = *((t_scl*)fts_word_get_ptr(argv + idx));

#define UNWRAP_SIZE(idx)\
 int size = fts_word_get_int(argv + idx);

#define FTL_ARG fts_word_t *argv
