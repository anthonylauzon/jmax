/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#ifndef _FTSDATA_H_
#define _FTSDATA_H_

#include "messages.h"

extern void fts_data_module_init(void);

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

struct fts_data {
  fts_data_class_t *class;
  fts_symbol_t name;
  int id;
  int cnst; /* flag: non zero if data is constant */
  int refcnt; /* reference counter */
};

extern void fts_data_init( fts_data_t *data, fts_data_class_t *class);
extern void fts_data_delete( fts_data_t *data);

/* (nos): data can be constant */
#define fts_data_is_const(data) ((data)->cnst != 0)
#define fts_data_set_const(data) ((data)->cnst = 1)
#define fts_data_set_var(data) ((data)->cnst = 0)

/* (nos): reference counting */
#define fts_data_refer(data) (++(data)->refcnt)
#define fts_data_derefer(data) (--(data)->refcnt)
#define fts_data_has_no_reference(data) ((data)->refcnt == 0)
#define fts_data_has_just_one_reference(data) ((data)->refcnt == 1)

/* class name */
#define fts_data_get_class_name(data) ((data)->class->data_class_name)
#define fts_data_is(data, cl_name) ((data)->class->data_class_name == (cl_name))

/* name */
#define fts_data_set_name(data, x) ((data)->name = (x))
#define fts_data_get_name(data) ((data)->name)

extern int fts_data_get_id( fts_data_t *data);
extern void fts_data_id_init(void);

extern void fts_data_export( fts_data_t *data);

extern void fts_data_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
extern void fts_data_remote_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
extern void fts_data_start_remote_call( fts_data_t *data, int key);
extern void fts_data_end_remote_call(void);

extern void fprintf_data(FILE *f, fts_data_t *data);

extern int fts_data_is_exported( fts_data_t *data);

#endif

