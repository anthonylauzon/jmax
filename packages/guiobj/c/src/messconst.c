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

#define MESSCONST_FLASH_TIME 125.0f

/************************************************
 *
 *  messconst
 *
 */
 
typedef struct 
{
  fts_object_t o;
  fts_message_t *mess;
  int value; /* for blicking */
} messconst_t;

/************************************************
 *
 *  user methods
 *
 */
 
static void 
messconst_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  this->value = 0;
  fts_object_ui_property_changed((fts_object_t *)this, fts_s_value);
}

static void
messconst_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  if(this->value == 0)
    {
      /* messbox on */
      this->value = 1;
      fts_object_ui_property_changed(o, fts_s_value);
  
      fts_timebase_add_call(fts_get_timebase(), o, messconst_off, 0, MESSCONST_FLASH_TIME);
    }

  fts_outlet_send(o, 0, fts_message_get_selector(this->mess), fts_message_get_ac(this->mess), fts_message_get_at(this->mess));
}

static void
messconst_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_outlet_send(o, 0, fts_message_get_selector(this->mess), fts_message_get_ac(this->mess), fts_message_get_at(this->mess));
}
 
/************************************************
 *
 *  deamons 
 *
 */
 
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
}

static void
messconst_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), o->argc-1, o->argv+1);
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
      fts_object_t *mess;

      if(ac == fts_is_tuple(at))
	{
	  fts_tuple_t *tup = fts_get_tuple(at);

	  /* create empty message */
	  mess = fts_object_create(fts_message_metaclass, 0, 0);
	  
	  /* set message to tup */
	  fts_message_set( (fts_message_t *)mess, fts_s_list, fts_tuple_get_size(tup), fts_tuple_get_atoms(tup));
	}
      else
	{
	  fts_symbol_t error;

	  /* try to create message */
	  mess = fts_object_create(fts_message_metaclass, ac, at);
	  error = fts_object_get_error( mess);
	  
	  if(error)
	    {
	      fts_object_destroy( mess);
	      fts_object_set_error(o, "%s", error);
	      return;
	    }
	}

      fts_object_refer( mess);
      
      this->mess = (fts_message_t *)mess;
      this->value = 0;
    }
  else
    fts_object_set_error(o, "Empty message or constant");
}

static void
messconst_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  messconst_t *this = (messconst_t *)o;

  fts_object_release((fts_object_t *)this->mess);
}

static fts_status_t
messconst_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(messconst_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, messconst_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, messconst_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, messconst_send_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_ui_properties, messconst_send_ui_properties); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_spost_description, messconst_spost_description); 
  
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
