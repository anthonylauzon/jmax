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

/*------------------------- gfloat class -------------------------------------*/

typedef struct {
  fts_object_t o;
  float f;
} gfloat_t;


static void
gfloat_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_value);
}


static void
gfloat_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}

static void
gfloat_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  fts_outlet_float(o, 0, this->f);
}

static void
gfloat_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  this->f = (float) fts_get_long(at);

  fts_outlet_float(o, 0, this->f);

  fts_object_ui_property_changed(o, fts_s_value);
}

static void
gfloat_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  this->f = fts_get_float(at);

  fts_outlet_send(o, 0, fts_s_float, ac, at);

  fts_object_ui_property_changed(o, fts_s_value);
}

/* in case of list, only the first value is significative */

static void
gfloat_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_long(at))
    gfloat_int(o, winlet, fts_s_int, 1, at);
  else if (fts_is_float(at))
    gfloat_float(o, winlet, fts_s_float, 1, at);
}


static void
gfloat_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  this->f = fts_get_float_arg(ac, at, 0, 0.0f);

  fts_object_ui_property_changed(o, fts_s_value);
}

static void gfloat_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w, font_index;
  fts_atom_t a;

  file = (FILE *)fts_get_ptr( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P flonum %d %d %d %d;\n", x, y, w, font_index);
}

/* Daemons to put the "value" property*/

static void
gfloat_get_value(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  gfloat_t *this = (gfloat_t *)obj;

  fts_set_float(value, this->f);
}


static void
gfloat_put_value(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  gfloat_t *this = (gfloat_t *)obj;

  this->f = fts_get_float(value);

  fts_object_ui_property_changed(obj, fts_s_value);
  fts_outlet_send(obj, 0, fts_s_float, 1, value);
}


static fts_status_t
gfloat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[1];

  fts_class_init(cl, sizeof(gfloat_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_s_send_properties, gfloat_send_properties, 0, 0); 
  fts_method_define(cl, fts_SystemInlet, fts_s_send_ui_properties, gfloat_send_ui_properties, 0, 0); 

  fts_method_define(cl, 0, fts_s_bang, gfloat_bang, 0, 0);

  t[0] = fts_t_int;
  fts_method_define(cl, 0, fts_s_int, gfloat_int, 1, t);

  t[0] = fts_t_float;
  fts_method_define(cl, 0, fts_s_float, gfloat_float, 1, t);
 
  fts_method_define_varargs(cl, 0, fts_s_list, gfloat_list);

  t[0] = fts_t_anything;	/* number */
  fts_method_define(cl, 0, fts_s_set, gfloat_set, 1, t);

  t[0] = fts_t_ptr;
  fts_method_define( cl, fts_SystemInlet, fts_s_save_dotpat, gfloat_save_dotpat, 1, t); 

  /* Add  the value daemons */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, gfloat_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, gfloat_put_value);

  t[0] = fts_t_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, t);

  return fts_Success;
}

void
gfloat_config(void)
{
  fts_class_install(fts_new_symbol("floatbox"),gfloat_instantiate);
}

