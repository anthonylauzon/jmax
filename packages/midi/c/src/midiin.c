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

#define NO_FILTER -1

static fts_symbol_t sym_notein = 0;
static fts_symbol_t sym_polyin = 0;
static fts_symbol_t sym_ctlin = 0;
static fts_symbol_t sym_pgmin = 0;
static fts_symbol_t sym_touchin = 0;
static fts_symbol_t sym_bendin = 0;

typedef struct _midiin_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int filter;
  fts_midi_status_t status;
  fts_midiport_callback_t callback;
} midiin_t;

/************************************************************
 *
 *  callbacks
 *
 */

static void
pgm_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  fts_outlet_int(listener, 1, channel);
  fts_outlet_int(listener, 0, x);
}

static void
pgm_channel_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  fts_outlet_int(listener, 0, x);
}

static void
note_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  fts_outlet_int(listener, 2, channel);
  fts_outlet_int(listener, 1, y);
  fts_outlet_int(listener, 0, x);
}

static void
note_channel_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  fts_outlet_int(listener, 1, y);
  fts_outlet_int(listener, 0, x);
}

static void
poly_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  fts_outlet_int(listener, 2, channel);
  fts_outlet_int(listener, 1, x);
  fts_outlet_int(listener, 0, y);
}

static void
poly_channel_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  fts_outlet_int(listener, 1, x);
  fts_outlet_int(listener, 0, y);
}

static void
poly_filter_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  midiin_t *this = (midiin_t *)listener;

  if(this->filter != NO_FILTER && this->filter == x)
    {
      fts_outlet_int(listener, 1, channel);
      fts_outlet_int(listener, 0, y);
    }
}

static void
poly_filter_channel_callback(fts_object_t *listener, int channel, int x, int y, double time)
{
  midiin_t *this = (midiin_t *)listener;

  if(this->filter != NO_FILTER && this->filter == x)
    fts_outlet_int(listener, 0, y);
}

/************************************************************
 *
 *  object
 *
 */

static void
midiin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  fts_symbol_t classname = fts_get_symbol(at);
  fts_midiport_t *port = 0;
  int channel = 0;
  int filter = NO_FILTER;

  /* skip class name */
  ac--;
  at++;

  if(ac > 0 && fts_is_object(at) && fts_midiport_has_superclass(fts_get_object(at)))
    {
      port = (fts_midiport_t *)fts_get_object(at);

      /* skip port argument */
      ac--;
      at++;
    }

  /* if there is still no port just get default */
  if(port)
    this->port = port;
  else
    this->port = fts_midiport_get_default();

  if(ac == 1)
    channel = fts_get_number_int(at);
  else if(ac > 1)
    {
      filter = fts_get_number_int(at);
      channel = fts_get_number_int(at + 1);
    }

  /* channel 0 is omni */
  if(channel < 0)
    this->channel = 0;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;

  this->filter = filter & 127;

  if(classname == sym_notein)
    {
      this->status = fts_midi_status_note;
      this->callback.channel_message = (channel == 0)? note_callback: note_channel_callback;
    }
  else if(classname == sym_polyin)
    {
      this->status = fts_midi_status_poly_pressure;

      if(filter == NO_FILTER)
	this->callback.channel_message = (channel == 0)? poly_callback: poly_channel_callback;
      else
	this->callback.channel_message = (channel == 0)? poly_filter_callback: poly_filter_channel_callback;	
    }
  else if(classname == sym_ctlin)
    {
      this->status = fts_midi_status_control_change;

      if(filter == NO_FILTER)
	this->callback.channel_message = (channel == 0)? poly_callback: poly_channel_callback;
      else
	this->callback.channel_message = (channel == 0)? poly_filter_callback: poly_filter_channel_callback;	
    }
  else if(classname == sym_pgmin)
    {
      this->status = fts_midi_status_program_change;
      this->callback.channel_message = (channel == 0)? pgm_callback: pgm_channel_callback;
    }
  else if(classname == sym_touchin)
    {
      this->status = fts_midi_status_channel_pressure;
      this->callback.channel_message = (channel == 0)? pgm_callback: pgm_channel_callback;
    }
  else if(classname == sym_bendin)
    {
      this->status = fts_midi_status_pitch_bend;
      this->callback.channel_message = (channel == 0)? pgm_callback: pgm_channel_callback;
    }
  
  if(this->port)
    fts_midiport_add_listener(this->port, this->status, this->channel, o, this->callback);
}

static void 
midiin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;

  if(this->port)
    fts_midiport_remove_listener(this->port, this->status, this->channel, o);
}

/************************************************************
 *
 *  class
 *
 */

static fts_status_t
midiin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t classname;
  int channel = 0;
  int filter = NO_FILTER;
  int n_outs = 0;

  /* skip class name */
  classname = fts_get_symbol(at);
  ac--;
  at++;

  /* skip port argument */
  if(ac && fts_is_object(at))
    {
      ac--;
      at++;
    }

  if(ac == 1)
    channel = fts_get_number_int(at);
  else if(ac > 1)
    {
      filter = fts_get_number_int(at);
      channel = fts_get_number_int(at + 1);
    }

  /* clip channel */
  if(channel < 0)
    channel = 0;
  else if(channel > 16)
    channel = 16;

  if(classname == sym_notein && ac <= 1)
    n_outs = 2 + (channel == 0);
  else if(classname == sym_polyin && ac <= 2)
    n_outs = 1 + (filter == NO_FILTER) + (channel == 0);
  else if(classname == sym_ctlin && ac <= 2)
    n_outs = 1 + (filter == NO_FILTER) + (channel == 0);
  else if(classname == sym_pgmin && ac <= 1)
    n_outs = 1 + (channel == 0);
  else if(classname == sym_touchin && ac <= 1)
    n_outs = 1 + (channel == 0);
  else if(classname == sym_bendin && ac <= 1)
    n_outs = 1 + (channel == 0);
  else
    return &fts_CannotInstantiate;    
  
  fts_class_init(cl, sizeof(midiin_t), 0, n_outs, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

void
midiin_config(void)
{
  sym_notein = fts_new_symbol("notein");
  sym_polyin = fts_new_symbol("polyin");
  sym_ctlin = fts_new_symbol("ctlin");
  sym_pgmin = fts_new_symbol("pgmin");
  sym_touchin = fts_new_symbol("touchin");
  sym_bendin = fts_new_symbol("bendin");

  fts_metaclass_install(sym_notein, midiin_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_polyin, midiin_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_ctlin, midiin_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_pgmin, midiin_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_touchin, midiin_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(sym_bendin, midiin_instantiate, fts_arg_type_equiv);
}
