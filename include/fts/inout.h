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

#ifndef _FTS_INOUT_H_
#define _FTS_INOUT_H_

/**
 *
 * Channel
 * A list of targets and origins
 *
 */

typedef struct _fts_channel_
{
  fts_objectlist_t targets; /* list of targets */
  fts_objectlist_t origins; /* list of origins */
} fts_channel_t;

#define fts_channel_has_target(c) (!fts_objectlist_is_empty( &(c)->targets))

FTS_API void fts_channel_init(fts_channel_t *channel);

FTS_API void fts_channel_add_target(fts_channel_t *channel, fts_object_t *target);
FTS_API void fts_channel_remove_target(fts_channel_t *channel, fts_object_t *target);

FTS_API void fts_channel_add_origin(fts_channel_t *channel, fts_object_t *origin);
FTS_API void fts_channel_remove_origin(fts_channel_t *channel, fts_object_t *origin);

FTS_API void fts_channel_send(fts_channel_t *channel, int outlet, fts_symbol_t s, int ac, const fts_atom_t *at);

FTS_API void fts_channel_find_friends(fts_channel_t *channel, int ac, const fts_atom_t *at);
FTS_API void fts_channel_propagate_input( fts_channel_t *channel, fts_propagate_fun_t propagate_fun, void *propagate_context, int outlet);


/**
 *
 * Label
 * A fts_object_t that contains a channel
 *
 */

FTS_API fts_class_t *fts_label_class;

typedef struct _fts_label_
{
  fts_object_t head;
  fts_channel_t channel;
} fts_label_t;

#define fts_label_get_channel(l) (&(l)->channel)

FTS_API fts_label_t *fts_label_get(fts_patcher_t *patcher, fts_symbol_t name);

#define fts_label_send(l, s, n, a) fts_channel_send( fts_label_get_channel(l), 0, (s), (n), (a))

#endif
