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

   We send the values as we get them; the device implement buffering
   if it need it.

   1024 char max per atom.
*/

static char outbuf[1024];

static void fts_client_send_string(char *msg)
{
  int i;

  for (i = 0; msg[i] != '\0' ; i++)
    {
      if (i >= 1024)
	{
	  fprintf(stderr, "String too long >%s<\n", msg); 
	}

      fts_char_dev_put(client_dev, msg[i]);      
    }
}

void fts_client_mess_start_msg(int type)
{
  fts_char_dev_put(client_dev, (char) type);
}

void fts_client_mess_add_int(int value)
{
  sprintf(outbuf, "%c%ld", LONG_POS_CODE, value);

  fts_client_send_string(outbuf);
}


void fts_client_mess_add_data( fts_data_t *data)
{
  sprintf(outbuf, "%c%ld", DATA_CODE, (data ? fts_data_get_id(data) : 0));

  fts_client_send_string(outbuf);
}


void fts_client_mess_add_object(fts_object_t *obj)
{
  sprintf(outbuf, "%c%ld", OBJECT_CODE, (obj ? fts_object_get_id(obj) : 0));

  fts_client_send_string(outbuf);
}


void fts_client_mess_add_connection(fts_connection_t *c)
{
  sprintf(outbuf, "%c%ld", CONNECTION_CODE, (c ? fts_connection_get_id(c) : 0));

  fts_client_send_string(outbuf);
}


void fts_client_mess_add_float(float value)
{
  sprintf(outbuf, "%c%f", FLOAT_CODE, value);

  fts_client_send_string(outbuf);
}

void fts_client_mess_add_sym(fts_symbol_t s)
{
  if (s)
    fts_client_mess_add_string(fts_symbol_name(s));
  else
    fts_client_mess_add_string("(null)");
}

void fts_client_mess_add_void()
{
  fts_client_send_string("v");
}

void fts_client_mess_add_string(const char *sp)
{
  sprintf(outbuf, "%c%s%c", STRING_START_CODE, sp, STRING_END_CODE);

  fts_client_send_string(outbuf);
}


void fts_client_mess_add_atoms(int ac, const fts_atom_t *args)
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
      else  if (fts_is_data(&args[i]))
	fts_client_mess_add_data( fts_get_data( &args[i]) );
      else  if (fts_is_void(&args[i]))
	fts_client_mess_add_void();
      else
	fprintf(stderr, "Wrong atom type in fts_client_mess_add_atoms: %lx\n",
		(unsigned long) fts_get_type(&args[i]));
    }
}


void fts_client_mess_send_msg(void)
{
  /*  Add the eom code  */

  fts_char_dev_put(client_dev, (char) EOM_CODE);
}

/* 
   Utility to send a message to an client object;
   put here for the convenience of object programmers

   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]*

   */

void fts_object_send_mess(fts_object_t *obj, fts_symbol_t selector, int argc, const fts_atom_t *args)
{
  fts_client_mess_start_msg(CLIENTMESS_CODE);
  fts_client_mess_add_object(obj);
  fts_client_mess_add_sym(selector);
  fts_client_mess_add_atoms(argc, args);
  fts_client_mess_send_msg();
}


void fts_client_upload_object(fts_object_t *obj)
{
  if (obj->id == FTS_NO_ID)
    fts_object_table_register(obj);

  /* First, check if the parent has been uploaded; if it is not,
     upload it; recursively, this will upload all the chain up
     to the root */

  if (obj->patcher && ((fts_object_t *)obj->patcher)->id == FTS_NO_ID)
    fts_client_upload_object((fts_object_t *) obj->patcher);

  if (fts_object_is_abstraction(obj) || fts_object_is_template(obj))
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

  fts_object_send_properties(obj);

  /* Also, send to the object the message upload, in the case
     the object have more data/initialization to do */

  fts_message_send(obj, fts_SystemInlet, fts_s_upload, 0, 0);
}


void fts_client_upload_connection(fts_connection_t *c)
{
  /* CONNECT (obj)from (int)outlet (obj)to (int)inlet */

  if (c->id == FTS_NO_ID)
    fts_connection_table_register(c);

  fts_client_mess_start_msg(NEW_CONNECTION_CODE);
  fts_client_mess_add_long(c->id);
  fts_client_mess_add_object(c->src);
  fts_client_mess_add_long(c->woutlet);
  fts_client_mess_add_object(c->dst);
  fts_client_mess_add_long(c->winlet);
  fts_client_mess_send_msg();
}


void fts_client_upload_patcher_content(fts_patcher_t *patcher)
{
  fts_object_t *p;

  /* When uploading a patcher content, we upload only the objects
     and connections that don't have yet an ID */

  /* upload all the objects */

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    if (p->id == FTS_NO_ID)
      fts_client_upload_object(p);

  /* For each object, for each outlet, upload all the connections */

  for (p = patcher->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    if (c->id == FTS_NO_ID)
	      fts_client_upload_connection(c);
	}
    }
}

/* Handling of connections and object release and refine */

void fts_client_release_connection(fts_connection_t *c)
{
  fts_client_mess_start_msg(CONNECTION_RELEASE_CODE);
  fts_client_mess_add_connection(c);
  fts_client_mess_send_msg();
}


void fts_client_redefine_connection(fts_connection_t *c)
{
  fts_client_mess_start_msg(REDEFINE_CONNECTION_CODE);
  fts_client_mess_add_connection(c);
  fts_client_mess_add_object(c->src);
  fts_client_mess_add_int(c->woutlet);
  fts_client_mess_add_object(c->dst);
  fts_client_mess_add_int(c->winlet);
  fts_client_mess_send_msg();
}

void fts_client_release_object(fts_object_t *obj)
{
  fts_client_mess_start_msg(OBJECT_RELEASE_CODE);
  fts_client_mess_add_object(obj);;
  fts_client_mess_send_msg();
}

void fts_client_release_object_data(fts_object_t *obj)
{
  fts_client_mess_start_msg(OBJECT_RELEASE_DATA_CODE);
  fts_client_mess_add_object(obj);;
  fts_client_mess_send_msg();
}










