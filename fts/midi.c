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
 */

#include <fts/fts.h>

/*
 * This files contains all things MIDI related:
 *
 *  - the declaration of the MIDI module
 *  - the MIDI parser 
 *  - the MIDI port
 *
 * MIDI files are handled in midifile.c
 */

/* 
 * Definition of the MIDI status bytes 
 */
#define MASK_CHANNEL_MESSAGE 0x80
#define STATUS_BYTE_SYSEX 0xf0
#define STATUS_BYTE_QUARTER_FRAME 0xf1
#define STATUS_BYTE_SYSEX_END 0xf7
#define STATUS_BYTE_REALTIME_TIMING_CLOCK 0xf8
#define STATUS_BYTE_REALTIME_TIMING_TICK
#define STATUS_BYTE_REALTIME_START 0xfa
#define STATUS_BYTE_REALTIME_CONTINUE 0xfb
#define STATUS_BYTE_REALTIME_STOP 0xfc
#define STATUS_BYTE_REALTIME_UNDEFINED 0xfd
#define STATUS_BYTE_REALTIME_ACTIVE_SENSING 0xfe
#define STATUS_BYTE_REALTIME_SYSTEM_RESET 0xff

#define STATUS_BYTE_SYSEX_NON_REALTIME 0x7e 
#define STATUS_BYTE_SYSEX_REALTIME 0x7f

#define RANGE_CH(n) (((n) < 1)? 1: (((n) > 16)? 16: (n)))
#define RANGE_VALUE(n) (((n) < 0)? 0: (((n) > 127)? 127: (n)))


#define SYSEX_BLOCK_SIZE 512

/*
 *  Definition of the midiport structures
 */
static fts_symbol_t fts_s__superclass = 0;
static fts_symbol_t fts_s_midiport = 0;

union _fts_midiport_callback_
{
  fts_midiport_poly_fun_t poly;
  fts_midiport_channel_fun_t channel;
  fts_midiport_sysex_fun_t sysex;
};

typedef struct fts_midiport_listener
{
  union _fts_midiport_callback_ callback;
  fts_object_t *listener;
  struct fts_midiport_listener *next;
} fts_midiport_listener_t;

struct fts_midiport_listeners
{
  fts_midiport_listener_t *note[17][128]; /* channels 1..16 + 0 for omni */
  fts_midiport_listener_t *poly_pressure[17][128];
  fts_midiport_listener_t *control_change[17][128]; /* controller number 1..128 + 0 for omni */
  fts_midiport_listener_t *program_change[17];
  fts_midiport_listener_t *channel_pressure[17];
  fts_midiport_listener_t *pitch_bend[17];
  fts_midiport_listener_t *system_exclusive;
};


/************************************************************
 *
 *  MIDI parser
 *
 */

void
fts_midiparser_init(fts_midiparser_t *parser)
{  
  parser->status = status_normal;
  parser->message = 0;
  parser->channel = 0;	
  parser->arg = NO_ARG;

  parser->mtc_status = mtc_status_invalid;
  parser->mtc_frame_count = 0;
  parser->mtc_type = 0;
  parser->mtc_hour = 0;
  parser->mtc_min = 0;
  parser->mtc_sec = 0;
  parser->mtc_frame = 0;

  parser->mtc_time = 0.0;
}

static void
midiparser_compute_time_code(fts_midiparser_t *parser)
{
  fts_atom_t av[5];
  float total_secs;
  double new_time;

  total_secs = (((parser->mtc_hour * 60) + parser->mtc_min) * 60) + parser->mtc_sec;

  /* add two to the frames to compensate the MTC delay */
  new_time = total_secs * 1000.0 + 
    (((float) parser->mtc_frame + 2) * (1000.0 / (float) (parser->mtc_type == 0? 24: (parser->mtc_type == 1 ? 25 : 30))));

  parser->mtc_time = new_time;

  /* raise the midi action */
  fts_set_int(&av[0], parser->mtc_type);
  fts_set_int(&av[1], parser->mtc_hour);
  fts_set_int(&av[2], parser->mtc_min);
  fts_set_int(&av[3], parser->mtc_sec);
  fts_set_int(&av[4], parser->mtc_frame);

  /* send time code here */
  /*fts_midiport_time_code(port, FTS_MIDI_MTC, 5 , av);*/
}

void
fts_midiparser_byte(fts_midiparser_t *parser, unsigned char byte)
{
  fts_midiport_t *port = (fts_midiport_t *)parser;

  /* real-time not implemented */
  if(byte >= STATUS_BYTE_REALTIME_TIMING_CLOCK)
    return;

  switch (parser->status)
    {
    case status_normal:
      
      switch (byte)
	{
	case STATUS_BYTE_QUARTER_FRAME:
	  parser->status = status_mtc_quarter_frame;
	  break;
	case STATUS_BYTE_SYSEX:
	  parser->status = status_sysex;
	  break;
	default:
	  if(byte & MASK_CHANNEL_MESSAGE)
	    {
	      parser->message = (byte >> 4) & 7;
	      parser->channel = (byte & 0xf) + 1;
	      parser->arg = NO_ARG;
	    }
	  else
	    {
	      switch (parser->message)
		{
		case channel_message_note_off:
		  
		  if (parser->arg == NO_ARG)
		    parser->arg = byte;
		  else
		    {
		      fts_midiport_input_note(port, parser->channel, parser->arg, 0, 0.0);
		      
		      /* reset for running status */
		      parser->arg = NO_ARG;
		    }

		  break;
		  
		case channel_message_note_on:
	  
		  if (parser->arg == NO_ARG)
		    parser->arg = byte;
		  else
		    {
		      fts_midiport_input_note(port, parser->channel, parser->arg, byte, 0.0);
		      
		      /* reset for running status */
		      parser->arg = NO_ARG;
		    }

		  break;
	  
		case channel_message_poly_pressure:
	  
		  if (parser->arg == NO_ARG)
		    parser->arg = byte;
		  else
		    {
		      fts_midiport_input_poly_pressure(port, parser->channel, parser->arg, byte, 0.0);
		      
		      /* reset for running status */
		      parser->arg = NO_ARG;
		    }
		  break;
	  
		case channel_message_control_change:
	  
		  if (parser->arg == NO_ARG)
		    parser->arg = byte;
		  else
		    {
		      fts_midiport_input_control_change(port, parser->channel, parser->arg, byte, 0.0);
		      
		      /* reset for running status */
		      parser->arg = NO_ARG;
		    }
		  break;
	  
		case channel_message_program_change:
	  
		  fts_midiport_input_program_change(port, parser->channel, byte, 0.0);
		  break;
	  
		case channel_message_channel_pressure:		
	  
		  fts_midiport_input_channel_pressure(port, parser->channel, byte, 0.0);
		  break;
	  
		case channel_message_pitch_bend:
	  
		  if (parser->arg == NO_ARG)
		    parser->arg = byte;
		  else
		    {
		      fts_midiport_input_pitch_bend(port, parser->channel, parser->arg + (byte << 7), 0.0);
		      
		      /* reset for running status */
		      parser->arg = NO_ARG;
		    }
		  break;
		}
	    }
	}
      
      break;
      
    case status_sysex:
      
      switch (byte)
	{
	case STATUS_BYTE_SYSEX_END:

	  /* end of sysex */
	  parser->status = status_normal;
	  
	  /* send sysex block */
	  fts_midiport_input_system_exclusive_call(port, 0.0);

	  break;
	  
	case STATUS_BYTE_SYSEX_REALTIME:
	  parser->status = status_sysex_realtime;
	  
	  break;
	  
	default:
	  break;
	}

      break;
	  
    case status_sysex_realtime:
      
      if(byte == STATUS_BYTE_SYSEX_END)
	{
	  /* end of sysex */
	  parser->status = status_normal;
	  
	  /* send sysex block */
	  fts_midiport_input_system_exclusive_call(port, 0.0);
	}
      else if (byte == 0x01)
	{
      	  parser->status = status_sysex_mtc;

	  fts_midiport_input_system_exclusive_byte(port, (int)byte);
	}
      else
	fts_midiport_input_system_exclusive_byte(port, (int)byte);

      break;
      
    case status_sysex_mtc:
      
      if(byte == STATUS_BYTE_SYSEX_END)
	{
	  /* end of sysex */
	  parser->status = status_normal;
	  
	  /* send sysex block */
	  fts_midiport_input_system_exclusive_call(port, 0.0);
	}
      else if (byte == 0x01)
	{
	  parser->status = status_sysex_mtc_frame;
	  parser->mtc_frame_count  = 0;

	  fts_midiport_input_system_exclusive_byte(port, (int)byte);
	}
      else
	fts_midiport_input_system_exclusive_byte(port, (int)byte);
      
      break;
      
    case status_sysex_mtc_frame:
	  
      switch (parser->mtc_frame_count)
	{
	case 0:
	  parser->mtc_type = (byte & 0x60) >> 5;
	  parser->mtc_hour = (byte & 0x1f);
	  break;
	case 1:
	  parser->mtc_min = byte;
	  break;
	case 2:
	  parser->mtc_sec = byte;
	  break;
	case 3:
	  parser->mtc_frame = byte;
	  break;
	default:
	  break;
	}
	  
      parser->mtc_frame_count++;
	  
      if (byte == STATUS_BYTE_SYSEX_END)
	{
	  parser->mtc_status = mtc_status_valid;
	  midiparser_compute_time_code(parser);

	  /* end of sysex */
	  parser->status = status_normal;
	  
	  /* send sysex block */
	  fts_midiport_input_system_exclusive_call(port, 0.0);
	}
      else
	fts_midiport_input_system_exclusive_byte(port, (int)byte);
	  
      break;
	  
    case status_mtc_quarter_frame:
      {
	if ((byte & 0xf0) == 0x00)
	  parser->mtc_status = mtc_status_coming;
	    
	switch ((byte & 0xf0) >> 4)
	  {
	  case 0:
	    if (parser->mtc_status == mtc_status_invalid)
	      parser->mtc_status = mtc_status_coming;
	    else if (parser->mtc_status == mtc_status_coming)
	      parser->mtc_status = mtc_status_valid;
		
	    if (parser->mtc_status == mtc_status_valid)
	      midiparser_compute_time_code(parser);
		
	    parser->mtc_frame = (parser->mtc_frame & 0xf0) | (byte & 0x0f);
	    break;
	  case 1:
	    parser->mtc_frame = (parser->mtc_frame & 0x0f) | (byte & 0x0f) << 4;
	    break;
	  case 2:
	    parser->mtc_sec = (parser->mtc_sec & 0xf0) | (byte & 0x0f);
	    break;
	  case 3:
	    parser->mtc_sec = (parser->mtc_sec & 0x0f) | (byte & 0x0f) << 4;
	    break;
	  case 4:
	    parser->mtc_min = (parser->mtc_min & 0xf0) | (byte & 0x0f);
	    break;
	  case 5:
	    parser->mtc_min = (parser->mtc_min & 0x0f) | (byte & 0x0f) << 4;
	    break;
	  case 6:
	    parser->mtc_hour = (parser->mtc_hour & 0xf0) | (byte & 0x0f);
	    break;
	  case 7:
	    parser->mtc_hour = (parser->mtc_hour & 0x0f) | (byte & 0x01) << 4;
	    parser->mtc_type = (byte & 0x60) >> 5;
	    break;
	  default:
	    break;
	  }
      
	parser->status = status_normal;
      }
      
      break;
      
    default:
      break;
    }
}

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
fts_object_is_midiport(fts_object_t *obj)
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

static fts_midiport_t *default_midiport = 0;

void fts_midiport_set_default( int argc, const fts_atom_t *argv)
{
  fts_object_t *obj;
  fts_atom_t a[1];

  fts_object_new_to_patcher( fts_get_root_patcher(), argc, argv, &obj);

  if (!obj)
    return;

  fts_object_get_prop( obj, fts_s_state, a);

  if ( !fts_is_object( a) || !fts_object_is_midiport( fts_get_object( a)) )
    {
      fts_object_delete_from_patcher( obj);
      return;
    }

  if (default_midiport)
    {
      fts_object_delete_from_patcher( (fts_object_t *)default_midiport);
    }

  default_midiport = (fts_midiport_t *)fts_get_object( a);
}

fts_midiport_t *fts_midiport_get_default(void)
{
  return default_midiport;
}

/************************************************************
 *
 *  Initialization of the midi module
 */ 

void 
fts_kernel_midi_init(void)
{
  fts_s_midiport = fts_new_symbol("midiport");
  fts_s__superclass = fts_new_symbol("_superclass");
}


