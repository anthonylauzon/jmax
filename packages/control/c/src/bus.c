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

static fts_class_t *bus_type = 0;
static fts_symbol_t bus_symbol = 0;

static fts_symbol_t throw_symbol = 0;
static fts_symbol_t catch_symbol = 0;

typedef struct _bus_
{
  fts_object_t head;
  fts_channel_t *channels;
  int n_channels;
} bus_t;

typedef struct _access_
{
  fts_object_t head;
  bus_t *bus;
  int index;
} access_t;


static fts_channel_t *
bus_get_channel(bus_t *bus, int index)
{
  int n_channels = bus->n_channels;

  if(index >= n_channels)
    return &bus->channels[n_channels - 1];
  else
    return &bus->channels[index];
}

static void
access_set_channel(access_t *this, bus_t *bus, int index)
{
  fts_channel_t *channel = bus_get_channel(bus, index);
  
  fts_object_release((fts_object_t *)this->bus);
  
  this->bus = bus;
  this->index = index;
  
  fts_object_refer((fts_object_t *)bus);
}

static void
access_set_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  int index = fts_get_number_int(at);

  if(index < 0)
    this->index = 0;
  else
    this->index = index;
}

/*****************************************************************************
 *
 *  throw
 *
 */

static void
throw_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  
  if(winlet == 0)
    fts_channel_send(bus_get_channel(this->bus, this->index), 0, s, ac, at);
  else
    fts_class_default_error_handler(o, winlet, s, ac, at);
}

static void
throw_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  int index = 0;

  if(ac > 1 && fts_is_number(at + 1))
    index = fts_get_number_int(at + 1);

  if(index < 0)
    index = 0;

  if(ac > 0 && fts_is_a(at, bus_type))
    {
      bus_t *bus = (bus_t *)fts_get_object(at);

      /*fts_channel_remove_origin(bus_get_channel(this->bus, this->index), o);*/
      access_set_channel(this, bus, index);
      /*fts_channel_add_origin(bus_get_channel(bus, index), o);*/
    }
  else if(winlet > 0)
    fts_object_signal_runtime_error(o, "bad value at inlet %d (bus required)", winlet);
  else
    fts_object_set_error(o, "bus required");
}

static void
throw_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  throw_set_channel(o, fts_system_inlet, 0, ac, at);
}	

static void
throw_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  /*fts_channel_remove_origin(bus_get_channel(this->bus, this->index), o);*/
  fts_object_release((fts_object_t *)this->bus);
}

static void
throw_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(access_t), throw_init, throw_delete);

  fts_class_set_default_handler(cl, throw_input);
  fts_class_inlet_anything(cl, 0);

  fts_class_inlet_varargs(cl, 1, throw_set_channel);
  fts_class_inlet(cl, 1, bus_type, throw_set_channel);
  fts_class_inlet_number(cl, 0, access_set_index);
}

/*****************************************************************************
 *
 *  catch
 *
 */

static void
catch_set_channel(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;
  int index = 0;

  if(ac > 1 && fts_is_number(at + 1))
    index = fts_get_number_int(at + 1);

  if(index < 0)
    index = 0;

  if(ac > 0 && fts_is_a(at, bus_type))
    {
      bus_t *bus = (bus_t *)fts_get_object(at);

      fts_channel_remove_target(bus_get_channel(this->bus, this->index), o);
      access_set_channel(this, bus, index);
      fts_channel_add_target(bus_get_channel(bus, index), o);
    }
  else if(winlet > 0)
    fts_object_signal_runtime_error(o, "bad value at inlet %d (bus required)", winlet);
  else
    fts_object_set_error(o, "bus required");
}

static void
catch_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  catch_set_channel(o, fts_system_inlet, 0, ac, at);
}	

static void
catch_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  access_t *this = (access_t *)o;

  /*fts_channel_remove_origin(bus_get_channel(this->bus, this->index), o);*/
  fts_object_release((fts_object_t *)this->bus);
}

static void
catch_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(access_t), catch_init, catch_delete);
  
  fts_class_inlet_varargs(cl, 0, catch_set_channel);
  fts_class_inlet(cl, 0, bus_type, catch_set_channel);
  fts_class_inlet_number(cl, 0, access_set_index);
}

/*****************************************************************************
 *
 *  bus
 *
 */

static void
bus_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  bus_t *this = (bus_t *)o;
  int n_channels = 0;
  int i;
  
  if(ac > 0 && fts_is_number(at))
    n_channels = fts_get_int(at);
  
  /* at least one channel */
  if(n_channels < 1)
    n_channels = 1;

  this->channels = (fts_channel_t *)fts_malloc(sizeof(fts_channel_t) * n_channels);

  for(i=0; i<n_channels; i++)
    fts_channel_init(this->channels + i);

  this->n_channels = n_channels;
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

static void
bus_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(bus_t), bus_init, bus_delete);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, bus_get_state);
}

void
bus_config(void)
{
  throw_symbol = fts_new_symbol("throw");
  catch_symbol = fts_new_symbol("catch");
  bus_symbol = fts_new_symbol("bus");

  bus_type = fts_class_install(bus_symbol, bus_instantiate);
  fts_class_install(throw_symbol, throw_instantiate);
  fts_class_install(catch_symbol, catch_instantiate);
}
