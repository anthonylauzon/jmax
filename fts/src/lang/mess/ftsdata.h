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

/* (nos): new (pseudo) generic stuff */
extern fts_data_t *fts_data_new(fts_symbol_t class_name, int ac, const fts_atom_t *at);
extern fts_data_t *fts_data_new_const(fts_symbol_t class_name, int ac, const fts_atom_t *at);

extern void fts_data_refer(fts_data_t *data);
extern int fts_data_release(fts_data_t *data);
extern int fts_data_get_size(fts_data_t *data);
extern int fts_data_get_atoms(fts_data_t *data, int ac, fts_atom_t *at);

/* (incognito): oops, little journey to the message world */
extern void fts_method_define_data(fts_class_t *class, int winlet, fts_method_t fun);
extern void fts_outlet_data(fts_object_t *o, int woutlet, fts_data_t *data);

/* (nos): "old" stuff */
extern void fts_data_init( fts_data_t *data, fts_data_class_t *class);
extern void fts_data_delete( fts_data_t *data);

extern void fts_data_export( fts_data_t *data);

extern fts_symbol_t fts_data_get_class_name(fts_data_t *data);

extern void fts_data_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
extern void fts_data_remote_call( fts_data_t *data, int key, int ac, const fts_atom_t *at);
extern void fts_data_start_remote_call( fts_data_t *data, int key);
extern void fts_data_end_remote_call(void);

extern int fts_data_get_id( fts_data_t *data);
extern void fts_data_id_init(void);

#define fts_data_set_name(data, x) ((data)->name = (x))
#define fts_data_get_name(data) ((data)->name)

extern void fprintf_data(FILE *f, fts_data_t *data);

#define fts_data_is_const(data) (data->cnst != 0)
extern int fts_data_is_exported( fts_data_t *data);
extern int fts_data_is(fts_data_t *data, fts_symbol_t class_name);

#endif

