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

#include "fts.h"

#define MAX_MIDI_EVENTS 512

static fts_symbol_t sym_midiport = 0;
static fts_symbol_t sym_send_channel_message = 0;
static fts_symbol_t sym_send_system_exclusive = 0;

static fts_symbol_t midiobj_get_classname(int ac, const fts_atom_t *at);
static fts_midiport_t * midiobj_get_midiport(int ac, const fts_atom_t *at);

static void midiobj_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void midiobj_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/************************************************
 *
 *  list utils
 *
 */
 
typedef struct 
{
  fts_atom_t *at;
  int ac;
  int alloc;
} list_t;

#define LIST_ALLOC_BLOCK 64

void
list_init(list_t *list)
{
  int i;
  
  list->at = fts_block_alloc(LIST_ALLOC_BLOCK * sizeof(fts_atom_t));
  list->ac = 0;
  list->alloc = LIST_ALLOC_BLOCK;
  
  /* set all atoms to void */
  for(i=0; i<LIST_ALLOC_BLOCK; i++)
    fts_set_void(list->at + i);
}

void
list_free(list_t *list)
{
  if(list->alloc)
    fts_block_alloc(list->alloc * sizeof(fts_atom_t));
}

void
list_set_size(list_t *list, int size)
{
  int alloc = list->alloc;

  if(size > alloc)
    {
      int i;

      if(alloc)
	fts_block_free(list->at, size * sizeof(fts_atom_t));

      while(alloc < size)
	alloc += LIST_ALLOC_BLOCK;

      list->at = fts_block_alloc(alloc * sizeof(fts_atom_t));

      /* set newly allocated region to void */
      for(i=list->alloc; i<size; i++)
	fts_set_void(list->at + i);
      
      list->alloc = alloc;
    }
  else
    {
      int i;

      if(size <= 0)
	size = 0;

      /* void region cut off at end */
      for(i=size; i<list->ac; i++)
	{
	  fts_atom_t *ap = list->at + i;

	  if(refdata_atom_is(ap))
	    refdata_atom_release(ap);
	  
	  fts_set_void(ap);
	}
    }

  list->ac = size;
}

/************************************************************
 *
 *  object
 *
 */

typedef struct _midiobj_
{
  fts_object_t o;
  fts_midiport_t *port;
  list_t list;
  int status;
  int channel;
  int value;
} midiobj_t;

/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */
extern fts_midiport_t *fts_midiport_get_default(void);
/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ */

/************************************************************
 *
 *  noteIn
 *
 */

static void
note_on_callback(fts_object_t *listener, int channel, int pitch, int velocity, double time)
{
  fts_outlet_int(listener, 2, channel);
  fts_outlet_int(listener, 1, velocity);
  fts_outlet_int(listener, 0, pitch);
}

static void
note_off_callback(fts_object_t *listener, int channel, int pitch, int velocity, double time)
{
  fts_outlet_int(listener, 2, channel);
  fts_outlet_int(listener, 1, 0);
  fts_outlet_int(listener, 0, pitch);
}

static void
note_in_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;
  fts_midiport_t *port = midiobj_get_midiport(ac, at);
  int channel = fts_get_int_arg(ac, at, 2, 0);
  fts_midiport_callback_t on_callback;
  fts_midiport_callback_t off_callback;

  this->port = port;
  this->status = fts_midi_status_note_on;
  this->channel = 0; /* defaults to omni */
  this->value = 0;

  if(ac > 2)
    midiobj_set_channel(o, 0, 0, 1, at + 2);

  on_callback.channel_message = note_on_callback;
  off_callback.channel_message = note_off_callback;

  fts_midiport_add_listener(port, fts_midi_status_note_on, this->channel, o, on_callback);
  fts_midiport_add_listener(port, fts_midi_status_note_off, this->channel, o, off_callback);
}

static void 
note_in_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;

  fts_midiport_remove_listener(this->port, fts_midi_status_note_on, this->channel, o);
  fts_midiport_remove_listener(this->port, fts_midi_status_note_off, this->channel, o);
}

/************************************************************
 *
 *  sysexIn
 *
 */

static void
sysex_callback(fts_object_t *o, int size, char *buffer, double time)
{
  midiobj_t *this = (midiobj_t *)o;
  int i;

  list_set_size(&this->list, size);

  for(i=0; i<size; i++)
    fts_set_int(this->list.at + i, (int)buffer[i]);

  fts_outlet_send(o, 0, fts_s_list, size, this->list.at);
}

static void
sysex_in_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;
  fts_midiport_t *port = midiobj_get_midiport(ac, at);
  int channel = fts_get_int_arg(ac, at, 2, 0);
  fts_midiport_callback_t callback;

  if(!port)
    port = fts_midiport_get_default();

  if(port)
    {
      this->port = port;
      this->status = fts_midi_status_system_exclusive;
      
      list_init(&this->list);
      
      callback.system_exclusive = sysex_callback;
      
      fts_midiport_add_listener(port, fts_midi_status_system_exclusive, 0, o, callback);
    }
}

static void 
sysex_in_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;

  fts_midiport_remove_listener(this->port, fts_midi_status_system_exclusive, 0, o);

  list_free(&this->list);
}

/************************************************************
 *
 *  noteOut
 *
 */

static void
note_out_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;
  fts_midiport_t *port = midiobj_get_midiport(ac, at);

  this->port = port;
  this->status = fts_midi_status_note_on;
  this->channel = 1; /* defaults to 1 */
  this->value = 0;
    
  if(ac == 3)
    midiobj_set_channel(o, 0, 0, 1, at + 2);
  else if(ac > 3)
    {
      midiobj_set_value(o, 0, 0, 1, at + 2);
      midiobj_set_channel(o, 0, 0, 1, at + 3);
    }
}

static void 
note_out_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;
}

/************************************************************
 *
 *  noteOut
 *
 */

static void
sysex_out_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;
  fts_midiport_t *port = midiobj_get_midiport(ac, at);

  if(!port)
    port = fts_midiport_get_default();
    
  this->port = port;
  this->status = fts_midi_status_system_exclusive;
}

static void 
sysex_out_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiobj_t *this = (midiobj_t *)o;
}

static void
sysex_out_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiobj_t *this = (midiobj_t *)o;

  if(this->port)
    fts_send_message((fts_object_t *)this->port, fts_SystemInlet, sym_send_system_exclusive, ac, at);
}

/************************************************************
 *
 *  user methods
 *
 */

static void
midiobj_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiobj_t *this = (midiobj_t *)o;
  int channel = fts_get_number_int(at);
  
  if(channel < 1)
    this->channel = 1;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;
}

static void
midiobj_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiobj_t *this = (midiobj_t *)o;
  
  this->value = fts_get_number_int(at);
}

static void
midiobj_out_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiobj_t *this = (midiobj_t *)o;
  fts_atom_t a[4];

  switch (ac)
    {
    default:
    case 3:
      midiobj_set_channel(o, 0, 0, 1, at + 2);
    case 2:
      midiobj_set_value(o, 0, 0, 1, at + 1);
    case 1:
      fts_set_int(a + 0, this->status);
      fts_set_int(a + 1, this->channel);
      fts_set_int(a + 2, fts_get_number_int(at));
      fts_set_int(a + 3, this->value);
      fts_send_message((fts_object_t *)this->port, fts_SystemInlet, sym_send_channel_message, 4, a);
    case 0:
      break;
    }
}

/************************************************************
 *
 *  class
 *
 */

static fts_symbol_t
midiobj_get_classname(int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_symbol(at))
    return fts_get_symbol(at);
  else
    return 0;
}

static fts_midiport_t *
midiobj_get_midiport(int ac, const fts_atom_t *at)
{
  if(ac > 1 && fts_is_object(at + 1) && fts_object_get_class_name(fts_get_object(at + 1)) == sym_midiport)
    return (fts_midiport_t *)fts_get_object(at + 1);
  else
    return 0;
}

int 
midiobj_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return 
    fts_is_symbol(at0) && fts_is_symbol(at1) &&
    (fts_get_symbol(at0) == fts_get_symbol(at1)) &&
    fts_is_object(at0 + 1) && fts_is_object(at1 + 1) &&
    (fts_get_object(at0 + 1) == fts_get_object(at1 + 1));
}

static fts_status_t
midiobj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t class = midiobj_get_classname(ac, at);
  fts_midiport_t *port = midiobj_get_midiport(ac, at);

  if(class == fts_new_symbol("notein"))
    {
      fts_class_init(cl, sizeof(midiobj_t), 0, 3, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, note_in_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, note_in_delete);
    }
  else if(class == fts_new_symbol("sysexin") || class == fts_new_symbol("midiin"))
    {
      fts_class_init(cl, sizeof(midiobj_t), 0, 1, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysex_in_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysex_in_delete);
    }
  else if(class == fts_new_symbol("noteout"))
    {
      fts_class_init(cl, sizeof(midiobj_t), 3, 0, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, note_out_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, note_out_delete);

      fts_method_define_varargs(cl, 0, fts_s_int, midiobj_out_send);
      fts_method_define_varargs(cl, 0, fts_s_float, midiobj_out_send);
      fts_method_define_varargs(cl, 0, fts_s_list, midiobj_out_send);

      fts_method_define_varargs(cl, 1, fts_s_int, midiobj_set_value);
      fts_method_define_varargs(cl, 1, fts_s_float, midiobj_set_value);

      fts_method_define_varargs(cl, 2, fts_s_int, midiobj_set_channel);
      fts_method_define_varargs(cl, 2, fts_s_float, midiobj_set_channel);
    }
  else if(class == fts_new_symbol("sysexout") || class == fts_new_symbol("midiout"))
    {
      fts_class_init(cl, sizeof(midiobj_t), 1, 0, 0);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, sysex_out_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, sysex_out_delete);

      fts_method_define_varargs(cl, 0, fts_s_int, sysex_out_send);
      fts_method_define_varargs(cl, 0, fts_s_float, sysex_out_send);
      fts_method_define_varargs(cl, 0, fts_s_list, sysex_out_send);
    }
  else
    return &fts_CannotInstantiate;    

  /*fts_method_define_varargs(cl, 0, fts_s_list, midiobj_input);*/

  return fts_Success;
}


void
midiobj_config(void)
{
  sym_midiport = fts_new_symbol("midiport");
  sym_send_channel_message = fts_new_symbol("send_channel_message");  
  sym_send_system_exclusive = fts_new_symbol("send_system_exclusive");

  fts_metaclass_install(fts_new_symbol("sysexin"), midiobj_instantiate, midiobj_equiv);
  fts_metaclass_install(fts_new_symbol("sysexout"), midiobj_instantiate, midiobj_equiv);
  fts_metaclass_install(fts_new_symbol("midiin"), midiobj_instantiate, midiobj_equiv);
  fts_metaclass_install(fts_new_symbol("midiout"), midiobj_instantiate, midiobj_equiv);
}
