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
#ifndef _ARRAY_H_
#define _ARRAY_H_

extern fts_atom_array_t *fts_atom_array_new(int ac);
extern fts_atom_array_t *fts_atom_array_new_fill(int ac, const fts_atom_t *at);

extern void fts_atom_array_free(fts_atom_array_t *v);

#define fts_atom_array_get(v, i)  ((v)->at[(i)])

#define fts_atom_array_set(v, i, a)  ((v)->at[(i)] = (a))

#define fts_atom_array_check(v, i)  ((i >= 0) && (i < (v)->ac))

extern void fprintf_atom_array(FILE *file, fts_atom_array_t *v);

extern void fts_atom_array_init(void);

#endif
