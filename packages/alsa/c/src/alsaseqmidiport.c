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
#include "alsaseqmidi.h"

/* #define ALSA_SEQ_MIDI_PORT_LOG 1 */


#define DEFAULT_MIDI_BS_SIZE 3
#define NAME_SIZE 64
#define INPUT_TYPE 1
#define OUTPUT_TYPE 2


fts_class_t* alsaseqmidiport_input_type = NULL;
fts_class_t* alsaseqmidiport_output_type = NULL;



/* callback for fts_s_sched_ready signal */
static void 
alsaseqmidiport_select (fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
{
  alsaseqmidiport_t *this = (alsaseqmidiport_t *)o;
  
  fts_midiparser_t *parser = &this->parser;
  int n, i;
  int event_size = 0;
  snd_seq_event_t* ev;

  do 
  {  
    /* Retreive an event from sequencer */
    snd_seq_event_input(this->seq, &ev);
  
    /* Get size of the encoded bytes stream */
    event_size = snd_seq_event_length(ev);
    /* Check if buffer_size is enough */
    if (event_size > this->buffer_size)
    {
#ifdef ALSA_SEQ_MIDI_PORT_LOG
      fts_log("[alsaseqmidiport] Resize buffer for decoding MIDI stream : %d, old buffe size: %d\n", event_size, this->buffer_size);
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
      this->buffer = realloc(this->buffer, event_size * sizeof(unsigned char));
      snd_midi_event_resize_buffer(this->midi_event_parser, event_size * sizeof(unsigned char));
      this->buffer_size = event_size;
    }
    snd_midi_event_reset_decode(this->midi_event_parser);
    /* Try to decode event */
    n = snd_midi_event_decode(this->midi_event_parser, this->buffer, this->buffer_size, ev);

    if (0 > n)
    {
      fts_object_error(o, "[alsaseqmidiport] cannot decode event");
    }
    else
    {
#ifdef ALSA_SEQ_MIDI_PORT_LOG
      fts_log("[alsaseqmidiport] %d bytes decoded \n", n);
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
      for ( i = 0; i < n; ++i)
      {	  
	fts_midievent_t *event = fts_midiparser_byte( parser, this->buffer[i]);
	if(event != NULL)
	{
	  fts_atom_t a, ret;
	      
	  fts_set_object(&a, (fts_object_t *)event);

	  fts_object_refer((fts_object_t *)event);
	  fts_midiport_input(o, 0, 1, &a, &ret);
	  fts_object_release((fts_object_t *)event);
	}
      }
    }  
    snd_seq_free_event(ev);
  } while (snd_seq_event_input_pending(this->seq, 0) > 0); 

}

  
static void
alsaseqmidiport_output(fts_object_t* o, fts_midievent_t* event, double time)
{
  alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
  snd_seq_event_t ev;
  long buf_size = 0;

  if (fts_midievent_is_channel_message(event))
  {
    snd_seq_ev_clear(&ev);
    snd_midi_event_reset_encode(this->midi_event_parser);
	
    if (fts_midievent_channel_message_has_second_byte(event))
    {
      unsigned char buffer[3];
	    
      buffer[0] = (unsigned char)(fts_midievent_channel_message_get_status_byte(event));
      buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);
      buffer[2] = (unsigned char)(fts_midievent_channel_message_get_second(event) & 0x7f);

      /* Encode MIDI bytecode to alsa sequencer event structure */
      buf_size = snd_midi_event_encode(this->midi_event_parser, buffer, 3, &ev);
      /* Check value of buf_size */
      if (3 != buf_size)
      {
	fts_log("[alsaseqmidiport] Error while encoding midi message ... \n");
      }
    }	
    else
    {
      unsigned char buffer[2];
	    	    
      buffer[0] = (unsigned char)fts_midievent_channel_message_get_status_byte(event);
      buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);

      /* Encode MIDI bytecode to alsa sequencer event structure */
      buf_size = snd_midi_event_encode(this->midi_event_parser, buffer, 3, &ev);
      /* Check value of buf_size */
      if (2 != buf_size)
      {
	fts_log("[alsaseqmidiport] Error while encoding midi message ... \n");
      }	    
    }
  }
  else
  {
    switch(fts_midievent_get_type(event))
    {
      case midi_song_position_pointer:
      {
        unsigned char buffer[3];
        
        buffer[0] = (unsigned char)fts_midievent_song_position_pointer_status_byte;
        buffer[1] = (unsigned char)(fts_midievent_song_position_pointer_get_first(event) & 0x7f);
        buffer[2] = (unsigned char)(fts_midievent_song_position_pointer_get_second(event) & 0x7f);

	/* Encode MIDI bytecode to alsa sequencer event structure */
	buf_size = snd_midi_event_encode(this->midi_event_parser, buffer, 3, &ev);
	/* Check value of buf_size */
	if (3 != buf_size)
	{
	  fts_log("[alsaseqmidiport] Error while encoding midi message ... \n");
	}	
      }
      break;

    case midi_song_select:
    {
      unsigned char buffer[2];
      
      buffer[0] = (unsigned char)fts_midievent_song_select_status_byte;
      buffer[1] = (unsigned char)(fts_midievent_song_select_get(event) & 0x7f);
      
      /* Encode MIDI bytecode to alsa sequencer event structure */
      buf_size = snd_midi_event_encode(this->midi_event_parser, buffer, 2, &ev);
      /* Check value of buf_size */
      if (2 != buf_size)
      {
	fts_log("[alsaseqmidiport] Error while encoding midi message ... \n");
      }	
    }
    break;
    
    default:
      break;
    }
  }
    
  snd_seq_ev_set_subs(&ev);
  snd_seq_ev_set_direct(&ev);
  snd_seq_ev_set_source(&ev, this->port_id);
  snd_seq_event_output_direct(this->seq, &ev);
}

/*
  Etablish connection between current midiport and port with device_name name 
*/
static int 
alsaseqmidiport_etablish_connection(alsaseqmidiport_t* this, fts_symbol_t device_name, int port_type)
{
  int err;
  snd_seq_addr_t addr_to_connect;
  const snd_seq_addr_t* obj_addr;

  obj_addr = snd_seq_port_info_get_addr(this->port_info);
  snd_seq_port_subscribe_alloca(&this->subs);

  err = snd_seq_parse_address(this->seq, &addr_to_connect, device_name);
  if (err < 0)
  {
    fts_log("[alsaseqmidiport] cannot parse address \n");
    return -1;
  }

  if (INPUT_TYPE == port_type)
  {
    snd_seq_port_subscribe_set_sender(this->subs, &addr_to_connect);
    snd_seq_port_subscribe_set_dest(this->subs, obj_addr);
  }
  else
  {
    snd_seq_port_subscribe_set_sender(this->subs, obj_addr);
    snd_seq_port_subscribe_set_dest(this->subs, &addr_to_connect);
  }
    
  err = snd_seq_get_port_subscription(this->seq, this->subs);
  if (0 == err) 
  {
    fts_log("[alsaseqmidi] Connection is already subscribed\n");
    return -2;
  }
    
  err = snd_seq_subscribe_port(this->seq, this->subs);
  if (err < 0) 
  {
    fts_log("[alsaseqmidiport] Connection failed (%s)\n", snd_strerror(errno));
    return -3;
  }
			
  return 0;
}


static void
alsaseqmidiport_midi_parser_init(alsaseqmidiport_t* this)
{
  this->buffer_size = DEFAULT_MIDI_BS_SIZE;
  this->buffer = fts_malloc(this->buffer_size * sizeof(unsigned char));
  /* Create midi parser */
  snd_midi_event_new(this->buffer_size * sizeof(unsigned char), &this->midi_event_parser);
  /* Initialize midi parser */
  snd_midi_event_init(this->midi_event_parser);
}

static void
alsaseqmidiport_midi_parser_delete(alsaseqmidiport_t* this)
{
  if (NULL != this->buffer)
  {
    fts_free(this->buffer);
    this->buffer= NULL;
  }
  this->buffer_size = 0;
  snd_midi_event_free(this->midi_event_parser);
}

static int
alsaseqmidiport_io_init(alsaseqmidiport_t* this, 
			fts_symbol_t device_name, 
			fts_symbol_t label_name, 
			fts_symbol_t port_address, 
			int port_type)
{
  int err = 0;
  fts_midiparser_t* parser = &this->parser;
  fts_atom_t k;
  fts_atom_t a;
  char port_name[NAME_SIZE];
  char label_name_str[NAME_SIZE];

  snd_seq_port_info_malloc(&this->port_info);
  alsaseqmidiport_midi_parser_init(this);

  /* Open handle to ALSA sequencer */
  if (INPUT_TYPE == port_type)
  {
    err = snd_seq_open(&this->seq, "default", SND_SEQ_OPEN_INPUT, 0);
  }
  else
  {
    err = snd_seq_open(&this->seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
  }

  if (err < 0)
  {
    fts_log("[alsaseqmidiport] alsaseqmidiport_io_init: Cannot open handle to ALSA sequencer \n");
  }

  /* Append jMax: to client name */
  snprintf(label_name_str, NAME_SIZE - 1, "jMax:%s", label_name); 
  label_name_str[NAME_SIZE - 1] = '\0';
  /* Set name of the client */
  snd_seq_set_client_name(this->seq, label_name_str);


  if (INPUT_TYPE == port_type)
  {
    /* Create port */
    snprintf(port_name, NAME_SIZE - 1, "input");
    port_name[NAME_SIZE - 1] = '\0';
    err = snd_seq_create_simple_port(this->seq, port_name,
				     SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
				     SND_SEQ_PORT_TYPE_APPLICATION);
    snprintf(label_name_str + strlen(label_name_str), 
	     NAME_SIZE - strlen(label_name_str) - 1, 
	     ":%s", port_name);
    label_name_str[NAME_SIZE - 1] = '\0';
    this->name = fts_new_symbol(label_name_str);

  }
  else
  {
    /* Create port */
    snprintf(port_name, NAME_SIZE - 1, "output");
    port_name[NAME_SIZE - 1] = '\0';
    err = snd_seq_create_simple_port(this->seq, port_name,
				     SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
				     SND_SEQ_PORT_TYPE_APPLICATION);
    snprintf(label_name_str + strlen(label_name_str), 
	     NAME_SIZE - 1 - strlen(label_name_str) - 1, 
	     ":%s", port_name);
    label_name_str[NAME_SIZE - 1] = '\0';
    this->name = fts_new_symbol(label_name_str);

  }
  if (err < 0)
  {
    fts_object_error((fts_object_t*)this, "error opening ALSA sequencer MIDI port (%s)", snd_strerror(err));
    fts_log("alsaseqmidiport: cannot open ALSA sequencer MIDI port %s (%s)",port_name, snd_strerror(err)); 
  }
  else
  {
    this->port_id = err;       
#ifdef ALSA_SEQ_MIDI_PORT_LOG
    fts_log("[alsaseqmidiport] input simple port created \n");
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
  }
	
  snd_seq_get_port_info(this->seq, this->port_id, this->port_info);

  /* Etablish connection */
  alsaseqmidiport_etablish_connection(this, port_address, port_type);
#ifdef ALSA_SEQ_MIDI_PORT_LOG
  fts_log("[alsaseqmidiport] connection etablished \n");
#endif /* ALSA_SEQ_MIDI_PORT_LOG */


  if (INPUT_TYPE == port_type)
  {
    fts_midiparser_init(parser);
  }
  fts_midiport_init((fts_midiport_t *)this);
  if (INPUT_TYPE == port_type)
  {
    fts_midiport_set_input((fts_midiport_t *)this);
  }
  else
  {
    fts_midiport_set_output((fts_midiport_t*)this, alsaseqmidiport_output);
  }
    
  /* insert into device hashtable */
#ifdef ALSA_SEQ_MIDI_PORT_LOG
  fts_log("[alsaseqmidiport] insert into device hashtable \n");
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
  fts_set_symbol(&k, this->name);
  fts_set_object(&a, (fts_object_t*)this);
#ifdef ALSA_SEQ_MIDI_PORT_LOG
  fts_log("[alsaseqmidiport] put object in hashtable with key %s \n", this->name);
#endif /* ALSA_SEQ_MIDI_PORT_LOG */

  if (INPUT_TYPE == port_type)
  {	
/*	fts_hashtable_put(&this->manager->destinations, &k, &a); */
    fts_hashtable_put(&this->manager->inputs, &k, &a);
  }
  else
  {
/*	fts_hashtable_put(&this->manager->sources, &k, &a); 	*/
    fts_hashtable_put(&this->manager->outputs, &k, &a);
  }

  return err;
}

static void
alsaseqmidiport_input_init (fts_object_t* o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t *ret)
{
  alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
  alsaseqmidi_t* manager;
  struct pollfd fds;
  int err;
  fts_symbol_t device_name;
  fts_symbol_t label_name;
  fts_symbol_t port_address;

#ifdef ALSA_SEQ_MIDI_PORT_LOG
  fts_log("[alsaseqmidiport] alsaseqmidiport_init\n");
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
  manager = (alsaseqmidi_t*)fts_get_object(at);
  label_name = fts_get_symbol(at + 1);
  device_name = fts_get_symbol(at + 2);
  port_address = fts_get_symbol(at + 3);

  this->manager = manager;
  this->seq = NULL;
  this->midi_event_parser = NULL;
  this->port_info = NULL;
  this->subs = NULL;

#ifdef ALSA_SEQ_MIDI_PORT_LOG    
  fts_log("[alsaseqmidiport] label name %s\n", label_name);
  fts_log("[alsaseqmidiport] device name %s\n", device_name);
  fts_log("[alsaseqmidiport] port address %s\n", port_address);
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
  err = alsaseqmidiport_io_init(this, device_name, label_name, port_address, INPUT_TYPE);

  if (0 == snd_seq_poll_descriptors(this->seq, &fds, 1, POLLIN))
  {
    fts_object_error(o, "cannot get file descriptor");
    return;
  }

  this->fd = fds.fd;    
    
  fts_sched_add(o, FTS_SCHED_READ, this->fd);
    
}
	

static void
alsaseqmidiport_output_init (fts_object_t* o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t *ret)
{
  alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
  alsaseqmidi_t* manager;
  int err;
  fts_symbol_t device_name;
  fts_symbol_t label_name;
  fts_symbol_t port_address;


  manager = (alsaseqmidi_t*)fts_get_object(at);
  label_name = fts_get_symbol(at + 1);
  device_name = fts_get_symbol(at + 2);
  port_address = fts_get_symbol(at + 3);

  this->manager = manager;
  this->seq = NULL;
  this->midi_event_parser = NULL;
  this->port_info = NULL;
  this->subs = NULL;

#ifdef ALSA_SEQ_MIDI_PORT_LOG
  fts_log("[alsaseqmidiport] label name %s\n", label_name);
  fts_log("[alsaseqmidiport] device name %s\n", device_name);
  fts_log("[alsaseqmidiport] port address %s\n", port_address);
#endif /* ALSA_SEQ_MIDI_PORT_LOG */
  err = alsaseqmidiport_io_init(this, device_name, label_name, port_address, OUTPUT_TYPE);
}
	

static void
alsaseqmidiport_io_delete(alsaseqmidiport_t* this, int port_type)
{
  fts_hashtable_t* ht;
  fts_atom_t k;

  fts_midiparser_t* parser = &this->parser;
  alsaseqmidiport_midi_parser_delete(this);
  snd_seq_port_info_free(this->port_info);
  snd_seq_unsubscribe_port(this->seq, this->subs);
  snd_seq_delete_port(this->seq, this->port_id);
  snd_seq_close(this->seq);

  /* Remove MIDIport from sources/destinations hashtables */    
  if (INPUT_TYPE == port_type)
  {
    fts_midiparser_reset(parser);
/*	ht = &this->manager->destinations; */
  }
  else
  {
/*	ht = &this->manager->sources; */
  }
#if 0
  fts_set_symbol(&k, this->name);
  fts_hashtable_remove(ht, &k);
#endif

  /* Remove MIDIport from inputs/outputs hashtables */    
  if (INPUT_TYPE == port_type)
  {
    ht = &this->manager->inputs;
  }
  else
  {
    ht = &this->manager->outputs;
  }
    
  fts_set_symbol(&k, this->name);
  fts_hashtable_remove(ht, &k);

  fts_midiport_reset((fts_midiport_t*)this);
}

static void
alsaseqmidiport_input_delete (fts_object_t* o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t *ret)
{
  alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
  alsaseqmidiport_io_delete(this, INPUT_TYPE);
  fts_sched_remove(o);
}

static void
alsaseqmidiport_output_delete (fts_object_t* o, fts_symbol_t s, int ac, const fts_atom_t* at, fts_atom_t *ret)
{
  alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
  alsaseqmidiport_io_delete(this, OUTPUT_TYPE);
}


static void
alsaseqmidiport_input_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(alsaseqmidiport_t), alsaseqmidiport_input_init, alsaseqmidiport_input_delete);

  fts_class_message_varargs(cl, fts_s_sched_ready, alsaseqmidiport_select);
}


static void
alsaseqmidiport_output_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(alsaseqmidiport_t), alsaseqmidiport_output_init, alsaseqmidiport_output_delete);
}


void 
alsaseqmidiport_config(void)
{
  alsaseqmidiport_input_type = fts_class_install(fts_new_symbol("alsaseqmidiport_input"), alsaseqmidiport_input_instantiate);
  alsaseqmidiport_output_type = fts_class_install(fts_new_symbol("alsaseqmidiport_output"), alsaseqmidiport_output_instantiate);
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
