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
  int out;
  int n;
} demux_t;

static void
demux_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  demux_t *this = (demux_t *)o;
  int n = fts_get_int_arg(ac, at, 0, 0);
  int i = fts_get_int_arg(ac, at, 1, 0);
  
  if(n < 2)
    n = 2;

  this->n = n;
  fts_object_set_outlets_number(o, n);

  if(i >= 0 && i < n)
    this->out = i;
  else
    this->out = 0;
}

static void
demux_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

/************************************************************
 *
 *  user methods
 *
 */

static void
demux_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  demux_t *this = (demux_t *)o;

  fts_outlet_send(o, this->out, s, ac, at);
}

static void
demux_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  demux_t *this = (demux_t *)o;
  int i = fts_get_number_int(at);
  int n = this->n;
  
  if(i >= 0 && i < n)
    this->out = i;
}

/************************************************************
 *
 *  class
 *
 */
static void
demux_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(demux_t), demux_init, demux_delete);
  
  fts_class_set_default_handler(cl, demux_input);
  fts_class_inlet(cl, 0, NULL, demux_input);
  
  fts_class_inlet_int(cl, 1, demux_set);
  fts_class_inlet_float(cl, 1, demux_set);

  fts_class_outlet_anything(cl, 0);
  fts_class_outlet_anything(cl, 1);
  }

void
demux_config(void)
{
  fts_class_install(fts_new_symbol("demux"), demux_instantiate);
}
