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


/* 
   Functions to read and write an atom from a file; should go in parser.c,
   after a good clean up.

   fts_read_atom return 1 if it read it, and zero if the
   file is at an end.

   the file format used is a human readible format; 

   newline, carriage return, space and tabs and nulls are considered token separators

   backslash quote special caracters, including itself,

   ; , $ and ' are considered as single char atoms, unless backquoted, when they are
   always a token by themselves.

   Only int, float and symbols are read.

   Anything between " " is considered an atom name, including
   the null string; " always start an new atom , unless quoted.

   An atom should not be longer than 1024 chars.

   Atoms are read and written to/from "fts_atom_file_t" 
   
*/

typedef struct fts_atom_file fts_atom_file_t;

FTS_API fts_atom_file_t *fts_atom_file_open(const char *name, const char *mode);
FTS_API void fts_atom_file_close(fts_atom_file_t *f);

FTS_API int fts_atom_file_read(fts_atom_file_t *f, fts_atom_t *at, char *separator);
FTS_API int fts_atom_file_write(fts_atom_file_t *f, const fts_atom_t *at, char separator);

