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
  double value;
} gfloat_t;


static void
gfloat_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;
  fts_atom_t a;

  fts_set_float( &a, this->value);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void
gfloat_update(gfloat_t *this, double f)
{
  if (this->value != f)
    {
      this->value = f;
      fts_update_request((fts_object_t *)this);
    }
}

static void
gfloat_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  fts_outlet_float(o, 0, this->value);
}

static void
gfloat_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;
  double f = fts_get_number_float(at);

  gfloat_update(this, f);
  fts_outlet_float(o, 0, this->value);
}

static void
gfloat_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  if (ac && fts_is_number(at))
    gfloat_number(o, 0, 0, 1, at);
}

static void
gfloat_incr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;
  double f;

  if(ac && fts_is_number(at))
    f = this->value + fts_get_number_float(at);
  else
    f = this->value + 1.0;

  gfloat_update(this, f);
  fts_outlet_float(o, 0, f);
}

static void
gfloat_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;
  double f = fts_get_number_float(at);

  gfloat_update(this, f);  
}

static void 
gfloat_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P flonum %d %d %d %d;\n", x, y, w, font_index);
}

static void
gfloat_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(gfloat_t), 0, 0);

  fts_class_method_varargs(cl, fts_s_update_real_time, gfloat_update_real_time); 
  fts_class_method_varargs(cl, fts_s_value, gfloat_number);
  fts_class_method_varargs(cl, fts_s_save_dotpat, gfloat_save_dotpat); 

  fts_class_method_varargs(cl, fts_s_bang, gfloat_bang);
  fts_class_method_varargs(cl, fts_s_set, gfloat_set);
  fts_class_method_varargs(cl, fts_new_symbol("incr"), gfloat_incr);

  fts_class_inlet_number(cl, 0, gfloat_number);
  fts_class_inlet_varargs(cl, 0, gfloat_varargs);

  fts_class_outlet_float(cl, 0);
}

void
gfloat_config(void)
{
  fts_class_install(fts_new_symbol("floatbox"), gfloat_instantiate);
}

