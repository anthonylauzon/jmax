/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *  Communication with the client.
 * 
 *
 */

#include <string.h>
#include <stdio.h> 

#include "protocol.h"
#include "sys.h"
#include "lang.h"
#include "runtime/devices.h"
#include "runtime/client/client.h"
#include "runtime/client/outgoing.h"



/******************************************************************************/
/*                                                                            */
/*             FTS ==> CLIENT Communication and unparsing functions           */
/*                                                                            */
/******************************************************************************/

/* Functions to open a message, close a message, send a valur, send a list of atoms.
   Messages to the client; there is only one client at this level, so we don't have
   any client argument (a part for the coding of the client id in the stream).

   Values are buffered in a local static buffer; a message can be aborted
   and restarted just by calling the fts_client_mess_start.

   We don't check the message length here. (we should.).
*/

static char outbuf[MAX_MESSAGE_LENGTH];
static char *outbuf_fill;

void
fts_client_mess_start_msg(int type)
{
  outbuf_fill = outbuf;

  /* Coding the type */

  *outbuf_fill = (char) type;
  outbuf_fill++;
}


void
fts_client_mess_add_long(long value)
{
  sprintf(outbuf_fill, "%c%ld", LONG_POS_CODE, value);

  outbuf_fill = outbuf_fill + strlen(outbuf_fill);
}

void
fts_client_mess_add_object(fts_object_t *obj)
{
  sprintf(outbuf_fill, "%c%ld", OBJECT_CODE, (obj ? fts_object_get_id(obj) : 0));

  outbuf_fill = outbuf_fill + strlen(outbuf_fill);
}


void
fts_client_mess_add_float(float value)
{
  sprintf(outbuf_fill, "%c%f", FLOAT_CODE, value);

  outbuf_fill = outbuf_fill + strlen(outbuf_fill);
}

void
fts_client_mess_add_sym(fts_symbol_t s)
{
  if (s)
    fts_client_mess_add_string(fts_symbol_name(s));
  else
    fts_client_mess_add_string("(null)");
}

void
fts_client_mess_add_string(const char *sp)
{
  *outbuf_fill = STRING_START_CODE;
  outbuf_fill++;

  strcpy(outbuf_fill, sp);
  outbuf_fill = outbuf_fill + strlen(outbuf_fill);

  *outbuf_fill = STRING_END_CODE;
  outbuf_fill++;
}


void
fts_client_mess_add_atoms(int ac, const fts_atom_t *args)
{
  int i;

  /* pointers and void are ignored */

  for (i = 0; i < ac; i++)
    {
      if (fts_is_long(&args[i]))
	fts_client_mess_add_long(fts_get_long(&args[i]));
      else  if (fts_is_float(&args[i]))
	fts_client_mess_add_float(fts_get_float(&args[i]));
      else  if (fts_is_symbol(&args[i]))
	fts_client_mess_add_sym(fts_get_symbol(&args[i]));
      else  if (fts_is_string(&args[i]))
	fts_client_mess_add_string(fts_get_string(&args[i]));
      else  if (fts_is_object(&args[i]))
	fts_client_mess_add_object(fts_get_object(&args[i]));
    }
}


void
fts_client_mess_send_msg(void)
{
  /*  Add the eom code  */

  *outbuf_fill = EOM_CODE;
  outbuf_fill++;

  /* write the message to the client/daemon */

  if (! client_dev)
    {
      fprintf(stderr,"!!!! SENDING A MESSAGE, BUT NO CLIENTS CONNECTED\n"); 
      fprintf(stderr,"%s\n",outbuf);
    }
  else
    {
      char *p;

      for (p = outbuf; p < outbuf_fill; p++)
	fts_char_dev_put(client_dev, *p);
    }
}

/* 
   Utility to send a message to an client object;
   put here for the convenience of object programmers

   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]*

   */

void
fts_object_send_mess(fts_object_t *obj, fts_symbol_t selector, int argc, const fts_atom_t *args)
{
  fts_client_mess_start_msg(CLIENTMESS_CODE);
  fts_client_mess_add_object(obj);
  fts_client_mess_add_sym(selector);
  fts_client_mess_add_atoms(argc, args);
  fts_client_mess_send_msg();
}


void
fts_client_upload_object(fts_object_t *obj)
{
  /* Assign an ID if needed */

  if (obj->id == FTS_NO_ID)
    fts_object_table_register(obj);

  /*
    Some object require special handling, because their description is
    not necessarly consistent.
    */

  if (fts_object_is_inlet(obj))
    {
      fts_inlet_t *inlet = (fts_inlet_t *) obj;

      fts_client_mess_start_msg(NEW_OBJECT_CODE);
      fts_client_mess_add_object((fts_object_t *) obj->patcher);
      fts_client_mess_add_long(obj->id);
      fts_client_mess_add_sym(fts_s_inlet);
      fts_client_mess_add_long(inlet->position);
      fts_client_mess_send_msg();
    }
  else if (fts_object_is_outlet(obj))
    {
      fts_outlet_t *outlet = (fts_outlet_t *) obj;

      fts_client_mess_start_msg(NEW_OBJECT_CODE);
      fts_client_mess_add_object((fts_object_t *) obj->patcher);
      fts_client_mess_add_long(obj->id);
      fts_client_mess_add_sym(fts_s_outlet);
      fts_client_mess_add_long(outlet->position);
      fts_client_mess_send_msg();
    }
  else if (fts_object_is_abstraction(obj))
    {
      /* Send the abstraction object */

      /* NEW_ABSTRACTION  (obj)pid (int)new-id [<args>]+ */

      fts_client_mess_start_msg(NEW_ABSTRACTION_CODE);
      fts_client_mess_add_object((fts_object_t *) obj->patcher);
      fts_client_mess_add_long(obj->id);
      fts_client_mess_add_atoms(obj->argc, obj->argv);
      fts_client_mess_send_msg();
    }
  else
    {
      /* Send the object */

      /* NEW  (obj)pid (int)new-id [<args>]+ */

      fts_client_mess_start_msg(NEW_OBJECT_CODE);
      fts_client_mess_add_object((fts_object_t *) obj->patcher);
      fts_client_mess_add_long(obj->id);
      fts_client_mess_add_atoms(obj->argc, obj->argv);
      fts_client_mess_send_msg();
    }

  /* if (fts_object_get_patcher(obj) && fts_patcher_is_open(fts_object_get_patcher(obj)))
     fts_object_send_properties(obj);
     */

  if (fts_object_get_patcher(obj))
    fts_object_send_properties(obj);
}

void
fts_client_upload_connection(fts_connection_t *c)
{
  /* CONNECT (obj)from (int)outlet (obj)to (int)inlet */

  fts_client_mess_start_msg(CONNECT_OBJECTS_CODE);
  fts_client_mess_add_object(c->src);
  fts_client_mess_add_long(c->woutlet);
  fts_client_mess_add_object(c->dst);
  fts_client_mess_add_long(c->winlet);
  fts_client_mess_send_msg();
}

void
fts_client_upload_patcher_content(fts_patcher_t *patcher)
{
  fts_object_t *p;

  /* upload all the objects */

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    fts_client_upload_object(p);

  /* For each object, for each outlet, upload all the connections */

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    fts_client_upload_connection(c);
	}
    }
}
