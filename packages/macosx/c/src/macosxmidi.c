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

static void
macosxmidi_get_sources( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_atom_t *atoms;
  int n, i;

  n = MIDIGetNumberOfSources();
  atoms = alloca(sizeof(fts_atom_t) * (n + 3));

  fts_set_symbol(atoms, fts_s_none);

  for(i=0; i<n; i++)
    {
      MIDIObjectRef ref = (MIDIObjectRef)MIDIGetSource(i);
      CFStringRef cfsr;
      fts_atom_t k, a;

      MIDIObjectGetStringProperty(ref, kMIDIPropertyName, &cfsr);
      fts_set_symbol(&k, fts_new_symbol_copy(CFStringGetCStringPtr(cfsr, 0)));

      atoms[i + 1] = k;
      
      if(!fts_hashtable_get(&this->sources, &k, &a))
        {
          fts_set_pointer(&a, ref);
          fts_hashtable_put(&this->sources, &k, &a);
        }
    }

  fts_set_symbol(atoms + n + 1, fts_s_internal);
  fts_set_symbol(atoms + n + 2, fts_s_export);
  
  /* send to client */
  
  post_atoms(n + 3, atoms);
  post("\n");
}

static void
macosxmidi_get_destinations( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_atom_t *atoms;
  int n, i;

  n = MIDIGetNumberOfDestinations();
  atoms = alloca(sizeof(fts_atom_t) * (n + 3));

  fts_set_symbol(atoms, fts_s_none);

  for(i=0; i<n; i++)
    {
      MIDIObjectRef ref = (MIDIObjectRef)MIDIGetDestination(i);
      CFStringRef cfsr;
      fts_atom_t k, a;
  
      MIDIObjectGetStringProperty(ref, kMIDIPropertyName, &cfsr);
      fts_set_symbol(&k, fts_new_symbol_copy(CFStringGetCStringPtr(cfsr, 0)));

      atoms[i + 1] = k;
      
      if(!fts_hashtable_get(&this->destinations, &k, &a))
        {
          fts_set_pointer(&a, ref);
          fts_hashtable_put(&this->destinations, &k, &a);
        }
    }

  fts_set_symbol(atoms + n + 1, fts_s_internal);
  fts_set_symbol(atoms + n + 2, fts_s_export);

  /* send to client */
  
  post_atoms(n + 3, atoms);
  post("\n");
}

static void
macosxmidi_set_input( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  int label_index = fts_get_int(at);
  fts_symbol_t source_name = fts_get_symbol(at + 1);
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, label_index);

  if(label != NULL)
    {
    if(source_name == fts_s_none)
      {
      fts_midilabel_set_input(label, NULL);

      /* send to client */
      }
    else if(source_name == fts_s_export)
      {
      fts_atom_t args[2];

      fts_set_object(args + 0, o);
      fts_set_symbol(args + 1, fts_midilabel_get_name(label));
      fts_midilabel_set_input(label, (fts_midiport_t *)fts_object_create(macosxmidi_input_type, 2, args));

      /* send to client */
      }
    else if(source_name == fts_s_internal)
      {
      fts_midilabel_set_internal(label);

      /* send to client */
      }
    else
      {
      /* get or create MIDI port */
      /* set input */

      /* send to client */
      }
    }
}

static void
macosxmidi_set_output( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  int label_index = fts_get_int(at);
  fts_symbol_t destination_name = fts_get_symbol(at + 1);
  fts_midilabel_t *label = fts_midimanager_get_label_by_index(mm, label_index);

  if(label != NULL)
    {
    if(destination_name == fts_s_none)
      {
      fts_midilabel_set_output(label, NULL);

      /* send to client */
      }
    else if(destination_name == fts_s_export)
      {
      fts_atom_t args[2];

      fts_set_object(args + 0, o);
      fts_set_symbol(args + 1, fts_midilabel_get_name(label));
      fts_midilabel_set_output(label, (fts_midiport_t *)fts_object_create(macosxmidi_output_type, 2, args));
      }
    else if(destination_name == fts_s_internal)
      {
      fts_midilabel_set_internal(label);

      /* send to client */
      }
    else
      {
      /* get or create MIDI port */
      /* set output */

      /* send to client */
      }
    }
}

/* insert <index> <name> */
static void
macosxmidi_insert_label( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  int index = fts_get_int(at);
  fts_symbol_t name = fts_get_symbol(at + 1);

  fts_midimanager_insert_label_at_index(mm, index, name);
  fts_outlet_send(o, 0, fts_s_insert, 2, at);
}

/* remove <index> */
static void
macosxmidi_remove_label( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  int index = fts_get_int(at);

  fts_midimanager_remove_label_at_index(mm, index);
  fts_outlet_send(o, 0, fts_s_remove, 1, at);
}

/* restore <name> <source id> <destination id> */
static void
macosxmidi_restore_label( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{

}

static void
macosxmidi_print( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_midilabel_t *label = fts_midimanager_get_labels(mm);
  fts_iterator_t keys, values;
  fts_atom_t k, a;

  post("sources\n");

  fts_hashtable_get_keys(&this->sources, &keys);
  fts_hashtable_get_values(&this->sources, &values);
  while(fts_iterator_has_more(&keys))
    {
    fts_symbol_t name;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &a);

    name = fts_get_symbol(&k);

    if(fts_is_pointer(&a))
      post("  %s inactive\n", name);
    else if(fts_is_object(&a))
      post("  %s active (%s)\n", name, macosxmidiport_get_name((macosxmidiport_t *)fts_get_object(&a)));
    }

  post("destinations\n");

  fts_hashtable_get_keys(&this->destinations, &keys);
  fts_hashtable_get_values(&this->destinations, &values);
  while(fts_iterator_has_more(&keys))
    {
    fts_symbol_t name;

    fts_iterator_next(&keys, &k);
    fts_iterator_next(&values, &a);

    name = fts_get_symbol(&k);

    if(fts_is_pointer(&a))
      post("  %s inactive\n", name);
    else if(fts_is_object(&a))
      post("  %s active (%s)\n", name, macosxmidiport_get_name((macosxmidiport_t *)fts_get_object(&a)));
    }

  post("labels\n");
  
  while(label)
    {
    fts_symbol_t name = fts_midilabel_get_name(label);
    macosxmidiport_t *input = (macosxmidiport_t *)fts_midilabel_get_input(label);
    macosxmidiport_t *output = (macosxmidiport_t *)fts_midilabel_get_output(label);
    fts_symbol_t input_name, output_name;

    if(input == NULL)
      input_name = fts_s_none;
    else if(fts_object_get_metaclass((fts_object_t *)input) == fts_midiport_type)
      input_name = fts_s_internal;
    else
      input_name = macosxmidiport_get_name(input);

    if(output == NULL)
      output_name = fts_s_none;
    else if(fts_object_get_metaclass((fts_object_t *)output) == fts_midiport_type)
      output_name = fts_s_internal;
    else
      output_name = macosxmidiport_get_name(output);

    post("  %s %s %s\n", name, input_name, output_name);

    label = fts_midilabel_get_next(label);
    }
}

static void
macosxmidi_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midimanager_t *mm = (fts_midimanager_t *)o;
  macosxmidi_t *this = (macosxmidi_t *)o;
  fts_midilabel_t *label = fts_midimanager_get_labels(mm);
  fts_iterator_t keys, values;
  fts_atom_t k, a;

  fts_hashtable_get_keys(&this->sources, &keys);
  
  fts_client_start_message( o, fts_s_sources);

  fts_log("[midimanager] upload sources\n");

  while(fts_iterator_has_more(&keys))
    {
      fts_symbol_t name;
      
      fts_iterator_next(&keys, &k);
      name = fts_get_symbol(&k);
      fts_client_add_symbol( o, name);  
    
      fts_log("[midimanager] upload sources %s\n", name);	
    }
  fts_client_add_symbol( o, fts_s_internal); 
  fts_client_add_symbol( o, fts_s_export); 

  fts_client_done_message( o);  

  fts_hashtable_get_keys(&this->destinations, &keys);
  
  fts_client_start_message( o, fts_s_destinations);
  
  while(fts_iterator_has_more(&keys))
    {
      fts_symbol_t name;
      fts_iterator_next(&keys, &k);
      name = fts_get_symbol(&k);
      fts_client_add_symbol( o, name); 
    }
  fts_client_add_symbol( o, fts_s_internal); 
  fts_client_add_symbol( o, fts_s_export); 

  fts_client_done_message( o);  

  /*while(label)
    {
    fts_symbol_t name = fts_midilabel_get_name(label);
    macosxmidiport_t *input = (macosxmidiport_t *)fts_midilabel_get_input(label);
    macosxmidiport_t *output = (macosxmidiport_t *)fts_midilabel_get_output(label);
    fts_symbol_t input_name, output_name;
    
    if(input == NULL)
    input_name = fts_s_none;
    else if(fts_object_get_metaclass((fts_object_t *)input) == fts_midiport_type)
    input_name = fts_s_internal;
    else
    input_name = macosxmidiport_get_name(input);
    
    if(output == NULL)
    output_name = fts_s_none;
    else if(fts_object_get_metaclass((fts_object_t *)output) == fts_midiport_type)
    output_name = fts_s_internal;
    else
    output_name = macosxmidiport_get_name(output);
    
    label = fts_midilabel_get_next(label);
    }*/
}


static void
macosxmidi_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  macosxmidi_t *this = (macosxmidi_t *)o;

  fts_hashtable_init(&this->sources, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  fts_hashtable_init(&this->destinations, FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
   
  MIDIClientCreate(CFSTR("jMax"), NULL, NULL, &this->client);
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

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_sources, macosxmidi_get_sources);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_destinations, macosxmidi_get_destinations);

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
