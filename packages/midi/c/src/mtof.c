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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */

#include <fts/fts.h>
#include <float.h>
#include <math.h>

typedef struct 
{
  fts_object_t o;
  double ref_freq;
  double ref_note;
} mtof_t;

static void
mtof_tune(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mtof_t *this = (mtof_t *)o;

  if(ac > 1)
    {
      if(fts_is_number(at))
	{
	  double note = fts_get_number_float(at);

	  if(note > 0.0)
	    this->ref_note = note;
	  else
	    this->ref_note = 0.0;
	}

      ac--;
      at++;
    }
  
  if(ac > 0 && fts_is_number(at))
    {
      double freq = fts_get_number_float(at);

      if(freq > 0.0)
	this->ref_freq = freq;	
      else
	this->ref_freq = 0.0;
    }
}
static void
mtof_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mtof_t *this = (mtof_t *)o;
  double midi = fts_get_number_float(at);
  double freq = this->ref_freq * exp(0.057762265 * (midi - this->ref_note));
  
  fts_outlet_float(o, 0, freq);
}

static void
ftom_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mtof_t *this = (mtof_t *)o;
  double freq = fts_get_number_float(at);

  if(freq > 0.0)
    {
      double midi = this->ref_note + log(freq / this->ref_freq) * 17.31234049;
      fts_outlet_float(o, 0, midi);
    }
  else
    fts_outlet_float(o, 0, DBL_MIN);    
}

static void
mtof_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  mtof_t *this = (mtof_t *)o;

  this->ref_note = 69.0;
  this->ref_freq = 440.0;

  mtof_tune(o, 0, 0, ac, at);
}

static fts_status_t
mtof_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(mtof_t), 1, 1, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, mtof_init);

  fts_method_define_varargs(cl, 0, fts_s_int, mtof_number);
  fts_method_define_varargs(cl, 0, fts_s_float, mtof_number);
  fts_method_define_varargs(cl, 0, fts_new_symbol("tune"), mtof_tune);

  fts_outlet_type_define_varargs(cl, 0,	fts_s_float);

  return fts_ok;
}

static fts_status_t
ftom_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(mtof_t), 1, 1, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, mtof_init);

  fts_method_define_varargs(cl, 0, fts_s_int, ftom_number);
  fts_method_define_varargs(cl, 0, fts_s_float, ftom_number);
  fts_method_define_varargs(cl, 0, fts_new_symbol("tune"), mtof_tune);

  return fts_ok;
}

void
mtof_config(void)
{
  fts_class_install(fts_new_symbol("mtof"), mtof_instantiate);
  fts_class_install(fts_new_symbol("ftom"), ftom_instantiate);
}


