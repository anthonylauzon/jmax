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
  scoob_trill,
  scoob_unvoiced
};


enum scoob_propidx_enum
{
  scoob_propidx_velocity = 0,
  scoob_propidx_channel
};

extern fts_class_t *scoob_class;
extern enumeration_t *scoob_type_enumeration;

#define scoob_set_type(s, x) ((s)->type = (x))
#define scoob_get_type(s) ((s)->type)

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
  int bar_num;
  int cue;
  double tempo;
  int meter_num;
  int meter_den;
} scomark_t;

enum scomark_type_enum
{
  scomark_bar = 0,
  scomark_marker
};

enum scomark_propidx_enum
{
  scomark_propidx_tempo = 0,
  scomark_propidx_cue,
  scomark_propidx_label,
  scomark_propidx_bar_num,
  scomark_propidx_meter
};

extern fts_class_t *scomark_class;
extern enumeration_t *scomark_type_enumeration;

#define scomark_get_type(s) ((s)->type)
#define scomark_set_type(s, t) ((s)->type = (t))
#define scomark_is_bar(s) ((s)->type == seqsym_bar)

extern int scomark_meter_symbol_get_quotient(fts_symbol_t sym, int *meter_num, int *meter_den);
extern fts_symbol_t scomark_meter_quotient_get_symbol(int meter_num, int meter_den);

extern void scomark_set_tempo(scomark_t *scomark, double tempo);
extern double scomark_get_tempo(scomark_t *scomark);
extern void scomark_set_cue(scomark_t *scomark, int cue);
extern int scomark_get_cue(scomark_t *scomark);
extern fts_symbol_t scomark_get_label(scomark_t *self);
extern void scomark_set_label(scomark_t *self, fts_symbol_t label);

extern int scomark_bar_get_number(scomark_t *self);
extern void scomark_bar_set_number(scomark_t *self, int num);
extern void scomark_bar_set_meter_from_quotient(scomark_t *scomark, int meter_num, int meter_den);
extern void scomark_bar_set_meter(scomark_t *scomark, fts_symbol_t meter_sym);
extern fts_symbol_t scomark_bar_get_meter(scomark_t *self);
extern void scomark_bar_get_meter_quotient(scomark_t *self, int *meter_num, int *meter_den);

extern void scomark_spost(fts_object_t *o, fts_bytestream_t *stream);

#endif
