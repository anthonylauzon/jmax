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

#ifndef _ATOMFILES_H_
#define _ATOMFILES_H_

struct _fts_atom_file_t_;
typedef struct _fts_atom_file_t_ fts_atom_file_t;

extern fts_atom_file_t *fts_atom_file_open(const char *name, const char *mode);
extern void fts_atom_file_close(fts_atom_file_t *f);

extern int fts_atom_file_read(fts_atom_file_t *f, fts_atom_t *at, char *separator);
extern int fts_atom_file_write(fts_atom_file_t *f, const fts_atom_t *at, char separator);

#endif
