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
#ifndef _DATA_ATOMS_H_
#define _DATA_ATOMS_H_

#include "fts.h"

#define data_atom_get_type(atom) ((fts_is_data(&atom))? (fts_data_get_class_name(fts_get_data(&atom))): (fts_get_type(&atom)))
#define data_atom_is(atom, type) (data_atom_get_type(atom) == (type))

#define data_atom_is_const(atom) (!fts_is_data(&atom) || fts_data_is_const(fts_get_data(&atom)))

/* reassign atom to a new value taking in account the fts_data referencies */
#define data_atom_set(ap, new) \
do{ \
  if(fts_is_data(ap)) \
    data_release(fts_get_data(ap)); \
  *(ap) = (new); \
  if(fts_is_data(&(new))) \
    fts_data_refer(fts_get_data(&(new))); \
} while(0)

/* release referencies and set atom to void */
#define data_atom_void(ap) \
do{ \
  if(fts_is_data(ap)) \
    data_release(fts_get_data(ap)); \
  fts_set_void(ap); \
} while(0)

#define data_atom_refer(ap) \
  do{if(fts_is_data(ap)) fts_data_refer(fts_get_data(ap));}while(0)

#define data_atom_release(ap) \
  do{if(fts_is_data(ap)) data_release(fts_get_data(ap));}while(0)

#endif
