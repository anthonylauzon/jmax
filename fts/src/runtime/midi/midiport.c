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

#include "fts.h"
#include "midiport.h"

void
fts_midiport_init(fts_midiport_t *port)
{
  int i, j;

  for(i=fts_midi_status_note_off; i<=fts_midi_status_pitch_bend; i++)
    for(j=0; j<=16; j++)
      port->channel_message_listeners[i][j] = 0;

  port->system_exclusive_listeners = 0;
}

void 
fts_midiport_add_listener(fts_midiport_t *port, fts_midi_status_t status, int channel, fts_object_t *listener, fts_midiport_callback_t fun)
{
  fts_midiport_listener_t *l = (fts_midiport_listener_t *)fts_malloc(sizeof(fts_midiport_listener_t));

  if(channel < 0)
    channel = 0;
  else if(channel > 16)
    channel = 16;  

  if(status < fts_midi_status_system_exclusive)
    {
      l->callback = fun;
      l->listener = listener;
      l->next = port->channel_message_listeners[status][channel];
      
      port->channel_message_listeners[status][channel] = l;
    }
  else
    {
      l->callback = fun;
      l->listener = listener;
      l->next = port->system_exclusive_listeners;
      
      port->system_exclusive_listeners = l;      
    }
}

void 
fts_midiport_remove_listener(struct _fts_midiport_ *port, fts_midi_status_t status, int channel, fts_object_t *listener)
{
  fts_midiport_listener_t **root;  
  fts_midiport_listener_t *l;
  fts_midiport_listener_t *freeme =  0;

  if(status < fts_midi_status_system_exclusive)
    root = &port->channel_message_listeners[status][channel];
  else
    root = &port->system_exclusive_listeners;

  l = *root;

  if(l && l->listener == listener)
    {
      *root = l->next;
      freeme = l;
    }
  else
    {
      while(l->next)
	{
	  if(l->next->listener == listener)
	    {
	      l->next = l->next->next;
	      freeme = l->next;

	      break;
	    }
	  
	  l = l->next;
	}
    }

  /* free removed listener */
  fts_free(freeme);
}

void
fts_midiport_channel_message(fts_midiport_t *port, fts_midi_status_t status, int channel, int x, int y, double time)
{
  fts_midiport_listener_t *l;

  /* run everybody registerd for this status and this channel */
  l = port->channel_message_listeners[status][channel];
  while(l)
    {
      l->callback.channel_message(l->listener, channel, x, y, time);
      l = l->next;
    }

  /* run everybody registerd for this status in onmi mode (channel 0) */
  l = port->channel_message_listeners[status][0];
  while(l)
    {
      l->callback.channel_message(l->listener, channel, x, y, time);
      l = l->next;
    }
}

void
fts_midiport_system_exclusive(fts_midiport_t *port, fts_midi_status_t status, int size, char *buf, double time)
{
  fts_midiport_listener_t *l = port->system_exclusive_listeners;

  while(l)
    {
      l->callback.system_exclusive(l->listener, size, buf, time);
      l = l->next;
    }
}
