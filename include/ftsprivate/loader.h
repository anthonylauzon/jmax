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

/**
 * Loads a jMax file and return the toplevel object contained in this file.
 *
 * This function will try the different file formats supported by jMax and will load the file that is
 * supposed to contain a jMax object.
 *
 * Currently supported formats are: bmax (jMax binary format) and pat (old MAX patch file format).
 * 
 * @fn fts_status_t fts_file_load( fts_symbol_t filename, fts_object_t *parent, int ac, const fts_atom_t *at, fts_object_t **ret)
 * @param filename the name of the file
 * @param parent the parent of the object contained in file
 * @param ac argument count
 * @param at arguments
 * @param ret pointer to returned object
 * @return a fts_status_t equals to fts_ok if load succeeded or to an error description if failed.
 */
extern fts_status_t fts_file_load( fts_symbol_t filename, fts_object_t *parent, int ac, const fts_atom_t *at, fts_object_t **ret);

/**
 * Status values returned by fts_file_load()
 */
extern fts_status_t fts_invalid_file_format_error;
extern fts_status_t fts_cannot_open_file_error;

#endif
