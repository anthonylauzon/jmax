/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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

