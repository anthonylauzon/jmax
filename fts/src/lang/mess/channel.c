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

/***************************************************************************
 *
 *  channel
 *
 */
void
fts_channel_init(fts_channel_t *channel)
{
  channel->origins = 0;
  channel->targets = 0;
}

static void
channel_add_access(fts_access_t **list, fts_access_t *access)
{
  access->next = *list;
  *list = access;
}

static void
channel_remove_access(fts_access_t **list, fts_access_t *access)
{
  fts_access_t **p; /* indirect precursor iteration */

  for (p=list; *p; p=&((*p)->next))
    {
      if(*p == access)
	{
	  *p = access->next;

	  return;
	}
    }
}

void
fts_channel_add_origin(fts_channel_t *channel, fts_access_t *origin)
{
  channel_add_access(&channel->origins, origin);
  origin->channel = channel;
}

void
fts_channel_add_target(fts_channel_t *channel, fts_access_t *target)
{
  channel_add_access(&channel->targets, target);
  target->channel = channel;
}

void
fts_channel_remove_origin(fts_channel_t *channel, fts_access_t *origin)
{
  channel_remove_access(&channel->origins, origin);
}

void
fts_channel_remove_target(fts_channel_t *channel, fts_access_t *target)
{
  channel_remove_access(&channel->targets, target);
}

void
fts_channel_output_message_from_targets(fts_channel_t *channel, int outlet, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  fts_access_t *target = channel->targets;

  while(target)
    {
      fts_outlet_send((fts_object_t *)target, outlet, selector, ac, at);
      target = target->next;
    }
}

void
fts_channel_find_friends(fts_channel_t *channel, int ac, const fts_atom_t *at)
{
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);
  fts_access_t *a; 

  for(a=channel->targets; a; a=a->next)
    fts_object_set_add(set, (fts_object_t *)a);

  for(a=channel->origins; a; a=a->next)
    fts_object_set_add(set, (fts_object_t *)a);
}
