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
 * Authors: Francois Dechelle, Norbert Schnell.
 *
 */

#include <fts/fts.h>
#include "param.h"

fts_symbol_t param_symbol = 0;
fts_type_t param_type = 0;
fts_class_t *param_class = 0;

static fts_symbol_t sym__remote_value = 0;

/********************************************************************
 *
 *   object
 *
 */

void 
param_add_listener(param_t *param, fts_object_t *listener)
{
  fts_objectlist_insert(&param->listeners, listener);
}

void 
param_remove_listener(param_t *param, fts_object_t *listener)
{
  fts_objectlist_remove(&param->listeners, listener);
}

void 
param_call_listeners(param_t *param, int ac, const fts_atom_t *at)
{
  fts_objectlist_cell_t *p;

  for (p = fts_objectlist_get_head(&param->listeners); p; p = fts_objectlist_get_next(p))
    fts_send_message(fts_objectlist_get_object(p), fts_SystemInlet, sym__remote_value, ac, at);
}

/********************************************************************
 *
 *  user methods
 *
 */

static void
param_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;
  int size = fts_array_get_size(&this->atoms);
  fts_atom_t *atoms = fts_array_get_atoms(&this->atoms);

  switch(size)
    {
    default:
      {
	fts_array_t out;
	int out_ac;
	fts_atom_t *out_at;

	/* create temporary array */
	fts_array_init(&out, size, atoms);
	fts_array_copy(&this->atoms, &out);

	out_ac = fts_array_get_size(&out);
	out_at = fts_array_get_atoms(&out);

	/* call listeners */
	param_call_listeners(this, out_ac, out_at);

	/* send from outlet */
	fts_outlet_send(o, 0, fts_s_list, out_ac, out_at);

	/* destroy temporary array */
	fts_array_destroy(&out);
      }
    case 1:
      {
	fts_atom_t *a = fts_array_get_atoms(&this->atoms);
	fts_atom_t out;

	/* copy to temporary atom */
	fts_atom_assign(&out, a);

	/* call listeners */
	param_call_listeners(this, 1, a);

	/* send from outlet */
	fts_outlet_send(o, 0, fts_get_selector(a), 1, a);

	/* clear temporary atom */
	fts_atom_void(&out);
      }
    case 0:
      break;
    }
}

static void
param_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;

  fts_array_set(&this->atoms, ac, at);
}

static void
param_set_and_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac)
    {
      param_set(o, 0, 0, ac, at);
      param_output(o, 0, 0, 0, 0);
    }
}

static void
param_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;

  fts_array_clear(&this->atoms);
}

static void
param_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;

  post("{");
  post_atoms(fts_array_get_size(&this->atoms), fts_array_get_atoms(&this->atoms));
  post("}\n");
}

static void
param_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;
  fts_array_t *array = fts_get_array(at);

  fts_array_set(array, fts_array_get_size(&this->atoms), fts_array_get_atoms(&this->atoms));
}

static void
param_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;
  param_t *in = param_atom_get(at);
  
  fts_array_copy(&in->atoms, &this->atoms);
  param_output(o, 0, 0, 0, 0);  
}

static void
param_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);

  fts_dumper_send(dumper, fts_s_set, fts_array_get_size(&this->atoms), fts_array_get_atoms(&this->atoms));
}

/********************************************************************
 *
 *   class
 *
 */

static void
param_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *)o;

  ac--;
  at++;

  this->keep = fts_s_no;

  fts_array_init(&this->atoms, ac, at);
  fts_objectlist_init(&this->listeners);
}

static void
param_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  param_t *this = (param_t *) o;

  fts_array_destroy(&this->atoms);
}

static void
param_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  param_t *this = (param_t *)obj;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
}

static void
param_get_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  param_t *this = (param_t *)obj;

  fts_set_symbol(value, this->keep);
}

static void
param_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *param)
{
  param_t *this = (param_t *) obj;

  fts_set_object_with_type(param, this, param_symbol);
}

static fts_status_t
param_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(param_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, param_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, param_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_print, param_print);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_instance, param_set_from_instance);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set, param_set);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dump, param_dump);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_get_array, param_get_array);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_set_from_array, param_set);

  fts_method_define_varargs(cl,fts_SystemInlet, fts_new_symbol("load_init"), param_output);

  fts_method_define_varargs(cl, fts_SystemInlet, sym__remote_value, param_set_and_output);

  fts_method_define_varargs(cl, 0, fts_s_bang, param_output);

  fts_method_define_varargs(cl, 0, fts_s_int, param_set_and_output);
  fts_method_define_varargs(cl, 0, fts_s_float, param_set_and_output);
  fts_method_define_varargs(cl, 0, fts_s_symbol, param_set_and_output);
  fts_method_define_varargs(cl, 0, fts_s_list, param_set_and_output);

  fts_method_define_varargs(cl, 0, fts_s_clear, param_clear);
  
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, param_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, param_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, param_get_state);
  
  return fts_Success;
}

/***************************************************************************
 *
 *  psend
 *
 */

typedef struct _psend_
{
  fts_object_t head;
  param_t *param;
} psend_t;

typedef struct _preceive_
{
  fts_object_t head;
  param_t *param;
} preceive_t;

static void
psend_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  psend_t *this = (psend_t *) o;

  param_set_and_output((fts_object_t *)this->param, 0, s, ac, at);
}

static void
psend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  psend_t *this = (psend_t *) o;
  param_t *param = 0;

  if(fts_is_a(at + 1, param_symbol))
    param = (param_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Wrong argument");
      return;
    }

  this->param = param;
  fts_object_refer(param);
}

static void
psend_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  psend_t *this = (psend_t *) o;

  fts_object_release(this->param);
}

fts_status_t
psend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(psend_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, psend_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, psend_delete);

  fts_method_define_varargs(cl, 0, fts_s_int, psend_input);
  fts_method_define_varargs(cl, 0, fts_s_float, psend_input);
  fts_method_define_varargs(cl, 0, fts_s_symbol, psend_input);
  fts_method_define_varargs(cl, 0, fts_s_list, psend_input);

  return fts_Success;
}

/***************************************************************************
 *
 *  preceive
 *
 */

static void
preceive_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preceive_t *this = (preceive_t *)o;

  switch(ac)
    {
    default:
      fts_outlet_send(o, 0, fts_s_list, ac, at);
    case 1:
      fts_outlet_send(o, 0, fts_get_selector(at), 1, at);
    case 0:
      break;
    }
}

static void
preceive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preceive_t *this = (preceive_t *)o;
  param_t *param = 0;

  if(fts_is_a(at + 1, param_symbol))
    param = (param_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Wrong argument");
      return;
    }

  param_add_listener(param, o);
  this->param = param;
}
  
static void
preceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preceive_t *this = (preceive_t *)o;

  param_remove_listener(this->param, (fts_object_t *)this);
}

fts_status_t
preceive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(preceive_t), 0, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, preceive_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, preceive_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, sym__remote_value, preceive_output);

  return fts_Success;
}


/***********************************************************************
 *
 * Initialisation
 *
 */

void
param_config(void)
{
  param_symbol = fts_new_symbol("param");
  param_type = param_symbol;

  sym__remote_value = fts_new_symbol("_remote_value");

  fts_class_install(param_symbol, param_instantiate);
  fts_class_install(fts_new_symbol("ps"), psend_instantiate);
  fts_class_install(fts_new_symbol("pr"), preceive_instantiate);

  param_class = fts_class_get_by_name(param_symbol);
}
