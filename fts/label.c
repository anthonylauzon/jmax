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
#include <ftsprivate/variable.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/label.h>

/***********************************************************************
 *
 * Channel
 *
 */

void fts_channel_init(fts_channel_t *channel)
{
  fts_objectlist_init( &channel->origins);
  fts_objectlist_init( &channel->targets);
}

void fts_channel_add_origin(fts_channel_t *channel, fts_object_t *origin)
{
  fts_objectlist_insert( &channel->origins, origin);
}

void fts_channel_add_target(fts_channel_t *channel, fts_object_t *target)
{
  fts_objectlist_insert( &channel->targets, target);
}

void fts_channel_remove_origin(fts_channel_t *channel, fts_object_t *origin)
{
  fts_objectlist_remove( &channel->origins, origin);
}

void fts_channel_remove_target(fts_channel_t *channel, fts_object_t *target)
{
  fts_objectlist_remove( &channel->targets, target);
}

void fts_channel_send(fts_channel_t *channel, int outlet, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  fts_objectlist_cell_t *p;

  for ( p = fts_objectlist_get_head( &channel->targets); p; p = fts_objectlist_get_next(p) )
    {
      fts_outlet_send( fts_objectlist_get_object(p), outlet, selector, ac, at);
    }
}

void fts_channel_find_friends(fts_channel_t *channel, int ac, const fts_atom_t *at)
{
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_objectlist_cell_t *p;

  for ( p = fts_objectlist_get_head( &channel->targets); p; p = fts_objectlist_get_next(p) )
    {
      if ( fts_object_get_patcher( fts_objectlist_get_object(p)) != 0)
	{
	  fts_objectset_add( set, fts_objectlist_get_object(p));
	}
    }

  for ( p = fts_objectlist_get_head( &channel->origins); p; p = p->next)
    {
      if ( fts_object_get_patcher( fts_objectlist_get_object(p)) != 0)
	{
	  fts_objectset_add( set, fts_objectlist_get_object(p));
	}
    }
}

void fts_channel_propagate_input( fts_channel_t *channel, fts_propagate_fun_t propagate_fun, void *propagate_context, int outlet)
{
  fts_objectlist_cell_t *p;

  for ( p = channel->targets.head; p; p = p->next)
    {
      propagate_fun(propagate_context, p->object, outlet);
    }
}


/***********************************************************************
 *
 *  label
 *
 */

static fts_hashtable_t *default_labels = 0;
fts_class_t *fts_label_class = 0;
fts_metaclass_t *fts_label_metaclass = 0;

static fts_label_t *
label_get_or_create(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_atom_t *value = fts_variable_get_value_or_void(scope, name);
  fts_label_t *label = 0;
  fts_atom_t key, a;
  
  fts_set_symbol(&key, name);

  if(value && !fts_is_void(value))
    {
      if(fts_is_object(value))
	{
	  fts_object_t *obj = fts_get_object(value);
	  
	  if(fts_object_get_class(obj) == fts_label_class)
	    label = (fts_label_t *)obj;
	  else
	    return 0; /* variable is not a label */
	}
      else
	return 0; /* variable is even not an object */
    }
  else if(default_labels == 0)
    {
      /* create hashtable if not existing yet */
      default_labels = (fts_hashtable_t *) fts_malloc(sizeof(fts_hashtable_t));
      fts_hashtable_init( default_labels, 0, FTS_HASHTABLE_MEDIUM);
    }
  else if(fts_hashtable_get(default_labels, &key, &a))
    label = (fts_label_t *)fts_get_object(&a);

  if(!label)
    {
      /* if there wasn't a variable nor a default, make a default */
      fts_set_void(&a);
      label = (fts_label_t *)fts_object_create(fts_label_class, 1, &a);
      
      fts_set_object(&a, (fts_object_t *)label);
      fts_hashtable_put(default_labels, &key, &a);
      
      fts_object_refer((fts_object_t *)label);
    }

  return label;
}

fts_label_t *
fts_label_get(fts_patcher_t *scope, fts_symbol_t name)
{
  fts_atom_t *value = fts_variable_get_value(scope, name);
  fts_label_t *label = 0;
  
  if(value)
    {
      if(fts_is_object(value))
	{
	  fts_object_t *obj = fts_get_object(value);
	  
	  if(fts_object_get_class(obj) == fts_label_class)
	    label = (fts_label_t *)obj;
	}
    }
  else if(default_labels)
    {
      fts_atom_t a, key;

      fts_set_symbol(&key, name);

      if(fts_hashtable_get(default_labels, &key, &a))
	label = (fts_label_t *)fts_get_object(&a);
    }
  
  /* don't return unconnected default labels */
  if(label && fts_channel_has_target(fts_label_get_channel(label)))
    return label;
  else
    return 0;
}

static void
label_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_channel_find_friends(&this->channel, ac, at);
}      

static void
label_send(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_label_send(this, s, ac, at);
}

static void
label_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object( value, obj);
}

static void
label_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  ac--;
  at++;

  fts_channel_init(&this->channel);

  if(ac == 0 || !fts_is_void(at))
    {
      /* add label as its own receive */
      fts_channel_add_target(&this->channel, o);
    }
}

static void
label_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);

  fts_channel_propagate_input( &this->channel, propagate_fun, propagate_context, 0);
}

static fts_status_t
label_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(fts_label_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, label_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, label_find_friends);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, label_get_state);

  fts_class_define_thru(cl, label_propagate_input);

  /* sending anything else to lable is like sending to all channel targets */
  fts_method_define_varargs(cl, 0, fts_s_anything, label_send);

  return fts_Success;
}

/***************************************************************************
 *
 *  send
 *
 */

typedef struct _send_
{
  fts_object_t head;
  fts_channel_t *channel;
} send_t;

typedef struct _receive_
{
  fts_object_t head;
  fts_channel_t *channel;
} receive_t;

static void
send_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  fts_channel_send( this->channel, 0, s, ac, at);
}

static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;
  fts_label_t *label = 0;

  if(fts_is_symbol(at + 1))
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      
      label = label_get_or_create(fts_object_get_patcher(o), name);
      fts_variable_add_user(fts_object_get_patcher(o), name, o);

      if(!label)
	{
	  fts_object_set_error(o, "Variable %s is not a label", name);
	  return;
	}
    }
  else if(fts_is_label(at + 1))
    label = (fts_label_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Wrong argument");
      return;
    }

  fts_channel_add_origin(fts_label_get_channel(label), (fts_object_t *)this);  
  this->channel = fts_label_get_channel(label);
}

static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  fts_channel_remove_origin(this->channel, (fts_object_t *)this);
}

static void
send_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *)o;

  fts_channel_find_friends(this->channel, ac, at);
}

static void
send_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);

  fts_channel_propagate_input( this->channel, propagate_fun, propagate_context, 0);
}

fts_status_t
send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(send_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, send_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, send_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  fts_method_define_varargs(cl, 0, fts_s_anything, send_anything);
  fts_class_define_thru(cl, send_propagate_input);

  return fts_Success;
}

/***************************************************************************
 *
 *  receive
 *
 */

static void
receive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *)o;
  fts_label_t *label = 0;

  if(fts_is_symbol(at + 1))
    {
      fts_symbol_t name = fts_get_symbol(at + 1);
      
      label = label_get_or_create(fts_object_get_patcher(o), name);
      fts_variable_add_user(fts_object_get_patcher(o), name, o);

      if(!label)
	{
	  fts_object_set_error(o, "Variable %s is not a label", name);
	  return;
	}
    }
  else if(fts_is_label(at + 1))
    label = (fts_label_t *)fts_get_object(at + 1);
  else
    {
      fts_object_set_error(o, "Wrong argument");
      return;
    }

  fts_channel_add_target(fts_label_get_channel(label), o);
  this->channel = fts_label_get_channel(label);
}
  
static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *)o;

  fts_channel_remove_target(this->channel, (fts_object_t *)this);
}

fts_status_t
receive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(receive_t), 0, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, receive_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, receive_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  return fts_Success;
}


/*************************************************************
 *
 *  inlet/outlet utilities
 *
 */

static int
label_connection_check(int ac, const fts_atom_t *at)
{
  return (ac == 0 || (ac == 1 && (fts_is_int(at) || fts_is_symbol(at) || fts_is_label(at))));
}

/***********************************************************************
 *
 * Initialisation
 *
 */

void 
fts_label_config(void)
{
  fts_label_metaclass = fts_class_install( fts_s_label, label_instantiate);
  fts_label_class = fts_class_get_by_name(fts_s_label);

  fts_alias_install(fts_s_receive, fts_s_inlet);
  fts_alias_install(fts_s_send, fts_s_outlet);
}
