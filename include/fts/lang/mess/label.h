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

#ifndef _FTS_LABEL_H_
#define _FTS_LABEL_H_

typedef struct _fts_label_
{
  fts_object_t o;
  fts_channel_t channel;
} fts_label_t;

extern fts_class_t *fts_label_class;

#define fts_label_get_channel(l) (&(l)->channel)

#define fts_object_is_label(o) (fts_object_get_class(o) == fts_label_class)

#define fts_label_get(p, s) ((fts_label_t *)fts_variable_get_object_always((p), (s), fts_label_class))

#define fts_label_is_connected(l) (fts_channel_has_target(fts_label_get_channel(l)))
#define fts_label_send(l, s, n, a) fts_channel_output_message_from_targets(fts_label_get_channel(l), 0, (s), (n), (a))

#endif
