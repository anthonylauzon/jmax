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

#ifndef _SAVER_H
#define _SAVER_H

struct fts_bmax_file;

typedef struct fts_bmax_file fts_bmax_file_t;

extern void fts_save_patcher_as_bmax(fts_symbol_t file, fts_object_t *patcher);
extern void fts_save_selection_as_bmax(FILE *file, fts_object_t *selection);
extern void fts_save_simple_as_bmax(const char *filename, fts_object_t *patcher);

/* Functions for objects data saving; not yet documented */

extern void fts_bmax_code_push_int(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_push_float(fts_bmax_file_t *f, float value);
extern void fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym);
extern void fts_bmax_code_set_int(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_set_float(fts_bmax_file_t *f, float value);
extern void fts_bmax_code_set_symbol(fts_bmax_file_t *f, fts_symbol_t sym);
extern void fts_bmax_code_pop_args(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_push_obj(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_mv_obj(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_pop_objs(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_make_obj(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_put_prop(fts_bmax_file_t *f, fts_symbol_t sym);
extern void fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs);
extern void fts_bmax_code_push_obj_table(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_pop_obj_table(fts_bmax_file_t *f);
extern void fts_bmax_code_connect(fts_bmax_file_t *f);
extern void fts_bmax_code_push_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at);

extern void fts_bmax_code_set_atom(fts_bmax_file_t *f, const fts_atom_t *a);


#endif
