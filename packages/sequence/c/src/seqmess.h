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
#ifndef _SEQMESS_H_
#define _SEQMESS_H_

#include <fts/fts.h>
#include "event.h"

extern fts_class_t *seqmess_class;

typedef struct _seqmess_
{
  fts_object_t head;
  fts_symbol_t s; /* selector (no arguments yet) */
  int position; /* vertical position */
} seqmess_t;

#define seqmess_set_selector(m, x) ((m)->s = (x))
#define seqmess_get_selector(m) ((m)->s)

#define seqmess_set_position(m, x) ((m)->position = (x))
#define seqmess_get_position(m) ((m)->position)

#endif
