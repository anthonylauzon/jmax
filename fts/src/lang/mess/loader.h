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
#ifndef _LOADER_H_
#define _LOADER_H_

/*
  A FTS binary file is made of the following:
  . a header:
    . magic number: 'bMax'
    . code size
    . number of symbols
  . code
  . symbol table
*/

typedef struct fts_binary_file_header_t {
  unsigned long magic_number;
  unsigned long code_size;
  unsigned long n_symbols;
} fts_binary_file_header_t;

/* 'bMax' */
#define FTS_BINARY_FILE_MAGIC 0x624D6178


/* Function: fts_binary_file_load
   Args:
      const char *name : file name
   Return value: int
     -1: an error occured
     other: loaded OK
   Description: loads and run a FTS binary file.
*/

extern fts_object_t *fts_binary_file_load(const char *name,
					  fts_object_t *parent,
					  int ac, const fts_atom_t *at,
					  fts_expression_state_t *e);

extern fts_object_t *fts_binary_filedesc_load(FILE *f,
					      fts_object_t *parent,
					      int ac, const fts_atom_t *at,
					      fts_expression_state_t *e);

#endif




