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
#ifndef _ATOMARRAY_H_
#define _ATOMARRAY_H_

typedef struct fts_atom_array
{
  fts_data_t dataobj;
  fts_atom_t *atoms;
  int size;	
  int alloc;
} fts_atom_array_t;

extern fts_data_class_t *fts_atom_array_data_class;
extern fts_symbol_t fts_s_atom_array;

extern fts_atom_array_t *fts_atom_array_new(int size);
extern fts_atom_array_t *fts_atom_array_new_from_atom_list(int ac, const fts_atom_t *at);
extern void fts_atom_array_delete(fts_atom_array_t *this);

extern fts_data_t *fts_atom_array_constructor(int ac, const fts_atom_t *at);

extern void fts_atom_array_set_from_atom_list(fts_atom_array_t *array, int offset, int ac, const fts_atom_t *at);
extern int fts_atom_array_get_atoms(fts_atom_array_t *array, int ac, fts_atom_t *at);

extern void fts_atom_array_copy(fts_atom_array_t *in, fts_atom_array_t *out);
extern void fts_atom_array_void(fts_atom_array_t *array);

extern void fts_atom_array_set_size(fts_atom_array_t *array, int size);
#define fts_atom_array_get_size(array) ((array)->size)

#define fts_atom_array_check_index(array, i)  ((i >= 0) && (i < (array)->size))
#define fts_atom_array_is_empty(array) ((array)->size == 0)

#define fts_atom_array_get_element(array, index) ((array)->atoms[index])
#define fts_atom_array_set_element(array, index, value) \
do{ \
  fts_atom_t *old = (array)->atoms + (index); \
  if(fts_is_data(old)) fts_data_derefer(fts_get_data(old)); \
  (array)->atoms[(index)] = (value); \
  if(fts_is_data(&value)) fts_data_refer(fts_get_data(&value)); \
} while(0);

extern void fts_atom_array_set_const(fts_atom_array_t *array, fts_atom_t constant);

extern void fts_atom_array_save_bmax(fts_atom_array_t *this, fts_bmax_file_t *f);

extern void fprintf_atom_array(FILE *file, fts_atom_array_t *v);

#endif
