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
   Support for FOS client/server messages
*/


#include <string.h>

#include <fts/sys.h>
#include <fts/lang.h>
#include <fts/runtime.h>

/* 
   Client message functions; use the prefix fts_mess_client.

   The FTS 2.0 protocol support objects in the atoms; so the
   messages include objects, not id, when possible (i.e. a part
   from the new message).

   NEW protocol; the old 1.x protocol is dropped, in favour of the following:
   All the fts_mess_client messages are atom strings; the message
   selector is a command, as before.
   The accepted messages are 

   not yet implemented, but soon !!!

   NEW  (obj) p (int)id [<args>]*
   REDEFINE_PATCHER (obj)patcher <name> <ins> <outs>
   REDEFINE_OBJECT (obj)object [<args>]*
   DELETE_OBJECT (obj)obj
   DELETE_CONNECTION (conn)c
   CONNECT (obj)from (int)outlet (obj)to (int)inlet
   DISCONNECT (obj)cid

   This two are temporary: they will be substituted with special
   protected mechanisms .. or extended with protection mechanism.

   MESS (obj)obj (int)inlet (symbol)selector [<args>]*

   Message handling

   PUTPROP (obj)o (symbol)name (any) value
   GETPROP (obj)o (symbol)name

   Get property ask a message back; the message back have
   exactly the PUTPROP format.

   These messages are s sent *to* the client; here we provide
   convenience functions to do it

   CLIENTPROP (obj)o (symbol)name (atom) value
   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]*

   REMOTE_CALL <key> <args> * 

   FTS_SHUTDOWN_CODE  quit fts.

   Other messages will be added ...
 */


/* Forward declarations */

static void fts_messtile_install_all(void);

static fts_symbol_t fts_s_download;
static fts_symbol_t fts_s_load_init;
static fts_symbol_t fts_s_setDescription;

/******************************************************************************/
/*                                                                            */
/*                INIT and Utilities                                          */
/*                                                                            */
/******************************************************************************/

static void fts_messtile_init(void);

/* Init function */

fts_module_t fts_messtile_module = {"MessTile", "The interpreter of the client commands for the message system", fts_messtile_init, 0, 0};

static void
fts_messtile_init(void)
{
  fts_messtile_install_all();

  fts_s_download = fts_new_symbol("download");
  fts_s_load_init = fts_new_symbol("load_init");
  fts_s_setDescription = fts_new_symbol("setDescription");
}

/* Debug  message log, post and on the standard error */

static void
printf_mess(const char *msg, int ac, const fts_atom_t *av)
{
  fprintf(stderr, "%s (%d args): ", msg, ac);
  fprintf_atoms(stderr, ac, av);
  fprintf(stderr, "\n");
}


#define INIT_TRACE 0

static int do_mess_trace = INIT_TRACE;

void fts_set_mess_trace(int b)
{
  do_mess_trace = b;
}


void
trace_mess(const char *msg, int ac, const fts_atom_t *av)
{
  if (do_mess_trace)
    {
      fprintf(stderr, "%s: ", msg); 
      fprintf_atoms(stderr, ac, av);
      fprintf(stderr, "\n");
    }
}


/******************************************************************************/
/*                                                                            */
/*                  FOS message functions                                     */
/*                                                                            */
/******************************************************************************/

/*    SAVE_PATCHER_BMAX  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_save_patcher_bmax(int ac, const fts_atom_t *av)
{
  trace_mess("Received save patcher bmax ", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_symbol(&av[1]))
    {
      fts_object_t *patcher;
      fts_symbol_t filename;

      patcher = (fts_object_t *) fts_get_object(&av[0]);
      filename = fts_get_symbol(&av[1]);

      if (patcher)
	fts_save_patcher_as_bmax(filename, patcher);
      else
	printf_mess("System Error in FOS message SAVE PATCHER BMAX: null patcher", ac, av);
    }
  else
    printf_mess("System Error in FOS message SAVE PATCHER BMAX: bad args", ac, av);
}


/*    LOAD_PATCHER_BMAX  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_bmax(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher bmax ", ac, av);

  if (ac == 3 && fts_is_object(&av[0]) && fts_is_int(&av[1]) && fts_is_symbol(&av[2]))
    {
      fts_object_t *parent;
      int id;
      fts_symbol_t filename;

      parent = (fts_object_t *) fts_get_object(&av[0]);
      id = fts_get_int(&av[1]);
      filename = fts_get_symbol(&av[2]);

      if (parent)
	{
	  fts_object_t *patcher;

	  patcher = fts_binary_file_load(fts_symbol_name(filename), parent, 0, 0, 0);

	  if (patcher == 0)
	    {
	      post("Cannot read jmax file %s\n", fts_symbol_name(filename));
	      return;
	    }

	  if (id != FTS_NO_ID)
	    fts_object_set_id(patcher, id);

	  /* Save the file name, for future autosaves and other services */

	  {
	    fts_atom_t a;

	    fts_set_symbol(&a, filename);
	    fts_object_put_prop(patcher, fts_s_filename, &a);
	  }

	  /* activate the post-load init, like loadbangs */	  
	  fts_message_send(patcher, fts_SystemInlet, fts_new_symbol("load_init"), 0, 0);
	}
      else
	printf_mess("System Error in FOS message LOAD PATCHER BMAX: null patcher", ac, av);

    }
  else
    printf_mess("System Error in FOS message LOAD PATCHER BMAX: bad args", ac, av);
}


/*    LOAD_PATCHER_DPAT  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_dpat(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher dpat", ac, av);

  if (ac == 3 && fts_is_object(&av[0]) && fts_is_int(&av[1]) && fts_is_symbol(&av[2]))
    {
      fts_object_t *parent;
      int id;
      fts_symbol_t filename;

      parent  = (fts_object_t *) fts_get_object(&av[0]);
      id       = fts_get_int(&av[1]);
      filename = fts_get_symbol(&av[2]);

      if (parent)
	{
	  fts_object_t *patcher;

	  patcher = fts_load_dotpat_patcher(parent, filename);

	  if (patcher == 0)
	    {
	      post("Cannot read .pat file %s\n", fts_symbol_name(filename));
	      return;
	    }

	  if (id != FTS_NO_ID)
	    fts_object_set_id(patcher, id);
	}

      else
	printf_mess("System Error in FOS message LOAD PATCHER DPAT: null patcher", ac, av);

    }
  else
    printf_mess("System Error in FOS message LOAD PATCHER DPAT: bad args", ac, av);
}


/* DECLARE_ABSTRACTION   <name> <file> */

static void
fts_mess_client_declare_abstraction(int ac, const fts_atom_t *av)
{
  trace_mess("Received declare abstraction ", ac, av);

  if (ac == 2 && fts_is_symbol(&av[0]) && fts_is_symbol(&av[1]))
    {
      fts_symbol_t abstraction;
      fts_symbol_t filename;

      abstraction = fts_get_symbol(&av[0]);
      filename = fts_get_symbol(&av[1]);

      fts_abstraction_declare(abstraction, filename);
    }
  else
    printf_mess("System Error in FOS message DEFINE ABSTRACTION: bad args", ac, av);

}

/* DECLARE_ABSTRACTION_PATH   <path> */

static void
fts_mess_client_declare_abstraction_path(int ac, const fts_atom_t *av)
{
  trace_mess("Received declare abstraction path ", ac, av);

  if (ac == 1 && fts_is_symbol(&av[0]))
    {
      fts_symbol_t path;

      path = fts_get_symbol(&av[0]);

      fts_abstraction_declare_path(path);
    }
  else
    printf_mess("System Error in FOS message DEFINE ABSTRACTION PATH: bad args", ac, av);
}

/* DECLARE_TEMPLATE   <name> <file> */

static void
fts_mess_client_declare_template(int ac, const fts_atom_t *av)
{
  trace_mess("Received declare template ", ac, av);

  if (ac == 2 && fts_is_symbol(&av[0]) && fts_is_symbol(&av[1]))
    {
      fts_symbol_t template;
      fts_symbol_t filename;

      template = fts_get_symbol(&av[0]);
      filename = fts_get_symbol(&av[1]);

      fts_template_declare(template, filename);
    }
  else
    printf_mess("System Error in FOS message DEFINE TEMPLATE: bad args", ac, av);

}

/* DECLARE_TEMPLATE_PATH   <path> */

static void
fts_mess_client_declare_template_path(int ac, const fts_atom_t *av)
{
  trace_mess("Received declare template path ", ac, av);

  if (ac == 1 && fts_is_symbol(&av[0]))
    {
      fts_symbol_t path;

      path = fts_get_symbol(&av[0]);

      fts_template_declare_path(path);
    }
  else
    printf_mess("System Error in FOS message DEFINE TEMPLATE PATH: bad args", ac, av);
}



/*    DOWNLOAD_OBJECT   (obj)p

      Send to the object the message "download". (system inlet)
 */

static void
fts_mess_client_download_object(int ac, const fts_atom_t *av)
{
  trace_mess("Received download object ", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *object;
      fts_object_t *p;

      object = (fts_object_t *) fts_get_object(&av[0]);

      if (! object)
	{
	  return;
	}

      fts_client_upload_object(object);
    }
  else
    printf_mess("System Error in FOS message DOWNLOAD OBJECT: bad args", ac, av);
}


/*    DOWNLOAD_CONNECTION   (obj)p

      Send to the connection the message "download". (system inlet)
      Do the actual work here; this is not good; but: it cannot be
      in the mess module because use the runtime !!
 */

static void
fts_mess_client_download_connection(int ac, const fts_atom_t *av)
{
  trace_mess("Received download connection ", ac, av);

  if (ac == 1 && fts_is_connection(&av[0]))
    {
      fts_connection_t *connection;
      fts_object_t *p;

      connection = (fts_connection_t *) fts_get_connection(&av[0]);

      if (! connection)
	{
	  return;
	}

      fts_client_upload_connection(connection);
    }
  else
    printf_mess("System Error in FOS message DOWNLOAD CONNECTION: bad args", ac, av);
}


/*    NEW  (obj)pid (int)new-id [<args>]* or
      NEW  (int)new-id [<args>]*

      Create a new object, in the parent patcher, with 
      local id new-lid, and description args.

      Now new accept an empty argument list; in this case
      fts_eval_object_description produce an error object.
 */

static void
fts_mess_client_new(int ac, const fts_atom_t *av)
{
  trace_mess("Received new", ac, av);

  if (ac >= 2 && fts_is_object(&av[0]) && fts_is_int(&av[1]))
    {
      fts_object_t *parent = fts_get_object(&av[0]);
      
      if(parent && fts_object_get_class_name(parent) == fts_s_patcher)
	{
	  /* new object in patcher */
	  int id = fts_get_int(&av[1]);
	  fts_object_t *obj = fts_eval_object_description((fts_patcher_t *)parent, ac - 2, av + 2);
	  
	  fts_object_set_id(obj, id);
	}
      else
	printf_mess("System Error in FOS message NEW:  parent not found", ac, av);
    }
  else
    printf_mess("System Error in FOS message NEW: bad args", ac, av);
}


/*
   REDEFINE_PATCHER (obj)old [<args>]*
   
   Redefine the patcher in place; for the moment, the
   accepted arguments are what the user type in the box
   (ie.  [<var name> : ] <args>*) without the class name,
   and to the translation to the real format here;
   in the future it will be the client to do the work.
   */

static void
fts_mess_client_redefine_patcher(int ac, const fts_atom_t *av)
{
  trace_mess("Received redefine patcher", ac, av);

  if (ac >= 1)
    {
      fts_atom_t argv[512];
      int argc;
      fts_patcher_t  *patcher;

      patcher = (fts_patcher_t *) fts_get_object(&av[0]);

      if (! patcher)
	{
	  printf_mess("System Error in FOS message REDEFINE PATCHER: redefining a non existing patcher", ac, av);
	  return;
	}

      if (fts_object_description_defines_variable(ac - 1, av + 1))
	{
	  /* Variable syntax */
	  /* argv[0] = av[1]; */
	  /* argv[1] = av[2]; */
	  fts_set_symbol(&argv[0], fts_s_patcher); /* "jpatcher" */

	  /* copy arguments (ignoring variable) */
	  for (argc = 1; (argc < ac - 2) && (argc < 512) ; argc++) 
	    argv[argc] = av[argc + 2];
	}
      else
	{
	  /* Plain syntax */
	  fts_set_symbol(&argv[0], fts_s_patcher);

	  for (argc = 1; (argc < ac) && (argc < 512) ; argc++)
	    argv[argc] = av[argc];
	}

      fts_patcher_redefine(patcher, argc, argv);

      fts_client_send_message((fts_object_t *)patcher, fts_s_setDescription, argc - 1, argv + 1);
    }
  else
    printf_mess("System Error in FOS message REDEFINE PATCHER: bad args", ac, av);
}

/*
   REDEFINE_OBJECT (obj)object (int) new_id [<args>]*
   
   Redefine an object (not a patcher, use in re-editing).
   */

static void
fts_mess_client_redefine_object(int ac, const fts_atom_t *av)
{
  trace_mess("Received redefine object", ac, av);

  if (ac >= 1 && fts_is_object(&av[0]))
    {
      fts_object_t  *object;
      int new_id;

      object = fts_get_object(&av[0]);
      new_id = fts_get_int(&av[1]);
      fts_object_redefine(object, new_id, ac - 2, av + 2);
    }
  else
    printf_mess("System Error in FOS message REDEFINE OBJECT: bad args", ac, av);
}


/* 
   DELETE_OBJECT (obj)obj

   Free (destroy) the object identified by id.
*/


static void
fts_mess_client_delete_object(int ac, const fts_atom_t *av)
{
  trace_mess("delete object", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *obj;

      obj = fts_get_object(&av[0]);

      if (! obj)
	{
	  printf_mess("System Error in FOS message DELETE_OBJECT: deleting a non existing object", ac, av);
	  return;
	}

      fts_object_delete_from_patcher(obj);
    }
  else
    printf_mess("System Error in FOS message DELETE_OBJECT: bad args", ac, av);
}





/* CONNECT (obj)from (int)outlet (obj)to (int)inlet

  Connect the outlet of a from object (identified by the from-id )
  to an inlet of a to object (identified by the to-id) */


static void
fts_mess_client_new_connection(int ac, const fts_atom_t *av)
{
  trace_mess("Received connect", ac, av);

  if ((ac == 5) &&
      fts_is_int(&av[0]) &&
      fts_is_object(&av[1]) &&
      fts_is_int(&av[2]) &&
      fts_is_object(&av[3]) &&
      fts_is_int(&av[4]))
    {
      int inlet, outlet;
      int id;
      fts_object_t *from, *to;
      fts_status_t ret;

      id     = fts_get_int(&av[0]);
      from   = fts_get_object(&av[1]);
      outlet = fts_get_int(&av[2]);
      to     = fts_get_object(&av[3]);
      inlet  = fts_get_int(&av[4]);

      if (to && from)
	{
	  fts_connection_new( id, from, outlet, to, inlet);
	}
      else
	printf_mess("System Error in FOS message CONNECT: Error trying to connect non existing objects", ac, av);
    }
  else
    printf_mess("System Error in FOS message CONNECT: bad args", ac, av);
}


/*
  DELETE_CONNECTION (conn)connection

  Disconnect the outlet of a from object (identified by the from-id )
  to an inlet of a to object  (identified by the to-id)
  */


static void
fts_mess_client_delete_connection(int ac, const fts_atom_t *av)
{
  trace_mess("Received delete connection", ac, av);

  if ((ac == 1) && fts_is_connection(&av[0]))
    {
      fts_connection_t *c;

      c = fts_get_connection(&av[0]);

      if (c)
	fts_connection_delete(c);
      else
	printf_mess("System Error in FOS message DELETE CONNECTION: disconnecting non existing connection", ac, av);
    }
  else
    printf_mess("System Error in FOS message DELETE CONNECTION: bad args", ac, av);
}


/*
   MESS (obj)o (int)inlet (symbol)selector [<args>]*

   Send to an object,
   in the inlet "inlet", the message specified by the selector and its arguments.
   */

static void 
fts_mess_client_mess(int ac, const fts_atom_t *av)
{
  trace_mess("Received mess", ac, av);

  if ((ac >= 3) &&
      fts_is_object(&av[0]) &&
      fts_is_int(&av[1]) &&
      fts_is_symbol(&av[2]))
    {
      int inlet;
      fts_object_t *obj;
      fts_symbol_t selector;

      obj    = fts_get_object(&av[0]);
      inlet  = fts_get_int(&av[1]);
      selector = fts_get_symbol(&av[2]);

      if (! obj)
	{
	  printf_mess("System Error in FOS message MESS: message to a non existing object", ac, av);
	  return;
	}

      /* Ignore failure; it is normal that the UI interface try to send
	 messages that have no methods defined
	 */

      fts_message_send(obj, inlet, selector, ac - 3, av + 3);
    }
  else
    printf_mess("System Error in FOS message MESS: bad args", ac, av);
}

/*
   PUTPROP (obj)o (symbol)name (any) value

   Send to an object  a new value for a specified property.
   */

static void 
fts_mess_client_put_prop(int ac, const fts_atom_t *av)
{
  trace_mess("Received put prop", ac, av);

  if ((ac == 3) &&
      fts_is_object(&av[0]) &&
      fts_is_symbol(&av[1]))
    {
      fts_object_t *obj;
      fts_symbol_t name;

      obj  = fts_get_object(&av[0]);
      name = fts_get_symbol(&av[1]);

      fts_object_put_prop(obj, name, &av[2]);
    }
  else
    printf_mess("System Error in FOS message PUTPROP: bad args", ac, av);
}


/*
   GETPROP (obj)o (symbol)name

   Ask an object to Send the client a property value.
   */

static void 
fts_mess_client_get_prop(int ac, const fts_atom_t *av)
{
  trace_mess("Received get prop", ac, av);

  if ((ac == 2) &&
      fts_is_object(&av[0]) &&
      fts_is_symbol(&av[1]))
    {
      fts_object_t *obj;
      fts_symbol_t name;

      obj  = fts_get_object(&av[0]);
      name = fts_get_symbol(&av[1]);

      fts_object_property_changed(obj, name);
    }
  else
    printf_mess("System Error in FOS message GETPROP: bad args", ac, av);
}


/*
   GETALLPROP (obj)o (symbol)name

   Ask a patcher object to Send the client a property value for
   all the objects inside the patcher.
   */

static void 
fts_mess_client_get_all_prop(int ac, const fts_atom_t *av)
{
  trace_mess("Received get prop", ac, av);

  if ((ac == 2) &&
      fts_is_object(&av[0]) &&
      fts_is_symbol(&av[1]))
    {
      fts_patcher_t *patcher;
      fts_object_t *obj;
      fts_symbol_t name;

      patcher  = (fts_patcher_t *) fts_get_object(&av[0]);
      name = fts_get_symbol(&av[1]);

      for (obj = patcher->objects ; obj; obj = obj->next_in_patcher)
	fts_object_property_changed(obj, name);
    }
  else
    printf_mess("System Error in FOS message GETPROP: bad args", ac, av);
}

/*
  REMOTE_CALL <key> <args> * 

   Get a fts data function call .
   */


static void 
fts_mess_client_remote_call(int ac, const fts_atom_t *av)
{
  trace_mess("Received remote call", ac, av);

  if ((ac >= 2) &&
      fts_is_data(&av[0]) &&
      fts_is_int(&av[1]))
    {
      fts_data_t *data;
      int key;

      data = fts_get_data(&av[0]);
      key  = fts_get_int(&av[1]);

      fts_data_call(data, key, ac - 2, av + 2);
    }
  else
    printf_mess("System Error in FOS message REMOTE_CALL: bad args", ac, av);
}

/*
  RECOMPUTE_ERRORS

  Ask to recompute all the errors.
  */

static void 
fts_mess_client_recompute_errors(int ac, const fts_atom_t *av)
{
  fts_do_recompute_errors();
}

/*
   SHUTDOWN

   Quit FTS
   */


static void 
fts_mess_client_shutdown(int ac, const fts_atom_t *av)
{
  trace_mess("Received shutdown", ac, av);

  fts_sched_halt();
}


static void
fts_messtile_install_all(void)
{
  fts_client_install(SAVE_PATCHER_BMAX_CODE, fts_mess_client_save_patcher_bmax);

  fts_client_install(LOAD_PATCHER_BMAX_CODE, fts_mess_client_load_patcher_bmax);
  fts_client_install(LOAD_PATCHER_DPAT_CODE, fts_mess_client_load_patcher_dpat);

  fts_client_install(DECLARE_ABSTRACTION_CODE, fts_mess_client_declare_abstraction);
  fts_client_install(DECLARE_ABSTRACTION_PATH_CODE, fts_mess_client_declare_abstraction_path);

  fts_client_install(DECLARE_TEMPLATE_CODE, fts_mess_client_declare_template);
  fts_client_install(DECLARE_TEMPLATE_PATH_CODE, fts_mess_client_declare_template_path);

  fts_client_install(DOWNLOAD_OBJECT_CODE, fts_mess_client_download_object);
  fts_client_install(DOWNLOAD_CONNECTION_CODE, fts_mess_client_download_connection);

  fts_client_install(NEW_OBJECT_CODE,  fts_mess_client_new);
  fts_client_install(REDEFINE_PATCHER_CODE,  fts_mess_client_redefine_patcher);
  fts_client_install(REDEFINE_OBJECT_CODE,  fts_mess_client_redefine_object);
  fts_client_install(DELETE_OBJECT_CODE,  fts_mess_client_delete_object);

  fts_client_install(NEW_CONNECTION_CODE,  fts_mess_client_new_connection);
  fts_client_install(DELETE_CONNECTION_CODE, fts_mess_client_delete_connection);

  fts_client_install(MESSAGE_CODE, fts_mess_client_mess);
  fts_client_install(PUTPROP_CODE,  fts_mess_client_put_prop);
  fts_client_install(GETPROP_CODE,  fts_mess_client_get_prop);
  fts_client_install(GETALLPROP_CODE,  fts_mess_client_get_all_prop);
  fts_client_install(REMOTE_CALL_CODE,  fts_mess_client_remote_call);
  fts_client_install(RECOMPUTE_ERRORS_CODE, fts_mess_client_recompute_errors);
  fts_client_install(FTS_SHUTDOWN_CODE,  fts_mess_client_shutdown);
}