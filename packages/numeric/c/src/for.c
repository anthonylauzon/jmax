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

typedef struct 
{
  fts_object_t o;
  double init;
  double limit;
  double incr;
  int is_int;
} for_t;

static void
for_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_t *this = (for_t *)o;

  if(this->is_int)
    {
      double init = this->init;
      double limit = this->limit;
      double incr = this->incr;
      double f;
      
      if(incr > 0.0 && init < limit)
	{
	  for(f=init; f<limit; f+=incr)
	    fts_outlet_float(o, 0, f);
	}
      else if(incr < 0.0 && init > limit)
	{
	  for(f=init; f>limit; f+=incr)
	    fts_outlet_float(o, 0, f);
	}
    }
  else
    {
      int init = this->init;
      int limit = this->limit;
      int incr = this->incr;
      int i;

      if(incr > 0 && init < limit)
	{
	  for(i=init; i<limit; i+=incr)
	    fts_outlet_int(o, 0, i);
	}
      else if(incr < 0 && init > limit)
	{
	  for(i=init; i>limit; i+=incr)
	    fts_outlet_int(o, 0, i);
	}      
    }
}

static void
for_set_init_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_t *this = (for_t *)o;

  this->init = fts_get_number_float(at);
  for_go(o, 0, 0, 0, 0);
}

static void
for_set_limit(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_t *this = (for_t *)o;

  this->limit = fts_get_number_float(at);
}

static void
for_set_incr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_t *this = (for_t *)o;

  this->incr = fts_get_number_float(at);
}

static void
for_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  for_t *this = (for_t *)o;
  
  switch(ac)
    {
    case 3:
      this->incr = fts_get_number_float(at + 2);
    case 2:
      this->limit = fts_get_number_float(at + 1);
    case 1:
      this->init = fts_get_number_float(at + 0);
    }
}

static void
for_set_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_t *this = (for_t *)o;

  for_set(o, 0, 0, ac, at);
  for_go(o, 0, 0, 0, 0);
}

/************************************************************
 *
 *  class
 *
 */

static void
for_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  for_t *this = (for_t *)o;
  int is_int = 1;
  int i;
  
  for(i=0; i<ac; i++)
    if(!fts_is_int(at + i))
      is_int = 0;

  this->init = 0.0;
  this->limit = 0.0;
  this->incr = 1.0;

  for_set(o, 0, 0, ac, at);
}

static void
for_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(for_t), for_init, NULL);

  fts_class_message_varargs(cl, fts_s_set, for_set);
  fts_class_message_varargs(cl, fts_s_bang, for_go);
  fts_class_inlet_varargs(cl, 0, for_set_and_go);

  fts_class_inlet_number(cl, 0, for_set_init_and_go);
  fts_class_inlet_number(cl, 1, for_set_limit);
  fts_class_inlet_number(cl, 2, for_set_incr);

  fts_class_outlet_number(cl, 0);
}

void
for_config(void)
{
  fts_class_install(fts_new_symbol("for"), for_instantiate);
}
