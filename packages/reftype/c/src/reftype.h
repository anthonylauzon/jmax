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

#ifndef _REFTYPE_H_
#define _REFTYPE_H_

typedef struct _refdata_ *(* reftype_constructor_t)(int ac, const fts_atom_t *at);
typedef void (* reftype_destructor_t) (struct _refdata_ *ref);
typedef reftype_constructor_t(* reftype_dispatcher_t)(int ac, const fts_atom_t *at);

typedef struct _reftype_
{
  fts_symbol_t sym;
  reftype_dispatcher_t dispatcher;
  reftype_destructor_t destructor;
} reftype_t;

extern reftype_t *reftype_declare(fts_symbol_t sym, reftype_dispatcher_t dispatcher, reftype_destructor_t destructor);

#define reftype_get_dispatcher(t) ((t)->dispatcher)
#define reftype_get_destructor(t) ((t)->destructor)

#define reftype_get_constructor(t, ac, at) (((t)->dispatcher)((ac), (at)))

#endif



