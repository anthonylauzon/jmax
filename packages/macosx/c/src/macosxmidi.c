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

#include <stdio.h>
#include <stdlib.h>

#include <fts/fts.h>
#include "macosxmidi.h"

static fts_symbol_t macosxmidi_symbol_iac_midi_source;
static fts_symbol_t macosxmidi_symbol_iac_midi_destination;

static int
macosxmidi_reference_get_id(MIDIEndpointRef ref)
{
  SInt32 id;
  MIDIObjectGetIntegerProperty((MIDIObjectRef)ref, kMIDIPropertyUniqueID, &id);
  return (int)id;
}

static fts_symbol_t
macosxmidi_reference_get_name(MIDIEndpointRef ref)
{
  CFStringRef cfsr;
  MIDIObjectGetStringProperty((MIDIObjectRef)ref, kMIDIPropertyName, &cfsr);  
  return fts_new_symbol_copy(CFStringGetCStringPtr(cfsr, CFStringGetSystemEncoding()));
}

/*
 MIDIEndpointRef
 macosxmidi_get_reference_by_id(int id, macosxmidi_type_t type)
 {
   MIDIObjectRef obj = NULL;
   MIDIObjectType got_type;

   MIDIObjectFindByUniqueID(id, &obj, &got_type);

   if(type == macosxmidi_any || (got_type & 0x0f) == type)
     return obj;
   else
     return NULL;
 }
 */

MIDIEndpointRef
macosxmidi_get_reference_by_id(int id, macosxmidi_type_t type)
{
  MIDIEndpointRef obj;
  int i;

  if(type == macosxmidi_any || type == macosxmidi_input) {
    for(i=0; i<MIDIGetNumberOfSources(); i++) {
      obj = MIDIGetSource(i);

      if(macosxmidi_reference_get_id(obj) == id)
        return obj;
    }
  }

  if(type == macosxmidi_any || type == macosxmidi_output) {
    for(i=0; i<MIDIGetNumberOfDestinations(); i++) {
      obj = MIDIGetDestination(i);

      if(macosxmidi_reference_get_id(obj) == id)
        return obj;
    }
  }

  return NULL;
}

static void
macosxmidi_hash_remove_invalid(fts_hashtable_t *ht, macosxmidi_type_t type)
{
  fts_iterator_t keys, values;
  fts_array_t invalid;
  fts_atom_t k, a;
  int i;

  fts_array_init(&invalid, 0, 0);
  
  fts_hashtable_get_keys(ht, &keys);
  fts_hashtable_get_values(ht, &values);

  /* kill invalid ids and midiports */
  while(fts_iterator_has_more(&keys)) {
    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &a);

    if(fts_is_int(&a)) {
      int id = fts_get_int(&a);

      /* schedule key of invalid ids for removal */
      if(macosxmidi_get_reference_by_id(id, type) == NULL)
        fts_array_append(&invalid, 1, &k);
    }
  }

  /* remove invalid id from hashtable */
  for(i=0; i<fts_array_get_size(&invalid); i++)
    fts_hashtable_remove(ht, fts_array_get_element(&invalid, i));

  fts_array_destroy(&invalid);
}

static int
macosxmidi_hash_get_id(fts_hashtable_t *ht, fts_atom_t *key)
{
  fts_atom_t a;

  if(fts_hashtable_get(ht, key, &a)) {
    if(fts_is_int(&a))
      return fts_get_int(&a);
    else if(fts_is_object(&a))
      return macosxmidiport_get_id((macosxmidiport_t *)fts_get_object(&a));
  }

  return 0;
}

/* hash utility */
static fts_symbol_t
macosxmidi_hash_insert_id(fts_hashtable_t *ht, fts_symbol_t name, int id)
{
  fts_atom_t a;
  fts_atom_t k;

  fts_set_symbol(&k, name);

  if(!fts_hashtable_get(ht, &k, &a)) {
    fts_set_int(&a, id);
    fts_hashtable_put(ht, &k, &a);
  } else if(macosxmidi_hash_get_id(ht, &k) != id) {
    const char *str = name;
    int len = strlen(str);
    char *new_str = alloca((len + 10) * sizeof(char));
    int num = 0;
    int dec = 1;
    int i;

    /* separate base name and index */
    for(i=len-1; i>=0; i--)  {
      if(len == (i + 1) && str[i] >= '0' && str[i] <= '9')
        num += (str[len = i] - '0') * dec;
      else
        new_str[i] = str[i];

      dec *= 10;
    }

    /* generate new key */
    while(fts_hashtable_get(ht, &k, &a) && macosxmidi_hash_get_id(ht, &k) != id) {
      sprintf(new_str + len, "%d", ++num);
      fts_set_symbol(&k, fts_new_symbol_copy(new_str));
    }

    /* put with new key */
    fts_set_int(&a, id);
    fts_hashtable_put(ht, &k, &a);
  }

  return fts_get_symbol(&k);
}

static fts_symbol_t
macosxmidi_insert_reference(fts_hashtable_t *ht, MIDIEndpointRef ref)
{
  return macosxmidi_hash_insert_id(ht, macosxmidi_reference_get_name(ref), macosxmidi_reference_get_id(ref));
}

static fts_midiport_t *
macosxmidi_create_midiport(macosxmidi_t *this, fts_metaclass_t *mcl, fts_symbol_t name, int id)
{
  fts_object_t *port = NULL;
  fts_atom_t args[3];

  /* create midiport */
  fts_set_object(args + 0, (fts_object_t *)this);
  fts_set_symbol(args + 1, name);
  fts_set_int(args + 2, id);
  port = fts_object_create(mcl, 2 + (id != 0), args);

  if(port == NULL || fts_object_get_error(port) == NULL)
    return (fts_midiport_t *)port;
  else {
    fts_object_destroy(port);
    return NULL;
  }
}

static void
macosxmidi_get_default_devices( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_midiport_t **input = (fts_midiport_t **)fts_get_pointer(at + 0);
  fts_symbol_t *input_name = (fts_symbol_t *)fts_get_pointer(at + 1);
  fts_midiport_t **output = (fts_midiport_t **)fts_get_pointer(at + 2);
  fts_symbol_t *output_name = (fts_symbol_t *)fts_get_pointer(at + 3);

  if(*input == NULL && MIDIGetNumberOfSources() > 0) {
    MIDIEndpointRef ref = MIDIGetSource(0);
    fts_symbol_t name = macosxmidi_insert_reference(&this->inputs, ref);
    *input = macosxmidi_create_midiport(this, macosxmidi_input_type, name, macosxmidi_reference_get_id(ref));
    *input_name = name;
  }

  if(*output == NULL && MIDIGetNumberOfDestinations() > 0) {
    MIDIEndpointRef ref = MIDIGetDestination(0);
    fts_symbol_t name = macosxmidi_insert_reference(&this->outputs, ref);
    *output = macosxmidi_create_midiport(this, macosxmidi_output_type, name, macosxmidi_reference_get_id(ref));
    *output_name = name;
  }
}

static void
macosxmidi_append_device_names( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_array_t *inputs = (fts_array_t *)fts_get_pointer(at + 0);
  fts_array_t *outputs = (fts_array_t *)fts_get_pointer(at + 1);
  int i;

  macosxmidi_hash_remove_invalid(&this->inputs, macosxmidi_input);
  macosxmidi_hash_remove_invalid(&this->outputs, macosxmidi_output);

  for(i=0; i<MIDIGetNumberOfSources(); i++) {
    MIDIEndpointRef ref = MIDIGetSource(i);
    fts_symbol_t name = macosxmidi_insert_reference(&this->inputs, ref);
    fts_array_append_symbol(inputs, name);
  }

  for(i=0; i<MIDIGetNumberOfDestinations(); i++) {
    MIDIEndpointRef ref = MIDIGetDestination(i);
    fts_symbol_t name = macosxmidi_insert_reference(&this->outputs, ref);
    fts_array_append_symbol(outputs, name);
  }

  fts_array_append_symbol(inputs, macosxmidi_symbol_iac_midi_destination);
  fts_array_append_symbol(outputs, macosxmidi_symbol_iac_midi_source);
}

static void
macosxmidi_get_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_midiport_t **ptr = (fts_midiport_t **)fts_get_pointer(at + 0);
  fts_symbol_t device_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_atom_t k, a;

  if(device_name == macosxmidi_symbol_iac_midi_destination) {
    fts_set_symbol(&k, label_name);
    
    if(fts_hashtable_get(&this->destinations, &k, &a)) {
      /* take midiport from Mac OS X virtual MIDI destination hashtable */
      *ptr = (fts_midiport_t *)fts_get_object(&a);
    } else {
      /* create Mac OS X virtual MIDI destination */
      *ptr = macosxmidi_create_midiport(this, macosxmidi_input_type, label_name, 0);
    }
  } else {
    fts_set_symbol(&k, device_name);
    
    if(fts_hashtable_get(&this->inputs, &k, &a)) {
      if(fts_is_object(&a)) {
        /* take midiport from inputs hashtable */
        macosxmidiport_t *port = (macosxmidiport_t *)fts_get_object(&a);
        int id = macosxmidiport_get_id(port);

        /* check if midiport is still valid */
        if(macosxmidi_get_reference_by_id(id, macosxmidi_input) == macosxmidiport_get_reference(port))
          *ptr = (fts_midiport_t *)port;
      } else if(fts_is_int(&a)) {
        /* create Mac OS X MIDI port */
        *ptr = macosxmidi_create_midiport(this, macosxmidi_input_type, device_name, fts_get_int(&a));
      }
    }
  }
}

static void
macosxmidi_get_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_midiport_t **ptr = (fts_midiport_t **)fts_get_pointer(at + 0);
  fts_symbol_t device_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_atom_t k, a;
  
  if(device_name == macosxmidi_symbol_iac_midi_source) {
    fts_set_symbol(&k, label_name);

    if(fts_hashtable_get(&this->sources, &k, &a)) {
      /* take midiport from Mac OS X virtual MIDI source hashtable */
      *ptr = (fts_midiport_t *)fts_get_object(&a);
    } else {
      /* create Mac OS X virtual MIDI source */
      *ptr = macosxmidi_create_midiport(this, macosxmidi_output_type, label_name, 0);
    }
  } else {
    fts_set_symbol(&k, device_name);
    
    if(fts_hashtable_get(&this->outputs, &k, &a)) {
      if(fts_is_object(&a)) {
        /* take midiport from inputs hashtable */
        macosxmidiport_t *port = (macosxmidiport_t *)fts_get_object(&a);
        int id = macosxmidiport_get_id(port);

        /* check if midiport is still valid */
        if(macosxmidi_get_reference_by_id(id, macosxmidi_output) == macosxmidiport_get_reference(port))
          *ptr = (fts_midiport_t *)port;
      } else
        /* create Mac OS X MIDI port */
        *ptr = macosxmidi_create_midiport(this, macosxmidi_output_type, device_name, fts_get_int(&a));
    }
  }
}

static void
macosxmidi_scan_hashtable(fts_hashtable_t *ht)
{
  fts_iterator_t keys, values;
  fts_atom_t k, a;

  fts_hashtable_get_keys(ht, &keys);
  fts_hashtable_get_values(ht, &values);
  
  while(fts_iterator_has_more(&keys)) {
    fts_symbol_t name;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &a);

    name = fts_get_symbol(&k);

    if(fts_is_int(&a)) {
      int id = fts_get_int(&a);
      if(macosxmidi_get_reference_by_id(id, macosxmidi_any) != NULL)
        post("  %s inactive (%d)\n", name, id);
      else
        post("  %s invalid id: %d!!\n", name, id);
    } else if(fts_is_object(&a)) {
      macosxmidiport_t *port = (macosxmidiport_t *)fts_get_object(&a);
      int id = macosxmidiport_get_id(port);
      if(macosxmidi_get_reference_by_id(id, macosxmidi_any) != NULL)
        post("  %s active (%d)\n", name, id);
      else
        post("  %s invalid midiport!! (%d)\n", name, id);
    } else 
      post("  %s ivalid atom in hashtable\n", name);
  }
}

static void
macosxmidi_state( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  int i, n;

  n = MIDIGetNumberOfSources();
  post("\n");
  post("Mac OS X proposes %d sources(s)\n", n);
  for(i=0; i<n; i++) {
    MIDIEndpointRef endpoint = MIDIGetSource(i);
    MIDIEntityRef entity = NULL;
    MIDIDeviceRef device = NULL;
    
    post("  %d: '%s' (%d)", i, macosxmidi_reference_get_name(endpoint), macosxmidi_reference_get_id(endpoint));
    post("\n");
  }

  n = MIDIGetNumberOfDestinations();
  post("\n");
  post("Mac OS X proposes %d destinations(s)\n", n);
  for(i=0; i<n; i++) {
    MIDIEndpointRef endpoint = MIDIGetDestination(i);
    MIDIEntityRef entity = NULL;
    MIDIDeviceRef device = NULL;

    post("  %d: '%s' (%d)", i, macosxmidi_reference_get_name(endpoint), macosxmidi_reference_get_id(endpoint));
    post("\n");
  }

  post("\n");
  post("Mac OS X MIDI manager inputs\n");
  macosxmidi_scan_hashtable(&this->inputs);

  post("\n");
  post("Mac OS X MIDI manager outputs\n");
  macosxmidi_scan_hashtable(&this->outputs);

  post("\n");
  post("Mac OS X MIDI manager declared sources\n");
  macosxmidi_scan_hashtable(&this->sources);

  post("\n");
  post("Mac OS X MIDI manager declared destinations\n");
  macosxmidi_scan_hashtable(&this->destinations);
}

static void
macosxmidi_notify(const MIDINotification *message, void *o)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  SInt32 id = message->messageID;
  ByteCount size = message->messageSize;

  /* use fifo to notify change */
}

static void
macosxmidi_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  int i;

  fts_hashtable_init(&this->inputs, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&this->outputs, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&this->sources, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&this->destinations, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
   
  MIDIClientCreate(CFSTR("jMax"), macosxmidi_notify, (void *)o, &this->client);
}

static void
macosxmidi_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;

  MIDIClientDispose(this->client);
}

static fts_status_t 
macosxmidi_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(macosxmidi_t), 1, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, macosxmidi_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, macosxmidi_delete);

  fts_midimanager_class_init(cl);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_midimanager_s_get_default_devices, macosxmidi_get_default_devices);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_midimanager_s_append_device_names, macosxmidi_append_device_names);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_midimanager_s_get_input, macosxmidi_get_input);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_midimanager_s_get_output, macosxmidi_get_output);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_state, macosxmidi_state);

  return fts_Success;
}

static void
mm_redirect( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_message((fts_object_t *)fts_midimanager_get(), fts_SystemInlet, s, ac, at);
}

/* temporary object for debugging */
static fts_status_t
mm_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_object_t), 1, 0, 0);
  fts_method_define_varargs(cl, 0, fts_s_anything, mm_redirect);
  return fts_Success;
}
  
void 
macosxmidi_config( void)
{
  macosxmidi_symbol_iac_midi_source = fts_new_symbol("IAC Source");
  macosxmidi_symbol_iac_midi_destination = fts_new_symbol("IAC Destination");
  
  fts_metaclass_t *mc = fts_class_install(fts_new_symbol("macosxmidi"), macosxmidi_instantiate);
  fts_midimanager_t *mm = (fts_midimanager_t *)fts_object_create(mc, 0, 0);

  fts_class_install(fts_new_symbol("mm"), mm_instantiate);
  
  fts_midimanager_set(mm);
}
