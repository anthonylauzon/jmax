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

#ifndef _FTS_FTSDATA_H_
#define _FTS_FTSDATA_H_

#include <fts/lang/mess/messages.h>

FTS_API void fts_data_module_init(void);

/************************************************************************
 *
 *  FTS data classes
 *
 */

#define FUNCTION_TABLE_SIZE 32

struct fts_data_class {
  fts_symbol_t data_class_name;
  fts_data_export_fun_t export_fun;
  fts_data_remote_constructor_t remote_constructor;
  fts_data_remote_destructor_t remote_destructor;
  fts_data_fun_t functions_table[FUNCTION_TABLE_SIZE];
};

FTS_API fts_data_class_t *fts_data_class_new( fts_symbol_t data_class_name);
FTS_API void fts_data_class_define_export_function( fts_data_class_t *class, fts_data_export_fun_t export_fun);
FTS_API void fts_data_class_define_remote_constructor( fts_data_class_t *class, fts_data_remote_constructor_t constructor);
FTS_API void fts_data_class_define_remote_destructor( fts_data_class_t *class, fts_data_remote_destructor_t destructor);
FTS_API void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t fun);


/************************************************************************
 *
 *  FTS data
 *
 */

struct fts_data 
{
  fts_data_class_t *class;
  int id;
};

FTS_API void fts_data_init( fts_data_t *data, fts_data_class_t *class);
FTS_API void fts_data_delete( fts_data_t *data);

/* class name */
#define fts_data_get_class_name(data) ((data)->class->data_class_name)
#define fts_data_is(data, cl_name) ((data)->class->data_class_name == (cl_name))

FTS_API int fts_data_get_id( fts_data_t *data);
FTS_API void fts_data_id_init(void);

FTS_API void fts_data_export( fts_data_t *data);

FTS_API void fts_data_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
FTS_API void fts_data_remote_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
FTS_API void fts_data_start_remote_call( fts_data_t *data, int key);
FTS_API void fts_data_end_remote_call(void);

FTS_API void fprintf_data(FILE *f, fts_data_t *data);

FTS_API int fts_data_is_exported( fts_data_t *data);

#endif

