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
 * Authors: Francois Dechelled, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "message.h"

#define MESSCONST_FLASH_TIME 125.0f

/************************************************
 *
 *  messconst
 *
 */
 
typedef struct 
{
  fts_object_t o;

  message_t *mess;
  
  /* blink when click */
  int value; 
  fts_alarm_t alarm;
} messconst_t;

/************************************************
 *
 *  user methods
 *
 */
 
static void
messconst_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  this->value = 1;
  fts_object_ui_property_changed(o, fts_s_value);

  fts_alarm_set_delay(&this->alarm, MESSCONST_FLASH_TIME);

  fts_outlet_send(o, 0, message_get_selector(this->mess), message_get_ac(this->mess), message_get_at(this->mess));
}

static void
messconst_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_outlet_send(o, 0, message_get_selector(this->mess), message_get_ac(this->mess), message_get_at(this->mess));
}

/************************************************
 *
 *  timer
 *
 */
 
static void 
messconst_tick(fts_alarm_t *alarm, void *calldata)
{
  messconst_t *this = (messconst_t *)calldata;

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
messconst_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messconst_t *this = (messconst_t *)obj;

  fts_set_int(value, this->value);
}


static void
messconst_put_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  messconst_t *this = (messconst_t *)obj;

  fts_outlet_bang(obj, 0);

  fts_object_ui_property_changed(obj, fts_s_value);
}

static void
messconst_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_value);
}

static void
messconst_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}
 
/************************************************
 *
 *    class
 *
 */

static void
messconst_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;
  
  ac--;
  at++;

  if(ac > 0)
    {
      message_t *mess = (message_t *)fts_object_create(message_class, ac, at);
      fts_symbol_t error = fts_object_get_error((fts_object_t *)mess);
      
      if(!error)
	{
	  fts_object_refer((fts_object_t *)mess);

	  this->mess = mess;
	  
	  this->value = 0;
	  fts_alarm_init(&(this->alarm), 0, messconst_tick, this);
	}
      else
	{
	  fts_object_destroy((fts_object_t *)mess);
	  fts_object_set_error(o, "%s", fts_symbol_name(error));
	}
    }
  else
    fts_object_set_error(o, "Empty message or constant");
}

static void
messconst_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_object_release((fts_object_t *)this->mess);

  fts_alarm_reset(&(this->alarm));
}

static fts_status_t
messconst_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(messconst_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messconst_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messconst_delete);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, messconst_send_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_ui_properties, messconst_send_ui_properties); 
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_bang, messconst_send);
  
  fts_method_define_varargs(cl, 0, fts_s_bang, messconst_bang);
  
  /* value daemons */
  fts_class_add_daemon(cl, obj_property_get, fts_s_value, messconst_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, messconst_put_value);
  
  return fts_Success;
}

void
messconst_config(void)
{
  fts_class_install(fts_new_symbol("messconst"), messconst_instantiate);
}
