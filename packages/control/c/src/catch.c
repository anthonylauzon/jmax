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
#include "bus.h"

/* "catch". get messages from a bus.
 * catch <bus> <ch1> .. <chn>
 */

typedef struct catch
{
  fts_object_t  o;
  fts_bus_t *bus;
  int noutlets;
  int *channels;
} catch_t;

static void
catch_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;
  int i;

  if(ac < 2 && fts_get_type(at + 1) != fts_s_bus)
    {
      fts_object_set_error(o, "no bus specified");
      return;
    }

  this->bus = fts_get_ptr(at+1);

  if (ac > 2)
    {
      this->noutlets = ac - 2;
      this->channels = (int *)fts_block_alloc(this->noutlets * sizeof(int));

      for (i = 0; i < this->noutlets; i++)
	{
	  int ch = fts_get_int(at + i + 2) - 1;

	  if ((ch < this->bus->nch) && (ch >= 0))
	    {
	      this->channels[i] = ch;
	      fts_bus_add_output(this->bus, this->channels[i], o, i);
	    }
	  else
	    this->channels[i] = -1;
	}
    }
  else
    {
      this->noutlets = this->bus->nch;
      this->channels = (int *)fts_block_alloc(this->noutlets * sizeof(int));

      for (i = 0; i < this->noutlets; i++)
	{
	  this->channels[i] = i;
	  fts_bus_add_output(this->bus, i, o, i);
	}
    }
}

static void
catch_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  catch_t *this = (catch_t *) o;
  int i;

  for (i = 0; i < this->noutlets; i++)
    if (this->channels[i] >= 0)
      fts_bus_remove_output(this->bus, this->channels[i], o, i);

  fts_block_free(this->channels, this->noutlets * sizeof(int));
}

static fts_status_t
catch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];
  int i, outlets;

  if (ac > 2)
    outlets = ac - 2;
  else
    outlets = ((fts_bus_t *) fts_get_ptr(at + 1))->nch;

  fts_class_init(cl, sizeof(catch_t), 0, outlets, 0); 

  /* define the system methods */

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, catch_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, catch_delete, 0, 0);

  return fts_Success;
}

void
catch_config(void)
{
  fts_metaclass_install(fts_new_symbol("catch"), catch_instantiate, fts_bus_equiv);
}
