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

#include "sys.h"
#include "lang.h"
#include "midiport.h"

#define SYSEX_BLOCK_SIZE 512

static fts_symbol_t fts_s__superclass = 0;
static fts_symbol_t fts_s_midiport = 0;

static fts_midiport_default_function_t fts_midiport_default_function = 0;

union _fts_midiport_callback_
{
  fts_midiport_poly_fun_t poly;
  fts_midiport_channel_fun_t channel;
  fts_midiport_sysex_fun_t sysex;
};

typedef struct _fts_midiport_listener_
{
  union _fts_midiport_callback_ callback;
  fts_object_t *listener;
  struct _fts_midiport_listener_ *next;
} fts_midiport_listener_t;

struct _fts_midiport_listeners_ 
{
  fts_midiport_listener_t *note[17][128]; /* channels 1..16 + 0 for omni */
  fts_midiport_listener_t *poly_pressure[17][128];
  fts_midiport_listener_t *control_change[17][128]; /* controller number 1..128 + 0 for omni */
  fts_midiport_listener_t *program_change[17];
  fts_midiport_listener_t *channel_pressure[17];
  fts_midiport_listener_t *pitch_bend[17];
  fts_midiport_listener_t *system_exclusive;
};

/****************************************************
 *
 *  MIDI port class
 *
 */

void 
fts_midiport_class_init(fts_class_t *cl)
{
  fts_atom_t a[1];

  fts_set_symbol(a, fts_s_midiport);

  fts_class_put_prop(cl, fts_s__superclass, a); /* set _superclass property to "midiport" */
}

void
fts_midiport_init(fts_midiport_t *port)
{
  port->listeners = 0;
  port->output = 0;

  port->sysex_at = 0;
  port->sysex_ac = 0;
  port->sysex_alloc = 0;
}

void
fts_midiport_delete(fts_midiport_t *port)
{
  if(port->sysex_alloc)
    fts_block_free(port->sysex_at, port->sysex_alloc);

  if(port->listeners)
    fts_free(port->listeners);
}

void
fts_midiport_set_input(fts_midiport_t *port)
{
  fts_midiport_listeners_t *listeners = fts_malloc(sizeof(fts_midiport_listeners_t));
  int i;

  for(i=0; i<=16; i++)
    {
      int j;

      for(j=0; j<=127; j++)
	{
	  listeners->note[i][j] = 0;
	  listeners->poly_pressure[i][j] = 0;
	  listeners->control_change[i][j] = 0;
	}

      listeners->program_change[i] = 0;
      listeners->channel_pressure[i] = 0;
      listeners->pitch_bend[i] = 0;
    }

  listeners->system_exclusive = 0;

  port->listeners = listeners;
}

void
fts_midiport_set_output(fts_midiport_t *port, fts_midiport_output_functions_t *functions)
{
  port->output = functions;
}

int
fts_midiport_check(fts_object_t *obj)
{
  fts_atom_t a[1];

  fts_object_get_prop(obj, fts_s__superclass, a);

  if(fts_is_symbol(a) && fts_get_symbol(a) == fts_s_midiport)
    return 1;
  else
    return 0;
}

/****************************************************
 *
 *  MIDI port listeners
 *
 */

static void 
add_poly_listener(fts_midiport_listener_t **list, fts_object_t *object, fts_midiport_poly_fun_t fun)
{
  fts_midiport_listener_t *l = (fts_midiport_listener_t *)fts_malloc(sizeof(fts_midiport_listener_t));

  l->callback.poly = fun;
  l->listener = object;
  l->next = *list;
  
  *list = l;
}

static void 
add_channel_listener(fts_midiport_listener_t **list, fts_object_t *object, fts_midiport_channel_fun_t fun)
{
  fts_midiport_listener_t *l = (fts_midiport_listener_t *)fts_malloc(sizeof(fts_midiport_listener_t));

  l->callback.channel = fun;
  l->listener = object;
  l->next = *list;
  
  *list = l;
}

static void 
remove_listener(fts_midiport_listener_t **list, fts_object_t *o)
{
  fts_midiport_listener_t *l = *list;

  if(l)
    {
      fts_midiport_listener_t *freeme = 0;
      
      if(l->listener == o)
	{
	  freeme = l;
	  *list = l->next;
	}
      else
	{
	  while(l->next)
	    {
	      if(l->next->listener == o)
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
}

#define CLIP_CHANNEL(c) (c < 0)? 0: ((c > 16)? 16: c)
#define CLIP_NUMBER(n) (n < 0)? 0: ((n > 128)? 128: n);

static void
call_poly_listeners(fts_midiport_listener_t *list[][128], int channel, int number, int value, double time)
{
  fts_midiport_listener_t *l; 

  l = list[channel][number + 1];
  while(l)
    {
      l->callback.poly(l->listener, channel, number, value, time);
      l = l->next;
    }

  l = list[channel][0];
  while(l)
    {
      l->callback.poly(l->listener, channel, number, value, time);
      l = l->next;
    }

  l = list[0][number + 1];
  while(l)
    {
      l->callback.poly(l->listener, channel, number, value, time);
      l = l->next;
    }

  l = list[0][0];
  while(l)
    {
      l->callback.poly(l->listener, channel, number, value, time);
      l = l->next;
    }
}

static void
call_channel_listeners(fts_midiport_listener_t **list, int channel, int value, double time)
{
  fts_midiport_listener_t *l; 

  l = list[channel];
  while(l)
    {
      l->callback.channel(l->listener, channel, value, time);
      l = l->next;
    }

  l = list[0];
  while(l)
    {
      l->callback.channel(l->listener, channel, value, time);
      l = l->next;
    }

}

/* add listeners */

void
fts_midiport_add_listener_note(fts_midiport_t *port, int channel, int number, fts_object_t *o, fts_midiport_poly_fun_t fun)
{
  add_poly_listener(&port->listeners->note[channel][number + 1], o, fun);
}

void
fts_midiport_add_listener_poly_pressure(fts_midiport_t *port, int channel, int number, fts_object_t *o, fts_midiport_poly_fun_t fun)
{
  add_poly_listener(&port->listeners->poly_pressure[channel][number + 1], o, fun);
}

void
fts_midiport_add_listener_control_change(fts_midiport_t *port, int channel, int number, fts_object_t *o, fts_midiport_poly_fun_t fun)
{
  add_poly_listener(&port->listeners->control_change[channel][number + 1], o, fun);
}

void
fts_midiport_add_listener_program_change(fts_midiport_t *port, int channel, fts_object_t *o, fts_midiport_channel_fun_t fun)
{
  add_channel_listener(&port->listeners->program_change[channel], o, fun);
}

void
fts_midiport_add_listener_channel_pressure(fts_midiport_t *port, int channel, fts_object_t *o, fts_midiport_channel_fun_t fun)
{
  add_channel_listener(&port->listeners->channel_pressure[channel], o, fun);
}

void
fts_midiport_add_listener_pitch_bend(fts_midiport_t *port, int channel, fts_object_t *o, fts_midiport_channel_fun_t fun)
{
  add_channel_listener(&port->listeners->pitch_bend[channel], o, fun);
}

void
fts_midiport_add_listener_system_exclusive(fts_midiport_t *port, fts_object_t *o, fts_midiport_sysex_fun_t fun)
{
  fts_midiport_listener_t *l = (fts_midiport_listener_t *)fts_malloc(sizeof(fts_midiport_listener_t));

  l->callback.sysex = fun;
  l->listener = o;
  l->next = port->listeners->system_exclusive;
  
  port->listeners->system_exclusive = l;
}

/* remove listeners */

void
fts_midiport_remove_listener_note(fts_midiport_t *port, int channel, int number, fts_object_t *o)
{
  remove_listener(&port->listeners->note[channel][number + 1], o);
}

void
fts_midiport_remove_listener_poly_pressure(fts_midiport_t *port, int channel, int number, fts_object_t *o)
{
  remove_listener(&port->listeners->poly_pressure[channel][number + 1], o);
}

void
fts_midiport_remove_listener_control_change(fts_midiport_t *port, int channel, int number, fts_object_t *o)
{
  remove_listener(&port->listeners->control_change[channel][number + 1], o);
}

void
fts_midiport_remove_listener_program_change(fts_midiport_t *port, int channel, fts_object_t *o)
{
  remove_listener(&port->listeners->program_change[channel], o);
}

void
fts_midiport_remove_listener_channel_pressure(fts_midiport_t *port, int channel, fts_object_t *o)
{
  remove_listener(&port->listeners->channel_pressure[channel], o);
}

void
fts_midiport_remove_listener_pitch_bend(fts_midiport_t *port, int channel, fts_object_t *o)
{
  remove_listener(&port->listeners->pitch_bend[channel], o);
}

void
fts_midiport_remove_listener_system_exclusive(fts_midiport_t *port, fts_object_t *o)
{
  remove_listener(&port->listeners->system_exclusive, o);
}

/* call listeners */

void
fts_midiport_input_note(fts_midiport_t *port, int channel, int number, int value, double time)
{
  call_poly_listeners(port->listeners->note, channel, number, value, time);
}

void
fts_midiport_input_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time)
{
  call_poly_listeners(port->listeners->poly_pressure, channel, number, value, time);
}

void
fts_midiport_input_control_change(fts_midiport_t *port, int channel, int number, int value, double time)
{
  call_poly_listeners(port->listeners->control_change, channel, number, value, time);
}

void
fts_midiport_input_program_change(fts_midiport_t *port, int channel, int value, double time)
{
  call_channel_listeners(port->listeners->program_change, channel, value, time);
}

void
fts_midiport_input_channel_pressure(fts_midiport_t *port, int channel, int value, double time)
{
  call_channel_listeners(port->listeners->channel_pressure, channel, value, time);
}

void
fts_midiport_input_pitch_bend(fts_midiport_t *port, int channel, int value, double time)
{
  call_channel_listeners(port->listeners->pitch_bend, channel, value, time);
}

void
fts_midiport_input_system_exclusive_byte(fts_midiport_t *port, int value)
{
  int index = port->sysex_ac;

  if(index >= port->sysex_alloc)
    {
      int new_alloc = port->sysex_alloc + SYSEX_BLOCK_SIZE;
      fts_atom_t *new_buf = fts_block_alloc(new_alloc * sizeof(fts_atom_t));
      int i;
      
      if(port->sysex_alloc)
	{
	  for(i=0; i<port->sysex_alloc; i++)
	    new_buf[i] = port->sysex_at[i];
	  
	  fts_block_free(port->sysex_at, port->sysex_alloc * sizeof(fts_atom_t));
	}
      
      port->sysex_at = new_buf;
      port->sysex_alloc = new_alloc;
    }

  fts_set_int(port->sysex_at + index, value);
  port->sysex_ac++;
}

void
fts_midiport_input_system_exclusive_call(fts_midiport_t *port, double time)
{
  fts_midiport_listener_t *l = port->listeners->system_exclusive;

  while(l)
    {
      l->callback.sysex(l->listener, port->sysex_ac, port->sysex_at, time);
      l = l->next;
    }

  port->sysex_ac = 0;
}

/****************************************************
 *
 *  MIDI port output
 *
 */

void fts_midiport_output_note(fts_midiport_t *port, int channel, int number, int value, double time)
{
  port->output->note((fts_object_t *)port, channel, number, value, time);
}

void fts_midiport_output_poly_pressure(fts_midiport_t *port, int channel, int number, int value, double time)
{
  port->output->poly_pressure((fts_object_t *)port, channel, number, value, time);
}

void fts_midiport_output_control_change(fts_midiport_t *port, int channel, int number, int value, double time)
{
  port->output->control_change((fts_object_t *)port, channel, number, value, time);
}

void fts_midiport_output_program_change(fts_midiport_t *port, int channel, int value, double time)
{
  port->output->program_change((fts_object_t *)port, channel, value, time);
}

void fts_midiport_output_channel_pressure(fts_midiport_t *port, int channel, int value, double time)
{
  port->output->channel_pressure((fts_object_t *)port, channel, value, time);
}

void fts_midiport_output_pitch_bend(fts_midiport_t *port, int channel, int value, double time)
{
  port->output->pitch_bend((fts_object_t *)port, channel, value, time);
}

void fts_midiport_output_system_exclusive_byte(fts_midiport_t *port, int value)
{
  port->output->sysex_byte((fts_object_t *)port, value);
}

void fts_midiport_output_system_exclusive_flush(fts_midiport_t *port, double time)
{
  port->output->sysex_flush((fts_object_t *)port, time);
}

/****************************************************
 *
 *  default MIDI port
 *
 */

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
fts_midiport_config(void)
{
  fts_s_midiport = fts_new_symbol("midiport");
  fts_s__superclass = fts_new_symbol("_superclass");
}
