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

#ifndef _SEQUENCE_scomark_H_
#define _SEQUENCE_scomark_H_

#include <fts/fts.h>
#include <fts/packages/data/data.h>
#include <fts/packages/sequence/event.h>

/* number of the first bar */
#define FIRST_BAR_NUMBER   1


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

FTS_API fts_class_t *scomark_class;
FTS_API enumeration_t *scomark_type_enumeration;

#define scomark_get_type(s) ((s)->type)
#define scomark_set_type(s, t) ((s)->type = (t))
#define scomark_is_bar(s) ((s)->type == seqsym_bar)

FTS_API int scomark_meter_symbol_get_quotient(fts_symbol_t sym, int *meter_num, int *meter_den);
FTS_API fts_symbol_t scomark_meter_quotient_get_symbol(int meter_num, int meter_den);

FTS_API void scomark_set_tempo(scomark_t *scomark, double tempo, double *old_tempo);
FTS_API void scomark_get_tempo(scomark_t *scomark, double *tempo);
FTS_API void scomark_unset_tempo(scomark_t *scomark);
FTS_API void scomark_set_cue(scomark_t *scomark, int cue);
FTS_API void scomark_get_cue(scomark_t *scomark, int *cue);
FTS_API void scomark_get_label(scomark_t *self, fts_symbol_t *label);
FTS_API void scomark_set_label(scomark_t *self, fts_symbol_t label);

FTS_API void scomark_bar_get_number(scomark_t *self, int *num);
FTS_API void scomark_bar_set_number(scomark_t *self, int num);
FTS_API void scomark_bar_set_meter(scomark_t *scomark, fts_symbol_t meter_sym, fts_symbol_t *old_meter);
FTS_API void scomark_bar_get_meter(scomark_t *self, fts_symbol_t *meter);
FTS_API void scomark_bar_set_meter_quotient(scomark_t *scomark, int meter_num, int meter_den);
FTS_API void scomark_bar_get_meter_quotient(scomark_t *self, int *meter_num, int *meter_den);

FTS_API void scomark_spost(fts_object_t *o, fts_bytestream_t *stream);

FTS_API void marker_track_dump_state(track_t *self, fts_dumper_t *dumper);
FTS_API void marker_track_unset_tempo_on_selection(track_t *marker_track, int ac, const fts_atom_t *at);
FTS_API scomark_t *marker_track_append_marker(track_t *marker_track, double time, int ac, const fts_atom_t *at, event_t **event);
FTS_API scomark_t *marker_track_insert_marker(track_t *marker_track, double time, fts_symbol_t type, event_t **event);
FTS_API void marker_track_renumber_bars(track_t *marker_track, event_t *start, int start_num, int upload);
FTS_API event_t *marker_track_append_bar(track_t *marker_track, event_t *start_evt);
FTS_API void marker_track_remove_events(track_t *marker_track, int ac, const fts_atom_t *at);
FTS_API void marker_track_clear(track_t *marker_track);
FTS_API void marker_track_collapse_markers(track_t *marker_track, int ac, const fts_atom_t *at);

/* import/export */
void marker_track_import_labels_txt (fts_object_t *o, int w, fts_symbol_t s, 
                                     int ac, const fts_atom_t *at);

#endif
