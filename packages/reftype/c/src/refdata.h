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

#ifndef _REFDATA_H_
#define _REFDATA_H_

#include "fts.h"
#include "reftype.h"

typedef struct _refdata_
{
  fts_data_t fts_data;
  reftype_t *type;
  int cnt;
  fts_object_t *creator;
} refdata_t;

/* called in individual new functions of reference type implementations */
void refdata_init(refdata_t *data, reftype_t *type);

/****************************************************
 *
 *  reference type data functions
 *
 */

#define refdata_refer(r) (++((refdata_t *)(r))->cnt)
#define refdata_release(r) do { \
  if(--((refdata_t *)(r))->cnt == 0) \
    reftype_get_destructor(((refdata_t *)(r))->type)((refdata_t *)(r)); \
  } while(0)

extern refdata_t *refdata_create(reftype_t *type, int ac, const fts_atom_t* at);
#define refdata_destroy(r) (reftype_get_destructor(((refdata_t *)(r))->type))((refdata_t *)(r))

#define refdata_set_creator(r, c) (((refdata_t *)(r))->creator = (c))
#define refdata_get_creator(r) (((refdata_t *)(r))->creator)

/****************************************************
 *
 *  reference type atoms
 *
 */

/* get reference out of reference type atom */
#define refdata_atom_get(ap) ((refdata_t *)fts_word_get_ptr(fts_atom_value(ap)))

/* test if atom is of reference type */
extern fts_type_t vector_type, int_vector_type, float_vector_type, matrix_type;
#define refdata_atom_is(ap) ( \
  fts_is_a((ap), vector_type) || \
  fts_is_a((ap), int_vector_type) || \
  fts_is_a((ap), float_vector_type) || \
  fts_is_a((ap), matrix_type))

/* refer and release (and maybe destroy) data from reference type atom */
#define refdata_atom_refer(ap) refdata_refer(refdata_atom_get(ap))
#define refdata_atom_release(ap) refdata_release(refdata_atom_get(ap))

#endif



