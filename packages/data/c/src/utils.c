/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"
#include "data.h"

int
data_atom_type_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (data_atom_get_type(at0[1]) == data_atom_get_type(at1[1]));
}
