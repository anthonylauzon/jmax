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

#ifndef _DATA_VALUE_H_
#define _DATA_VALUE_H_

#include <fts/fts.h>

typedef struct _value_
{
  fts_object_t o;
  fts_atom_t a;		
} value_t;

extern fts_class_t *value_class;
extern fts_symbol_t value_symbol;
extern fts_type_t value_type;

extern void value_set(value_t *value, fts_atom_t a);
#define value_get(v) ((v)->a)

/* value atoms */
#define value_atom_set(ap, x) fts_set_object_with_type((ap), (x), value_type)
#define value_atom_get(ap) ((value_t *)fts_get_object(ap))
#define value_atom_is(ap) (fts_is_a((ap), value_type))

#endif
