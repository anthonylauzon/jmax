/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


#include "fts.h"

#define DEFAULT_DURATION 125.0f

static void button_tick(fts_alarm_t *alarm, void *calldata);

typedef struct 
{
  fts_object_t o;
  int value;
  float duration;
  int color;
  fts_alarm_t alarm;
} button_t;

/************************************************
 *
 *    object
 *
 */

static void
button_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_value);
}

static void
button_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}
 
static void
button_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  fts_alarm_init(&(this->alarm), 0, button_tick, this);
  this->value = 0;
  this->duration = DEFAULT_DURATION;
  this->color = 1;
}

static void
button_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  fts_alarm_unarm(&(this->alarm));
}

/************************************************
 *
 *  user methods
 *
 */
 
static void
button_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;

  fts_outlet_bang(o, 0);

  this->value = this->color;

  fts_object_ui_property_changed(o, fts_s_value);

  fts_alarm_set_delay(&this->alarm, this->duration);
  fts_alarm_arm(&this->alarm);
}

static void
button_duration(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;
  float time = fts_get_number_float(at);

  if(time > 0.0f)
    this->duration = time;
}

static void
button_color(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  button_t *this = (button_t *)o;
  int color = fts_get_int(at);

  this->color = color;
}

/************************************************
 *
 *  timer
 *
 */
 
static void 
button_tick(fts_alarm_t *alarm, void *calldata)
{
  button_t *this = (button_t *)calldata;


  this->value = 0;
  fts_object_ui_property_changed((fts_object_t *)this, fts_s_value);
}

/************************************************
 *
 *  deamons 
 *
 */
 
/* daemon to get the "value" property */

static void
button_get_value(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  button_t *this = (button_t *)obj;

  fts_set_int(value, this->value);
}


static void
button_put_value(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  button_t *this = (button_t *)obj;

  fts_outlet_bang(obj, 0);

  this->value = this->color;
  fts_object_ui_property_changed(obj, fts_s_value);

  fts_alarm_set_delay(&(this->alarm), this->duration);
  fts_alarm_arm(&(this->alarm));
}

/* Daemon for the color  propriety */

static void
button_get_color(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  button_t *this = (button_t *)obj;

  fts_set_int(value, this->color);
}


static void
button_put_color(fts_daemon_action_t action, fts_object_t *obj,
		 fts_symbol_t property, fts_atom_t *value)
{
  button_t *this = (button_t *)obj;

  this->color = fts_get_int(value);
}


/************************************************
 *
 *    class
 *
 */
 
static fts_status_t
button_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(button_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, button_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, button_delete, 0, 0);

  fts_method_define(cl, fts_SystemInlet, fts_s_send_properties, button_send_properties, 0, 0); 
  fts_method_define(cl, fts_SystemInlet, fts_s_send_ui_properties, button_send_ui_properties, 0, 0); 

  fts_method_define(cl, fts_SystemInlet, fts_s_bang, button_bang, 0, 0);

  /* user methods */
  fts_method_define_varargs(cl, 0, fts_s_anything, button_bang);

  a[0] = fts_s_number;
  fts_method_define(cl, 0, fts_new_symbol("duration"), button_duration, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("color"), button_color, 1, a);

  /* value daemons */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, button_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, button_put_value);

  /* color daemons  */

  fts_class_add_daemon(cl, obj_property_get, fts_s_color, button_get_color);
  fts_class_add_daemon(cl, obj_property_put, fts_s_color, button_put_color);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_bang, 1, a);

  return fts_Success;
}


void
button_config(void)
{
  fts_class_install(fts_new_symbol("button"),button_instantiate);
}

