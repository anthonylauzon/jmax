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

#include <fts/sys.h>
#include <fts/lang.h>
#include "messP.h"

void fts_channel_init(fts_channel_t *channel)
{
  fts_object_list_init( &channel->origins);
  fts_object_list_init( &channel->targets);
}

void fts_channel_add_origin(fts_channel_t *channel, fts_object_t *origin)
{
  fts_object_list_insert( &channel->origins, origin);
}

void fts_channel_add_target(fts_channel_t *channel, fts_object_t *target)
{
  fts_object_list_insert( &channel->targets, target);
}

void fts_channel_remove_origin(fts_channel_t *channel, fts_object_t *origin)
{
  fts_object_list_remove( &channel->origins, origin);
}

void fts_channel_remove_target(fts_channel_t *channel, fts_object_t *target)
{
  fts_object_list_remove( &channel->targets, target);
}

void fts_channel_output_message_from_targets(fts_channel_t *channel, int outlet, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  fts_object_list_cell_t *p;

  for ( p = channel->targets.head; p; p = p->next)
    {
      fts_outlet_send( p->object, outlet, selector, ac, at);
    }
}

void fts_channel_find_friends(fts_channel_t *channel, int ac, const fts_atom_t *at)
{
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);
  fts_object_list_cell_t *p;

  for ( p = channel->targets.head; p; p = p->next)
    {
      if ( fts_object_get_patcher( p->object) != 0)
	fts_object_set_add( set, p->object);
    }

  for ( p = channel->origins.head; p; p = p->next)
    {
      if ( fts_object_get_patcher( p->object) != 0)
	fts_object_set_add( set, p->object);
    }
}

void fts_channel_propagate_input( fts_channel_t *channel, fts_propagate_fun_t propagate_fun, void *propagate_context, int outlet)
{
  fts_object_list_cell_t *p;

  for ( p = channel->targets.head; p; p = p->next)
    {
      propagate_fun(propagate_context, p->object, outlet);
    }
}
