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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

#include <fts/fts.h>

typedef struct 
{
  fts_object_t head;
  fts_audioport_t *port;
} monitor_t;

static void 
monitor_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_activate();
}

static void 
monitor_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_dsp_desactivate();
}

static void 
monitor_update_real_time(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *this = (monitor_t *) o;
  int active = fts_dsp_get_active();
  fts_atom_t a;

  fts_set_int( &a, active);
  fts_client_send_message_real_time(o, fts_s_value, 1, &a);
}

static void 
monitor_dsp_active(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_update_request(o);
}

static void 
monitor_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *this = (monitor_t *) o;
  int active = fts_dsp_get_active();

  if(active)
    fts_dsp_desactivate();
  else
    fts_dsp_activate();

  fts_update_request(o);
}

static void 
monitor_get_value(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  monitor_t *this = (monitor_t *)obj;
  int active = fts_dsp_get_active();

  fts_set_int(value, active);
}

static void 
monitor_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *this  = (monitor_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  int inlet = fts_get_int(at + 2);
  
  if(this->port)
    {
      fts_object_t *outdispatcher = fts_audioport_get_output_dispatcher(this->port);
      
      if(outdispatcher && fts_audioport_get_output_channels(this->port) >= 2)
	(*propagate_fun)(propagate_context, outdispatcher, inlet);
    }
}

static void 
monitor_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *this = (monitor_t *)o;

  this->port = 0;

  this->port = fts_audioport_get_default(o);
  if (!this->port)
    {
      fts_object_set_error( o, "Default audio port is not defined");
      return;    
    }

  fts_dsp_active_add_listener(o, monitor_dsp_active);
}

static void 
monitor_delete( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  monitor_t *this = (monitor_t *)o;

  fts_dsp_active_remove_listener(o);
}

static void 
monitor_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(monitor_t), monitor_init, monitor_delete);

  fts_class_message_varargs(cl, fts_s_propagate_input, monitor_propagate_input);
  fts_class_message_varargs(cl, fts_s_update_real_time, monitor_update_real_time); 

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, monitor_get_value);

  fts_class_message_varargs(cl, fts_s_bang, monitor_bang);
  fts_class_message_varargs(cl, fts_s_start, monitor_start);
  fts_class_message_varargs(cl, fts_s_stop, monitor_stop);

  fts_dsp_declare_inlet(cl, 0);
  fts_dsp_declare_inlet(cl, 1);
}

void 
monitor_config( void)
{
  fts_class_install(fts_new_symbol( "monitor~"), monitor_instantiate);
}
