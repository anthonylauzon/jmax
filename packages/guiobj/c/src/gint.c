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
gint_update(gint_t *this, int n)
{
  if (this->n != n)
    {
      this->n = n;
      fts_object_ui_property_changed((fts_object_t *)this, fts_s_value);
    }
}

static void
gint_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;

  fts_outlet_int(o, 0, this->n);
}

static void
gint_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int n = fts_get_number_int(at);

  gint_update(this, n);
  fts_outlet_int(o, 0, n);
}

static void
gint_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;

  if (ac && fts_is_number(at))
    {
      int n = fts_get_number_int(at);

      gint_update(this, n);
      fts_outlet_int(o, 0, n);
    }
}

static void
gint_incr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int n;
  
  if(ac && fts_is_number(at))
    n = this->n + fts_get_number_int(at);
  else
    n = this->n + 1;

  gint_update(this, n);
  fts_outlet_int(o, 0, n);
}

static void
gint_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gint_t *this = (gint_t *)o;
  int n = fts_get_number_int(at);

  gint_update(this, n);
}

static void 
gint_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
  fts_class_init(cl, sizeof(gint_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_dotpat, gint_save_dotpat);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, gint_send_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_ui_properties, gint_send_ui_properties); 

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, gint_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, gint_put_value);

  fts_method_define_varargs(cl, 0, fts_s_bang, gint_bang);

  fts_method_define_varargs(cl, 0, fts_s_int, gint_number);
  fts_method_define_varargs(cl, 0, fts_s_float, gint_number);

  fts_method_define_varargs(cl, 0, fts_s_list, gint_list);

  fts_method_define_varargs(cl, 0, fts_new_symbol("incr"), gint_incr);

  fts_method_define_varargs(cl, 0, fts_s_set, gint_set);

  fts_outlet_type_define_varargs(cl, 0, fts_s_int);

  return fts_Success;
}

void
gint_config(void)
{
  fts_class_install(fts_new_symbol("intbox"),gint_instantiate);
}

