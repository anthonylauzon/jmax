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
#include <math.h>

typedef struct
{
  fts_object_t head;
  int linear;
  double inlow;
  double inhigh;
  double outlow;
  double outhigh;
  double base;
  double in_shift;
  double in_scale;
  double out_shift;
  double out_scale;
} logscale_t;

/********************
 *
 *  computes:
 *    out = out_low + out_range * {log[(in-in_low)/in_range]/log(base)-1} / (base - 1)
 *  as:
 *    out = out_scale * log(in_scale * in + in_shift) + out_shift
 *  with:
 *    in_range = in_high - in_low
 *    in_scale = (base - 1) / in_range
 *    in_shift = -in_low * in_scale - 1.
 *    out_range = out_high - out_low
 *    out_scale = out_range / log(base)
 *    out_shift = out_low - out_scale
 *
 */

static void
logscale_compute_params(logscale_t *this)
{
  double in_range, out_range;

  if(this->base <= 0.)
    {
      post("error: logscale: base must be > 0.\n");
      return;
    }

  in_range = this->inhigh - this->inlow;
  out_range = this->outhigh - this->outlow;

  if(in_range == 0.0)
    in_range = 1.0;
  
  if(out_range == 0.0)
    out_range = 1.0;

  if(this->base == 1.)
    {
      this->in_scale = out_range / in_range;
      this->in_shift = -this->inlow * this->in_scale + this->outlow;
      this->linear = 1;
    }
  else
    {      
      this->in_scale = (this->base - 1.0) / in_range;
      this->in_shift = -this->inlow * this->in_scale + 1;
      this->out_scale = out_range / log(this->base);
      this->out_shift = this->outlow;
      this->linear = 0;
    }
}

static void
logscale_set_inlow(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  if(fts_is_number(at))
    this->inlow = fts_get_number_float(at);

  logscale_compute_params(this);
}

static void
logscale_set_inhigh(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  if(fts_is_number(at))
    this->inhigh = fts_get_number_float(at);

  logscale_compute_params(this);
}

static void
logscale_set_outlow(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  if(fts_is_number(at))
    this->outlow = fts_get_number_float(at);

  logscale_compute_params(this);
}

static void
logscale_set_outhigh(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  if(fts_is_number(at))
    this->outhigh = fts_get_number_float(at);

  logscale_compute_params(this);
}

static void
logscale_set_base(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  if(fts_is_number(at))
    this->base = fts_get_number_float(at);

  logscale_compute_params(this);
}

static void
logscale_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  switch(ac)
    {
    default:
    case 5:
      if(fts_is_number(at + 4))
	this->base = fts_get_number_float(at + 4);
    case 4:
      if(fts_is_number(at + 3))
	this->outhigh = fts_get_number_float(at + 3);
    case 3:
      if(fts_is_number(at + 2))
	this->outlow = fts_get_number_float(at + 2);
    case 2:
      if(fts_is_number(at + 1))
	this->inhigh = fts_get_number_float(at + 1);
    case 1:
      if(fts_is_number(at + 0))
	this->inlow = fts_get_number_float(at + 0);
    case 0:
      break;
    }

  logscale_compute_params(this);
}

static void
logscale_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;
  double f = fts_get_number_float(at);

  if(this->linear)
    f = f * this->in_scale + this->in_shift;
  else
    f = this->out_scale * log(f * this->in_scale + this->in_shift) + this->out_shift;
  
  fts_outlet_float(o, 0, f);
}

static void
logscale_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  logscale_t *this = (logscale_t *)o;

  this->inlow = 0.0;
  this->inhigh = 0.0;
  this->outlow = 0.0;
  this->outhigh = 0.0;
  this->base = 1.0;

  logscale_set(o, 0, 0, ac, at);
}

static fts_status_t
logscale_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(logscale_t), 6, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, logscale_init);

  fts_method_define_varargs(cl, 0, fts_s_set, logscale_set);

  fts_method_define_varargs(cl, 0, fts_s_int, logscale_number);
  fts_method_define_varargs(cl, 0, fts_s_float, logscale_number);

  fts_method_define_varargs(cl, 1, fts_s_int, logscale_set_inlow);
  fts_method_define_varargs(cl, 1, fts_s_float, logscale_set_inlow);

  fts_method_define_varargs(cl, 2, fts_s_int, logscale_set_inhigh);
  fts_method_define_varargs(cl, 2, fts_s_float, logscale_set_inhigh);

  fts_method_define_varargs(cl, 3, fts_s_int, logscale_set_outlow);
  fts_method_define_varargs(cl, 3, fts_s_float, logscale_set_outlow);

  fts_method_define_varargs(cl, 4, fts_s_int, logscale_set_outhigh);
  fts_method_define_varargs(cl, 4, fts_s_float, logscale_set_outhigh);

  fts_method_define_varargs(cl, 5, fts_s_int, logscale_set_base);
  fts_method_define_varargs(cl, 5, fts_s_float, logscale_set_base);

  fts_outlet_type_define_varargs(cl, 0, fts_s_float);
  
  return fts_Success;
}

void
logscale_config(void)
{
  fts_class_install(fts_new_symbol("logscale"), logscale_instantiate);
}
