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
 * Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
 *
 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/datalib.h"

static fts_class_t *channel_class = 0;

typedef struct channel
{
  fts_object_t o;
  struct _receive_ *receives; /* list of receives */
  struct _send_ *sends; /* list of sends */
} channel_t;

typedef struct _send_
{
  fts_object_t  o;
  channel_t *channel; /* send channel */
  struct _send_ *next; /* next in list of channel */
} send_t;

typedef struct _receive_
{
  fts_object_t  o;
  channel_t *channel; /* receive channel */
  struct _receive_ *next; /* next in list of channel */
} receive_t;

/***************************************************************************
 *
 *  channel
 *
 */
static void
channel_add_receive(channel_t *channel, receive_t *receive)
{
  receive->next = channel->receives;
  channel->receives = receive;
}

static void
channel_remove_receive(channel_t *channel, receive_t *receive)
{
  receive_t **p; /* indirect precursor iteration */

  for (p = &(channel->receives); *p ; p = &((*p)->next))
    {
      if (*p == receive)
	{
	  *p = receive->next;

	  return;
	}
    }
}

static void
channel_add_send(channel_t *channel, send_t *send)
{
  send->next = channel->sends;
  channel->sends = send;
}

static void
channel_remove_send(channel_t *channel, send_t *send)
{
  send_t **p; /* indirect precursor iteration */

  for(p = &(channel->sends); *p ; p = &((*p)->next))
    {
      if(*p == send)
	{
	  *p = send->next;

	  return;
	}
    }
}

static void
channel_add_to_set(channel_t *channel, fts_object_set_t *set)
{
  send_t *s;
  receive_t  *r;

  for (r = channel->receives; r ; r = r->next)
    fts_object_set_add(set, (fts_object_t *) r);

  for (s = channel->sends; s ; s = s->next)
    fts_object_set_add(set, (fts_object_t *) s);
}

void
fts_channel_send(fts_object_t *o, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  channel_t *this = (channel_t *) o;
  receive_t *rcv = this->receives;

  while(rcv)
    {
      fts_outlet_send((fts_object_t *)rcv, 0, selector, ac, at);
      rcv = rcv->next;
    }
}

static void
channel_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  channel_t *this = (channel_t *) o;

  this->sends = 0;
  this->receives = 0;
}

static void
channel_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  channel_t *this = (channel_t *) o;
}

static void
channel_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  channel_t *this = (channel_t *)obj;

  fts_set_object_with_type(value, this, fts_s_channel);
}

static fts_status_t
channel_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(channel_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, channel_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, channel_delete);

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, channel_get_state);

  return fts_Success;
}

/***************************************************************************
 *
 *  send
 *
 */

/* send anything resend everything channel the channel to all the receives */
static void
send_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  fts_channel_send((fts_object_t *)this->channel, s, ac, at);
}

static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;
  fts_symbol_t name = fts_get_symbol(at + 1);
  channel_t *channel = (channel_t *)fts_variable_get_object_always(fts_object_get_patcher(o), name, channel_class);

  fts_object_refer((fts_object_t *)channel);
  channel_add_send(channel, this);
  fts_variable_add_user(fts_object_get_patcher(o), name, o);
  this->channel = channel;
}

static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  channel_remove_send(this->channel, this);

  fts_object_release((fts_object_t *)this->channel);
}

static void
send_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  channel_add_to_set(this->channel, set);
}

fts_status_t
fts_send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(send_t), 1, 0, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, send_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, send_delete);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  fts_method_define_varargs(cl, 0, fts_s_anything, send_anything);

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
  receive_t *this = (receive_t *) o;
  fts_symbol_t name = fts_get_symbol(at + 1);
  channel_t *channel = (channel_t *)fts_variable_get_object_always(fts_object_get_patcher(o), name, channel_class);

  fts_object_refer((fts_object_t *)channel);
  channel_add_receive(channel, this);
  fts_variable_add_user(fts_object_get_patcher(o), name, o);
  this->channel = channel;
}

static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *) o;

  channel_remove_receive(this->channel, this);

  fts_object_release((fts_object_t *)this->channel);
}

static void
receive_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  channel_add_to_set(this->channel, set);
}

fts_status_t
fts_receive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(receive_t), 0, 1, 0); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, receive_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, receive_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  return fts_Success;
}

void
fts_channel_config(void)
{
  fts_class_install(fts_s_channel, channel_instantiate);
  channel_class = fts_class_get_by_name(fts_s_channel);
}
