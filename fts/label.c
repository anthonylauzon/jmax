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
#include <ftsprivate/patcher.h>
#include <ftsprivate/label.h>
#include <ftsprivate/object.h>
#include <ftsprivate/variable.h>

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

fts_class_t *fts_label_class = 0;

fts_label_t *
fts_label_get_or_create(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_atom_t *value = fts_name_get_value(patcher, name);
  fts_label_t *label = NULL;

  if(fts_is_object(value))
    {
      fts_object_t *obj = fts_get_object(value);
      
      if(fts_object_get_class(obj) == fts_label_class)
	return (fts_label_t *)obj;
    }

  /* create new label */
  label = (fts_label_t *)fts_object_create(fts_label_class, patcher, 0, 0);
  
  /* name the label */
  fts_object_set_name((fts_object_t *)label, name);
  
  return label;
}

fts_label_t *
fts_label_get(fts_patcher_t *patcher, fts_symbol_t name)
{
  fts_atom_t *value = fts_name_get_value(patcher, name);
  fts_label_t *label = NULL;
  
  if(value && fts_is_object(value) && fts_object_get_class(fts_get_object(value)))
    label = (fts_label_t *)fts_get_object(value);

  return label;
}

static void
label_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_channel_find_friends(&this->channel, ac, at);
}      

static void
label_varargs(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_label_send(this, NULL, ac, at);
}

static void
label_mess(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  if(ac > 0 && fts_is_symbol(at))
    fts_label_send(this, fts_get_symbol(at), ac - 1, at + 1);
  else
    fts_object_error(o, "invalid message selector");
}

static void
label_input_handler(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;

  fts_label_send(this, s, ac, at);
}

static void
label_add_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;
  fts_channel_t *channel = fts_label_get_channel(this);

  fts_channel_add_target(channel, fts_get_object(at));
}

static void
label_remove_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;
  fts_channel_t *channel = fts_label_get_channel(this);

  fts_channel_remove_target(channel, fts_get_object(at));
}

static void
label_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *) o;
  
  fts_channel_init(&this->channel);
}

static void
label_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
    /*fts_label_t *this = (fts_label_t *) o;*/
}

static void
label_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_label_t *this = (fts_label_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);

  fts_channel_propagate_input( &this->channel, propagate_fun, propagate_context, 0);
}

static void
label_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_label_t), label_init, label_delete);

  fts_class_message_varargs(cl, fts_s_propagate_input, label_propagate_input);
  fts_class_message_varargs(cl, fts_s_find_friends, label_find_friends);

  fts_class_message_varargs(cl, fts_s_add_listener, label_add_listener);
  fts_class_message_varargs(cl, fts_s_remove_listener, label_remove_listener);

  fts_class_message_varargs(cl, fts_s_send, label_varargs);
  fts_class_message_varargs(cl, fts_new_symbol("mess"), label_mess);

  fts_class_input_handler(cl, label_input_handler);
}

void 
fts_label_config(void)
{
  fts_label_class = fts_class_install( NULL, label_instantiate);
}
