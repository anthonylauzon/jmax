/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/fts.h>
#include "alsamidi.h"

fts_class_t *alsarawmidiport_type = NULL;

static void 
alsarawmidiport_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  fts_midiparser_t *parser = &this->parser;
  int n, i;

  n = snd_rawmidi_read( this->handle_in, this->receive_buffer, ALSA_SYSEX_BUFFER_LENGTH);
  if ( n <= 0 && n != -EAGAIN)
    {
      fts_post( "snd_rawmidi_read returns %d !!!\n", n);
      return;
    }

  for ( i = 0; i < n; i++)
    {
      fts_midievent_t *event = fts_midiparser_byte( parser, this->receive_buffer[i]);

      if(event != NULL)
	{
	  fts_atom_t a;

	  fts_set_object(&a, (fts_object_t *)event);

	  fts_object_refer((fts_object_t *)event);
	  fts_midiport_input(o, 0, 0, 1, &a);
	  fts_object_release((fts_object_t *)event);
	}
    }
}

static void
alsarawmidiport_output(fts_object_t *o, fts_midievent_t *event, double time)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;

  if(fts_midievent_is_channel_message(event))
    {
      if(fts_midievent_channel_message_has_second_byte(event))
	{
	  unsigned char buffer[3];

	  buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
	  buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);
	  buffer[2] = (unsigned char)(fts_midievent_channel_message_get_second(event) & 0x7f);

	  snd_rawmidi_write( this->handle_out, buffer, 3);
	}
      else
	{
	  unsigned char buffer[2];

	  buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
	  buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);

	  snd_rawmidi_write(this->handle_out, buffer, 2);
	}
    }
  else 
    {
      switch(fts_midievent_get_type(event))
	{
	case midi_system_exclusive:
	  {
	    unsigned char buffer[ALSA_SYSEX_BUFFER_LENGTH];
	    int size = fts_midievent_system_exclusive_get_size(event);
	    fts_atom_t *atoms = fts_midievent_system_exclusive_get_atoms(event);
	    int i, n;
	    
	    buffer[0] = STATUS_BYTE_SYSEX;
	    
	    for(i=0, n=1; i<size; i++)
	      {
		buffer[n++] = fts_get_int(atoms + i) & 0x7f;
		
		if(n == ALSA_SYSEX_BUFFER_LENGTH)
		  {
		    snd_rawmidi_write( this->handle_out, buffer, ALSA_SYSEX_BUFFER_LENGTH);
		    n = 0;
		  }
	      }
	    
	    buffer[n++] = STATUS_BYTE_SYSEX_END;
	    snd_rawmidi_write(this->handle_out, buffer, n);
	  }
	  break;
	  
	case midi_real_time:
	  {
	    unsigned char byte = (unsigned char)fts_midievent_real_time_get_status_byte(event);
	    snd_rawmidi_write(this->handle_out, &byte, 1);
	  }
	  break;
	  
	default:
	  break;
	}
    }
}

static void 
alsarawmidiport_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  fts_midiparser_t *parser = &this->parser;
  struct pollfd fds;
  int err, fd;
  fts_atom_t k, a;
  
  this->manager = (alsamidi_t *)fts_get_object(at);
  this->name = fts_get_symbol(at + 1);
  this->hw_name = fts_get_symbol(at + 2);

  if( (err = snd_rawmidi_open( &this->handle_in, &this->handle_out, this->hw_name, O_RDWR | SND_RAWMIDI_NONBLOCK)) < 0)
    {
      fts_object_error(o, "error opening ALSA raw MIDI port (%s)", snd_strerror( err));
      fts_log("alsarawmidiport: cannot open ALSA raw MIDI port %s (%s)\n", this->name, snd_strerror( err));
      return;
    }

  if ( snd_rawmidi_poll_descriptors( this->handle_in, &fds, 1) == 0)
    {
      fts_object_error(o, "cannot get file descriptor");
      return;
    }

  this->fd = fds.fd;

  fts_sched_add(o, FTS_SCHED_READ, this->fd);

  fts_midiparser_init(parser);
  fts_midiport_init((fts_midiport_t *)this);
  fts_midiport_set_input((fts_midiport_t *)this);
  fts_midiport_set_output((fts_midiport_t *)this, alsarawmidiport_output);

  /* insert into device hashtable */
  fts_set_symbol(&k, this->name);
  fts_set_object(&a, o);
  fts_hashtable_put(&this->manager->devices, &k, &a);
  
  this->sysex_head = 0;
}

static void
alsarawmidiport_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsarawmidiport_t *this = (alsarawmidiport_t *)o;
  fts_midiparser_t *parser = &this->parser;
  fts_atom_t k, a;
  
  fts_sched_remove(o);
  fts_midiparser_reset(parser);
  fts_midiport_reset((fts_midiport_t *)this);

  if(this->handle_in)
    {
      snd_rawmidi_drain( this->handle_in); 
      snd_rawmidi_close( this->handle_in);	
    }

  if( this->handle_out)
    {
      snd_rawmidi_drain( this->handle_out); 
      snd_rawmidi_close( this->handle_out);	
    }

  /* replace midiport by hardware name in device hashtable */
  fts_set_symbol(&k, this->name);
  fts_set_symbol(&a, this->hw_name);
  fts_hashtable_put(&this->manager->devices, &k, &a);
}

static void 
alsarawmidiport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof( alsarawmidiport_t), alsarawmidiport_init, alsarawmidiport_delete);

  fts_class_message_varargs(cl, fts_s_sched_ready, alsarawmidiport_select);
}

void 
alsarawmidiport_config( void)
{
  alsarawmidiport_type = fts_class_install(NULL, alsarawmidiport_instantiate);
}
