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

/************************************************************
 *
 *  object
 *
 */

typedef struct 
{
  fts_object_t o;
  int open;
} oneshot_t;

static void
oneshot_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  oneshot_t *this = (oneshot_t *)o;
  
  if(ac == 0)
    this->open = 1;
  else if(ac == 1 && fts_is_int(at))
    {
      int n = fts_get_int(at);

      this->open = (n != 0);
    }
  else
    fts_object_set_error(o, "bad arguments");
}

/************************************************************
 *
 *  user methods
 *
 */

static void
oneshot_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  oneshot_t *this = (oneshot_t *)o;

  if(winlet == 0)
    {
      if(this->open != 0)
	{
	  this->open = 0;
	  fts_outlet_send(o, 0, s, ac, at);
	}
    }
  else if(s == NULL)
    {
      if(ac == 0)
	this->open = 1;
      else if(ac > 0 && fts_is_number(at))
	this->open = fts_get_number_int(at);    
      else
	fts_object_signal_runtime_error(o, "bad input for inlet 1");
    }
}

static void
oneshot_dummy(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

/************************************************************
 *
 *  class
 *
 */
static void
oneshot_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(oneshot_t), oneshot_init, NULL);

  fts_class_input_handler(cl, oneshot_input);

  fts_class_inlet_thru(cl, 0);
  fts_class_inlet_void(cl, 1, oneshot_dummy); /* bang dummy */
  fts_class_inlet_number(cl, 1, oneshot_dummy); /* number dummy */

  fts_class_outlet_thru(cl, 0);
}

void
oneshot_config(void)
{
  fts_class_install(fts_new_symbol("oneshot"), oneshot_instantiate);
}
