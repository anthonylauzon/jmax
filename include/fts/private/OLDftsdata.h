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

#ifndef _FTS_PRIVATE_FTSDATA_H_
#define _FTS_PRIVATE_FTSDATA_H_


/*
 * This API is made private in order to remove it from the objects
 * (currently, only explode and qlist use it)
 * It will be used for now by patcherdata only.
 */

extern void fts_data_module_init(void);


typedef struct fts_data_class fts_data_class_t;

typedef void (*fts_data_fun_t)(fts_data_t *d, int ac, const fts_atom_t *at);
typedef void (*fts_data_export_fun_t)(fts_data_t *d);
typedef fts_data_t *(*fts_data_remote_constructor_t)(int ac, const fts_atom_t *at);
typedef void (*fts_data_remote_destructor_t)(fts_data_t *d);

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

extern fts_data_class_t *fts_data_class_new( fts_symbol_t data_class_name);
extern void fts_data_class_define_export_function( fts_data_class_t *class, fts_data_export_fun_t export_fun);
extern void fts_data_class_define_remote_constructor( fts_data_class_t *class, fts_data_remote_constructor_t constructor);
extern void fts_data_class_define_remote_destructor( fts_data_class_t *class, fts_data_remote_destructor_t destructor);
extern void fts_data_class_define_function( fts_data_class_t *class, int key, fts_data_fun_t fun);


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

extern void fts_data_init( fts_data_t *data, fts_data_class_t *class);
extern void fts_data_delete( fts_data_t *data);

/* class name */
#define fts_data_get_class_name(data) ((data)->class->data_class_name)
#define fts_data_is(data, cl_name) ((data)->class->data_class_name == (cl_name))

extern int fts_data_get_id( fts_data_t *data);
extern void fts_data_id_init(void);

extern void fts_data_export( fts_data_t *data);

extern void fts_data_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
extern void fts_data_remote_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
extern void fts_data_start_remote_call( fts_data_t *data, int key);
extern void fts_data_end_remote_call(void);

extern void fprintf_data(FILE *f, fts_data_t *data);

extern int fts_data_is_exported( fts_data_t *data);


/************************************************************************
 *
 * FTS data IDs
 *
 */

extern void fts_data_id_put( int id, fts_data_t *d);
extern fts_data_t *fts_data_id_get( int id);
extern void fts_data_id_remove( int id, fts_data_t *d);

#endif

