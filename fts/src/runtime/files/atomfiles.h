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
#ifndef _ATOMFILES_H_
#define _ATOMFILES_H_

struct _fts_atom_file_t_;
typedef struct _fts_atom_file_t_ fts_atom_file_t;

extern fts_atom_file_t *fts_atom_file_open(const char *name, const char *mode);
extern void fts_atom_file_close(fts_atom_file_t *f);

extern int fts_atom_file_read(fts_atom_file_t *f, fts_atom_t *at);
extern int fts_atom_file_write(fts_atom_file_t *f, const fts_atom_t *at, char separator);

#endif
