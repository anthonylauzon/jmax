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
#ifndef _MIDIVAL_H_
#define _MIDIVAL_H_

#include <fts/fts.h>
#include "event.h"

extern fts_class_t *midival_class;

typedef struct _midival_
{
  fts_object_t head;
  int value;
  int number;
  int channel;
} midival_t;

#define MIDIVAL_DEF_VALUE 64
#define MIDIVAL_DEF_NUMBER 64
#define MIDIVAL_DEF_CHANNEL 1

#define midival_set_value(n, x) ((n)->value = (x))
#define midival_get_value(n) ((n)->value)

#define midival_set_number(n, x) ((n)->number = (x))
#define midival_get_number(n) ((n)->number)

#define midival_set_channel(n, x) ((n)->channel = (x))
#define midival_get_channel(n, x) ((n)->channel)

#endif
