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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette, Zack Settel.
 *
 */

#include <fts/fts.h>
#include <utils/c/include/utils.h>

#define DRUNK_MAX 0x7FFF

typedef struct 
{
  fts_object_t ob;
  int state;
  double scaler;
  int range;
} drunk_t;

/* output a value  */
static void
drunk_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *this = (drunk_t *)o;
  int value;
  int offset;

  offset = 1 + (int)(this->scaler * (0x7FFF & fts_random()));

  if (this->state > this->range)
    this->state = this->range;
  else if (this->state < 0)
    this->state = 0;

  if(fts_random() & 512)
    offset = -offset;

  value = offset + this->state;

  if (value < 0 || value > this->range)
    value = this->state - offset;

  this->state = value;

  fts_outlet_int(o, 0, value);
}

static void
drunk_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *this = (drunk_t *)o;
  int n = fts_get_number_int(at);

  if(n < 0)
    n = 0;
  else if (n > this->range)
    n = this->range;

  this->state = n;

  fts_outlet_int(o, 0, n);
}

static void
drunk_set_correlation(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *this = (drunk_t *)o;
  int n = fts_get_number_int(at);

  if(n > this->range / 2)
    n = this->range / 2;
  
  this->scaler = (double)n / (double)DRUNK_MAX;
}

static void
drunk_set_range(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *this = (drunk_t *)o;
  int n = fts_get_number_int(at);

  if(n < 1)
    n = 1;

  this->range = n;

  if(this->state >= n)
    this->state = n - 1;
}

static void
drunk_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *this = (drunk_t *)o;

  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	drunk_set_correlation(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	drunk_set_range(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	{
	  int n;
	  
	  if(ac > 0 && fts_is_number(at))
	    n = fts_get_number_int(at);
	  
	  this->state = n % this->range;
	}
    case 0:
      break;
    }
}


static void
drunk_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  switch(ac)
    {
    default:
    case 3:
      if(fts_is_number(at + 2))
	drunk_set_correlation(o, 0, 0, 1, at + 2);
    case 2:
      if(fts_is_number(at + 1))
	drunk_set_range(o, 0, 0, 1, at + 1);
    case 1:
      if(fts_is_number(at + 0))
	drunk_number(o, 0, 0, 1, at);
    case 0:
      break;
    }
}

static void
drunk_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  drunk_t *this = (drunk_t *)o;
  
  this->range = 128;
  this->state = this->range / 2;
  this->scaler = (double)1.0 / (double)DRUNK_MAX;

  if(ac > 0 && fts_is_number(at))
    drunk_set_range(o, 0, 0, 1, at);

  if(ac > 1 && fts_is_number(at + 1))
    drunk_set_correlation(o, 0, 0, 1, at + 1);
}


static fts_status_t
drunk_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(drunk_t), 3, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, drunk_init);

  fts_method_define_varargs(cl, 0, fts_s_set, drunk_set);

  fts_method_define_varargs(cl, 0, fts_s_bang, drunk_bang);
  fts_method_define_varargs(cl, 0, fts_s_int, drunk_number);
  fts_method_define_varargs(cl, 0, fts_s_float, drunk_number);
  fts_method_define_varargs(cl, 0, fts_s_list, drunk_atoms);

  fts_method_define_varargs(cl, 1, fts_s_int, drunk_set_range);
  fts_method_define_varargs(cl, 1, fts_s_float, drunk_set_range);

  fts_method_define_varargs(cl, 2, fts_s_int, drunk_set_correlation);
  fts_method_define_varargs(cl, 2, fts_s_float, drunk_set_correlation);

  return fts_Success;
}

void
drunk_config(void)
{
  fts_class_install(fts_new_symbol("drunk"), drunk_instantiate);
}

