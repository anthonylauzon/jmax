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
 */

#include <fts/fts.h>
#include "alsaseqmidi.h"

#define DEFAULT_MIDI_BS_SIZE 3
#define NAME_SIZE 64
#define INPUT_TYPE 1
#define OUTPUT_TYPE 2

fts_metaclass_t* alsaseqmidiport_input_type = NULL;
fts_metaclass_t* alsaseqmidiport_output_type = NULL;



/* callback for fts_s_sched_ready signal */
static void 
alsaseqmidiport_select( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsaseqmidiport_t *this = (alsaseqmidiport_t *)o;
  alsaseqmidi_t* manager = this->manager;
  
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
	  fts_log("[alsaseqmidiport] need to resize buffer for decoding MIDI stream \n");
	  fts_log("[alsaseqmidiport] event_size : %d, old buffe size: %d\n", event_size, this->buffer_size);
	  this->buffer = realloc(this->buffer, event_size * sizeof(unsigned char));
	  snd_midi_event_resize_buffer(this->midi_event_parser, event_size * sizeof(unsigned char));
	  this->buffer_size = event_size;
      }
      fts_log("[alsaseqmidiport] reset decoder \n");
      snd_midi_event_reset_decode(this->midi_event_parser);
      /* Try to decode event */
      n = snd_midi_event_decode(this->midi_event_parser, this->buffer, this->buffer_size, ev);

      if (0 > n)
      {
	  fts_object_set_error(o, "cannot decode event");
      }
      else
      {
	  fts_log("[alsaseqmidiport] %d bytes decoded \n", n);
	  for ( i = 0; i < n; ++i)
	  {	  
	      fts_midievent_t *event = fts_midiparser_byte( parser, this->buffer[i]);
	      fts_log("[alsaseqmidiport] parse byte %d of %d \n", i, n);      
	      if(event != NULL)
	      {
		  fts_atom_t a;
	      
		  fts_set_object(&a, (fts_object_t *)event);
		  fts_midiport_input(o, 0, 0, 1, &a);
	      }
	  }
      }
  
      fts_log("[alsaseqmidiport] free memory allocated for event \n");
      snd_seq_free_event(ev);
  } while (snd_seq_event_input_pending(this->seq, 0) > 0); 

}

  
static void
alsaseqmidiport_output(fts_object_t* o, fts_midievent_t* event, double time)
{
    alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
    snd_seq_event_t ev;
    long buf_size = 0;

    fts_log("[alsaseqmidiport] output function call \n");
    if (fts_midievent_is_channel_message(event))
    {
	fts_log("[alsaseqmidiport] try to send a channel message \n");
	snd_seq_ev_clear(&ev);
	snd_midi_event_reset_encode(this->midi_event_parser);
	
	if (fts_midievent_channel_message_has_second_byte(event))
	{
	    unsigned char buffer[3];
	    
	    buffer[0] = (unsigned char)(fts_midievent_channel_message_get_status_byte(event));
	    buffer[1] = (unsigned char)(fts_midievent_channel_message_get_first(event) & 0x7f);
	    buffer[2] = (unsigned char)(fts_midievent_channel_message_get_second(event) & 0x7f);

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

	    buf_size = snd_midi_event_encode(this->midi_event_parser, buffer, 3, &ev);
	    /* Check value of buf_size */
	    if (2 != buf_size)
	    {
		fts_log("[alsaseqmidiport] Error while encoding midi message ... \n");
	    }	    
	}
    }
    
    fts_log("[alsaseqmidiport] set subs \n");
    snd_seq_ev_set_subs(&ev);
    fts_log("[alsaseqmidiport] set direct \n");
    snd_seq_ev_set_direct(&ev);
    fts_log("[alsaseqmidiport] set source \n");
    snd_seq_ev_set_source(&ev, this->port_id);
    fts_log("[alsaseqmidiport] event_output_direct \n");
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
    this->buffer = malloc(this->buffer_size * sizeof(unsigned char));
    /* Create midi parser */
    snd_midi_event_new(this->buffer_size * sizeof(unsigned char), &this->midi_event_parser);
    /* Initialize midi parser */
    snd_midi_event_init(this->midi_event_parser);
}

static void
alsaseqmidiport_input_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
    fts_midiparser_t* parser = &this->parser;
    struct pollfd fds;
    int err, fd;
    fts_atom_t k, a;
    char port_name[NAME_SIZE];
    char label_name_str[NAME_SIZE];
    fts_symbol_t device_name;
    fts_symbol_t label_name;


    fts_log("[alsaseqmidiport] alsaseqmidiport_init\n");
    this->manager = (alsaseqmidi_t*)fts_get_object(at);
    label_name = fts_get_symbol(at + 1);
    device_name = fts_get_symbol(at + 2);

    fts_log("[alsaseqmidiport] label name %s\n", label_name);
    fts_log("[alsaseqmidiport] device name %s\n", device_name);

    snd_seq_port_info_malloc(&this->port_info);
    alsaseqmidiport_midi_parser_init(this);

    /* Open handle to ALSA sequencer */
    err = snd_seq_open(&this->seq, "default", SND_SEQ_OPEN_INPUT, 0);
    if (err < 0)
    {
	fts_log("[alsaseqmidiport] cannot open handle to ALSA sequencer \n");
    }
    /* Append jmax: to client name */
    snprintf(label_name_str, NAME_SIZE, "jmax:%s", label_name); 
    this->name = fts_new_symbol_copy(label_name_str);

    /* Set name of the client */
    snd_seq_set_client_name(this->seq, this->name);

    /* Create port */
    fts_log("[alsaseqmidiport] try to create bidirectionnal simple port \n");
    snprintf(port_name, NAME_SIZE, "input");
    if ((err = snd_seq_create_simple_port(this->seq, port_name,
					  SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
					  SND_SEQ_PORT_TYPE_APPLICATION)) < 0)
    {
	fts_object_set_error(o, "Error opening ALSA sequencer MIDI port (%s)", snd_strerror(err));
	fts_log("alsaseqmidiport: cannot open ALSA sequencer MIDI port %s (%s)",port_name, snd_strerror(err)); 
    }
    else
    {
	this->port_id = err;       
	fts_log("[alsaseqmidiport] input simple port created \n");
    }

    snd_seq_get_port_info(this->seq, this->port_id, this->port_info);

    /* Etablish connection */
    alsaseqmidiport_etablish_connection(this, device_name, INPUT_TYPE);
    fts_log("[alsaseqmidiport] connection etablished \n");

    fts_log("[alsaseqmidiport] set poll descriptors \n");
    fts_log("[alsaseqmidiport] number of poll descriptors: %d\n",
	    snd_seq_poll_descriptors_count(this->seq, POLLIN));
    if (0 == snd_seq_poll_descriptors(this->seq, &fds, 1, POLLIN))
    {
	fts_object_set_error(o, "Cannot get file descriptor");
	return;
    }

    this->fd = fds.fd;    
    
    fts_sched_add(o, FTS_SCHED_READ, this->fd);


    fts_midiparser_init(parser);
    fts_midiport_init((fts_midiport_t *)this);
    fts_midiport_set_input((fts_midiport_t *)this);
    
    /* insert into device hashtable */
    fts_log("[alsaseqmidiport] insert into device hashtable \n");
    fts_set_symbol(&k, this->name);
    fts_set_object(&a, o);
    fts_log("[alsaseqmidiport] put object in hashtable with key %s \n", this->name);
    fts_hashtable_put(&this->manager->sources, &k, &a);
/*    fts_hashtable_put(&this->manager->destinations, &k, &a); */
/*    fts_hashtable_put(&this->manager->inputs, &k, &a);    */
/* fts_hashtable_put(&this->manager->outputs, &k, &a); */


    fts_log("[alsaseqmidiport] FOO: end of init \n");
    
}
	

static void
alsaseqmidiport_output_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
    fts_midiparser_t* parser = &this->parser;
    fts_atom_t k, a;
    int err;
    char port_name[NAME_SIZE];
    char label_name_str[NAME_SIZE];
    fts_symbol_t device_name;
    fts_symbol_t label_name;


    fts_log("[alsaseqmidiport] alsaseqmidiport_output_init\n");
    this->manager = (alsaseqmidi_t*)fts_get_object(at);
    label_name = fts_get_symbol(at + 1);
    device_name = fts_get_symbol(at + 2);

    fts_log("[alsaseqmidiport] label name %s\n", label_name);
    fts_log("[alsaseqmidiport] device name %s\n", device_name);

    snd_seq_port_info_malloc(&this->port_info);
    alsaseqmidiport_midi_parser_init(this);

    /* Open handle to ALSA sequencer */

    err = snd_seq_open(&this->seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
    if (err < 0)
    {
	fts_log("[alsaseqmidiport] cannot open handle to ALSA sequencer \n");
    }

    /* Append jmax: to client name */
    snprintf(label_name_str, NAME_SIZE, "jmax:%s", label_name);
    this->name = fts_new_symbol_copy(label_name_str);

    /* Set name of the client */
    snd_seq_set_client_name(this->seq, this->name);

    /* Create port */
    snprintf(port_name, NAME_SIZE, "output");
    fts_log("[alsaseqmidiport] try to create output simple port \n");
    if ((err = snd_seq_create_simple_port(this->seq, port_name,
					  SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
					  SND_SEQ_PORT_TYPE_APPLICATION)) < 0)
    {
	fts_object_set_error(o, "[alsaseqmidiport] Error opening ALSA sequencer MIDI port (%s)", snd_strerror(err));
	fts_log("[alsaseqmidiport] cannot create ALSA sequencer MIDI port %s (%s)",port_name, snd_strerror(err)); 
    }
    else
    {
	this->port_id = err;       
	fts_log("[alsaseqmidiport] input simple port created \n");
    }

    snd_seq_get_port_info(this->seq, this->port_id, this->port_info);
    
    /* Etablish connection */
    alsaseqmidiport_etablish_connection(this, device_name, OUTPUT_TYPE);
    fts_log("[alsaseqmidiport] connection established \n");

    fts_midiparser_init(parser);
    fts_midiport_init((fts_midiport_t*) this);
    fts_midiport_set_output((fts_midiport_t *)this, alsaseqmidiport_output);
    
    /* insert into device hashtable */
    fts_log("[alsaseqmidiport] insert into device hashtable \n");
    fts_set_symbol(&k, this->name);
    fts_set_object(&a, o);
    fts_log("[alsaseqmidiport] put object in hashtable with key %s \n", this->name);
/*    fts_hashtable_put(&this->manager->sources, &k, &a); */
    fts_hashtable_put(&this->manager->destinations, &k, &a);
/*    fts_hashtable_put(&this->manager->inputs, &k, &a); */
/*    fts_hashtable_put(&this->manager->outputs, &k, &a); */



    fts_log("[alsaseqmidiport] FOO: end of init \n");
    
}
	

static void
alsaseqmidiport_sub_delete(alsaseqmidiport_t* this)
{
    free(this->buffer);
    snd_midi_event_free(this->midi_event_parser);
    snd_seq_port_info_free(this->port_info);
    snd_seq_unsubscribe_port(this->seq, this->subs);
    snd_seq_delete_port(this->seq, this->port_id);
    snd_seq_close(this->seq);
}

static void
alsaseqmidiport_input_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
    fts_midiparser_t* parser = &this->parser;
    fts_sched_remove(o);
    fts_midiparser_reset(parser);
    alsaseqmidiport_sub_delete(this);
}

static void
alsaseqmidiport_output_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidiport_t* this = (alsaseqmidiport_t*)o;
    fts_midiparser_t* parser = &this->parser;
    fts_midiparser_reset(parser);
    alsaseqmidiport_sub_delete(this);
}


static fts_status_t
alsaseqmidiport_input_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    fts_class_init(cl, sizeof(alsaseqmidiport_t), 0, 0, 0);

    fts_log("[alsaseqmidiport] alsaseqmidiport_input_instantiate\n");
    fts_midiport_class_init(cl);

    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, alsaseqmidiport_input_init);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, alsaseqmidiport_input_delete);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sched_ready, alsaseqmidiport_select);

    return fts_Success;
}


static fts_status_t
alsaseqmidiport_output_instantiate(fts_class_t* cl, int ac, const fts_atom_t* at)
{
    fts_class_init(cl, sizeof(alsaseqmidiport_t), 0, 0, 0);

    fts_log("[alsaseqmidiport] alsaseqmidiport_output_instantiate\n");
    fts_midiport_class_init(cl);

    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, alsaseqmidiport_output_init);
    fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, alsaseqmidiport_output_delete);

    return fts_Success;
}


void 
alsaseqmidiport_config(void)
{
    alsaseqmidiport_input_type = fts_class_install(fts_new_symbol("alsaseqmidiport_input"), alsaseqmidiport_input_instantiate);
    alsaseqmidiport_output_type = fts_class_install(fts_new_symbol("alsaseqmidiport_output"), alsaseqmidiport_output_instantiate);
}
