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
#ifndef _OBJECTSET_H_
#define _OBJECTSET_H_


struct fts_object_set;
typedef struct fts_object_set fts_object_set_t;

extern fts_object_set_t *fts_object_set_new(void);
extern void fts_object_set_delete(fts_object_set_t *set);

extern void fts_object_set_add(fts_object_set_t *set, fts_object_t *object);
extern void fts_object_set_remove(fts_object_set_t *set, fts_object_t *object);
extern void fts_object_set_remove_all(fts_object_set_t *set);

extern int fts_object_set_have_member(fts_object_set_t *set, fts_object_t *object);

extern void fts_object_set_send_message(fts_object_set_t *set, int winlet, fts_symbol_t sel,
					int ac, const fts_atom_t *av);

/* Iterators */

struct fts_object_set_iterator;
typedef struct fts_object_set_iterator fts_object_set_iterator_t;

extern void fts_object_set_iterator_init( fts_object_set_iterator_t *iter, const fts_object_set_t *set);
extern fts_object_set_iterator_t *fts_object_set_iterator_new( const fts_object_set_t *set);
extern void fts_object_set_iterator_free(fts_object_set_iterator_t *iter);

extern void fts_object_set_iterator_next( fts_object_set_iterator_t *iter);
extern int fts_object_set_iterator_end( const fts_object_set_iterator_t *iter);
extern fts_object_t *fts_object_set_iterator_current( const fts_object_set_iterator_t *iter);


#endif
