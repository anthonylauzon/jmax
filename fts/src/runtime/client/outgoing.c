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


/*
 *  Communication with the client.
 * 
 *
 */

/*
 * Uncommenting OUTGOING_DEBUG_TRACE will produce a trace
 * of the outgoing messages on the standard error
 */

/*  #define OUTGOING_DEBUG_TRACE  */

#include <string.h>

#include <fts/runtime/client/protocol.h>
#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime/devices.h>
#include <fts/runtime/time.h>
#include <fts/runtime/client/client.h>
#include <fts/runtime/client/outgoing.h>

#ifdef INCOMING_DEBUG_TRACE
extern const char *protocol_printable_cmd( int cmd);
#endif

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
*/


static void fts_client_send_string(const char *msg)
{
  int i;

  for (i = 0; msg[i] != '\0' ; i++)
    fts_char_dev_put(client_dev, msg[i]);
}

static void fts_client_send_int(int value)
{
  fts_char_dev_put(client_dev, (unsigned char) (((unsigned int) value >> 24) & 0xff));
  fts_char_dev_put(client_dev, (unsigned char) (((unsigned int) value >> 16) & 0xff));
  fts_char_dev_put(client_dev, (unsigned char) (((unsigned int) value >> 8) & 0xff));
  fts_char_dev_put(client_dev, (unsigned char) (((unsigned int) value) & 0xff));
}

void fts_client_start_msg( int type)
{
  if ( !client_dev )
    return;

  fts_char_dev_put( client_dev, (char)type);

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf(stderr, "Sending '%s' ", protocol_printable_cmd( type));
#endif

}

void fts_client_add_int(int value)
{
  if ( !client_dev )
    return;

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf( stderr, "%d ", value);
#endif

  fts_char_dev_put(client_dev, INT_CODE);
  fts_client_send_int(value);
}


void fts_client_add_data( fts_data_t *data)
{
  if ( !client_dev )
    return;

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf_data( stderr, data);
#endif

  fts_char_dev_put( client_dev, DATA_CODE);
  fts_client_send_int( data ? fts_data_get_id(data) : 0);
}

void fts_client_add_object(fts_object_t *obj)
{  
  if ( !client_dev )
    return;

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf_object( stderr, obj);
#endif

  fts_char_dev_put(client_dev, OBJECT_CODE);
  fts_client_send_int( obj ? fts_object_get_id(obj) : 0);
}


void fts_client_add_connection(fts_connection_t *c)
{
  if ( !client_dev )
    return;

  fts_char_dev_put(client_dev, CONNECTION_CODE);
  fts_client_send_int(c ? fts_connection_get_id(c) : 0);

}


void fts_client_add_float(float value)
{
  if ( !client_dev )
    return;

  fts_char_dev_put(client_dev, FLOAT_CODE);
  fts_client_send_int( *((unsigned int *)&value) );
}

/*
  Symbol cache handling

  The symbol cache transforms a symbol in an integer id,
  that will be send other the client connection.

  The first time a symbol is send to the client, it is
  allocated an id and this id together with its name is sent
  to the client.
  The next time it is sent to the client, the id only is
  sent.

  The cache has a limited size: if there is no more
  cache id available, a symbol is sent as a string.
  (i.e. the symbol cache does not have a LRU-like replacement 
  algorithm).

*/

#define MAX_CACHE_INDEX 512

static int first_unused_symbol_cache_index = 0;

static int cache_symbol( fts_symbol_t s)
{
  if (first_unused_symbol_cache_index < MAX_CACHE_INDEX)
    {
      fts_symbol_set_cache_index( s, first_unused_symbol_cache_index++);
      return 1;
    }

  return 0;
}

void fts_client_add_symbol(fts_symbol_t s)
{
  if (! client_dev)
    return;

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf_symbol( stderr, s);
#endif
  
  if ( fts_symbol_get_cache_index(s) >= 0 )   /* Is symbol cached ? */
    {
      fts_char_dev_put( client_dev, SYMBOL_CACHED_CODE);
      fts_client_send_int( fts_symbol_get_cache_index(s));
    }
  else if (cache_symbol(s))   /* Try to cache it and if succeeded, send a cache definition */
    {
      fts_char_dev_put(client_dev, SYMBOL_AND_DEF_CODE);
      fts_client_send_int( fts_symbol_get_cache_index(s));
      fts_client_send_string( fts_symbol_name(s));
      fts_char_dev_put(client_dev, STRING_END_CODE);
    }
  else   /* Send it as string, but with a SYMBOL_CODE */
    {
      fts_char_dev_put(client_dev, SYMBOL_CODE);
      fts_client_send_string(fts_symbol_name(s));
      fts_char_dev_put(client_dev, STRING_END_CODE);
    }
}

void fts_client_add_string(const char *s)
{
  if (!client_dev)
    return;

  fts_char_dev_put(client_dev, STRING_CODE);
  fts_client_send_string(s);
  fts_char_dev_put(client_dev, STRING_END_CODE);
}


static void 
fts_client_add_atom(const fts_atom_t *atom)
{
  if (fts_is_int( atom))
    fts_client_add_int( fts_get_long(atom));
  else  if (fts_is_float( atom))
    fts_client_add_float( fts_get_float(atom));
  else  if (fts_is_symbol( atom))
    fts_client_add_symbol( fts_get_symbol(atom));
  else  if (fts_is_string( atom))
    fts_client_add_string( fts_get_string(atom));
  else  if (fts_is_object( atom))
    fts_client_add_object( fts_get_object(atom));
  else  if (fts_is_data( atom))
    fts_client_add_data( fts_get_data( atom) );
  else
    fprintf(stderr, "Wrong atom type in fts_client_add_atoms: %lx\n", (unsigned long) fts_get_type(atom));
}

void 
fts_client_add_atoms(int ac, const fts_atom_t *args)
{
  int i;
  
  for (i=0; i<ac; i++)
    fts_client_add_atom(&args[i]);
}


void 
fts_client_done_msg(void)
{
  if ( !client_dev )
    return;

  /*  Add the eom code  */

  fts_char_dev_put(client_dev, (char) EOM_CODE);

#ifdef OUTGOING_DEBUG_TRACE      
  fprintf(stderr, "<EOM>\n");
#endif
}

/* 
   Utility to send a message to an client object;
   put here for the convenience of object programmers
   
   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]* 
*/

void fts_client_send_message(fts_object_t *obj, fts_symbol_t selector, int argc, const fts_atom_t *args)
{
  fts_client_start_msg(CLIENTMESS_CODE);
  fts_client_add_object(obj);
  fts_client_add_symbol(selector);
  fts_client_add_atoms(argc, args);
  fts_client_done_msg();
}

void fts_client_send_message_from_atom_list(fts_object_t *obj, fts_symbol_t selector, fts_atom_list_t *atom_list)
{
  fts_atom_list_iterator_t *iterator = fts_atom_list_iterator_new(atom_list);

  fts_client_start_msg(CLIENTMESS_CODE);
  fts_client_add_object(obj);
  fts_client_add_symbol(fts_s_set);

  while (! fts_atom_list_iterator_end(iterator))
    {
      fts_client_add_atom(fts_atom_list_iterator_current(iterator));
      fts_atom_list_iterator_next(iterator);
    }

  fts_client_done_msg();
  fts_atom_list_iterator_free(iterator);
}

/* (nos:) This is a new upload function, which is part of a generic object creation client/server API.
 * Other than "fts_client_upload_object" it is called by an object in order to upload itself.
 * In the creation of an object from the client this happens inside the "upload" method, which this way
 * has quite a different semantic than the same message send by "fts_client_upload_object".
 */
void fts_client_upload(fts_object_t *obj, fts_symbol_t classname, int ac, const fts_atom_t *at)
{
  if (!fts_object_has_id(obj))
    fts_object_table_register(obj);

  fts_client_start_msg(NEW_OBJECT_CODE);

  /* this is to be compatible with the NEW_OBJECT_CODE */
  fts_client_add_object((fts_object_t *)0);
  fts_client_add_data((fts_data_t *)0);

  fts_client_add_int(fts_object_get_id(obj));
  fts_client_add_symbol(classname);
  fts_client_add_atoms(ac, at);
  fts_client_done_msg();

  /*fts_object_send_properties(obj);*/
}

void fts_client_upload_object(fts_object_t *obj)
{
  int do_var = 0;

  if (!fts_object_has_id(obj))
    fts_object_table_register(obj);

  /* First, check if the parent has been uploaded; if it is not,
     upload it; recursively, this will upload all the chain up
     to the root */

  if (obj->patcher && !fts_object_has_id((fts_object_t *)obj->patcher))
    fts_client_upload_object((fts_object_t *) obj->patcher);

  /* 
     NEW_OBJECT_VAR_CODE (obj)parent (dta) data (int)new-id (symbol) var [<args>]+
     NEW_OBJECT_CODE (obj)parent (dta) data (int)new-id [<args>]+
     */

  if(fts_object_description_defines_variable(obj->argc, obj->argv))
    do_var = 1;
  
  if (do_var)
    fts_client_start_msg(NEW_OBJECT_VAR_CODE);
  else
    fts_client_start_msg(NEW_OBJECT_CODE);

  fts_client_add_object((fts_object_t *) obj->patcher);
  
  if (obj->patcher)
    fts_client_add_data((fts_data_t *) obj->patcher->data);
  else
    fts_client_add_data((fts_data_t *) 0);

  fts_client_add_int(fts_object_get_id(obj));
  
  if (do_var)
    {
      fts_client_add_symbol(fts_get_symbol(&obj->argv[0]));

      fts_client_add_atoms(obj->argc - 2, obj->argv + 2);
    }
  else
    fts_client_add_atoms(obj->argc, obj->argv);

  fts_client_done_msg();

  fts_object_send_properties(obj);

  /* Also, send to the object the message upload, in the case the object have more data/initialization to do */
  fts_message_send(obj, fts_SystemInlet, fts_s_upload, 0, 0);
}


void fts_client_upload_connection(fts_connection_t *c)
{
  /* CONNECT (obj)from (int)outlet (obj)to (int)inlet */

  if (c->id == FTS_NO_ID)
    fts_connection_table_register(c);

  fts_client_start_msg(NEW_CONNECTION_CODE);

  if (c->src->patcher)
    fts_client_add_data((fts_data_t *) c->src->patcher->data);
  else
    fts_client_add_data((fts_data_t *) 0);

  fts_client_add_int(c->id);
  fts_client_add_object(c->src);
  fts_client_add_int(c->woutlet);
  fts_client_add_object(c->dst);
  fts_client_add_int(c->winlet);
  fts_client_add_int(c->type);
  fts_client_done_msg();
}


/* Handling of connections and object release and refine */

void fts_client_release_connection(fts_connection_t *c)
{
  fts_client_start_msg(CONNECTION_RELEASE_CODE);
  fts_client_add_connection(c);
  fts_client_done_msg();
}


void fts_client_redefine_connection(fts_connection_t *c)
{
  fts_client_start_msg(REDEFINE_CONNECTION_CODE);
  fts_client_add_connection(c);
  fts_client_add_object(c->src);
  fts_client_add_int(c->woutlet);
  fts_client_add_object(c->dst);
  fts_client_add_int(c->winlet);
  fts_client_add_int(c->type);
  fts_client_done_msg();
}

void fts_client_release_object(fts_object_t *obj)
{
  fts_client_start_msg(OBJECT_RELEASE_CODE);
  fts_client_add_object(obj);;
  fts_client_done_msg();
}

void fts_client_release_object_data(fts_object_t *obj)
{
  fts_client_start_msg(OBJECT_RELEASE_DATA_CODE);
  fts_client_add_object(obj);;
  fts_client_done_msg();
}
