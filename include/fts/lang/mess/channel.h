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
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */
#ifndef _FTS_CHANNEL_H_
#define _FTS_CHANNEL_H_

#include <fts/sys.h>
#include <fts/lang/mess/classes.h>
#include <fts/lang/utils.h>

typedef struct _fts_channel_
{
  fts_object_list_t targets; /* list of targets */
  fts_object_list_t origins; /* list of origins */
} fts_channel_t;

#define fts_channel_has_target(c) (!fts_object_list_is_empty( &(c)->targets))

extern void fts_channel_init(fts_channel_t *channel);

extern void fts_channel_add_target(fts_channel_t *channel, fts_object_t *target);
extern void fts_channel_remove_target(fts_channel_t *channel, fts_object_t *target);

extern void fts_channel_add_origin(fts_channel_t *channel, fts_object_t *origin);
extern void fts_channel_remove_origin(fts_channel_t *channel, fts_object_t *origin);

extern void fts_channel_output_message_from_targets(fts_channel_t *channel, int outlet, fts_symbol_t s, int ac, const fts_atom_t *at);
extern void fts_channel_find_friends(fts_channel_t *channel, int ac, const fts_atom_t *at);
extern void fts_channel_propagate_input( fts_channel_t *channel, fts_propagate_fun_t propagate_fun, void *propagate_context, int outlet);

#endif
