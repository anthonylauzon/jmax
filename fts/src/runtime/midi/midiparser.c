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
#include "runtime/devices.h"
#include "runtime/sched.h"
#include "runtime/time.h"
#include "midiport.h"
#include "midiparser.h"

/* MIDI status bytes */
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

/************************************************************
 *
 *  bytestream input callback
 *
 */

void
fts_midiparser_init(fts_midiparser_t *parser)
{  
  parser->status = status_normal;
  parser->message = 0;
  parser->channel = 0;	
  parser->arg = NO_ARG;

  parser->mtc_status = mtc_invalid;
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

  if (new_time < parser->mtc_time)
    fts_clock_reset(fts_new_symbol("mtc"));

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
	  parser->mtc_status = mtc_valid;
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
	  parser->mtc_status = mtc_coming;
	    
	switch ((byte & 0xf0) >> 4)
	  {
	  case 0:
	    if (parser->mtc_status == mtc_invalid)
	      parser->mtc_status = mtc_coming;
	    else if (parser->mtc_status == mtc_coming)
	      parser->mtc_status = mtc_valid;
		
	    if (parser->mtc_status == mtc_valid)
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
