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
#include <CoreMIDI/MIDIServices.h>

#include <fts/fts.h>
#include "macosxmidi.h"

static void 
macosxmidi_bang( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  CFStringRef str;
  SInt32 id;
  const char *name;
  int n_dest, n_src, n_dev, n_ent;
  int i, j, k;

  n_src = MIDIGetNumberOfSources();
  post("%d sources\n", n_src);

  for(i=0; i<n_src; i++)
    {
      MIDIEndpointRef src = MIDIGetSource(i);

      MIDIObjectGetStringProperty((MIDIObjectRef)src, kMIDIPropertyName, &str);
      MIDIObjectGetIntegerProperty((MIDIObjectRef)src, kMIDIPropertyUniqueID, &id);
      name = CFStringGetCStringPtr(str, 0);

      post("  %d: %s (%d)\n", i, name, id);
    }

  n_dest = MIDIGetNumberOfDestinations();
  post("%d destinations\n", n_dest);

  for(i=0; i<n_dest; i++)
    {
      MIDIEndpointRef dst = MIDIGetDestination(i);

      MIDIObjectGetStringProperty((MIDIObjectRef)dst, kMIDIPropertyName, &str);
      MIDIObjectGetIntegerProperty((MIDIObjectRef)dst, kMIDIPropertyUniqueID, &id);
      name = CFStringGetCStringPtr(str, 0);

      post("  %d: %s (%d)\n", i, name, id);
    }

  n_dev = MIDIGetNumberOfDevices();
  post("%d devices: \n", n_dev);

  for(i=0; i<n_dev; i++)
    {
      MIDIDeviceRef dev;

      dev = MIDIGetDevice(i);
      n_ent = MIDIDeviceGetNumberOfEntities(dev);

      MIDIObjectGetStringProperty((MIDIObjectRef)dev, kMIDIPropertyName, &str);
      MIDIObjectGetIntegerProperty((MIDIObjectRef)dev, kMIDIPropertyUniqueID, &id);
      name = CFStringGetCStringPtr(str, 0);

      post("  %d: %s (%d) (%d entities)\n", i, name, id, n_ent);

      for(j=0; j<n_ent; j++)
	{
	  MIDIEntityRef ent = MIDIDeviceGetEntity(dev, j);

	  MIDIObjectGetStringProperty((MIDIObjectRef)ent, kMIDIPropertyName, &str);
	  MIDIObjectGetIntegerProperty((MIDIObjectRef)ent, kMIDIPropertyUniqueID, &id);
	  name = CFStringGetCStringPtr(str, 0);

	  post("    %d: %s (%d)\n", j, name, id);
	}
    }
}

MIDIObjectRef *
macosxmidi_get_by_unique_id(int id, MIDIObjectType exspect)
{
  MIDIObjectRef obj = NULL;
  MIDIObjectType type;
  
  MIDIObjectFindByUniqueID(id, &obj, &type);

  if(exspect == MACOSXMIDI_OBJECT_TYPE_ANY || (type & 0x0f) == exspect)
    return obj;
  else
    return NULL;
}

static void
macosxmidi_check_references(fts_hashtable_t *ht)
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

      /* remind key of invalid id in order to remove from hashtable */
      if(macosxmidi_get_by_unique_id(id, MACOSXMIDI_OBJECT_TYPE_ANY) == NULL)
        fts_array_append(&invalid, 1, &k);
    }
  }

  for(i=0; i<fts_array_get_size(&invalid); i++)
    fts_hashtable_remove(ht, fts_array_get_element(&invalid, i));

  fts_array_destroy(&invalid);
}

static fts_symbol_t
macosxmidi_insert_reference(fts_hashtable_t *ht, MIDIObjectRef ref)
{
  SInt32 id;
  CFStringRef cfsr;
  fts_symbol_t name;
  fts_atom_t k, a;

  MIDIObjectGetIntegerProperty((MIDIObjectRef)ref, kMIDIPropertyUniqueID, &id);
  MIDIObjectGetStringProperty(ref, kMIDIPropertyName, &cfsr);
  name = fts_new_symbol_copy(CFStringGetCStringPtr(cfsr, 0));

  fts_set_symbol(&k, name);

  if(!fts_hashtable_get(ht, &k, &a)) {
    fts_set_int(&a, (int)id);
    fts_hashtable_put(ht, &k, &a);
  }

  return name;
}

static fts_symbol_t
macosxmidi_get_port_name(macosxmidiport_t *port)
{
  fts_symbol_t name = fts_s_none;

  if(port == NULL)
    name = fts_s_none;
  else if(fts_object_get_metaclass((fts_object_t *)port) == fts_midiport_type)
    name = fts_s_internal;
  else
    name = macosxmidiport_get_name(port);

  if(name == NULL)
    name = fts_s_export;

  return name;
}

static void
macosxmidi_update_sources( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  int n, i;

  macosxmidi_check_references(&this->sources);
  
  /* send source names to client */
  fts_client_start_message(o, fts_s_sources);
  fts_client_add_symbol(o, fts_s_none);
  
  n = MIDIGetNumberOfSources();
  for(i=0; i<n; i++) {
    MIDIObjectRef ref = (MIDIObjectRef)MIDIGetSource(i);
    fts_symbol_t name = macosxmidi_insert_reference(&this->sources, ref);
    fts_client_add_symbol(o, name);
  }
  
  fts_client_add_symbol(o, fts_s_internal);
  fts_client_add_symbol(o, fts_s_export);
  fts_client_done_message( o);
}

static void
macosxmidi_update_destinations( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_atom_t *atoms;
  int n, i;

  macosxmidi_check_references(&this->destinations);
  
  /* send source names to client */
  fts_client_start_message(o, fts_s_destinations);
  fts_client_add_symbol(o, fts_s_none);

  n = MIDIGetNumberOfDestinations();
  for(i=0; i<n; i++) {
    MIDIObjectRef ref = (MIDIObjectRef)MIDIGetDestination(i);
    fts_symbol_t name = macosxmidi_insert_reference(&this->destinations, ref);
    fts_client_add_symbol(o, name);
  }

  fts_client_add_symbol(o, fts_s_internal);
  fts_client_add_symbol(o, fts_s_export);
  fts_client_done_message( o);
}

static void
macosxmidi_update( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;

  fts_midilabel_t *label = fts_midimanager_get_labels(mm);
  int n = fts_midimanager_get_n_labels(mm);
  int i;

  /* upload labels with inputs and outputs */
  for(i=0; i<n; i++) {
    fts_symbol_t name = fts_midilabel_get_name(label);
    macosxmidiport_t *input = (macosxmidiport_t *)fts_midilabel_get_input(label);
    macosxmidiport_t *output = (macosxmidiport_t *)fts_midilabel_get_output(label);

    /* check input */
    if(input != NULL) {
      int id = macosxmidiport_get_id(input);

      if(id != 0 && macosxmidi_get_by_unique_id(id, kMIDIObjectType_Source) == NULL) {
        fts_midilabel_set_input(label, NULL, fts_s_none);
        input = NULL;
      }
    }

    /* check output */
    if(output != NULL) {
      int id = macosxmidiport_get_id(output);

      if(id != 0 && macosxmidi_get_by_unique_id(id, kMIDIObjectType_Destination) == NULL) {
        fts_midilabel_set_output(label, NULL, fts_s_none);
        output = NULL;
      }
    }

    label = fts_midilabel_get_next(label);
  }

  /* upload sources and destinations */
  macosxmidi_update_sources(o, 0, 0, 0, 0);
  macosxmidi_update_destinations(o, 0, 0, 0, 0);
}

static void
macosxmidi_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  fts_midilabel_t *label = fts_midimanager_get_labels(mm);
  int n = fts_midimanager_get_n_labels(mm);
  int i;

  macosxmidi_update(o, 0, 0, 0, 0);

  /* upload labels with inputs and outputs */
  for(i=0; i<n; i++) {
    fts_symbol_t name = fts_midilabel_get_name(label);
    macosxmidiport_t *input = (macosxmidiport_t *)fts_midilabel_get_input(label);
    macosxmidiport_t *output = (macosxmidiport_t *)fts_midilabel_get_output(label);
    fts_atom_t a[4];

    fts_set_int(a, i);
    fts_set_symbol(a + 1, name);
    fts_set_symbol(a + 2, macosxmidi_get_port_name(input));
    fts_set_symbol(a + 3, macosxmidi_get_port_name(output));
    fts_client_send_message(o, fts_s_insert, 4, a);

    label = fts_midilabel_get_next(label);
  }
}

/* insert <index> <name> */
static void
macosxmidi_insert_label( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);

  /* check if name is not already used */
  if(fts_midimanager_get_label_by_name(mm, name) != NULL)
    name = fts_midimanager_get_fresh_label_name(mm, name);

  /* insert label and send to client */
  fts_midimanager_insert_label_at_index(mm, index, name);
  fts_client_send_message(o, fts_s_insert, 2, at);
}

/* remove <index> */
static void
macosxmidi_remove_label( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  int index = fts_get_int(at);

  /* remove label and send to client */
  fts_midimanager_remove_label_at_index(mm, index);
  fts_client_send_message(o, fts_s_remove, 1, at);
}

static void
macosxmidi_set_input( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, index);

  /* reset output to none if it is set to internal */
  if(macosxmidi_get_port_name((macosxmidiport_t *)fts_midilabel_get_output(label)) == fts_s_internal) {
    
    /* no need to change internal to internal */
    if(name == fts_s_internal)
      return;

    fts_midimanager_reset_output(mm, index);
  }

  if (name == fts_s_none){
    fts_midimanager_reset_input(mm, index);
  } else if(name == fts_s_internal) {
    fts_midimanager_set_internal(mm, index);
  } else if(name == fts_s_export) {
    fts_object_t *port;
    fts_atom_t args[2];

    /* create Mac OS X MIDI destination */
    fts_set_object(args + 0, o);
    fts_set_symbol(args + 1, fts_midilabel_get_name(label));
    port = fts_object_create(macosxmidi_input_type, 2, args);

    if(fts_object_get_error(port) == NULL) {
      fts_midimanager_set_input(mm, index, (fts_midiport_t *)port, name);
      macosxmidi_update_destinations(o, 0, 0, 0, 0);
    } else {
      fts_object_destroy(port);
      fts_midimanager_reset_input(mm, index);
    }
    
  } else {
    fts_atom_t k, a;

    fts_set_symbol(&k, name);
    if(fts_hashtable_get(&this->sources, &k, &a)) {
  
      if(fts_is_object(&a)) {
        fts_midimanager_set_input(mm, index, (fts_midiport_t *)fts_get_object(&a), name);
      } else {
        /* create new midiport */
        int id = fts_get_int(&a);
        fts_object_t *port;
        fts_atom_t args[3];

        fts_set_object(args + 0, o);
        fts_set_symbol(args + 1, name);
        fts_set_int(args + 2, id);
        port = fts_object_create(macosxmidi_input_type, 3, args);

        if(fts_object_get_error(port) == NULL) {
          fts_midimanager_set_input(mm, index, (fts_midiport_t *)port, name);
        } else {
          fts_object_destroy(port);
          fts_midimanager_reset_input(mm, index);
        }
      }
    }
  }

  macosxmidi_update(o, 0, 0, 0, 0);
}

static void
macosxmidi_set_output( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, index);

  /* reset input to none if it is set to internal */
  if(macosxmidi_get_port_name((macosxmidiport_t *)fts_midilabel_get_input(label)) == fts_s_internal) {
    
    /* no need to change internal to internal */
    if(name == fts_s_internal)
      return;

    fts_midimanager_reset_input(mm, index);
  }

  if (name == fts_s_none){
    fts_midimanager_reset_output(mm, index);
  } else if(name == fts_s_internal) {
    fts_midimanager_set_internal(mm, index);
  } else if(name == fts_s_export) {
    fts_object_t *port;
    fts_atom_t args[2];

    /* create Mac OS X MIDI destination */
    fts_set_object(args + 0, o);
    fts_set_symbol(args + 1, fts_midilabel_get_name(label));
    port = fts_object_create(macosxmidi_output_type, 2, args);

    if(fts_object_get_error(port) == NULL) {
      fts_midimanager_set_output(mm, index, (fts_midiport_t *)port, name);
      macosxmidi_update_sources(o, 0, 0, 0, 0);
    } else {
      fts_object_destroy(port);
      fts_midimanager_reset_output(mm, index);
    }
    
  } else {
    fts_atom_t k, a;

    fts_set_symbol(&k, name);
    if(fts_hashtable_get(&this->destinations, &k, &a)) {
      
      if(fts_is_object(&a)) {
        fts_midimanager_set_output(mm, index, (fts_midiport_t *)fts_get_object(&a), name);
      } else {
        /* create new midiport */
        int id = fts_get_int(&a);
        fts_object_t *port;
        fts_atom_t args[3];

        fts_set_object(args + 0, o);
        fts_set_symbol(args + 1, name);
        fts_set_int(args + 2, id);
        port = fts_object_create(macosxmidi_output_type, 3, args);

        if(fts_object_get_error(port) == NULL) {
          fts_midimanager_set_output(mm, index, (fts_midiport_t *)port, name);
        } else {
          fts_object_destroy(port);
          fts_midimanager_reset_output(mm, index);
        }
      }
    }
  }

  macosxmidi_update(o, 0, 0, 0, 0);
}

/* restore <name> <source id> <destination id> */
static void
macosxmidi_restore_label( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{

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
      
      if(macosxmidi_get_by_unique_id(id, MACOSXMIDI_OBJECT_TYPE_ANY) != NULL)
        post("  %s inactive (%d)\n", name, id);
      else
        post("  %s invalid id!!\n", name);
      
    } else if(fts_is_object(&a)) {
      macosxmidiport_t *port = (macosxmidiport_t *)fts_get_object(&a);
      int id = macosxmidiport_get_id(port);

      if(macosxmidi_get_by_unique_id(id, MACOSXMIDI_OBJECT_TYPE_ANY) != NULL)
        post("  %s active (%d)\n", name, id);
      else
        post("  %s invalid midiport!!\n", name);

    } else {
      
      post("  %s ivalid atom in hashtable\n", name);
      
    }
  }
}

static void
macosxmidi_print( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_midilabel_t *label = fts_midimanager_get_labels(mm);

  post("sources\n");
  macosxmidi_scan_hashtable(&this->sources);
    
  post("destinations\n");
  macosxmidi_scan_hashtable(&this->destinations);

  post("labels\n");
  while(label) {
    fts_symbol_t name = fts_midilabel_get_name(label);
    macosxmidiport_t *input = (macosxmidiport_t *)fts_midilabel_get_input(label);
    macosxmidiport_t *output = (macosxmidiport_t *)fts_midilabel_get_output(label);
    fts_symbol_t input_name;
    fts_symbol_t output_name;

    input_name = macosxmidi_get_port_name(input);
    output_name = macosxmidi_get_port_name(output);
    
    post("  %s %s %s\n", name, input_name, output_name);

    label = fts_midilabel_get_next(label);
  }
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
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, macosxmidi_upload);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, macosxmidi_print);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_insert, macosxmidi_insert_label); /* insert <index> <name> */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_remove, macosxmidi_remove_label); /* remove <name>  */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_restore, macosxmidi_restore_label); /* restore <name> <source id> <destination id> */
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_input, macosxmidi_set_input); /* input <name> <source> */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_output, macosxmidi_set_output); /* output <name> <destination> */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sources, macosxmidi_update_sources);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_destinations, macosxmidi_update_destinations);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_bang, macosxmidi_bang);

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
  fts_metaclass_t *mc = fts_class_install(fts_new_symbol("macosxmidi"), macosxmidi_instantiate);
  fts_midimanager_t *mm = (fts_midimanager_t *)fts_object_create(mc, 0, 0);

  fts_class_install(fts_new_symbol("mm"), mm_instantiate);
  
  fts_midimanager_set(mm);
}
