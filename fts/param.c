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

static fts_symbol_t sym__remote_value = 0;

fts_metaclass_t *fts_param_metaclass = 0;

void 
fts_param_add_listener(fts_param_t *param, fts_object_t *object, fts_method_t callback)
{
  fts_param_listener_t *l = (fts_param_listener_t *)fts_malloc(sizeof(fts_param_listener_t));

  l->callback = callback;
  l->object = object;
  l->next = param->listeners;
  
  param->listeners = l;  
}

void 
fts_param_remove_listener(fts_param_t *param, fts_object_t *object)
{
  fts_param_listener_t **p = &param->listeners;
  
  while(*p)
    {
      if ((*p)->object == object)
	{
	  fts_param_listener_t *freeme = *p;
	  
	  /* remove from list */
	  *p = (*p)->next;
	  
	  /* free entry */
	  fts_free(freeme);
	  break;
	}
      else
	p = &((*p)->next);
    }
}

static void 
param_call_listeners(fts_param_t *param)
{
  if(fts_is_tuple(&param->value))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&param->value);
      int n = fts_tuple_get_size(tuple);
      const fts_atom_t *a = fts_tuple_get_atoms(tuple);
      fts_param_listener_t *listener = param->listeners;
      
      /* call listeners */
      while(listener)
	{
	  listener->callback(listener->object, 0, 0, n, a);
	  listener = listener->next;
	}
      
      /* send from outlet */
      fts_outlet_atoms((fts_object_t *)param, 0, n, a);
    }
  else if(!fts_is_void(&param->value))
    {
      fts_param_listener_t *listener = param->listeners;
      
      /* call listeners */
      while(listener)
	{
	  listener->callback(listener->object, 0, 0, 1, &param->value);
	  listener = listener->next;
	}
      
      /* send from outlet */
      fts_outlet_atom((fts_object_t *)param, 0, &param->value);
    }
}

void 
fts_param_set(fts_param_t *param, const fts_atom_t *value)
{
  fts_atom_assign(&param->value, value);  
  param_call_listeners(param);
}

void 
fts_param_set_int(fts_param_t *param, int i)
{
  fts_atom_release(&param->value);
  fts_set_int(&param->value, i);
  param_call_listeners(param);
}

void 
fts_param_set_float(fts_param_t *param, double f)
{
  fts_atom_release(&param->value);
  fts_set_float(&param->value, f);
  param_call_listeners(param);
}

/********************************************************************
 *
 *  user methods
 *
 */

static void
param_update(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  param_call_listeners(this);
}
  
static void
param_set_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  
  if(ac == 1)
    fts_atom_assign(&this->value, at);
  else if(ac > 1)
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
      fts_atom_t a;
      
      fts_set_object(&a, (fts_object_t *)tuple);
      fts_atom_assign(&this->value, &a);
    }
}

static void
param_input_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  param_set_atoms(o, 0, 0, ac, at);
  param_call_listeners(this);
}

static void
param_input_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac == 1 && s == fts_get_selector(at))
    param_input_atoms(o, 0, 0, ac, at);
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);
}

static void
param_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  fts_atom_void(&this->value);
}

static void
param_post(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_bytestream_t *stream = fts_post_get_stream(ac, at);

  fts_spost(stream, "(:param ");
  fts_spost_atoms(stream, 1, &this->value);
  fts_spost(stream, ")");
}

static void
param_get_array(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_array_t *array = (fts_array_t *)fts_get_pointer(at);

  if(fts_is_tuple(&this->value))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&this->value);

      fts_array_copy(array, fts_tuple_get_array(tuple));
    }
  else
    fts_array_append(array, 1, &this->value);
}

static void
param_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_param_t *in = (fts_param_t *)fts_get_object(at);

  fts_atom_assign(&this->value, &in->value);
  param_call_listeners(this);
}

static void
param_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);

  if(fts_is_tuple(&this->value))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&this->value);
      int n = fts_tuple_get_size(tuple);
      const fts_atom_t *a = fts_tuple_get_atoms(tuple);

      fts_dumper_send(dumper, fts_s_set, n, a);
    }
  else
    fts_dumper_send(dumper, fts_s_set, 1, &this->value);    
}

/********************************************************************
 *
 *   class
 *
 */

static void
param_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  fts_set_void(&this->value);
  this->keep = fts_s_no;

  param_set_atoms(o, 0, 0, ac, at);
}

static void
param_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *) o;

  fts_atom_void(&this->value);
}

static void
param_set_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_param_t *this = (fts_param_t *)obj;

  if(this->keep != fts_s_args && fts_is_symbol(value))
    this->keep = fts_get_symbol(value);
}

static void
param_get_keep(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_param_t *this = (fts_param_t *)obj;

  fts_set_symbol(value, this->keep);
}

static void
param_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

static fts_status_t
param_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_param_t), 1, 1, 0);
  
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, param_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, param_delete);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_post, param_post);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_instance, param_set_from_instance);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set, param_set_atoms);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_dump, param_dump);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_get_array, param_get_array);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_set_from_array, param_set_atoms);

  fts_method_define_varargs(cl, fts_system_inlet, sym__remote_value, param_input_atoms);
  fts_method_define_varargs(cl,fts_system_inlet, fts_new_symbol("load_init"), param_update);

  fts_method_define_varargs(cl, 0, fts_s_bang, param_update);

  fts_method_define_varargs(cl, 0, fts_s_int, param_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_float, param_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_symbol, param_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_list, param_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_anything, param_input_anything);

  fts_method_define_varargs(cl, 0, fts_s_clear, param_clear);
  
  fts_class_add_daemon(cl, obj_property_put, fts_s_keep, param_set_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_keep, param_get_keep);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, param_get_state);
  
  return fts_ok;
}

/***************************************************************************
 *
 *  psend
 *
 */

typedef struct _psend_
{
  fts_object_t head;
  fts_param_t *param;
} psend_t;

static void
psend_input_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  psend_t *this = (psend_t *) o;

  param_input_atoms((fts_object_t *)this->param, 0, 0, ac, at);
}

static void
psend_input_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  psend_t *this = (psend_t *) o;

  param_input_anything((fts_object_t *)this->param, 0, s, ac, at);
}

static void 
psend_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), o->argc-1, o->argv+1);
}

static void
psend_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  psend_t *this = (psend_t *) o;
  fts_param_t *param = (fts_param_t *)fts_get_object(at);

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

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, psend_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, psend_delete);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_spost_description, psend_spost_description);

  fts_method_define_varargs(cl, 0, fts_s_int, psend_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_float, psend_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_symbol, psend_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_list, psend_input_atoms);
  fts_method_define_varargs(cl, 0, fts_s_anything, psend_input_anything);

  return fts_ok;
}

/***************************************************************************
 *
 *  preceive
 *
 */

typedef struct _preceive_
{
  fts_object_t head;
  fts_param_t *param;
} preceive_t;

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
  fts_param_t *param = (fts_param_t *)fts_get_object(at);

  fts_param_add_listener(param, o, preceive_output);
  this->param = param;
}
  
static void
preceive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  preceive_t *this = (preceive_t *)o;

  fts_param_remove_listener(this->param, o);
}

fts_status_t
preceive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(preceive_t), 0, 1, 0); 

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_init, preceive_init);
  fts_method_define_varargs(cl, fts_system_inlet, fts_s_delete, preceive_delete);

  fts_method_define_varargs(cl, fts_system_inlet, fts_s_spost_description, psend_spost_description);

  return fts_ok;
}


/***********************************************************************
 *
 * Initialisation
 *
 */

void
fts_kernel_param_init(void)
{
  fts_s_param = fts_new_symbol("param");
  sym__remote_value = fts_new_symbol("_remote_value");

  fts_param_metaclass = fts_class_install(fts_s_param, param_instantiate);
}
