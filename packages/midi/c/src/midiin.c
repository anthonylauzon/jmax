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

/************************************************************
 *
 *  object
 *
 */

typedef struct _midiin_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int filter;
} midiin_t;

typedef struct _midiin_classdata_
{
  fts_midi_status_t status;
  fts_midiport_callback_t callback;
} midiin_classdata_t;

static void
midiin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  midiin_classdata_t *classdata = (midiin_classdata_t *)fts_object_get_user_data(o);
  fts_midiport_t *port = 0;
  int channel = 0;
  int filter = NO_FILTER;

  /* skip class name */
  ac--;
  at++;

  if(ac > 1 && fts_is_object(at + 1) && fts_object_get_class_name(fts_get_object(at + 1)) == fts_s__midiport)
    {
      port = (fts_midiport_t *)fts_get_object(at + 1);

      /* skip port argument */
      ac--;
      at++;
    }

  /* if there is still no port just get default */
  if(!port)
    port = fts_midiport_get_default();

  if(ac == 2)
    channel = fts_get_int_arg(ac, at, 1, 0);
  else if(ac == 3)
    {
      filter = fts_get_int_arg(ac, at, 1, NO_FILTER);
      channel = fts_get_int_arg(ac, at, 2, 0);
    }

  this->port = port;

  /* channel 0 is omni */
  if(channel < 0)
    this->channel = 0;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;

  this->filter = filter & 127;

  if(port)
    fts_midiport_add_listener(port, classdata->status, this->channel, o, classdata->callback);
}

static void 
midiin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiin_t *this = (midiin_t *)o;
  midiin_classdata_t *classdata = (midiin_classdata_t *)fts_object_get_user_data(o);

  if(this->port)
    fts_midiport_remove_listener(this->port, classdata->status, this->channel, o);
}

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
 *  class
 *
 */

static fts_status_t
midiin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  midiin_classdata_t *classdata = fts_malloc(sizeof(midiin_classdata_t));
  fts_symbol_t type;
  int channel = 0;
  int filter = NO_FILTER;
  int n_outs = 0;

  /* skip class name */
  ac--;
  at++;

  /* skip port argument */
  if(ac && fts_is_object(at))
    {
      ac--;
      at++;
    }

  /* generic midiin not yet implemented */
  if(!ac)
    return &fts_CannotInstantiate;

  type = fts_get_symbol(at);

  if(ac == 2)
    channel = fts_get_int_arg(ac, at, 1, 0);
  else if(ac == 3)
    {
      filter = fts_get_int_arg(ac, at, 1, NO_FILTER);
      channel = fts_get_int_arg(ac, at, 2, 0);
    }
 
  if(channel < 0)
    channel = 0;
  else if(channel > 16)
    channel = 16;

  if((type == fts_new_symbol("note") || type == fts_new_symbol("nt")) && ac <= 2)
   {
      classdata->status = fts_midi_status_note;
      classdata->callback.channel_message = (channel == 0)? note_callback: note_channel_callback;
      
      n_outs = (channel == 0)? 3: 2;
    }
  else if((type == fts_new_symbol("poly-pressure") || type == fts_new_symbol("pp")) && ac <= 3)
    {
      classdata->status = fts_midi_status_poly_pressure;

      if(filter == NO_FILTER)
	{
	  classdata->callback.channel_message = (channel == 0)? poly_callback: poly_channel_callback;
	  n_outs = (channel == 0)? 3: 2;
	}
      else
	{
	  classdata->callback.channel_message = (channel == 0)? poly_filter_callback: poly_filter_channel_callback;	
	  n_outs = (channel == 0)? 2: 1;
	}
    }
  else if((type == fts_new_symbol("control-change") || type == fts_new_symbol("cc")) && ac <= 3)
    {
      classdata->status = fts_midi_status_control_change;

      if(filter == NO_FILTER)
	{
	  classdata->callback.channel_message = (channel == 0)? poly_callback: poly_channel_callback;
	  n_outs = (channel == 0)? 3: 2;
	}
      else
	{
	  classdata->callback.channel_message = (channel == 0)? poly_filter_callback: poly_filter_channel_callback;	
	  n_outs = (channel == 0)? 2: 1;
	}
    }
  else if((type == fts_new_symbol("program-change") || type == fts_new_symbol("pc")) && ac <= 2)
    {
      classdata->status = fts_midi_status_program_change;
      classdata->callback.channel_message = (channel == 0)? pgm_callback: pgm_channel_callback;
      
      n_outs = (channel == 0)? 2: 1;
    } 
  else if((type == fts_new_symbol("channel-pressure") || type == fts_new_symbol("cp")) && ac <= 2)
   {
      classdata->status = fts_midi_status_channel_pressure;
      classdata->callback.channel_message = (channel == 0)? pgm_callback: pgm_channel_callback;
      
      n_outs = (channel == 0)? 2: 1;
    }
  else if((type == fts_new_symbol("pitch-bend") || type == fts_new_symbol("pb")) && ac <= 2)
    {
      classdata->status = fts_midi_status_pitch_bend;
      classdata->callback.channel_message = (channel == 0)? pgm_callback: pgm_channel_callback;
      
      n_outs = (channel == 0)? 2: 1;
    }
  else
    return &fts_CannotInstantiate;    
  
  fts_class_init(cl, sizeof(midiin_t), 0, n_outs, (void *)classdata);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiin_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, midiin_delete);

  return fts_Success;
}

int
midiin_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  /* skip class name */
  ac0--;
  ac1--;
  at0++;
  at1++;

  if(ac0 <= 0 || ac1 <= 0)
    return 0;

  /* skip port argument */
  if(fts_is_object(at0))
    {
      ac0--;
      at0++;
    }

  if(fts_is_object(at1))
    {
      ac1--;
      at1++;      
    }

  if(ac0 <= 0 || ac1 <= 0 || !fts_is_symbol(at0) || !fts_is_symbol(at1))
    return 0;

  return (fts_get_symbol(at0) == fts_get_symbol(at1));
}

void
midiin_config(void)
{
  fts_metaclass_install(fts_new_symbol("midiin"), midiin_instantiate, midiin_equiv);
}
