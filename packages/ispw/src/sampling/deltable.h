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
#ifndef _DELTABLE_H_
#define _DELTABLE_H_


#include "delbuf.h"

extern del_buf_t *delay_table_get_delbuf(fts_symbol_t delay_name);

/* for delwriter (just a single delwrite~) */

extern void delay_table_add_delwrite(fts_object_t *o, fts_symbol_t delay_name, del_buf_t *buf);
extern void delay_table_remove_delwrite(fts_object_t *o, fts_symbol_t delay_name);
extern void delay_table_delwrite_scheduled(fts_symbol_t delay_name);
extern int delay_table_is_delwrite_scheduled(fts_symbol_t delay_name);

/* for delreader (delread~ and vd~) */

typedef struct
{
  fts_object_t  obj;
  fts_symbol_t name;
  fts_object_t *next; /* DCE: pointer to the other delread for the same delay line */
} delay_table_delreader_t;

extern void delay_table_add_delreader(fts_object_t *o, fts_symbol_t delay_name);
extern void delay_table_remove_delreader(fts_object_t *o, fts_symbol_t delay_name);
extern void delay_table_delreader_scheduled(fts_symbol_t delay_name);
extern int delay_table_is_delreader_scheduled(fts_symbol_t delay_name);

/* property handling */

extern fts_atom_t *delay_table_get_or_delreader_prop(fts_symbol_t delay_name, fts_symbol_t property);

#endif /* _DELTABLE_H_ */

