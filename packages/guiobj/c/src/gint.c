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

/*------------------------- gint class -------------------------------------*/

typedef struct {
  fts_object_t o;
  int n;
} gint_t;


static void
gint_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_value);
}


static void
gint_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}


static void
gint_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;

  fts_outlet_int(o, 0, this->n);
}


static void
gint_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int nn = fts_get_int(at);

  if (this->n != nn)
    {
      fts_object_ui_property_changed(o, fts_s_value);
      this->n = nn;
    }

  fts_outlet_send(o, 0, fts_s_int, ac, at);
}


static void
gint_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int nn = (int) fts_get_float(at);

  if (this->n != nn)
    {
      fts_object_ui_property_changed(o, fts_s_value);
      this->n = nn;
    }

  fts_outlet_int(o, 0, this->n);
}

/* in case of list, only the first value is significative */

static void
gint_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_int(at))
    gint_int(o, winlet, fts_s_int, 1, at);
  else if (fts_is_float(at))
    gint_float(o, winlet, fts_s_float, 1, at);
}


static void
gint_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int nn = fts_get_int_arg(ac, at, 0, 0);

  if (this->n != nn)
    {
      this->n = nn;
      fts_object_ui_property_changed(o, fts_s_value);
    }
}


static void gint_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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

  fprintf( file, "#P number %d %d %d %d;\n", x, y, w, font_index);
}

static void
gint_get_value(fts_daemon_action_t action, fts_object_t *obj,
	       fts_symbol_t property, fts_atom_t *value)
{
  gint_t *this = (gint_t *)obj;

  fts_set_int(value, this->n);
}

static void
gint_put_value(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  gint_t *this = (gint_t *)obj;
  int nn = fts_get_int(value);

  if (this->n != nn)
    {
      this->n = nn;
      fts_object_ui_property_changed(obj, fts_s_value);
    }

  fts_outlet_send(obj, 0, fts_s_int, 1, value);
}

static fts_status_t gint_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(gint_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_s_send_properties, gint_send_properties, 0, 0); 
  fts_method_define(cl, fts_SystemInlet, fts_s_send_ui_properties, gint_send_ui_properties, 0, 0); 

  fts_method_define(cl, 0, fts_s_bang, gint_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, gint_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, gint_float, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, gint_list);

  a[0] = fts_s_anything;	/* number */
  fts_method_define(cl, 0, fts_s_set, gint_set, 1, a);

  a[0] = fts_s_ptr;
  fts_method_define( cl, fts_SystemInlet, fts_s_save_dotpat, gint_save_dotpat, 1, a); 

   /* Add  the value daemon */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, gint_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, gint_put_value);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
gint_config(void)
{
  fts_class_install(fts_new_symbol("intbox"),gint_instantiate);
}

