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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#include <fts/fts.h>

static fts_class_t *bus_class = 0;
static fts_metaclass_t *bus_type = 0;
static fts_symbol_t bus_symbol = 0;

static fts_symbol_t throw_symbol = 0;
static fts_symbol_t catch_symbol = 0;

typedef struct _bus_
{
  fts_object_t head; /* channel 0 for bus access */
  fts_channel_t *channels; /* index 0: bus, channel n: bus  n-1 */
  int n_channels;
} bus_t;

typedef struct _access_
{
  fts_object_t head; /* bus listens to itself */
  bus_t *bus;
  int channel;
  int n_channels;
} access_t;

#define bus_get_channel(b, i) (&((b)->channels[i + 1]))
#define bus_get_size(b) ((b)->n_channels)

#define BUS_NOWHERE -2
#define BUS_CHANNEL -1

static int
clip_channel(int channel, int n_channels)
{
  if(channel >= 0 && channel < n_channels )
    return channel;
  else
    return BUS_NOWHERE;
}

/*****************************************************************************
 *
 *  throw
 *
 */

static void
throw_bus_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  bus_t *bus = this->bus;
  int n_channels = bus_get_size(bus);

  if(winlet < n_channels)
    {  
      fts_channel_send(bus_get_channel(bus, winlet), 0, s, ac, at);
      fts_channel_send(bus_get_channel(bus, BUS_CHANNEL), winlet, s, ac, at);
    }
}

static void
throw_channel_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  bus_t *bus = this->bus;
  
  if(this->channel != BUS_NOWHERE)
    {
      fts_channel_send(bus_get_channel(bus, this->channel), 0, s, ac, at);
      fts_channel_send(bus_get_channel(bus, BUS_CHANNEL), this->channel, s, ac, at);
    }
}

static void
throw_set_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  fts_channel_remove_origin(bus_get_channel(this->bus, BUS_CHANNEL), o);
  fts_object_release((fts_object_t *)this->bus);

  this->bus = (bus_t *)fts_get_object(at + 0);

  fts_object_refer((fts_object_t *)this->bus);
  fts_channel_add_origin(bus_get_channel(this->bus, BUS_CHANNEL), o);
}

static void
throw_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  int n_channels = bus_get_size(this->bus);
  int channel = fts_get_number_int(at + 0);
  
  channel = clip_channel(channel, n_channels); 

  if(channel != this->channel)
    {
      if(this->channel != BUS_NOWHERE)
	fts_channel_remove_origin(bus_get_channel(this->bus, this->channel), o);

      if(channel != BUS_NOWHERE)
	fts_channel_add_origin(bus_get_channel(this->bus, channel), o);

      this->channel = channel;
    }
}

static void
throw_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  this->bus = (bus_t *)fts_get_object(at + 0);
  fts_object_refer((fts_object_t *)this->bus);
  
  this->n_channels = bus_get_size(this->bus);

  if(ac == 2)
    {
      int channel = fts_get_int(at + 1);
      this->channel = clip_channel(channel, this->n_channels);
    }
  else
    this->channel = BUS_CHANNEL;

  if(this->channel != BUS_NOWHERE)
    fts_channel_add_origin(bus_get_channel(this->bus, this->channel), o);
}	

static void
throw_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  if(this->channel != BUS_NOWHERE)
    fts_channel_remove_target(bus_get_channel(this->bus, this->channel), o);
  
  fts_object_release((fts_object_t *)this->bus);  
}

static fts_status_t
throw_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_a( at, bus_type) && (ac == 1 || (ac == 2 && fts_is_int(at + 1))))  
    {
      if(ac == 1) 
	{
	  /* bus */
	  bus_t *bus = (bus_t *)fts_get_object(at);
	  int n_channels = bus_get_size(bus);
	  int i;

	  fts_class_init(cl, sizeof(access_t), n_channels + 1, 0, 0);
	  
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, throw_init);
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, throw_delete);
	  
	  for(i=0; i<n_channels; i++)
	    fts_method_define_varargs(cl, i, fts_s_anything, throw_bus_input);
	  
	  fts_method_define_varargs(cl, n_channels, bus_symbol, throw_set_bus);
	}
      else
	{
	  /* channel */
	  fts_class_init(cl, sizeof(access_t), 2, 0, 0);
	  
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, throw_init);
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, throw_delete);
	  
	  fts_method_define_varargs(cl, 0, fts_s_anything, throw_channel_input);
	  fts_method_define_varargs(cl, 1, fts_s_int, throw_set_channel);
	}

      return fts_ok;
    }
  else
    return &fts_CannotInstantiate;
}

/*****************************************************************************
 *
 *  catch
 *
 */

static void
catch_set_bus(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  fts_channel_remove_target(bus_get_channel(this->bus, BUS_CHANNEL), o);
  fts_object_release((fts_object_t *)this->bus);

  this->bus = (bus_t *)fts_get_object(at + 0);

  fts_object_refer((fts_object_t *)this->bus);
  fts_channel_add_target(bus_get_channel(this->bus, BUS_CHANNEL), o);
}

static void
catch_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  int n_channels = bus_get_size(this->bus);
  int channel = fts_get_number_int(at + 0);
  
  channel = clip_channel(channel, n_channels); 

  if(channel != this->channel)
    {
      if(this->channel != BUS_NOWHERE)
	fts_channel_remove_target(bus_get_channel(this->bus, this->channel), o);

      if(channel != BUS_NOWHERE)
	fts_channel_add_target(bus_get_channel(this->bus, channel), o);

      this->channel = channel;
    }
}

static void
catch_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  this->bus = (bus_t *)fts_get_object(at + 0);
  fts_object_refer((fts_object_t *)this->bus);
  
  this->n_channels = bus_get_size(this->bus);

  if(ac == 2)
    {
      int channel = fts_get_int(at + 1);
      this->channel = clip_channel(channel, this->n_channels);
    }
  else
    this->channel = BUS_CHANNEL;

  if(this->channel != BUS_NOWHERE)
    fts_channel_add_target(bus_get_channel(this->bus, this->channel), o);
}	

static void
catch_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  if(this->channel != BUS_NOWHERE)
    fts_channel_remove_target(bus_get_channel(this->bus, this->channel), o);
  
  fts_object_release((fts_object_t *)this->bus);  
}

static fts_status_t
catch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_a(at, bus_type) && (ac == 1 || (ac == 2 && fts_is_int(at + 1))))
    {
      if(ac == 1) 
	{
	  /* bus */
	  bus_t *bus = (bus_t *)fts_get_object(at);
	  int n_channels = bus_get_size(bus);

	  fts_class_init(cl, sizeof(access_t), 1, n_channels, 0);
	  
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, catch_init);
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, catch_delete);
	  
	  fts_method_define_varargs(cl, 0, bus_symbol, catch_set_bus);
	}
      else
	{
	  /* channel */
	  fts_class_init(cl, sizeof(access_t), 1, 1, 0);
	  
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, catch_init);
	  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, catch_delete);
	  
	  fts_method_define_varargs(cl, 0, fts_s_int, catch_set_channel);
	}

      return fts_ok;
    }
  else
    return &fts_CannotInstantiate;
}

/*****************************************************************************
 *
 *  bus
 *
 */

static void
bus_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;

  fts_channel_send(bus_get_channel(this, winlet), 0, s, ac, at);
  fts_channel_send(bus_get_channel(this, BUS_CHANNEL), winlet, s, ac, at);
}

static void
bus_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;
  int n_channels = 0;
  int i;
  
  if(ac == 0)
    n_channels = 1;
  else if(ac == 1 && fts_is_int(at))
    n_channels = fts_get_int(at);

  this->channels = (fts_channel_t *)fts_malloc(sizeof(fts_channel_t) * (n_channels + 1));

  for(i=0; i<n_channels+1; i++)
    fts_channel_init(this->channels + i);

  this->n_channels = n_channels;

  fts_channel_add_target(bus_get_channel(this, BUS_CHANNEL), o);  
}

static void
bus_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;
  
  fts_free(this->channels);
}

static void
bus_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

static fts_status_t
bus_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int n_channels = 0;
  int i;

  if(ac == 0)
    n_channels = 1;
  else if(ac == 1 && fts_is_int(at))
    n_channels = fts_get_int(at);
  else
    return &fts_CannotInstantiate;

  fts_class_init(cl, sizeof(bus_t), n_channels, n_channels, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, bus_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, bus_delete);

  for(i=0; i<n_channels; i++)
    fts_method_define_varargs(cl, i, fts_s_anything, bus_input);
	  
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, bus_get_state);
  
  return fts_ok;
}

void
bus_config(void)
{
  throw_symbol = fts_new_symbol("throw");
  catch_symbol = fts_new_symbol("catch");
  bus_symbol = fts_new_symbol("bus");

  bus_type = fts_metaclass_install(bus_symbol, bus_instantiate, fts_first_arg_equiv);
  fts_metaclass_install(throw_symbol, throw_instantiate, fts_never_equiv);
  fts_metaclass_install(catch_symbol, catch_instantiate, fts_never_equiv);
}
