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

#ifndef _FTS_SAVER_H
#define _FTS_SAVER_H

typedef struct fts_bmax_file fts_bmax_file_t;

/* Functions for objects data saving; not yet documented */

FTS_API void fts_bmax_code_push_int(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_push_float(fts_bmax_file_t *f, float value);
FTS_API void fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym);
FTS_API void fts_bmax_code_set_int(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_set_float(fts_bmax_file_t *f, float value);
FTS_API void fts_bmax_code_set_symbol(fts_bmax_file_t *f, fts_symbol_t sym);
FTS_API void fts_bmax_code_pop_args(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_push_obj(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_mv_obj(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_pop_objs(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_make_obj(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_put_prop(fts_bmax_file_t *f, fts_symbol_t sym);
FTS_API void fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs);
FTS_API void fts_bmax_code_push_obj_table(fts_bmax_file_t *f, int value);
FTS_API void fts_bmax_code_pop_obj_table(fts_bmax_file_t *f);
FTS_API void fts_bmax_code_connect(fts_bmax_file_t *f);
FTS_API void fts_bmax_code_push_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at);
FTS_API void fts_bmax_code_return(fts_bmax_file_t *f);

FTS_API void fts_bmax_code_set_atom(fts_bmax_file_t *f, const fts_atom_t *a);

FTS_API void fts_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx);

FTS_API void fts_bmax_save_message(fts_bmax_file_t *file, fts_symbol_t selector, int ac, const fts_atom_t *at);


#endif
