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

#ifndef _FTS_PRIVATE_SAVER_H
#define _FTS_PRIVATE_SAVER_H

typedef struct fts_bmax_file fts_bmax_file_t;
struct _saver_dumper;

struct fts_bmax_file {
  FILE *file;
  fts_binary_file_header_t header; 
  fts_symbol_t *symbol_table;
  unsigned int symbol_table_size;
  int symbol_table_static;
  struct _saver_dumper *dumper;
};

/* used by package class only!!! */
extern void fts_bmax_code_push_atoms(fts_bmax_file_t *f, int ac, const fts_atom_t *at);
extern void fts_bmax_code_push_symbol(fts_bmax_file_t *f, fts_symbol_t sym);
extern void fts_bmax_code_pop_args(fts_bmax_file_t *f, int value);
extern void fts_bmax_code_obj_mess(fts_bmax_file_t *f, int inlet, fts_symbol_t sel, int nargs);
extern void fts_bmax_code_return(fts_bmax_file_t *f);
extern void fts_bmax_code_new_object(fts_bmax_file_t *f, fts_object_t *obj, int objidx);

extern int fts_bmax_file_open( fts_bmax_file_t *f, const char *name, int dobackup, fts_symbol_t *symbol_table, int symbol_table_size);
extern void fts_bmax_file_sync( fts_bmax_file_t *f);
extern void fts_bmax_file_close( fts_bmax_file_t *f);

extern void fts_save_patcher_as_bmax( fts_symbol_t file, fts_object_t *patcher);
extern void fts_save_selection_as_bmax( FILE *file, fts_object_t *selection);
extern void fts_save_simple_as_bmax( const char *filename, fts_object_t *patcher);

#endif
