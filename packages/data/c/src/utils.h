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
#ifndef _DATA_UTILS_H_
#define _DATA_UTILS_H_

#include "fts.h"
#include "types.h"

#define data_outlet(O, WOUTLET, DATA) \
do { \
  fts_connection_t *__conn; \
  fts_atom_t __a; \
  fts_set_data(&__a, (DATA)); \
  __conn = (O)->out_conn[(WOUTLET)]; \
  fts_send(__conn, fts_s_int, 1, &__a); \
} while (0)


#define data_atom_outlet(o, woutlet, atom) \
do{ \
  fts_outlet_send(o, woutlet, data_atom_get_type(atom), 1, &atom); \
}while(0)

extern int data_atom_type_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);

#endif




