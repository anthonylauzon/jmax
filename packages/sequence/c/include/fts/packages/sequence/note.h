/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#ifndef _SEQUENCE_scoob_H_
#define _SEQUENCE_scoob_H_

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/event.h>

/*{ 
*  scooob, score object (generalization of note)
*/
typedef struct
{
  propobj_t propobj;
  fts_symbol_t type;
  double pitch;
  double interval;
  double duration;
} scoob_t;

enum scoob_type_enum
{
  scoob_note = 0,
  scoob_interval,
  scoob_rest,
  scoob_trill
};

extern fts_class_t *scoob_class;
extern enumeration_t *scoob_type_enumeration;

#define scoob_set_pitch(s, x) ((s)->pitch = (x))
#define scoob_get_pitch(s) ((s)->pitch)

#define scoob_set_interval(s, x) ((s)->interval = (x))
#define scoob_get_interval(s) ((s)->interval)

#define scoob_set_duration(s, x) ((s)->duration = (x))
#define scoob_get_duration(s) ((s)->duration)

void scoob_set_velocity(scoob_t *self, int velocity);
int scoob_get_velocity(scoob_t *self);
void scoob_set_channel(scoob_t *self, int channel);
int scoob_get_channel(scoob_t *self);

#define scoob_get_type_index(s) (enumeration_get_index(scoob_type_enumeration, (s)->type))
#define scoob_set_type_by_index(s, i) ((s)->type = enumeration_get_name(scoob_type_enumeration, (i)))

/* scoob default MIDI properties */
void scoob_set_velocity(scoob_t *self, int velocity);
int scoob_get_velocity(scoob_t *self);

void scoob_set_channel(scoob_t *self, int channel);
int scoob_get_channel(scoob_t *self);

/* scoob properties */
#define scoob_property_get(s, n, p) propobj_get_property_by_name((propobj_t *)(s), (n), (p))
#define scoob_property_set(s, n, v) propobj_set_property_by_name((propobj_t *)(s), (n), (v))

#define scoob_property_get_by_index(s, i, p) propobj_get_property_by_index((propobj_t *)(s), (i), (p))
#define scoob_property_set_by_index(s, i, v) propobj_set_property_by_index((propobj_t *)(s), (i), (v))

void scoob_copy(scoob_t *org, scoob_t *copy);

/*} */

/*{ 
*  scomark, score marker (bars, tempo, etc.)
*/
typedef struct
{
  propobj_t propobj;
  fts_symbol_t type;
  double tempo;
  int beat;
  int beat_type;
} scomark_t;

enum scomark_type_enum
{
  scomark_tempo = 0,
  scomark_bar,
  scomark_end,
  scomark_double
};

extern fts_class_t *scomark_class;
extern enumeration_t *scomark_type_enumeration;

#define scomark_set_type(s, t) ((s)->type = (t))
#define scomark_get_type(s) ((s)->type)

#define scomark_set_tempo(s, t) ((s)->tempo = (t))
#define scomark_get_tempo(s) ((s)->tempo)

#define scomark_set_beat(s, b) ((s)->beat = (b))
#define scomark_get_beat(s) ((s)->beat)

#define scomark_get_beat_type(s) ((s)->beat_type)
#define scomark_set_beat_type(s, b) ((s)->beat_type = (b))

void scomark_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

#endif
