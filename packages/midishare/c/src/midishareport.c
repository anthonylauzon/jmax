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
 */

#include <unistd.h>
#include <MidiShare.h>
#include <fts/fts.h>

#define SYSEX_REALTIME 0x7f

static fts_class_t* midishare_reference_type = 0;
static fts_class_t* midishareport_type = 0;

static union magic{char word[4]; void *ptr;} jmax_magic = {{'j', 'm', 'a', 'x'}};

/*************************************************
 *
 *  MidiShare references
 *
 *  This is the thing which is regitered to MidiShare for being connected.
 *  A jMax application can register multiple references.
 *
 */

static fts_hashtable_t midishare_reference_table;

typedef struct _midishare_reference_
{
  fts_object_t head;
  fts_symbol_t name;
  int number;
  fts_midiport_t *ports[256];
  int count; /* reference count */
} midishare_reference_t;

#define midishare_refer(r) ((r)->count++)
#define midishare_release(r) ((r)->count--)
#define midishare_no_reference(r) ((r)->count == 0)
#define midishare_port_is_free(r, p) ((r)->ports[p] == 0)

static void midishareport_input_event(fts_midiport_t *port, MidiEvPtr evt);

static void
midishare_dispatch(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midishare_reference_t *ref = (midishare_reference_t *)o;
  int n_events = MidiCountEvs(ref->number);
  int i;

  for(i=0; i<n_events; i++) 
  {
    MidiEvPtr evt = MidiGetEv(ref->number);
    int port = Port(evt);

    if(ref->ports[port])
      midishareport_input_event(ref->ports[port], evt);
  }
}

static midishare_reference_t *
midishare_reference_hash(fts_symbol_t name)
{
  fts_atom_t atom, k;

  fts_set_symbol( &k, name);
  if(fts_hashtable_get( &midishare_reference_table, &k, &atom))
    return (midishare_reference_t *)fts_get_pointer(&atom);
  else
    return 0;
}


static void
midishare_reference_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  int number;
  midishare_reference_t *ref;
  fts_atom_t atom, k;
  int i;
  fts_symbol_t name = fts_get_symbol(at);

  /* establish connection to MidiShare */
  number = MidiOpen((MidiName)name);

  /* set jMax magic word */
  MidiSetInfo(number, jmax_magic.ptr);
  
  /* init reference */
  ref->name = name;
  ref->number = number;

/*  
  for(i=0; i<256; i++)
    ref->ports[i] = 0;
*/
  ref->count = 0;
  
  /* insert reference to hashtable */
  fts_set_symbol( &k, name);
  fts_object_refer((fts_object_t*)ref);
  fts_set_object(&atom, (fts_object_t*)ref);
  fts_hashtable_put(&midishare_reference_table, &k, &atom);

  return ref;

}

static midishare_reference_t *
midishare_register(fts_symbol_t name, int num, fts_midiport_t *port)
{
  fts_atom_t a;
  if(MidiShare())
  {
    midishare_reference_t *ref = midishare_reference_hash(name);
      
    /* create new reference */
    if(!ref)
    {
      fts_set_symbol(&a, name);
      ref = (midishare_reference_t*)fts_object_create(midishare_reference_type, 1, &a);
    }

    /* add MidiShare polling to the scheduler (for now it is one entry per reference) */
    if(midishare_no_reference(ref))
      fts_sched_add( (fts_object_t *)ref, FTS_SCHED_ALWAYS);

    midishare_refer(ref);

    /* register port */
    if(ref->ports[num] == 0)
      ref->ports[num] = port;
	  
    return ref;
  }
  else
    return 0;
}

static void
midishare_unregister(midishare_reference_t *ref, int num)
{
  ref->ports[num] = 0;

  midishare_release(ref);

  /* remove MidiShare polling from the scheduler */
  if(midishare_no_reference(ref))
    fts_sched_remove( (fts_object_t *)ref);
}

static void
midishare_reference_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(midishare_reference_t), midishare_reference_init, 0);
  
  fts_class_message_varargs(cl, fts_s_sched_ready, midishare_dispatch);
}

void
midishare_reference_config()
{
  midishare_reference_type = fts_class_install(fts_new_symbol("midishare_reference"), midishare_reference_instantiate);
}

/*************************************************
 *
 *  MidiShare port
 *
 */

typedef struct _midishareport_
{
  fts_midiport_t head;
  midishare_reference_t *ref;
  int port;
  MidiEvPtr sysex;
} midishareport_t;

static void
midishareport_input_event(fts_midiport_t *port, MidiEvPtr evt)
{
  int type = EvType(evt);
  fts_midievent_t* event;

  switch(type)
  {
  case typeKeyOn:
    event = fts_midievent_note_new(Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1));
    break;
  case typeKeyOff:
    event = fts_midievent_note_new(Chan(evt) + 1, MidiGetField(evt, 0), 0);
    break;
  case typeKeyPress:
    event = fts_midievent_poly_pressure_new(Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1));
    break;
  case typeCtrlChange:
    event = fts_midievent_control_change_new(Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1));
    break;
  case typeProgChange:
    event = fts_midievent_program_change_new(Chan(evt) + 1, MidiGetField(evt, 0));
    break;
  case typeChanPress:
    event = fts_midievent_channel_pressure_new(Chan(evt) + 1, MidiGetField(evt, 0));
    break;
  case typePitchWheel:
    event = fts_midievent_pitch_bend_new(Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1));
    break;
#ifdef TYPE_SYS_EX_IS_IMPLEMENTED
  case typeSysEx:
  {
    int size = MidiCountFields(evt);
    int j;
	
    for(j=0; j<size - 2; j++)
      fts_midiport_input_system_exclusive_byte(port, MidiGetField(evt, j + 1));
	
    fts_midiport_input_system_exclusive_call(port, 0.0);
  }
#endif 
  break;
  }
  
  if (event != NULL)
  {
    fts_atom_t a;
    fts_set_object(&a, (fts_object_t*) event);
    fts_object_refer((fts_object_t*)event);
    fts_midiport_input((fts_object_t*)port, 0, 0, 1, &a);
    fts_object_release((fts_object_t*)event);

  }
  MidiFreeEv(evt);
}

/************************************************************
 *
 *  MIDI port interface methods
 *
 */

static void
midishareport_send_note(fts_object_t *o, int channel, int number, int value, double time)
{
  midishareport_t *self = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeKeyOn);
  
  Port(evt) = self->port;
  Chan(evt) = (channel - 1) & 0x0F;
  
  MidiSetField(evt, 0, number & 127);
  MidiSetField(evt, 1, value & 127);
  
  MidiSendIm(self->ref->number, evt);
}

static void
midishareport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  midishareport_t *self = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeKeyPress);

  Port(evt) = self->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, number & 127);
  MidiSetField(evt, 1, value & 127);

  MidiSendIm(self->ref->number, evt);
}

static void
midishareport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  midishareport_t *self = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeCtrlChange);

  Port(evt) = self->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, number & 127);
  MidiSetField(evt, 1, value & 127);

  MidiSendIm(self->ref->number, evt);
}

static void
midishareport_send_program_change(fts_object_t *o, int channel, int value, double time)
{	
  midishareport_t *self = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeProgChange);

  Port(evt) = self->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, value & 127);

  MidiSendIm(self->ref->number, evt);
}

static void
midishareport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  midishareport_t *self = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeChanPress);

  Port(evt) = self->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, value & 127);

  MidiSendIm(self->ref->number, evt);
}

static void
midishareport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  midishareport_t *self = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typePitchWheel);

  Port(evt) = self->port;
  Chan(evt) = (channel - 1) & 0x0F;
	
  MidiSetField(evt, 0, value & 127);
  MidiSetField(evt, 1, (value >> 7) & 127);
	
  MidiSendIm(self->ref->number, evt);
}

static void
midishareport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  midishareport_t *self = (midishareport_t *)o;

  if(self->sysex == 0)
  {
    self->sysex = MidiNewEv(typeSysEx);

    Port(self->sysex) = self->port;
    Chan(self->sysex) = 0;
      
    /* send sysex real-time message */
    MidiAddField(self->sysex, SYSEX_REALTIME);
  }
  
  MidiAddField(self->sysex, value);
}

static void
midishareport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  midishareport_t *self = (midishareport_t *)o;

  /* send sysex event to MidiShare */
  MidiSendIm(self->ref->number, self->sysex);

  /* reset sysex event */
  self->sysex = 0;
}

static void
midishareport_output(fts_object_t* o, fts_midievent_t* event, double time)
{
  
  if (fts_midievent_is_channel_message(event))
  {
    int channel = fts_midievent_channel_message_get_channel(event);
    int first = fts_midievent_channel_message_get_first(event);
    int second = fts_midievent_channel_message_get_second(event);

    switch(fts_midievent_get_type(event))
    {
    case midi_note:
      midishareport_send_note(o, channel, first, second, time);      
      break;

    case midi_poly_pressure:
      midishareport_send_poly_pressure(o, channel, first, second, time);      
      break;

    case midi_control_change:
      midishareport_send_control_change(o, channel, first, second, time); 
      break;

    case midi_program_change:
      midishareport_send_program_change(o, channel, first, time);
      break;

    case midi_channel_pressure:
      midishareport_send_channel_pressure(o, channel, first, time);
      break;

    case midi_pitch_bend:
      midishareport_send_pitch_bend(o, channel, first, time);
      break;
    }
  }
  
#if 0
  midishareport_send_system_exclusive_byte,
  midishareport_send_system_exclusive_flush,
#endif 

}




/************************************************************
 *
 *  methods
 *
 */

static void
midishareport_reset_unused(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *self = (midishareport_t *)o;
  int refnum;
  int i;
  
  for(i=1; i<=MidiCountAppls(); ++i)
  {
    refnum = MidiGetIndAppl(i);

    post("Midishare (%d): '%s': %p\n", refnum, MidiGetName(refnum), MidiGetInfo(refnum));

    /*MidiClose(refnum);*/
  }
}

/************************************************************
 *
 *  class
 *
 */

static int
midishareport_check(int ac, const fts_atom_t *at, fts_symbol_t *name, int *port)
{
  midishare_reference_t *ref;

  if(ac > 0 && fts_is_symbol(at))
  {
    *name = fts_get_symbol(at);
      
    /* skip MidiShare name */
    ac--;
    at++;
  }
  else
    *name = fts_new_symbol("jmax");
  
  if(ac > 0 && fts_is_int(at))
  {
    int p = fts_get_int(at);
	  
    if(p >= 0 && p < 256)
      *port = p;
    else
      return 0;
  }
  else
    *port = 0;

  ref = midishare_reference_hash(*name);

  if(ref && !midishare_port_is_free(ref, *port))
    return 0;

  return 1;
}

static void
midishareport_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *self = (midishareport_t *)o;
  fts_symbol_t name = 0;
  midishare_reference_t *ref;
  int port = 0;
  
  if(midishareport_check(ac, at, &name, &port))
  {

    fts_log("[midishare] check succes for %s\n", name);
    fts_midiport_init((fts_midiport_t*)self);
    fts_midiport_set_input((fts_midiport_t*)self);
    fts_midiport_set_output((fts_midiport_t*)self, midishareport_output);
      
    self->ref = midishare_register(name, port, &self->head);
    self->port = port;
    self->sysex = 0;
  }
  else
    fts_object_error(o, "bad arguments");
}

static void 
midishareport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *self = (midishareport_t *)o;
  
  midishare_unregister(self->ref, self->port);
  
  fts_midiport_reset((fts_midiport_t*)o);
}

static void
midishareport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midishareport_t), midishareport_init, midishareport_delete);
  
  fts_midiport_class_init(cl);
  
  fts_class_message_varargs(cl, fts_new_symbol("reset_unused"), midishareport_reset_unused);
}

void
midishareport_config(void)
{
  fts_hashtable_init( &midishare_reference_table, FTS_HASHTABLE_MEDIUM);

  midishareport_type = fts_class_install( fts_new_symbol("midishareport"), midishareport_instantiate);
}

void
midishareport_cleanup(void)
{
  fts_iterator_t iter;

  /* close all registered connections to MidiShare */
  fts_hashtable_get_values( &midishare_reference_table, &iter);

  while ( fts_iterator_has_more( &iter))
  {
    fts_atom_t v;
    midishare_reference_t *ref;
    
    fts_iterator_next( &iter, &v);
    
    ref = (midishare_reference_t *)fts_get_pointer( &v);
    
    /* close MidiShare application */
    MidiClose(ref->number);
    
    /* free reference */
    fts_free(ref);  
  }
}


/**************************************************
 *
 * Midishare manager
 *
 *
 ***************************************************/
typedef struct 
{
  fts_midimanager_t manager;
  fts_array_t inputs;
  fts_array_t outputs;
} midishare_manager_t;


static void
midishare_manager_update_inputs()
{

}

static void
midishare_manager_update_outputs()
{

}

static void
midishare_manager_get_default_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  fts_midiport_t** ptr = (fts_midiport_t**)fts_get_pointer(at + 0);
  fts_symbol_t port_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_atom_t* a;
  
  a = fts_array_get_element(&self->inputs, 0);
  if (fts_is_object(a))
  {
    *ptr = (fts_midiport_t*)(fts_get_object(a));
  }

}

static void
midishare_manager_get_default_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  fts_midiport_t** ptr = (fts_midiport_t**)fts_get_pointer(at + 0);
  fts_symbol_t port_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_atom_t* a;
  
  a = fts_array_get_element(&self->outputs, 0);
  if (fts_is_object(a))
  {
    *ptr = (fts_midiport_t*)(fts_get_object(a));
  }

}

static void
midishare_manager_append_inputs(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  fts_array_t* inputs = (fts_array_t*)fts_get_pointer(at);
  fts_atom_t* a;
  midishareport_t* port;
  a = fts_array_get_element(&self->inputs, 0);
  if (fts_is_object(a))
  {
    port = (midishareport_t*)(fts_get_object(a));
    fts_array_append_symbol(inputs, fts_new_symbol("jMax midishare"));
  }
}

static void
midishare_manager_append_outputs(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  fts_array_t* outputs = (fts_array_t*)fts_get_pointer(at);
  fts_atom_t* a;
  midishareport_t* port;
  a = fts_array_get_element(&self->outputs, 0);
  if (fts_is_object(a))
  {
    port = (midishareport_t*)(fts_get_object(a));
    fts_array_append_symbol(outputs, fts_new_symbol("jMax midishare"));
  }

}

static void
midishare_manager_get_input(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  fts_midiport_t** ptr = (fts_midiport_t**)fts_get_pointer(at + 0);
  fts_symbol_t port_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_atom_t* a;
 
   post("Get %s input port \n", port_name);
  a = fts_array_get_element(&self->inputs, 0);
  if (fts_is_object(a))
  {
    *ptr = (fts_midiport_t*)(fts_get_object(a));
  }

}

static void
midishare_manager_get_output(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  fts_midiport_t** ptr = (fts_midiport_t**)fts_get_pointer(at + 0);
  fts_symbol_t port_name = fts_get_symbol(at + 1);
  fts_symbol_t label_name = fts_get_symbol(at + 2);
  fts_atom_t* a;
  
  post("Get %s output port \n", port_name);
  a = fts_array_get_element(&self->outputs, 0);
  if (fts_is_object(a))
  {
    *ptr = (fts_midiport_t*)(fts_get_object(a));
  }

}

static void
midishare_manager_init(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  midishareport_t* port;
  fts_atom_t a;

  fts_array_init(&self->inputs, 0, 0);
  fts_array_init(&self->outputs, 0, 0);  
  
  /* create a midishareport object */
  /* and put it in array */
  port = (midishareport_t*)fts_object_create(midishareport_type, 0, 0);
  
  fts_object_refer((fts_object_t*)port);
  fts_set_object(&a, (fts_object_t*)port);
  fts_array_append(&self->inputs, 1, &a);
  
  fts_object_refer((fts_object_t*)port);
  fts_set_object(&a, (fts_object_t*)port);
  fts_array_append(&self->outputs, 1, &a);

}

static void
midishare_manager_delete(fts_object_t* o, int winlet, fts_symbol_t s, int ac, const fts_atom_t* at)
{
  midishare_manager_t* self = (midishare_manager_t*)o;
  
  fts_array_destroy(&self->inputs);
  fts_array_destroy(&self->outputs);
}

static void
midishare_manager_instantiate(fts_class_t* cl)
{
  fts_class_init(cl, sizeof(midishare_manager_t), midishare_manager_init, midishare_manager_delete);
  
  /* FTS MIDI manager interface implementation */
  fts_class_message_varargs(cl, fts_midimanager_s_get_default_input, midishare_manager_get_default_input);
  fts_class_message_varargs(cl, fts_midimanager_s_get_default_output, midishare_manager_get_default_output);
  fts_class_message_varargs(cl, fts_midimanager_s_append_input_names, midishare_manager_append_inputs);
  fts_class_message_varargs(cl, fts_midimanager_s_append_output_names, midishare_manager_append_outputs);
  fts_class_message_varargs(cl, fts_midimanager_s_get_input, midishare_manager_get_input);
  fts_class_message_varargs(cl, fts_midimanager_s_get_output, midishare_manager_get_output);


}


void 
midishare_config(void)
{
  fts_class_t* mc = fts_class_install(fts_new_symbol("midishare_manager"), midishare_manager_instantiate);
  midishare_reference_config();
  midishareport_config();

  fts_midiconfig_add_manager((fts_midimanager_t*)fts_object_create(mc, 0, 0));
}
/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
