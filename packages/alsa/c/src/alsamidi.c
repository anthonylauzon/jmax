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
#include "alsamidi.h"

#define ALSAMIDI_FIFO_SIZE 512
#define NAME_SIZE 64

static void
alsamidi_scan_devices(alsamidi_t *this, int stream, fts_array_t *array)
{
  snd_ctl_card_info_t *cardinfo;
  snd_rawmidi_info_t *midiinfo;
  int card;

  snd_ctl_card_info_alloca(&cardinfo);
  snd_rawmidi_info_alloca(&midiinfo);

  /* iterate over all cards */
  card = -1;
  snd_card_next(&card);
  while(card >= 0) 
    {
      snd_ctl_t *handle;
      char name[NAME_SIZE];
      int device;      
      
      snprintf(name, NAME_SIZE, "hw:%d", card);

      /* open card control */
      if(snd_ctl_open(&handle, name, 0) < 0)
	continue;

      /* get card info */
      if(snd_ctl_card_info(handle, cardinfo) < 0)
	{
	  snd_ctl_close(handle);
	  continue;
	}

      /* iterate over all devices of given card */
      device = -1;
      snd_ctl_rawmidi_next_device(handle, &device);
      while(device >= 0)
	{
	  char device_name[NAME_SIZE], hw_name[NAME_SIZE];
	  fts_symbol_t device_symbol, hw_symbol;
	  int subdevice, n_subdevices;
	  fts_atom_t k, a;
	  
	  snd_rawmidi_info_set_device(midiinfo, device);
	  snd_rawmidi_info_set_subdevice(midiinfo, 0);
	  snd_rawmidi_info_set_stream(midiinfo, stream);

	  if (snd_ctl_rawmidi_info(handle, midiinfo) < 0)
	    continue;

	  snprintf(device_name, NAME_SIZE, "%s %s", snd_ctl_card_info_get_name(cardinfo), snd_rawmidi_info_get_name(midiinfo));
	  snprintf(hw_name, NAME_SIZE, "hw:%d,%d", card, device);

	  device_symbol = fts_new_symbol(device_name);
	  hw_symbol = fts_new_symbol(hw_name);

	  /* enter device to hashtable id not already done */
	  fts_set_symbol(&k, device_symbol);
	  if(!fts_hashtable_get(&this->devices, &k, &a))
	    {
	      fts_set_symbol(&a, hw_symbol);
	      fts_hashtable_put(&this->devices, &k, &a);
	    }

	  /* append name to array */
	  fts_array_append_symbol(array, device_symbol);

	  /* next device */
	  if(snd_ctl_rawmidi_next_device(handle, &device) < 0) 
	    break;
	}

      snd_ctl_close(handle);
      
      /* next card */
      if (snd_card_next(&card) < 0) 
	break;
    }
}

static void
alsamidi_update_inputs(alsamidi_t *this)
{
  if(fts_array_get_size(&this->inputs) == 0)
    alsamidi_scan_devices(this, SND_RAWMIDI_STREAM_INPUT, &this->inputs);
}

static void
alsamidi_update_outputs(alsamidi_t *this)
{
  if(fts_array_get_size(&this->outputs) == 0)
    alsamidi_scan_devices(this, SND_RAWMIDI_STREAM_OUTPUT, &this->outputs);
}

static fts_midiport_t *
alsamidi_get_midiport(alsamidi_t *this, fts_symbol_t name)
{
  fts_midiport_t *port = NULL;
  fts_atom_t k, a;

  fts_set_symbol(&k, name);
  if(fts_hashtable_get(&this->devices, &k, &a)) 
    {
      if(fts_is_object(&a)) 
	port = (fts_midiport_t *)fts_get_object(&a);
      else if(fts_is_symbol(&a))
	{
	  fts_symbol_t hw_name = fts_get_symbol(&a);
	  fts_atom_t args[3];

	  /* create midiport */
	  fts_set_object(args + 0, (fts_object_t *)this);
	  fts_set_symbol(args + 1, name);
	  fts_set_symbol(args + 2, hw_name);
	  port = (fts_midiport_t *)fts_object_create(alsarawmidiport_type, NULL, 3, args);
	}
    }

  return port;
}

static void
alsamidi_get_default_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_symbol_t *name = (fts_symbol_t *)fts_get_pointer(at);

  alsamidi_update_inputs(this);
  
  if(fts_array_get_size(&this->inputs) > 0)
    *name = fts_get_symbol(fts_array_get_element(&this->inputs, 0));
}

static void
alsamidi_get_default_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_symbol_t *name = (fts_symbol_t *)fts_get_pointer(at);

  alsamidi_update_outputs(this);
  
  if(fts_array_get_size(&this->outputs) > 0)
    *name = fts_get_symbol(fts_array_get_element(&this->outputs, 0));
}

static void
alsamidi_append_inputs( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_array_t *inputs = (fts_array_t *)fts_get_pointer(at);

  alsamidi_update_inputs(this);
  fts_array_append(inputs, fts_array_get_size(&this->inputs), fts_array_get_atoms(&this->inputs));
}

static void
alsamidi_append_outputs( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_array_t *outputs = (fts_array_t *)fts_get_pointer(at);

  alsamidi_update_outputs(this);
  fts_array_append(outputs, fts_array_get_size(&this->outputs), fts_array_get_atoms(&this->outputs));
}

static void
alsamidi_get_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_midiport_t **ptr = (fts_midiport_t **)fts_get_pointer(at + 0);
  fts_symbol_t device_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_midiport_t *port = alsamidi_get_midiport(this, device_name);

  if(port && fts_midiport_is_input(port))
    *ptr = port;
}

static void
alsamidi_get_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_midiport_t **ptr = (fts_midiport_t **)fts_get_pointer(at + 0);
  fts_symbol_t device_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_midiport_t *port = alsamidi_get_midiport(this, device_name);

  if(port && fts_midiport_is_output(port))
    *ptr = port;
}

static void
alsamidi_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);
  int i;

  alsamidi_update_inputs(this);
  alsamidi_update_outputs(this);
  
  fts_spost(stream, "ALSA MIDI inputs:\n");
  for(i=0; i<fts_array_get_size(&this->inputs); i++)
    {
      fts_atom_t k, a;

      fts_set_symbol(&k, fts_get_symbol(fts_array_get_element(&this->inputs, i)));
      if(fts_hashtable_get(&this->devices, &k, &a))
	{
	  if(fts_is_object(&a))
	    {
	      alsarawmidiport_t *port = (alsarawmidiport_t *)fts_get_object(&a);
	      fts_spost(stream, "  '%s': active (%s)\n", fts_get_symbol(&k), port->hw_name);
	    }
	  else if(fts_is_symbol(&a))
	    fts_spost(stream, "  '%s': inactive (%s)\n", fts_get_symbol(&k), fts_get_symbol(&a));
	}
    }

  fts_spost(stream, "ALSA MIDI outputs:\n");
  for(i=0; i<fts_array_get_size(&this->outputs); i++)
    {
      fts_atom_t k, a;

      fts_set_symbol(&k, fts_get_symbol(fts_array_get_element(&this->outputs, i)));
      if(fts_hashtable_get(&this->devices, &k, &a))
	{
	  if(fts_is_object(&a))
	    {
	      alsarawmidiport_t *port = (alsarawmidiport_t *)fts_get_object(&a);
	      fts_spost(stream, "  '%s': active (%s)\n", fts_get_symbol(&k), port->hw_name);
	    }
	  else if(fts_is_symbol(&a))
	    fts_spost(stream, "  '%s': inactive (%s)\n", fts_get_symbol(&k), fts_get_symbol(&a));
	}
    }
}

static void
alsamidi_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;

  fts_array_init(&this->inputs, 0, 0);
  fts_array_init(&this->outputs, 0, 0);
  fts_hashtable_init(&this->devices, FTS_HASHTABLE_SMALL);
}

static void
alsamidi_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  alsamidi_t *this = (alsamidi_t *)o;

  fts_array_destroy(&this->inputs);
  fts_array_destroy(&this->outputs);
  fts_hashtable_destroy(&this->devices);
}

static void 
alsamidi_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, NULL, sizeof(alsamidi_t), alsamidi_init, alsamidi_delete);

  /* FTS MIDI manager interface implementation */
  fts_class_message_varargs(cl, fts_midimanager_s_get_default_input, alsamidi_get_default_input);
  fts_class_message_varargs(cl, fts_midimanager_s_get_default_output, alsamidi_get_default_output);
  fts_class_message_varargs(cl, fts_midimanager_s_append_input_names, alsamidi_append_inputs);
  fts_class_message_varargs(cl, fts_midimanager_s_append_output_names, alsamidi_append_outputs);
  fts_class_message_varargs(cl, fts_midimanager_s_get_input, alsamidi_get_input);
  fts_class_message_varargs(cl, fts_midimanager_s_get_output, alsamidi_get_output);

  /* debug print */
  fts_class_message_varargs(cl, fts_s_print, alsamidi_print);
}

void 
alsamidi_config( void)
{
  fts_class_t *mc = fts_class_install(NULL, alsamidi_instantiate);

  fts_midiconfig_add_manager((fts_midimanager_t *)fts_object_create(mc, NULL, 0, 0));
}
