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
 */

#ifndef _SEQUENCE_scoob_H_
#define _SEQUENCE_scoob_H_

#include <fts/fts.h>
#include "event.h"

extern fts_class_t *scoob_class;

enum scoob_type_enum
{
  scoob_none = 0,
  scoob_note,
  scoob_interval,
  scoob_rest,
  scoob_trill,
  n_scoob_types
};

typedef struct _scoob_
{
  fts_object_t head;
  enum scoob_type_enum type;
  double pitch;
  double interval;
  double duration;
  fts_array_t properties;
} scoob_t;

#define scoob_DEF_PITCH 64
#define scoob_DEF_DURATION 100

#define scoob_set_type(n, x) ((n)->type = (x))
#define scoob_get_type(n) ((n)->type)

#define scoob_set_pitch(n, x) ((n)->pitch = (x))
#define scoob_get_pitch(n) ((n)->pitch)

#define scoob_set_interval(n, x) ((n)->interval = (x))
#define scoob_get_interval(n) ((n)->interval)

#define scoob_set_duration(n, x) ((n)->duration = (x))
#define scoob_get_duration(n) ((n)->duration)

extern void scoob_set_velocity(scoob_t *this, int velocity);
extern int scoob_get_velocity(scoob_t *this);

extern void scoob_set_channel(scoob_t *this, int channel);
extern int scoob_get_channel(scoob_t *this);

#endif
