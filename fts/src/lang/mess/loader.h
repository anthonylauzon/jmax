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

#ifndef _LOADER_H_
#define _LOADER_H_

/*
  A FTS binary file is made of the following:
  . a header:
    . magic number: 'bMax'
    . code size
    . number of symbols
  . code
  . symbol table
*/

typedef struct fts_binary_file_header_t {
  unsigned long magic_number;
  unsigned long code_size;
  unsigned long n_symbols;
} fts_binary_file_header_t;

/* 'bMax' */
#define FTS_BINARY_FILE_MAGIC 0x624D6178


/* Function: fts_binary_file_load
   Args:
      const char *name : file name
   Return value: int
     -1: an error occured
     other: loaded OK
   Description: loads and run a FTS binary file.
*/

extern fts_object_t *fts_binary_file_load(const char *name,
					  fts_object_t *parent,
					  int ac, const fts_atom_t *at,
					  fts_expression_state_t *e);

extern fts_object_t *fts_binary_filedesc_load(FILE *f,
					      fts_object_t *parent,
					      int ac, const fts_atom_t *at,
					      fts_expression_state_t *e);

#endif




