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

#define LIST_INPUT 1
#define LIST_OUTPUT 2

#define NAME_SIZE 64
#define ADDRESS_SIZE 64

#define perm_ok(pinfo,bits) ((snd_seq_port_info_get_capability(pinfo) & (bits)) == (bits))

static fts_symbol_t alsaseqmidi_symbol_jmax_prefix;
static fts_symbol_t alsaseqmidi_symbol_default_unset;
static fts_symbol_t alsaseqmidi_symbol_alsaseq_midi_source;
static fts_symbol_t alsaseqmidi_symbol_alsaseq_midi_destination;

/*
  Check permission of given port
  Extract from aconnect.c (alsa-utils)
*/
static int check_permission(snd_seq_port_info_t *pinfo, int perm)
{
    if (perm) {
	if (perm & LIST_INPUT) {
	    if (perm_ok(pinfo, SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ))
		goto __ok;
	}
	if (perm & LIST_OUTPUT) {
	    if (perm_ok(pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE))
		goto __ok;
	}
	return 0;
    }
__ok:
    if (snd_seq_port_info_get_capability(pinfo) & SND_SEQ_PORT_CAP_NO_EXPORT)
	return 0;
    return 1;
}


/*
  This function open an handle to ALSA Sequencer and loop on client and port 
  to know which are the port with correct perm.
  This code is inspired by aconnect.c (alsa-utils)
*/
static void
alsaseqmidi_scan_clients(alsaseqmidi_t* this, int perm, fts_hashtable_t* ht)
{
    snd_seq_client_info_t* cinfo;
    snd_seq_port_info_t* pinfo;
    int err;

    snd_seq_client_info_alloca(&cinfo);
    snd_seq_port_info_alloca(&pinfo);
    snd_seq_client_info_set_client(cinfo, -1);
    /* client loop query */
    while (snd_seq_query_next_client(this->seq, cinfo) >= 0) 
    {
	/* reset query info */
	snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
	snd_seq_port_info_set_port(pinfo, -1);
	/* current client port loop query */
	while (snd_seq_query_next_port(this->seq, pinfo) >= 0) 
	{
	    /* Check if port is open with the wanted perm */
	    if (check_permission(pinfo, perm)) 
	    {
		char* port_name;
		fts_symbol_t port_name_symbol;
		char* address;
		fts_symbol_t address_symbol;
		fts_atom_t k, a;
		
		port_name = calloc(NAME_SIZE, sizeof(char));
		address = calloc(NAME_SIZE, sizeof(char));
		/* Get client name */
		snprintf(port_name, NAME_SIZE, "%s", snd_seq_client_info_get_name(cinfo));
		/* Get client port name */
		snprintf(port_name + strlen(port_name), NAME_SIZE - strlen(port_name) - 1, "::%s", snd_seq_port_info_get_name(pinfo));
		port_name_symbol = fts_new_symbol(port_name);

		/* Get client ID */
		snprintf(address, ADDRESS_SIZE, "%d", snd_seq_client_info_get_client(cinfo));
		/* Get port ID */
		snprintf(address + strlen(address), ADDRESS_SIZE - strlen(address) - 1, ":%d", snd_seq_port_info_get_port(pinfo));
		address_symbol = fts_new_symbol(address);

		fts_set_symbol(&k, port_name_symbol);
		/* Insert in hashtable if not already done */
		if (!fts_hashtable_get(ht, &k, &a))
		{
		    fts_set_symbol(&a, address_symbol);
		    fts_hashtable_put(ht, &k, &a);
		}
		free(address);
		free(port_name);		
	    }
	}
    }
}


static void
alsaseqmidi_update_inputs(alsaseqmidi_t* this)
{
    alsaseqmidi_scan_clients(this, LIST_INPUT, &this->destinations);
}


static void
alsaseqmidi_update_outputs(alsaseqmidi_t* this)
{
    alsaseqmidi_scan_clients(this, LIST_OUTPUT, &this->sources);
}


static fts_symbol_t
alsaseqmidi_get_default_io(alsaseqmidi_t* this, fts_hashtable_t* ht)
{
    fts_iterator_t keys;
    fts_iterator_t values;
    fts_atom_t k;
    fts_atom_t a;
    
    fts_hashtable_get_keys(ht, &keys);
    fts_hashtable_get_values(ht, &values);
    if (fts_iterator_has_more(&keys))
    {
	fts_iterator_next(&keys, &k);
	fts_iterator_next(&values, &a);
	return fts_get_symbol(&k);
    }
    else
    {
	return alsaseqmidi_symbol_default_unset;
    }
}

static void
alsaseqmidi_get_default_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;
    fts_symbol_t* name = (fts_symbol_t*)fts_get_pointer(at);

    alsaseqmidi_update_inputs(this);
    *name = alsaseqmidi_get_default_io(this, &this->inputs);
}


static void
alsaseqmidi_get_default_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;
    fts_symbol_t* name = (fts_symbol_t*)fts_get_pointer(at);


    alsaseqmidi_update_outputs(this);

    *name = alsaseqmidi_get_default_io(this, &this->outputs);
}


/* 
   MIDI Configuration pop up update 
*/
static void
alsaseqmidi_append_io(alsaseqmidi_t* this, fts_array_t* array, fts_hashtable_t* ht)
{
    fts_iterator_t keys, values;
    fts_atom_t k, a;
    int i;
    fts_hashtable_get_keys(ht, &keys);
    fts_hashtable_get_values(ht, &values);
    
    while (fts_iterator_has_more(&keys))
    {
	fts_iterator_next(&keys, &k);
	fts_iterator_next(&values, &a);

	if (fts_is_symbol(&a))
	{
	    fts_array_append_symbol(array, fts_get_symbol(&k));
	}
    }
}

static void
alsaseqmidi_append_inputs(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;
    fts_array_t* inputs = (fts_array_t*)fts_get_pointer(at);

    alsaseqmidi_update_inputs(this);
    alsaseqmidi_append_io(this, inputs, &this->destinations);
}


static void
alsaseqmidi_append_outputs(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;
    fts_array_t* outputs = (fts_array_t*)fts_get_pointer(at);

    alsaseqmidi_update_outputs(this);
    alsaseqmidi_append_io(this, outputs, &this->sources);
}



static fts_midiport_t*
alsaseqmidi_create_midiport(alsaseqmidi_t* this, fts_class_t* cl, fts_symbol_t device_name, fts_symbol_t label_name, fts_symbol_t port_address)
{
    fts_object_t* port = NULL;
    fts_atom_t args[4];    
    
    /* Create midiport */
    fts_set_object(args + 0, (fts_object_t*) this);
    fts_set_symbol(args + 1, label_name);
    fts_set_symbol(args + 2, device_name);
    fts_set_symbol(args + 3, port_address);
    fts_log("[alsaseqmidi] Create alsaseqmidiport label_name: %s, device_name: %s, port_address: %s\n", label_name, device_name, port_address);
    
    port = fts_object_create(cl, NULL, 4, args);

    return (fts_midiport_t*)port;
}


static void
alsaseqmidi_get_io(alsaseqmidi_t* this, fts_class_t* alsaseqmidiport_type, const fts_atom_t* at, fts_hashtable_t* ht, fts_symbol_t* default_port)
{
    fts_midiport_t** ptr = (fts_midiport_t**)fts_get_pointer(at + 0);
    fts_symbol_t device_name = fts_get_symbol(at + 1);
    fts_symbol_t label_name = fts_get_symbol(at + 2);    
    fts_symbol_t port_address;
    fts_atom_t k,a;

    fts_log("[alsaseqmidi] alsaseqmidi_get_io; device name %s, label name %s\n",
	    device_name, label_name);

    fts_set_symbol(&k, device_name);
    if (fts_hashtable_get(ht, &k, &a))       
    {
	fts_log("[alsaseqmidi] symbol %s in hashtable \n", device_name);
	if (fts_is_object(&a))
	{
	    alsaseqmidiport_t* port = (alsaseqmidiport_t*)fts_get_object(&a);
	    fts_log("[alsaseqmidi] get current alsaseqmidiport \n");
	    /* ADD a check to know if midiport is still valid */
	    *ptr = (fts_midiport_t*)port;
	}
	else if (fts_is_symbol(&a))
	{
	    fts_midiport_t* port;
	    fts_log("[alsaseqmidi] create alsaseqmidiport \n");
	    port_address = fts_get_symbol(&a);
	    /* Create midiport */
	    port = alsaseqmidi_create_midiport(this, alsaseqmidiport_type, device_name,
					       label_name, port_address);
	    if (port)
	    {
		*ptr = port;
	    }	    
	}
	
    }
    else
    {
	fts_log("[alsaseqmidi] suymbol %s not in hashtable \n", device_name);
    }

/*
  if (alsaseqmidi_symbol_default_unset == *default_port)
  {
  *default_port = device_name;
  }
*/
}


static void
alsaseqmidi_get_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;

    alsaseqmidi_get_io(this, alsaseqmidiport_input_type, at, &this->destinations, 
		       &this->default_input);
}


static void
alsaseqmidi_get_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;

    alsaseqmidi_get_io(this, alsaseqmidiport_output_type, at, &this->sources, &this->default_output);
}



static void
alsaseqmidi_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*) o;
    int err;
    char client_name[NAME_SIZE];
    fts_hashtable_init(&this->inputs, fts_symbol_class, FTS_HASHTABLE_SMALL);
    fts_hashtable_init(&this->outputs, fts_symbol_class, FTS_HASHTABLE_SMALL);
    fts_hashtable_init(&this->sources, fts_symbol_class, FTS_HASHTABLE_SMALL);
    fts_hashtable_init(&this->destinations, fts_symbol_class, FTS_HASHTABLE_SMALL);

    /* Open a client to sequencer */
    err = snd_seq_open(&this->seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
    if (err < 0)
    {
	fts_object_set_error(o, "cannot create ALSA sequencer client ...");
	fts_log("[alsaseqmidi] cannot create ALSA sequencer handle\n");
	return;
    }
    fts_log("[alsaseqmidi] ALSA sequencer handle created \n");
    
    snprintf(client_name, NAME_SIZE, "jMax_alsa::");
    snd_seq_set_client_name(this->seq, client_name);
    this->client_name = fts_new_symbol(client_name);    
    fts_log("[alsaseqmidi] ALSA sequencer client name set (%s)\n", client_name);
    
}

static void
alsaseqmidi_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;

    fts_hashtable_destroy(&this->inputs);
    fts_hashtable_destroy(&this->outputs);
    fts_hashtable_destroy(&this->sources);
    fts_hashtable_destroy(&this->destinations);

}


static void
alsaseqmidi_scan_hashtable(fts_hashtable_t* ht)
{
    fts_iterator_t keys;
    fts_iterator_t values;
    fts_atom_t k;
    fts_atom_t a;

    fts_hashtable_get_keys(ht, &keys);
    fts_hashtable_get_values(ht, &values);

    while (fts_iterator_has_more(&keys))
    {
	fts_symbol_t name;
	
	fts_iterator_next(&keys, &k);
	fts_iterator_next(&values, &a);
	
	name = fts_get_symbol(&k);

	if (fts_is_object(&a))
	{
	    alsaseqmidiport_t* port = (alsaseqmidiport_t*)fts_get_object(&a);
	    snd_seq_t* seq = port->seq;
	    if (seq != NULL)
	    {
		post("hashtabke keys: %s \t %s has a valid ALSA sequencer handle \n",
		     name, port->name);
	    }
	    else
	    {
		post("hashtable keys: %s \t %s has not a valid ALSA sequencer handle \n",
		     name, port->name);
	    }
	}
	else
	{
	    if (fts_is_symbol(&a))
	    {
		post("hashtable keys: %s \t values is not an object, is a symbol %s \n", 
		     name, fts_get_symbol(&a));
	    }
	    else
	    {
		post("hashtable keys: %s \t values is not an object nor a symbol \n", name);
	    }
	}
    }
    
    
}
static void
alsaseqmidi_print(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
    alsaseqmidi_t* this = (alsaseqmidi_t*)o;
    fts_bytestream_t *stream = fts_post_get_stream(ac, at);
    
    fts_spost(stream, "\n");
    fts_spost(stream, "ALSA Sequencer MIDI manager inputs\n");
    alsaseqmidi_scan_hashtable(&this->inputs);
    
    fts_spost(stream, "\n");
    fts_spost(stream, "ALSA Sequencer MIDI manager outputs\n");
    alsaseqmidi_scan_hashtable(&this->outputs);  
    fts_spost(stream, "\n");
    fts_spost(stream, "ALSA Sequencer MIDI manager declared sources\n");
    alsaseqmidi_scan_hashtable(&this->sources);
    
    fts_spost(stream, "\n");
    fts_spost(stream, "ALSA Sequencer MIDI manager declared destination\n");
    alsaseqmidi_scan_hashtable(&this->destinations);
}

static void
alsaseqmidi_instantiate(fts_class_t* cl)
{
    fts_class_init(cl, sizeof(alsaseqmidi_t), alsaseqmidi_init, alsaseqmidi_delete);

    /* FTS MIDI manager interface implementation */
    fts_class_message_varargs(cl, fts_midimanager_s_get_default_input, alsaseqmidi_get_default_input);
    fts_class_message_varargs(cl, fts_midimanager_s_get_default_output, alsaseqmidi_get_default_output);
    fts_class_message_varargs(cl, fts_midimanager_s_append_input_names, alsaseqmidi_append_inputs);
    fts_class_message_varargs(cl, fts_midimanager_s_append_output_names, alsaseqmidi_append_outputs);
    fts_class_message_varargs(cl, fts_midimanager_s_get_input, alsaseqmidi_get_input);
    fts_class_message_varargs(cl, fts_midimanager_s_get_output, alsaseqmidi_get_output);

    /* debug print */
    fts_class_message_varargs(cl, fts_s_print, alsaseqmidi_print);
}

void
alsaseqmidi_config(void)
{
    fts_class_t* mc = fts_class_install(fts_new_symbol("alsaseqmidi"), alsaseqmidi_instantiate);
    fts_object_t* o;
    alsaseqmidi_symbol_jmax_prefix = fts_new_symbol("jMax");
    alsaseqmidi_symbol_default_unset = fts_new_symbol("default unset");
    alsaseqmidi_symbol_alsaseq_midi_destination = fts_new_symbol("ALSA Destination");
    alsaseqmidi_symbol_alsaseq_midi_source = fts_new_symbol("ALSA Source");

    o = fts_object_create(mc, NULL, 0, 0);

    if (o != NULL)
      fts_midiconfig_add_manager((fts_midimanager_t*)o);
}








