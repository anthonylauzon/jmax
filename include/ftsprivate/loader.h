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

#ifndef _FTS_PRIVATE_LOADER_H_
#define _FTS_PRIVATE_LOADER_H_

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




