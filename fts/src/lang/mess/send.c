/*
 * New implementation of send/receive: the connections correspond strictly
 * to what the user does (and the connection method has been scrapped).
 * the receive_list is just a place holder that point to a list of receives with the same name,
 * not a fts object anymore.
 * The receive_list is never removed, once installed, after all is not worth to do housekeeping.
 * Must be in the kernel to have the message box working, and export a function to
 * send to every receive a message.
 */


#include "sys.h"
#include "lang/mess.h"
#include "lang/utils.h"
#include "lang/mess/messP.h"

/* "receive_list".  
   The Receive_List is created on the CP when the first "send" or "receive" 
   for a given symbol is created.  It do the whole intracp and intercp
   dispatching of messages.
   */

typedef struct receive_list
{
  fts_symbol_t name;

  struct receive *first_receive;
} receive_list_t;

/* The send object */

typedef struct send
{
  fts_object_t  o;

  receive_list_t *receive_list;
} send_t;


/* The receive object */

typedef struct receive
{
  fts_object_t  o;

  fts_symbol_t name;

  struct receive *next_receive;
} receive_t;


/******************************************************************************/
/*                                                                            */
/*        SEND/RECEIVE IPC support                                            */
/*                                                                            */
/******************************************************************************/

/* The receive_list class is put in a file global
   variable to easy the new operation on the receive_list */

static fts_hash_table_t receive_list_table; /* the name binding table */


/* Convenience function to make a new or get an existing receive_list of a given symbol */

static receive_list_t *
get_or_create_receive_list(fts_symbol_t name)
{
  fts_atom_t a[2];
  receive_list_t *t;

  void *d;

  if (fts_hash_table_lookup(&receive_list_table, name, &d))
    {
      t = (receive_list_t *) d;
    }
  else
    {
      t = (receive_list_t *) fts_malloc(sizeof(receive_list_t));

      t->name = name;
      t->first_receive = 0;


      fts_hash_table_insert(&receive_list_table, name, (void *)t);
    }

  return t;
}



/* Global function, used to send a message to receives, like
   in message box; return non zero if at least one receive exists.
 */


int fts_send_message_to_receives(fts_symbol_t name, fts_symbol_t selector, int ac, const fts_atom_t *at)
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


int fts_receive_exists(fts_symbol_t name)
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
    {
      fts_outlet_send((fts_object_t *) r, 0, selector,  ac, at);
    }
}



static void
receive_list_add_receive(receive_t *r)
{
  /** Get a receive_list, Add the receive to its receive list,  return it*/

  receive_list_t *t;

  t = get_or_create_receive_list(r->name);

  r->next_receive = t->first_receive;

  t->first_receive = r;
}

static void
receive_list_remove_receive(receive_t *r)
{
  /* Indirect precursor iteration */
  receive_list_t *t;
  receive_t **pr;

  t = get_or_create_receive_list(r->name);

  for (pr = &(t->first_receive); *pr ; pr = &((*pr)->next_receive))
    {
      if (*(pr) == r)
	{
	  *(pr) = r->next_receive;

	  return;
	}
    }
}

/******************************************************************************/
/*                                                                            */
/*        The SEND object                                                     */
/*                                                                            */
/******************************************************************************/


/* "send". Send messages commonly. */


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


  fts_method_define_varargs(cl, 0, fts_s_anything, send_anything);

  return fts_Success;
}

/* this variable is accessed by macros, 
   and implicitly used by the dsp compiler */

fts_metaclass_t *fts_send_metaclass;

static void
internal_send_config(void)
{
  fts_metaclass_create(fts_new_symbol("send"),send_instantiate, fts_always_equiv);
  fts_metaclass_alias(fts_new_symbol("s"),     fts_new_symbol("send"));

  fts_send_metaclass = fts_metaclass_get_by_name(fts_new_symbol("send"));
}


/******************************************************************************/
/*                                                                            */
/*        The RECEIVE object                                                  */
/*                                                                            */
/******************************************************************************/

/* "receive".  Receive messages commonly. */


static void
receive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *this = (receive_t *) o;

  this->name = fts_get_symbol_arg(ac, at, 1, 0);

  receive_list_add_receive(this);
}


static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  receive_t *receive = (receive_t *) o;

  receive_list_remove_receive(receive);
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

  return fts_Success;
}

/* this variable is accessed by macros, 
   and implicitly used by the dsp compiler */

fts_metaclass_t *fts_receive_metaclass;

static void
internal_receive_config(void)
{
  fts_metaclass_create(fts_new_symbol("receive"),receive_instantiate, fts_always_equiv);
  fts_metaclass_alias(fts_new_symbol("r"), fts_new_symbol("receive"));

  fts_receive_metaclass = fts_metaclass_get_by_name(fts_new_symbol("receive"));
}


/* file config */


void
send_config(void)
{
  fts_hash_table_init(&receive_list_table);

  /* configuring  the classes */

  internal_send_config();
  internal_receive_config();
}

/* function to support the send/receive navigation */


fts_object_t *fts_send_get_first_receive(fts_object_t *send)
{
  send_t *this = (send_t *) send;


  return (fts_object_t *) this->receive_list->first_receive;
}

fts_object_t *fts_receive_get_next_receive(fts_object_t *receive)
{
  return (fts_object_t *) ((receive_t *)receive)->next_receive;
}

