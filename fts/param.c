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
 */

#include <fts/fts.h>

fts_class_t *fts_param_class = 0;

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
      fts_outlet_varargs((fts_object_t *)param, 0, n, a);
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
param_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  if(ac == 1)
    fts_atom_assign(&this->value, at);
  else if(ac > 1)
  {
    fts_tuple_t *tuple = (fts_tuple_t *)fts_object_create(fts_tuple_class, ac, at);
    fts_atom_t a;

    fts_set_object(&a, (fts_object_t *)tuple);
    fts_atom_assign(&this->value, &a);
  }  
}

static void
param_set_from_instance(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_param_t *param = (fts_param_t *)fts_get_object(at);

  fts_atom_assign(&this->value, &param->value);
}

static void
param_input_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  fts_atom_assign(&this->value, at);
  param_call_listeners(this);
}

static void
param_output_from_recieve(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_varargs(o, 0, ac, at);
}

static void
param_add_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  
  fts_param_add_listener(this, fts_get_object(at), param_output_from_recieve);
}

static void
param_remove_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  
  fts_param_remove_listener(this, fts_get_object(at));
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
param_get_value(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  fts_return(&this->value);
}

static void
param_dump_state(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  
  if(fts_is_tuple(&this->value))
    {
      fts_tuple_t *tuple = (fts_tuple_t *)fts_get_object(&this->value);
      int size = fts_tuple_get_size(tuple);
      const fts_atom_t *atoms = fts_tuple_get_atoms(tuple);
      
      fts_dumper_send(dumper, fts_s_set, size, atoms);
    }
  else if(!fts_is_void(&this->value))
    fts_dumper_send(dumper, fts_s_set, 1, &this->value);    
}

static void
param_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_atom_t a;

  if(this->persistence == 1)
    {
      param_dump_state(o, 0, 0, ac, at);

      /* save persistence flag */
      fts_set_int(&a, 1);
      fts_dumper_send(dumper, fts_s_persistence, 1, &a);
    }

  fts_name_dump_method(o, 0, 0, ac, at);
}

static void
param_update_gui(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;
  fts_atom_t a;    

  if(this->persistence >= 0)
    {
      fts_set_int(&a, this->persistence);
      fts_client_send_message(o, fts_s_persistence, 1, &a);
    }

  fts_name_gui_method(o, 0, 0, 0, 0);
}

static void
param_persistence(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *)o;

  if(ac > 0)
    {
      if(fts_is_number(at) && this->persistence >= 0)
	{
	  this->persistence = fts_get_number_int(at);
	  fts_client_send_message(o, fts_s_persistence, 1, at);
	}
    }
  else
    {
      fts_atom_t a;

      fts_set_int(&a, this->persistence);
      fts_return(&a);
    }
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
  this->persistence = 0;

  if(ac > 0)
  {
    param_set(o, 0, NULL, ac, at);
    this->persistence = -1;
  }
}

static void
param_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_param_t *this = (fts_param_t *) o;

  fts_atom_void(&this->value);
}

static void
param_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_param_t), param_init, param_delete);

  fts_class_message_varargs(cl, fts_s_name, fts_name_set_method);
  fts_class_message_varargs(cl, fts_s_update_gui, param_update_gui);

  fts_class_message_varargs(cl, fts_s_dump_state, param_dump_state);

  fts_class_message_varargs(cl, fts_s_persistence, param_persistence);
  fts_class_message_varargs(cl, fts_s_dump, param_dump);

  fts_class_message_varargs(cl, fts_s_post, param_post);

  fts_class_message_varargs(cl, fts_s_get_tuple, param_get_value);

  fts_class_message_varargs(cl, fts_new_symbol("load_init"), param_update);

  fts_class_message_varargs(cl, fts_s_add_listener, param_add_listener);
  fts_class_message_varargs(cl, fts_s_remove_listener, param_remove_listener);

  fts_class_message_varargs(cl, fts_s_clear, param_clear);
  fts_class_message_varargs(cl, fts_s_send, param_input_atom);

  fts_class_message_atom(cl, fts_s_set, param_set);
  fts_class_message(cl, fts_s_set_from_instance, cl, param_set_from_instance);
  
  fts_class_inlet_bang(cl, 0, param_update);
  fts_class_inlet_atom(cl, 0, param_input_atom);
  fts_class_outlet_atom(cl, 0);
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

  fts_param_class = fts_class_install(fts_s_param, param_instantiate);
}
