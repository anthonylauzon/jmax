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

#define SYSEX_ATOMS_ALLOC_BLOCK 256

static fts_midiport_default_function_t fts_midiport_default_function = 0;

fts_symbol_t fts_s__superclass = 0;
fts_symbol_t fts_s_midiport = 0;

void
fts_midiport_config(void)
{
  fts_s_midiport = fts_new_symbol("midiport");
  fts_s__superclass = fts_new_symbol("_superclass");
}

fts_midiport_t *
fts_midiport_get_default(void)
{
  return (fts_midiport_default_function)? fts_midiport_default_function(): 0;
}

void
fts_midiport_set_default_function(fts_midiport_default_function_t fun)
{
  fts_midiport_default_function = fun;
}

void
fts_midiport_init(fts_midiport_t *port, fts_midiport_channel_message_output_t chmess_out, fts_midiport_system_exclusive_output_t sysex_out)
{
  int i, j;

  for(i=fts_midi_status_note; i<=fts_midi_status_pitch_bend; i++)
    for(j=0; j<=16; j++)
      port->channel_message_listeners[i][j] = 0;

  port->system_exclusive_listeners = 0;

  port->channel_message_output = chmess_out;
  port->system_exclusive_output = sysex_out;

  port->sysex_at = 0;
  port->sysex_ac = 0;
  port->sysex_alloc = 0;
}

void
fts_midiport_delete(fts_midiport_t *port)
{
  if(port->sysex_alloc)
    fts_block_alloc(port->sysex_alloc * sizeof(fts_atom_t));
}

void 
fts_midiport_class_init(fts_class_t *cl)
{
  fts_atom_t a[1];

  fts_set_symbol(a, fts_s_midiport);

  fts_class_put_prop(cl, fts_s__superclass, a); /* set _superclass property to "midiport" */
}

int 
fts_midiport_has_superclass(fts_object_t *obj)
{
  fts_atom_t a[1];

  fts_object_get_prop(obj, fts_s__superclass, a);

  if(fts_is_symbol(a) && fts_get_symbol(a) == fts_s_midiport)
    return 1;
  else
    return 0;
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
      freeme = l;
      *root = l->next;
    }
  else
    {
      while(l->next)
	{
	  if(l->next->listener == listener)
	    {
	      freeme = l->next;
	      l->next = l->next->next;

	      break;
	    }
	  
	  l = l->next;
	}
    }

  /* free removed listener */
  if(freeme)
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
fts_midiport_system_exclusive(fts_midiport_t *port, double time)
{
  fts_midiport_listener_t *l = port->system_exclusive_listeners;

  while(l)
    {
      l->callback.system_exclusive(l->listener, port->sysex_ac, port->sysex_at, time);
      l = l->next;
    }

  port->sysex_ac = 0;
}

void
fts_midiport_system_exclusive_add_byte(fts_midiport_t *port, int value)
{
  int index = port->sysex_ac;

  if(index >= port->sysex_alloc)
    {
      int new_alloc = port->sysex_alloc + SYSEX_ATOMS_ALLOC_BLOCK;
      fts_atom_t *new_at = fts_block_alloc(new_alloc * sizeof(fts_atom_t));
      int i;
      
      if(port->sysex_alloc)
	{
	  for(i=0; i<port->sysex_alloc; i++)
	    new_at[i] = port->sysex_at[i];
	  
	  fts_block_free(port->sysex_at, port->sysex_alloc * sizeof(fts_atom_t));
	}
      
      port->sysex_at = new_at;
      port->sysex_alloc = new_alloc;
    }

  fts_set_int(port->sysex_at + index, value);
  port->sysex_ac++;
}
