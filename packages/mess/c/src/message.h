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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#ifndef _MESS_MESSAGE_H_
#define _MESS_MESSAGE_H_

/************************************************************************
 *
 *  message object
 *
 *  note: this message object is not intended to be send anywhere as an object
 *
 */

#define MESSAGE_OK 0

typedef struct
{
  fts_object_t o;
  fts_symbol_t s;
  int ac;
  fts_atom_t *at;
} message_t;

extern fts_class_t *message_class;
extern fts_symbol_t message_symbol;
extern fts_type_t message_type;

extern void message_clear(message_t *mess);
extern void message_set(message_t *mess, fts_symbol_t s, int ac, const fts_atom_t *at);

#define message_get_selector(m) ((m)->s)
#define message_get_ac(m) ((m)->ac)
#define message_get_at(m) ((m)->at)

#define message_output(o, i, m) do { \
    fts_object_refer((fts_object_t *)(m)); \
    fts_outlet_send((o), (i), (m)->s, (m)->ac, (m)->at); \
    fts_object_release((fts_object_t *)(m)); \
  } while(0);

/* message atoms */
#define message_atom_set(ap, x) fts_set_object_with_type((ap), (x), message_type)
#define message_atom_get(ap) ((message_t *)fts_get_object(ap))
#define message_atom_is(ap) (fts_is_a((ap), message_type))

#endif
