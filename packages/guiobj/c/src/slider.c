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


/*
 *  Note that this class is almost the same as gint, but it have a different
 *  set of handler properties, so it is a different class.
 */

#include <fts/fts.h>

fts_symbol_t sym_setOrientation = 0;
fts_symbol_t sym_setMaxValue = 0;
fts_symbol_t sym_setMinValue = 0;

/*------------------------- slider class -------------------------------------*/

typedef struct {
  fts_object_t o;
  int n;
} slider_t;


static void
slider_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t a[1];

  fts_object_get_prop(o, fts_s_orientation, a);
  
  if( fts_get_int(a))
    fts_client_send_message(o, sym_setOrientation, 1, a);

  fts_object_get_prop(o, fts_s_min_value, a);
  
  if( fts_get_int(a))
    fts_client_send_message(o, sym_setMinValue, 1, a);

  fts_object_get_prop(o, fts_s_max_value, a);

  if( fts_get_int(a))
    fts_client_send_message(o, sym_setMaxValue, 1, a);
}

static void
slider_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}

static void slider_set_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n;

  n = fts_get_int_arg( ac, at, 0, 0);

  if ( this->n != n)
    {
      this->n = n;
      fts_object_ui_property_changed( (fts_object_t *)this, fts_s_value);
    }

  fts_outlet_int( o, 0, n);
}

static void
slider_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;

  fts_outlet_int(o, 0, this->n);
}


static void
slider_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n = fts_get_int(at);

  if (this->n != n)
    {
      fts_object_ui_property_changed(o, fts_s_value);
      this->n = n;
    }

  fts_outlet_int(o, 0, n);
}


static void
slider_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n = (int) fts_get_float(at);

  if (this->n != n)
    {
      fts_object_ui_property_changed(o, fts_s_value);
      this->n = n;
    }

  fts_outlet_int(o, 0, n);
}

/* in case of list, only the first value is significative */

static void
slider_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_int(at))
    slider_int(o, winlet, fts_s_int, 1, at);
  else if (fts_is_float(at))
    slider_float(o, winlet, fts_s_float, 1, at);
}


static void
slider_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
  int n = fts_get_int_arg(ac, at, 0, 0);

  if (this->n != n)
    {
      this->n = n;
      fts_object_ui_property_changed(o, fts_s_value);
    }
}


static void
slider_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  slider_t *this = (slider_t *)obj;

  fts_set_int(value, this->n);
}

static void
slider_put_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  slider_t *this = (slider_t *)obj;
  int n = fts_get_int(value);

  this->n = n;

  fts_outlet_int(obj, 0, n);
  fts_object_ui_property_changed(obj, fts_s_value);
}


static void
slider_assist(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_symbol_t cmd = fts_get_symbol_arg(ac, at, 0, 0);

  if (cmd == fts_s_object)
    {
      fts_object_blip(o, "slider");
    }
  else if (cmd == fts_s_inlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      fts_object_blip(o, "<number>: set and output the value, set <number>: set only");
    }
  else if (cmd == fts_s_outlet)
    {
      int n = fts_get_int_arg(ac, at, 1, 0);

      fts_object_blip(o, "output value", n);
    }
}

static void slider_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  int x, y, w, min_value, max_value;
  fts_atom_t a;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_width, &a);
  w = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_min_value, &a);
  min_value = fts_get_int( &a);
  fts_object_get_prop( o, fts_s_max_value, &a);
  max_value = fts_get_int( &a);

  fprintf( file, "#P slider %d %d %d %d;\n", x, y, w, max_value - min_value + 1);
}


static fts_status_t
slider_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(slider_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, slider_send_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_ui_properties, slider_send_ui_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol( "setValue"), slider_set_value); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), slider_assist); 

  fts_method_define_varargs(cl, 0, fts_s_set, slider_set);
  fts_method_define_varargs(cl, 0, fts_s_bang, slider_bang);

  fts_method_define_varargs(cl, 0, fts_s_int, slider_int);
  fts_method_define_varargs(cl, 0, fts_s_float, slider_float);
  fts_method_define_varargs(cl, 0, fts_s_list, slider_list);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_dotpat, slider_save_dotpat); 

   /* Add  the value daemon */
  fts_class_add_daemon(cl, obj_property_get, fts_s_value, slider_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, slider_put_value);

  fts_outlet_type_define_varargs(cl, 0, fts_s_int);

  return fts_Success;
}

void
slider_config(void)
{
  fts_class_install(fts_new_symbol("slider"),slider_instantiate);
  sym_setOrientation = fts_new_symbol("setOrientation");
  sym_setMaxValue = fts_new_symbol("setMaxValue");
  sym_setMinValue = fts_new_symbol("setMinValue");
}

