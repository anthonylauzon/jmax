/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/*
 *  Note that this class is almost the same as gint, but it have a different
 *  set of handler properties, so it is a different class.
 */

#include "fts.h"

/*------------------------- slider class -------------------------------------*/

typedef struct {
  fts_object_t o;
  int n;
} slider_t;


static void
slider_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_value);
  fts_object_property_changed(o, fts_s_min_value);
  fts_object_property_changed(o, fts_s_max_value);
}

static void
slider_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
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
  int nn = fts_get_int(at);

  if (this->n != nn)
    {
      fts_object_ui_property_changed(o, fts_s_value);
      this->n = nn;
    }

  fts_outlet_send(o, 0, fts_s_int, ac, at);
}


static void
slider_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  slider_t *this = (slider_t *)o;
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
  int nn = fts_get_int_arg(ac, at, 0, 0);

  if (this->n != nn)
    {
      this->n = nn;
      fts_object_ui_property_changed(o, fts_s_value);
    }
}


static void
slider_get_value(fts_daemon_action_t action, fts_object_t *obj,
	       fts_symbol_t property, fts_atom_t *value)
{
  slider_t *this = (slider_t *)obj;

  fts_set_int(value, this->n);
}

static void
slider_put_value(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  slider_t *this = (slider_t *)obj;

  this->n = fts_get_int(value);

  fts_outlet_send(obj, 0, fts_s_int, 1, value);
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


static fts_status_t
slider_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(slider_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_s_send_properties, slider_send_properties, 0, 0); 
  fts_method_define(cl, fts_SystemInlet, fts_s_send_ui_properties, slider_send_ui_properties, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("assist"), slider_assist); 

  fts_method_define(cl, 0, fts_s_bang, slider_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, slider_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, slider_float, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, slider_list);

  a[0] = fts_s_anything; /* number */
  fts_method_define(cl, 0, fts_s_set, slider_set, 1, a);

   /* Add  the value daemon */
  fts_class_add_daemon(cl, obj_property_get, fts_s_value, slider_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, slider_put_value);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
slider_config(void)
{
  fts_class_install(fts_new_symbol("slider"),slider_instantiate);
}

