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
#include "bus.h"

/* "throw". send messages to a bus.
 * throw <bus> <ch1> .. <chn>
 */

typedef struct throw
{
  fts_object_t o;
  fts_bus_t *bus;
  int ninlets;
  int *channels;
} throw_t;

static void
throw_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;
  int ch;

  ch = this->channels[winlet];

  if (ch >= 0)
    fts_bus_send_message(this->bus, ch, s, ac, at);
}

static void
throw_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  throw_t *this = (throw_t *) o;
  int i;

  if(ac < 2 && fts_get_type(at + 1) != fts_s_bus)
    {
      fts_object_set_error(o, "no bus specified");
      return;
    }

  this->bus = (fts_bus_t *)fts_get_ptr(at + 1);

  if (ac > 2)
    {
      this->ninlets = ac - 2;
      this->channels = (int *)fts_block_alloc(this->ninlets * sizeof(int));

      for (i = 0; i < this->ninlets; i++)
	{
	  int ch = fts_get_int(at + i + 2) - 1;

	  if ((ch < this->bus->nch) && (ch >= 0))
	    {
	      this->channels[i] = ch;
	      fts_bus_add_input(this->bus, this->channels[i], o);
	    }
	  else
	    this->channels[i] = -1;
	}
    }
  else
    {
      this->ninlets = this->bus->nch;
      this->channels = (int *)fts_block_alloc(this->ninlets * sizeof(int));

      for (i = 0; i < this->ninlets; i++)
	{
	  this->channels[i] = i;
	  fts_bus_add_input(this->bus, i, o);
	}
    }
}

static void
throw_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  throw_t *this = (throw_t *) o;

  for (i = 0; i < this->ninlets; i++)
    if (this->channels[i] >= 0)
      fts_bus_remove_input(this->bus, this->channels[i], o);

  fts_block_free(this->channels, this->ninlets * sizeof(int));
}

static fts_status_t
throw_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];
  int i, inlets;

  /* initialize the class */

  if (ac > 2)
    inlets = ac - 2;
  else
    inlets = ((fts_bus_t *) fts_get_ptr(at + 1))->nch;

  fts_class_init(cl, sizeof(throw_t), inlets, 0, 0); 

  /* define the system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, throw_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, throw_delete, 0, 0);

  for (i = 0; i < inlets; i++)
    fts_method_define_varargs(cl, i, fts_s_anything, throw_anything);

  return fts_Success;
}

void
throw_config(void)
{
  fts_metaclass_install(fts_new_symbol("throw"), throw_instantiate, fts_bus_equiv);
}
