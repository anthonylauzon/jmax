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

#include <fts/fts.h>

static void midiout_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void midiout_set_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

static fts_symbol_t sym_omni = 0;

static int 
midiout_check(int ac, const fts_atom_t *at, fts_midiport_t **port, int *channel, int *number)
{
  *port = 0;
  *channel = 1;
  *number = 0;

  if(ac > 0)
    {
      if(fts_is_object(at))
	{
	  fts_object_t *obj = fts_get_object(at);

	  if(fts_midiport_check(obj) && fts_midiport_is_output((fts_midiport_t *)obj))
	    {
	      *port = (fts_midiport_t *)fts_get_object(at);
	  
	      /* skip port argument */
	      ac--;
	      at++;
	    }
	  else
	    return 0;
	}
  
      if(ac == 2)
	{
	  if(fts_is_number(at))
	    {
	      int n = fts_get_number_int(at);;
	      
	      *number = (n < 0)? 0: ((n > 127)? 127: n);      
	      
	      /* skip port argument */
	      ac--;
	      at++;
	    }
	  else
	    return 0;
	}
      else
	*number = 0;

      if(ac == 1)
	{
      
	  if(fts_is_number(at))
	    {
	      int n = fts_get_number_int(at);
	  
	      *channel = (n < 0)? 0: ((n > 16)? 16: n);
	    }
	  else
	    return 0;
	}
    }
  
  /* if there is still no port just get default */
  if(!*port)
    *port = fts_midiport_get_default();
  
  if(!*port)
    return 0;

  return 1;
}

/************************************************************
 *
 *  object
 *
 */

typedef struct _midiout_
{
  fts_object_t o;
  fts_midiport_t *port;
  int channel;
  int number;
} midiout_t;

static void
midiout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  midiout_t *this = (midiout_t *)o;
  fts_midiport_t *port = 0;

  midiout_check(ac - 1, at + 1, &this->port, &this->channel, &this->number);
}

static void
midiout_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  int channel = fts_get_number_int(at);
  
  if(channel < 1)
    this->channel = 1;
  else if(channel > 16)
    this->channel = 16;
  else
    this->channel = channel;
}

static void
midiout_set_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;
  
  this->number = fts_get_number_int(at) & 127;
}

static void
noteout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 3:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 2:
	  midiout_set_number(o, 0, 0, 1, at + 1);
	case 1:
	  {
	    int value = fts_get_number_int(at);
	    
	    if(value < 0)
	      value = 0;
	    else if(value > 127)
	      value = 127;
	    
	    fts_midiport_output_note(this->port, this->channel, value, this->number, 0.0);
	  }
	case 0:
	  break;
	}
    }
}

static void
polyout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 3:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 2:
	  midiout_set_number(o, 0, 0, 1, at + 1);
	case 1:
	  {
	    int value = fts_get_number_int(at);
	    
	    if(value < 0)
	      value = 0;
	    else if(value > 127)
	      value = 127;
	    
	    fts_midiport_output_poly_pressure(this->port, this->channel, this->number, value, 0.0);
	  }
	case 0:
	  break;
	}
    }
}

static void
ctlout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 3:
	  midiout_set_channel(o, 0, 0, 1, at + 2);
	case 2:
	  midiout_set_number(o, 0, 0, 1, at + 1);
	case 1:
	  {
	    int value = fts_get_number_int(at);
	    
	    if(value < 0)
	      value = 0;
	    else if(value > 127)
	      value = 127;
	    
	    fts_midiport_output_control_change(this->port, this->channel, this->number, value, 0.0);
	  }
	case 0:
	  break;
	}
    }
}

static void
pgmout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 2:
	  midiout_set_channel(o, 0, 0, 1, at + 1);
	case 1:
	  {
	    int value = fts_get_number_int(at);
	    
	    if(value < 0)
	      value = 0;
	    else if(value > 127)
	      value = 127;
	    
	    fts_midiport_output_program_change(this->port, this->channel, fts_get_number_int(at) & 127, 0.0);
	  }
	case 0:
	  break;
	}
    }
}

static void
touchout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 2:
	  midiout_set_channel(o, 0, 0, 1, at + 1);
	case 1:
	  {
	    int value = fts_get_number_int(at);
	    
	    if(value < 0)
	      value = 0;
	    else if(value > 127)
	      value = 127;
	    
	    fts_midiport_output_channel_pressure(this->port, this->channel, value, 0.0);
	  }
	case 0:
	  break;
	}
    }
}

static void
bendout_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  midiout_t *this = (midiout_t *)o;

  if(this->port)
    {
      switch (ac)
	{
	default:
	case 2:
	  midiout_set_channel(o, 0, 0, 1, at + 1);
	case 1:
	  {
	    int value = fts_get_number_int(at);
	    
	    if(value < 0)
	      value = 0;
	    else if(value > 127)
	      value = 127;
	    
	    fts_midiport_output_pitch_bend(this->port, this->channel, value << 7, 0.0);
	  }
	case 0:
	  break;
	}
    }
}

/************************************************************
 *
 *  class
 *
 */

static fts_status_t
noteout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiout_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, noteout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, noteout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, noteout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_number);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_number);
  
  fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  return fts_Success;
}

static fts_status_t
polyout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiout_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, polyout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, polyout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, polyout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_number);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_number);
  
  fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  return fts_Success;
}

static fts_status_t
ctlout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiout_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), 3, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, ctlout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, ctlout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, ctlout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_number);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_number);
  
  fts_method_define_varargs(cl, 2, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 2, fts_s_float, midiout_set_channel);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  return fts_Success;
}

static fts_status_t
pgmout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiout_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, pgmout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, pgmout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, pgmout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  return fts_Success;
}

static fts_status_t
touchout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiout_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, touchout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, touchout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, touchout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  return fts_Success;
}


static fts_status_t
bendout_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_midiport_t *port;
  int channel;
  int number;

  if(!midiout_check(ac - 1, at + 1, &port, &channel, &number))
    return &fts_CannotInstantiate;    

  fts_class_init(cl, sizeof(midiout_t), 2, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_int, bendout_send);
  fts_method_define_varargs(cl, 0, fts_s_float, bendout_send);
  fts_method_define_varargs(cl, 0, fts_s_list, bendout_send);
  
  fts_method_define_varargs(cl, 1, fts_s_int, midiout_set_channel);
  fts_method_define_varargs(cl, 1, fts_s_float, midiout_set_channel);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, midiout_init);

  return fts_Success;
}



static int 
midiout_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{ 
  fts_midiport_t *port;
  int channel;
  int number;

  return midiout_check(ac1 - 1, at1 + 1, &port, &channel, &number);
}

void
midiout_config(void)
{
  sym_omni = fts_new_symbol("omni");

  fts_metaclass_install(fts_new_symbol("noteout"), noteout_instantiate, midiout_equiv);
  fts_metaclass_install(fts_new_symbol("polyout"), polyout_instantiate, midiout_equiv);
  fts_metaclass_install(fts_new_symbol("ctlout"), ctlout_instantiate, midiout_equiv);
  fts_metaclass_install(fts_new_symbol("pgmout"), pgmout_instantiate, midiout_equiv);
  fts_metaclass_install(fts_new_symbol("touchout"), touchout_instantiate, midiout_equiv);
  fts_metaclass_install(fts_new_symbol("bendout"), bendout_instantiate, midiout_equiv);
}
