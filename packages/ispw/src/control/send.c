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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

/* New implementation of send/receive: the connections correspond strictly to what the user 
 * does (and the connection method has been scrapped).
 * The receive_list is just a place holder that point to a list of receives with the same name,
 * not a fts object anymore.
 * The receive_list is never removed, once installed, after all is not worth to do housekeeping.
 * Must be in the kernel to have the message box working, and export a function to send to every 
 * receive a message.
 */

/* "receive_list".  
 * The Receive_List is created on the CP when the first "send" or "receive" for a given symbol is created. 
 * It do the whole intracp and intercp dispatching of messages.
 */

typedef struct receive_list
{
  fts_symbol_t name;
  struct receive *first_receive;
  struct send *first_send;
} receive_list_t;

/* The send object */
typedef struct send
{
  fts_object_t  o;
  receive_list_t *receive_list;
  struct send *next_send;
} send_t;


/* The receive object */
typedef struct receive
{
  fts_object_t  o;
  receive_list_t *receive_list;
  struct receive *next_receive;
} receive_t;


/***************************************************************************
 *
 *  send/receive support
 *
 */

static fts_hash_table_t receive_list_table; /* the name binding table */

/* make a new or get an existing receive_list of a given symbol */
static receive_list_t *
get_or_create_receive_list(fts_symbol_t name)
{
  fts_atom_t a[2];
  fts_atom_t d;
  receive_list_t *t;

  if (name == 0)
    return 0;


  if (fts_hash_table_lookup(&receive_list_table, name, &d))
    {
      t = (receive_list_t *) fts_get_ptr(&d);
    }
  else
    {
      t = (receive_list_t *) fts_malloc(sizeof(receive_list_t));

      t->name = name;
      t->first_receive = 0;
      t->first_send = 0;

      fts_set_ptr(&d, t);
      fts_hash_table_insert(&receive_list_table, name, &d);
    }

  return t;
}


/* send a message to receives (return non zero if at least one receive exists) */
int 
ispw_send_message_to_receives(fts_symbol_t name, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  int ret = 0;
  receive_list_t *t;
  receive_t *r;

  t = get_or_create_receive_list(name);

  if (t)
    {
      for (r = t->first_receive; r ; r = r->next_receive)
	{
	  fts_outlet_send((fts_object_t *) r, 0, selector,  ac, at);

	  ret = 1;
	}
    }


  return ret;
}


int 
ispw_receive_exists(fts_symbol_t name)
{
  int ret = 0;
  receive_list_t *t;
  receive_t *r;

  t = get_or_create_receive_list(name);

  return t && t->first_receive;
}

/* Function used locally by the send message */
static void fts_send_message_to_receive_list(receive_list_t *t, fts_symbol_t selector, int ac, const fts_atom_t *at)
{
  receive_t  *r;

  for (r = t->first_receive; r ; r = r->next_receive)
    fts_outlet_send((fts_object_t *) r, 0, selector,  ac, at);
}


static void
receive_list_add_receive(receive_list_t *t, receive_t *r)
{
  r->next_receive = t->first_receive;
  t->first_receive = r;
}


static void
receive_list_remove_receive(receive_list_t *t, receive_t *r)
{
  receive_t **pr;  /* Indirect precursor iteration */

  for (pr = &(t->first_receive); *pr ; pr = &((*pr)->next_receive))
    {
      if (*(pr) == r)
	{
	  *(pr) = r->next_receive;

	  return;
	}
    }
}

static void
receive_list_add_send(receive_list_t *t, send_t *r)
{
  r->next_send = t->first_send;
  t->first_send = r;
}

static void
receive_list_remove_send(receive_list_t *t, send_t *r)
{
  send_t **pr; /* Indirect precursor iteration */

  for (pr = &(t->first_send); *pr ; pr = &((*pr)->next_send))
    {
      if (*(pr) == r)
	{
	  *(pr) = r->next_send;

	  return;
	}
    }
}

static void
receive_list_add_to_set(receive_list_t *t, fts_object_set_t *set)
{
  send_t     *s;
  receive_t  *r;

  for (r = t->first_receive; r ; r = r->next_receive)
    fts_object_set_add(set, (fts_object_t *) r);

  for (s = t->first_send; s ; s = s->next_send)
    fts_object_set_add(set, (fts_object_t *) s);
}

/***************************************************************************
 *
 *  send
 *
 */

/* send anything resend everything receive_list the receive_list to all the receives */
static void
send_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  fts_send_message_to_receive_list(this->receive_list, s, ac, at);
}


static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  this->receive_list = get_or_create_receive_list(fts_get_symbol_arg(ac, at, 1, 0));
  receive_list_add_send(this->receive_list, this);
}


static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;

  receive_list_remove_send(this->receive_list, this);
}


static void
send_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this = (send_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  receive_list_add_to_set(this->receive_list, set);
}

static void
send_dspgraph_replace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  send_t *this  = (send_t *)o;
  fts_dspgraph_t *graph = (fts_dspgraph_t *)fts_get_ptr(at + 0);
  int in = fts_get_int(at + 1);
  receive_t *rcv;

  rcv = this->receive_list->first_receive;
  while(rcv)
    {
      fts_dspgraph_insert(graph, (fts_object_t *)rcv, 0);
      rcv = rcv->next_receive;
    }
}

static fts_status_t
send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */
  fts_class_init(cl, sizeof(send_t), 1, 0, 0); 

  /* define the system methods */
  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, send_init, 2, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, send_delete, 0, 0);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_dspgraph_replace, send_dspgraph_replace);

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

  this->receive_list = get_or_create_receive_list(fts_get_symbol_arg(ac, at, 1, 0));
  receive_list_add_receive(this->receive_list, this);
}


static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *) o;

  receive_list_remove_receive(this->receive_list, this);
}


static void
receive_find_friends(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);

  receive_list_add_to_set(this->receive_list, set);
}


static fts_status_t
receive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */
  fts_class_init(cl, sizeof(receive_t), 0, 1, 0); 

  /* define the system methods */
  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, receive_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, receive_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_friends, send_find_friends);

  return fts_Success;
}

void
send_config(void)
{
  fts_hash_table_init(&receive_list_table);

  /* configuring the classes */
  fts_class_install(fts_new_symbol("send"), send_instantiate);
  fts_class_alias(fts_new_symbol("s"), fts_new_symbol("send"));

  fts_class_install(fts_new_symbol("receive"), receive_instantiate);
  fts_class_alias(fts_new_symbol("r"), fts_new_symbol("receive"));
}
