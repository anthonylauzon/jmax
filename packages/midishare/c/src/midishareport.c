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

#include <unistd.h>
#include <MidiShare.h>
#include <fts/fts.h>

#define SYSEX_REALTIME 0x7f

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

static midishare_reference_t *
midishare_reference_new(fts_symbol_t name)
{
  int number;
  midishare_reference_t *ref;
  fts_atom_t atom, k;
  int i;

  /* establish connection to MidiShare */
  number = MidiOpen((MidiName)name);

  /* set jMax magic word */
  MidiSetInfo(number, jmax_magic.ptr);
  
  /* make new reference */
  ref = fts_malloc(sizeof(midishare_reference_t));
  
  /* init reference */
  ref->name = name;
  ref->number = number;
  
  for(i=0; i<256; i++)
    ref->ports[i] = 0;
  
  ref->count = 0;
  
  /* insert reference to hashtable */
  fts_set_symbol( &k, name);
  fts_set_pointer(&atom, ref);
  fts_hashtable_put(&midishare_reference_table, &k, &atom);

  return ref;
}

static midishare_reference_t *
midishare_register(fts_symbol_t name, int num, fts_midiport_t *port)
{
  if(MidiShare())
    {
      midishare_reference_t *ref = midishare_reference_hash(name);
      
      /* create new reference */
      if(!ref)
	ref = midishare_reference_new(name);

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
  
  switch(type)
    {
    case typeKeyOn:
      fts_midiport_input_note(port, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
      break;
    case typeKeyOff:
      fts_midiport_input_note(port, Chan(evt) + 1, MidiGetField(evt, 0), 0, 0.0);
      break;
    case typeKeyPress:
      fts_midiport_input_poly_pressure(port, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
      break;
    case typeCtrlChange:
      fts_midiport_input_control_change(port, Chan(evt) + 1, MidiGetField(evt, 0), MidiGetField(evt, 1), 0.0);
      break;
    case typeProgChange:
      fts_midiport_input_program_change(port, Chan(evt) + 1, MidiGetField(evt, 0), 0.0);
      break;
    case typeChanPress:
      fts_midiport_input_channel_pressure(port, Chan(evt) + 1, MidiGetField(evt, 0), 0.0);
      break;
    case typePitchWheel:
      fts_midiport_input_pitch_bend(port, Chan(evt) + 1, MidiGetField(evt, 0) + (MidiGetField(evt, 1) << 7), 0.0);
      break;
    case typeSysEx:
      {
	int size = MidiCountFields(evt);
	int j;
	
	for(j=0; j<size - 2; j++)
	  fts_midiport_input_system_exclusive_byte(port, MidiGetField(evt, j + 1));
	
	fts_midiport_input_system_exclusive_call(port, 0.0);
      }
      
      break;
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
  midishareport_t *this = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeKeyOn);
  
  Port(evt) = this->port;
  Chan(evt) = (channel - 1) & 0x0F;
  
  MidiSetField(evt, 0, number & 127);
  MidiSetField(evt, 1, value & 127);
  
  MidiSendIm(this->ref->number, evt);
}

static void
midishareport_send_poly_pressure(fts_object_t *o, int channel, int number, int value, double time)
{
  midishareport_t *this = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeKeyPress);

  Port(evt) = this->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, number & 127);
  MidiSetField(evt, 1, value & 127);

  MidiSendIm(this->ref->number, evt);
}

static void
midishareport_send_control_change(fts_object_t *o, int channel, int number, int value, double time)
{
  midishareport_t *this = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeCtrlChange);

  Port(evt) = this->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, number & 127);
  MidiSetField(evt, 1, value & 127);

  MidiSendIm(this->ref->number, evt);
}

static void
midishareport_send_program_change(fts_object_t *o, int channel, int value, double time)
{	
  midishareport_t *this = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeProgChange);

  Port(evt) = this->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, value & 127);

  MidiSendIm(this->ref->number, evt);
}

static void
midishareport_send_channel_pressure(fts_object_t *o, int channel, int value, double time)
{
  midishareport_t *this = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typeChanPress);

  Port(evt) = this->port;
  Chan(evt) = (channel - 1) & 0x0F;

  MidiSetField(evt, 0, value & 127);

  MidiSendIm(this->ref->number, evt);
}

static void
midishareport_send_pitch_bend(fts_object_t *o, int channel, int value, double time)
{
  midishareport_t *this = (midishareport_t *)o;

  MidiEvPtr evt = MidiNewEv(typePitchWheel);

  Port(evt) = this->port;
  Chan(evt) = (channel - 1) & 0x0F;
	
  MidiSetField(evt, 0, value & 127);
  MidiSetField(evt, 1, (value >> 7) & 127);
	
  MidiSendIm(this->ref->number, evt);
}

static void
midishareport_send_system_exclusive_byte(fts_object_t *o, int value)
{
  midishareport_t *this = (midishareport_t *)o;

  if(this->sysex == 0)
    {
      this->sysex = MidiNewEv(typeSysEx);

      Port(this->sysex) = this->port;
      Chan(this->sysex) = 0;
      
      /* send sysex real-time message */
      MidiAddField(this->sysex, SYSEX_REALTIME);
    }
  
  MidiAddField(this->sysex, value);
}

static void
midishareport_send_system_exclusive_flush(fts_object_t *o, double time)
{
  midishareport_t *this = (midishareport_t *)o;

  /* send sysex event to MidiShare */
  MidiSendIm(this->ref->number, this->sysex);

  /* reset sysex event */
  this->sysex = 0;
}

static fts_midiport_output_functions_t midishareport_output_functions =
{
  midishareport_send_note,
  midishareport_send_poly_pressure,
  midishareport_send_control_change,
  midishareport_send_program_change,
  midishareport_send_channel_pressure,
  midishareport_send_pitch_bend,
  midishareport_send_system_exclusive_byte,
  midishareport_send_system_exclusive_flush,
};

/************************************************************
 *
 *  methods
 *
 */

static void
midishareport_reset_unused(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *this = (midishareport_t *)o;
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
 *  get midiport variable
 *
 */
static void
midishareport_get_state(fts_daemon_action_t action, fts_object_t *o, fts_symbol_t property, fts_atom_t *value)
{
  midishareport_t *this = (midishareport_t *)o;

  fts_set_object(value, o);
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
  midishareport_t *this = (midishareport_t *)o;
  fts_symbol_t name = 0;
  midishare_reference_t *ref;
  int port = 0;
  
  fts_symbol_t name;
  int port;

  if(midishareport_check(ac, at, &name, &port))
    {
      fts_midiport_init(&this->head);
      fts_midiport_set_input(&this->head);
      fts_midiport_set_output(&this->head, &midishareport_output_functions);
      
      this->ref = midishare_register(name, port, &this->head);
      this->port = port;
      this->sysex = 0;
    }
  else
    fts_object_set_error(o, "Wrong arguments");
}

static void 
midishareport_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midishareport_t *this = (midishareport_t *)o;
  
  midishare_unregister(this->ref, this->port);
}

static void
midishareport_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(midishareport_t), midishareport_init, midishareport_delete);
  
  fts_midiport_class_init(cl);
  
  fts_class_message_varargs(cl, fts_new_symbol("reset_unused"), midishareport_reset_unused);
  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, midishareport_get_state);
  }

void
midishareport_config(void)
{
  fts_hashtable_init(&midishare_reference_table, 0, FTS_HASHTABLE_MEDIUM);

  fts_class_install( fts_new_symbol("midishareport"), midishareport_instantiate);
}

void
midishareport_cleanup(void)
{
  fts_iterator_t i;

  /* close all registered connections to MidiShare */
  fts_hashtable_get_values( &midishare_reference_table, &i);

  while ( fts_iterator_has_more( &iter))
    {
      fts_atom_t v;
      midishare_reference_t *ref;

      fts_iterator_next( &i, &v);

      ref = (midishare_reference_t *)fts_get_pointer( &v);
      
      /* close MidiShare application */
      MidiClose(ref->number);
      
      /* free reference */
      fts_free(ref);  
    }
}

fts_module_t midishare_module = {"midishare", "midishare MIDI classes", midishareport_config, midishareport_cleanup, 0};
