/*
   Support for FOS client/server messages
*/


#include <string.h>
#include <stdio.h>

#include "sys.h"
#include "lang.h"
#include "runtime.h"

/* 
   Client message functions; use the prefix fts_mess_client.

   The FTS 2.0 protocol support objects in the atoms; so the
   messages include objects, not id, when possible (i.e. a part
   from the new message).

   NEW protocol; the old 1.x protocol is dropped, in favour of the following:
   All the fts_mess_client messages are atom strings; the message
   selector is a command, as before.
   The accepted messages are 

   OPEN_PATCHER   (obj)p
   CLOSE_PATCHER  (obj)p
   PATCHER_LOADED (obj)p

   not yet implemented, but soon !!!

   NEW  (obj) p (int)id [<args>]*
   REDEFINE (obj)old [<args>]*
   REPLACE (obj)old (obj)new
   FREE (obj)obj
   CONNECT (obj)from (int)outlet (obj)to (int)inlet
   DISCONNECT (obj)from (int)outlet (obj)to (int)inlet

   This two are temporary: they will be substituted with special
   protected mechanisms .. or extended with protection mechanism.

   MESS (obj)obj (int)inlet (symbol)selector [<args>]*
   NMESS (symbol)name (int)inlet (symbol)selector [<args>]*

   Message handling

   PUTPROP (obj)o (symbol)name (any) value
   GETPROP (obj)o (symbol)name

   Get property ask a message back; the message back have
   exactly the PUTPROP format.

   These messages are s sent *to* the client; here we provide
   convenience functions to do it

   CLIENTPROP (obj)o (symbol)name (atom) value
   CLIENTMESS (obj)obj (symbol)selector [(atom)<args>]*

   Other messages will be added ...
 */


/* Forward declarations */

static void fts_messtile_install_all();

static fts_symbol_t fts_s_open;
static fts_symbol_t fts_s_download;
static fts_symbol_t fts_s_close;
static fts_symbol_t fts_s_patcher;
static fts_symbol_t fts_s_inlet;
static fts_symbol_t fts_s_outlet;
static fts_symbol_t fts_s_load_init;

/******************************************************************************/
/*                                                                            */
/*                INIT and Utilities                                          */
/*                                                                            */
/******************************************************************************/

static void fts_messtile_init();

/* Init function */

fts_module_t fts_messtile_module = {"MessTile", "The interpreter of the client commands for the message system",
				    fts_messtile_init, 0};

static void
fts_messtile_init()
{
  fts_messtile_install_all();

  fts_s_inlet = fts_new_symbol("inlet");
  fts_s_outlet = fts_new_symbol("outlet");
  fts_s_patcher = fts_new_symbol("patcher");
  fts_s_open = fts_new_symbol("open");
  fts_s_download = fts_new_symbol("download");
  fts_s_close = fts_new_symbol("close");
  fts_s_load_init = fts_new_symbol("load_init");
}


/* Debug  message log, post and on the standard error */

static void
post_mess(const char *msg, int ac, const fts_atom_t *av)
{
  post("%s (%d args): ", msg, ac);
  postatoms(ac, av);
  post("\n");
}

#ifdef DEBUG
#define INIT_TRACE 1
#else
#define INIT_TRACE 0
#endif

static int do_mess_trace = INIT_TRACE;

void fts_set_mess_trace(int b)
{
  do_mess_trace = b;
}


static void
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
      fts_bmax_file_t *f;

      patcher = (fts_object_t *) fts_get_object(&av[0]);
      filename = fts_get_symbol(&av[1]);

      if (patcher)
	{
	  f = fts_open_bmax_file_for_writing(fts_symbol_name(filename));
	  fts_bmax_code_new_patcher(f, patcher);
	  fts_close_bmax_file(f);
	}
      else
	post_mess("System Error in FOS message SAVE PATCHER BMAX: null patcher", ac, av);
    }
  else
    post_mess("System Error in FOS message SAVE PATCHER BMAX: bad args", ac, av);
}


/*    SAVE_PATCHER_TPAT  (obj)p (sym)filename

      Save the patcher in a tpat binary file.
 */

static void
fts_mess_client_save_patcher_tpat(int ac, const fts_atom_t *av)
{
  trace_mess("Received save patcher tpat ", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_symbol(&av[1]))
    {
      post_mess("Don't know yet how to save a tpat file: null patcher", ac, av);
    }
  else
    post_mess("System Error in FOS message SAVE PATCHER TPAT: bad args", ac, av);
}


/*    LOAD_PATCHER_BMAX  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_bmax(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher bmax ", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_symbol(&av[1]))
    {
      post_mess("Don't know yet how to load a bmax file: null patcher", ac, av);
    }
  else
    post_mess("System Error in FOS message LOAD PATCHER BMAX: bad args", ac, av);
}


/*    LOAD_PATCHER_TPAT  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_tpat(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher tpat ", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_symbol(&av[1]))
    {
      post_mess("Don't know yet how to load a tpat file: null patcher", ac, av);
    }
  else
    post_mess("System Error in FOS message LOAD PATCHER TPAT: bad args", ac, av);
}


/*    LOAD_PATCHER_DPAT  (obj)p (sym)filename

      Save the patcher in a bmax binary file.
 */

static void
fts_mess_client_load_patcher_dpat(int ac, const fts_atom_t *av)
{
  trace_mess("Received load patcher dpat", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_symbol(&av[1]))
    {
      fts_object_t *patcher;
      fts_symbol_t filename;

      patcher = (fts_object_t *) fts_get_object(&av[0]);
      filename = fts_get_symbol(&av[1]);

      if (patcher)
	{
	  fts_object_t *ret;

	  ret = fts_load_dotpat_patcher(patcher, fts_symbol_name(filename));

	  if (ret == 0)
	    post("Cannot read file %s\n", filename);
	}
      else
	post_mess("System Error in FOS message SAVE PATCHER BMAX: null patcher", ac, av);

    }
  else
    post_mess("System Error in FOS message LOAD PATCHER DPAT: bad args", ac, av);
}



/*    DOWNLOAD_PATCHER   (obj)p

      Send to the patcher the message "download". (system inlet)
      Do the actual work here; this is not good; but: it cannot be
      in the mess module because use the runtime !!
 */

static void
fts_mess_client_download_patcher(int ac, const fts_atom_t *av)
{
  trace_mess("Received download patcher ", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_patcher_t *patcher;
      fts_object_t *p;

      patcher = (fts_patcher_t *) fts_get_object(&av[0]);

      if (! patcher)
	{
	  post_mess("System Error in FOS message DOWNLOAD PATCHER: null patcher", ac, av);
	  return;
	}

      /* Code all the objects */

      for (p = patcher->objects; p ; p = p->next_in_patcher)
	{
	  /* Assign an ID if needed */

	  if (p->id == FTS_NO_ID)
	    fts_object_table_register(p);

	  /* Check if it is an abstraction; 
	     the trick is: if the object is a patcher, but
	     it first argument is not, then we consider it an abstraction,
	     (or template or whatever) and we send a different message
	     to tell it to the client 

	     Some object require special handling, because their description is
	     not necessarly consistent.
	     */

	  if (fts_object_is_inlet(p))
	    {
	      fts_inlet_t *inlet = (fts_inlet_t *) p;

	      fts_client_mess_start_msg(NEW_OBJECT_CODE);
	      fts_client_mess_add_object((fts_object_t *) p->patcher);
	      fts_client_mess_add_long(p->id);
	      fts_client_mess_add_sym(fts_s_inlet);
	      fts_client_mess_add_long(inlet->position);
	      fts_client_mess_send_msg();
	    }
	  else if (fts_object_is_outlet(p))
	    {
	      fts_outlet_t *outlet = (fts_outlet_t *) p;

	      fts_client_mess_start_msg(NEW_OBJECT_CODE);
	      fts_client_mess_add_object((fts_object_t *) p->patcher);
	      fts_client_mess_add_long(p->id);
	      fts_client_mess_add_sym(fts_s_outlet);
	      fts_client_mess_add_long(outlet->position);
	      fts_client_mess_send_msg();
	    }
	  else if (fts_object_is_abstraction(p))
	    {
	      /* Send the abstraction object */

	      /* NEW_ABSTRACTION  (obj)pid (int)new-id [<args>]+ */

	      fts_client_mess_start_msg(NEW_ABSTRACTION_CODE);
	      fts_client_mess_add_object((fts_object_t *) p->patcher);
	      fts_client_mess_add_long(p->id);
	      fts_client_mess_add_atoms(p->argc, p->argv);
	      fts_client_mess_send_msg();
	    }
	  else
	    {
	      /* Send the object */

	      /* NEW  (obj)pid (int)new-id [<args>]+ */

	      fts_client_mess_start_msg(NEW_OBJECT_CODE);
	      fts_client_mess_add_object((fts_object_t *) p->patcher);
	      fts_client_mess_add_long(p->id);
	      fts_client_mess_add_atoms(p->argc, p->argv);
	      fts_client_mess_send_msg();
	    }

	  fts_object_send_properties(p);
	}

      /* For each object, for each outlet, code all the connections */

      for (p = patcher->objects; p ; p = p->next_in_patcher)
	{
	  int outlet;

	  for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	    {
	      fts_connection_t *c;

	      for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
		{
		  /* CONNECT (obj)from (int)outlet (obj)to (int)inlet */

		  fts_client_mess_start_msg(CONNECT_OBJECTS_CODE);
		  fts_client_mess_add_object(c->src);
		  fts_client_mess_add_long(c->woutlet);
		  fts_client_mess_add_object(c->dst);
		  fts_client_mess_add_long(c->winlet);
		  fts_client_mess_send_msg();
		}
	    }
	}
    }
  else
    post_mess("System Error in FOS message DOWNLOAD PATCHER: bad args", ac, av);
}


/*    OPEN_PATCHER   (obj)p

      Send to the patcher the message "open". (system inlet)
 */

static void
fts_mess_client_open_patcher(int ac, const fts_atom_t *av)
{
  trace_mess("Received open patcher ", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *patcher;

      patcher = (fts_object_t *) fts_get_object(&av[0]);

      if (patcher)
	fts_message_send(patcher, fts_SystemInlet, fts_s_open, 0, 0);
      else
	post_mess("System Error in FOS message OPEN PATCHER: null patcher", ac, av);
    }
  else
    post_mess("System Error in FOS message OPEN PATCHER: bad args", ac, av);
}


/*    CLOSE_PATCHER  (obj)p
      Send to the patcher the message "close".(system inlet)
 */


static void
fts_mess_client_close_patcher(int ac, const fts_atom_t *av)
{
  trace_mess("Received close patcher ", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *patcher;

      patcher = (fts_object_t *) fts_get_object(&av[0]);

      if (patcher)
	fts_message_send(patcher, fts_SystemInlet, fts_s_close, 0, 0);
      else
	post_mess("System Error in FOS message CLOSE PATCHER: null patcher ", ac, av);
    }
  else
    post_mess("System Error in FOS message CLOSE PATCHER: bad args", ac, av);
}


/*
    PATCHER_LOADED (obj)p
      Send to the patcher the message "load_init".(system inlet)
    */


static void
fts_mess_client_patcher_loaded(int ac, const fts_atom_t *av)
{
  trace_mess("Received patcher loaded", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *patcher;

      patcher = (fts_object_t *) fts_get_object(&av[0]);

      if (patcher)
	fts_message_send(patcher, fts_SystemInlet, fts_s_load_init, 0, 0);
      else
	post_mess("System Error in FOS message PATCHER LOADED: null patcher", ac, av);
    }
  else
    post_mess("System Error in FOS message PATCHER LOADED: bad args", ac, av);
}



/*    NEW  (obj)pid (int)new-id [<args>]+

      Create a new object, in the parent patcher, with 
      local id new-lid, and description args.
 */

static void
fts_mess_client_new(int ac, const fts_atom_t *av)
{
  trace_mess("Received new", ac, av);

  if (ac >= 3 && fts_is_object(&av[0]) && fts_is_int(&av[1]))
    {
      int pid, id;
      fts_patcher_t *parent;
      fts_object_t  *new;

      parent = (fts_patcher_t *) fts_get_object(&av[0]);
      id  = fts_get_int(&av[1]);

      new = fts_object_new(parent, id, ac - 2, av + 2);

      if (! new)
	{
	  post_mess("Error in object creation", ac - 2, av + 2);
	  return;
	}

      /* No, too much !!; must be smarted, may be an explicit request */
      /* fts_object_send_properties(new); */
    }
  else
    post_mess("System Error in FOS message NEW: bad args", ac, av);
}


/* 
   REDEFINE (obj)old   [<args>]+
*/

static void
fts_mess_client_redefine(int ac, const fts_atom_t *av)
{
  trace_mess("Received redefine", ac, av);

  if (ac >= 1 && fts_is_object(&av[0]))
    {
      int pid, id;
      fts_object_t  *new;
      fts_object_t  *old;

      old =  fts_get_object(&av[0]);

      if (! old)
	{
	  post_mess("System Error in FOS message REDEFINE: redefining a non existing object", ac, av);
	  return;
	}

      new = fts_object_redefine(old, ac - 1, av + 1);

      if (! new)
	{
	  post_mess("Error in object creation", ac - 1, av + 1);
	  return;
	}
    }
  else
    post_mess("System Error in FOS message REDEFINE: bad args", ac, av);
}


/* 
   REPLACE (obj)old  (obj)new
*/

static void
fts_mess_client_replace(int ac, const fts_atom_t *av)
{
  trace_mess("Received replace", ac, av);

  if (ac == 2 && fts_is_object(&av[0]) && fts_is_object(&av[0]))
    {
      int pid, id;
      fts_object_t  *old;
      fts_object_t  *new;

      old =  fts_get_object(&av[0]);

      if (! old)
	{
	  post_mess("System Error in FOS message REPLACE: redefining a non existing object", ac, av);
	  return;
	}

      new =  fts_get_object(&av[1]);

      if (! new)
	{
	  post_mess("System Error in FOS message REPLACE: redefining to a non existing object", ac, av);
	  return;
	}

      fts_object_replace(old, new);
    }
  else
    post_mess("System Error in FOS message REPLACE: bad args", ac, av);
}


/* 
   FREE (obj)obj

   Free (destroy) the object identified by id.
*/


static void
fts_mess_client_free(int ac, const fts_atom_t *av)
{
  trace_mess("Received free", ac, av);

  if (ac == 1 && fts_is_object(&av[0]))
    {
      fts_object_t *obj;

      obj = fts_get_object(&av[0]);

      if (! obj)
	{
	  post_mess("System Error in FOS message FREE: freeing a non existing object", ac, av);
	  return;
	}

      fts_object_delete(obj);
    }
  else
    post_mess("System Error in FOS message FREE: bad args", ac, av);
}


/* CONNECT (obj)from (int)outlet (obj)to (int)inlet

  Connect the outlet of a from object (identified by the from-id )
  to an inlet of a to object (identified by the to-id) */


static void
fts_mess_client_connect(int ac, const fts_atom_t *av)
{
  trace_mess("Received connect", ac, av);

  if ((ac == 4) &&
      fts_is_object(&av[0]) &&
      fts_is_int(&av[1]) &&
      fts_is_object(&av[2]) &&
      fts_is_int(&av[3]))
    {
      int inlet, outlet;

      fts_object_t *from, *to;
      fts_status_t ret;

      from = fts_get_object(&av[0]);
      to   = fts_get_object(&av[2]);

      outlet = fts_get_int(&av[1]);
      inlet  = fts_get_int(&av[3]);

      if (to && from)
	{
	  ret = fts_object_connect(from, outlet, to, inlet);

	  if (ret)
	    post("Error connecting object %d outlet %d to object %d inlet %d: %s\n",
		 from->id, outlet, to->id, inlet, ret->description);
	}
      else
	post_mess("Error trying to connect non existing objects", ac, av);
    }
  else
    post_mess("System Error in FOS message CONNECT: bad args", ac, av);
}


/*
  DISCONNECT (obj)from (int)outlet (obj)to (int)inlet

  Disconnect the outlet of a from object (identified by the from-id )
  to an inlet of a to object  (identified by the to-id)
  */


static void
fts_mess_client_disconnect(int ac, const fts_atom_t *av)
{
  trace_mess("Received disconnect", ac, av);

  if ((ac == 4) &&
      fts_is_object(&av[0]) &&
      fts_is_int(&av[1]) &&
      fts_is_object(&av[2]) &&
      fts_is_int(&av[3]))
    {
      int inlet, outlet;

      fts_object_t *from, *to;
      fts_status_t ret;

      from = fts_get_object(&av[0]);
      to   = fts_get_object(&av[2]);

      outlet = fts_get_int(&av[1]);
      inlet  = fts_get_int(&av[3]);

      if (to && from)
	{
	  ret = fts_object_disconnect(from, outlet, to, inlet);

	  if (ret)
	    post("System Error in DISCONNECT: from %d, outlet %d to %d inlet %d: %s\n",
		 from->id, outlet, to->id, inlet, ret->description);
	}
      else
	post_mess("System Error in FOS message DISCONNECT: disconnecting non existing object", ac, av);
    }
  else
    post_mess("System Error in FOS message DISCONNECT: bad args", ac, av);
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
      fts_status_t ret;

      obj    = fts_get_object(&av[0]);
      inlet  = fts_get_int(&av[1]);
      selector = fts_get_symbol(&av[2]);

      if (! obj)
	{
	  post_mess("System Error in FOS message MESS: message to a non existing object", ac, av);
	  return;
	}

      ret = fts_message_send(obj, inlet, selector, ac - 3, av + 3);
      
      if (ret != fts_Success)
	{
	  post("Error in FOS message MESS: %s sending message %s to object of class %s\n",
	       ret->description, fts_symbol_name(selector),
	       fts_symbol_name(fts_object_get_class_name(obj)));

	  post_mess("Message MESSAGE_CODE", ac, av);
	}
    }
  else
    post_mess("System Error in FOS message MESS: bad args", ac, av);
}

/*
   NMESS (symbol)name (int)inlet (symbol)selector [<args>]*

   Send to an object identified by the symbol name,
   in the inlet "inlet", the message specified by the selector and its arguments.

   OBSOLETE, must be quickly substituted with something else
   */

static void 
fts_mess_client_nmess(int ac, const fts_atom_t *av)
{
  trace_mess("Received nmess", ac, av);

  /* Note that at the moment the inlet is ignored, because
     named "receives" recevies only on in let 0 */

  if ((ac >= 3) &&
      fts_is_symbol(&av[0]) &&
      fts_is_int(&av[1]) &&
      fts_is_symbol(&av[2]))
    {
      int inlet;

       fts_symbol_t selector, name;
       fts_status_t ret;

       name   = fts_get_symbol(&av[0]);
       inlet  = fts_get_int(&av[1]);
       selector = fts_get_symbol(&av[2]);

       if (! fts_send_message_to_receives(name, selector, ac - 3, av + 3))
	{
	  post("Error: receive %s do not exists", name);
	  return;
	}
    }
  else
    post_mess("System Error in FOS message NMESS: bad args", ac, av);
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
    post_mess("System Error in FOS message PUTPROP: bad args", ac, av);
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
    post_mess("System Error in FOS message GETPROP: bad args", ac, av);
}


static void
fts_messtile_install_all()
{
  fts_client_mess_install(SAVE_PATCHER_BMAX_CODE, fts_mess_client_save_patcher_bmax);
  fts_client_mess_install(SAVE_PATCHER_TPAT_CODE, fts_mess_client_save_patcher_tpat);

  fts_client_mess_install(LOAD_PATCHER_BMAX_CODE, fts_mess_client_load_patcher_bmax);
  fts_client_mess_install(LOAD_PATCHER_TPAT_CODE, fts_mess_client_load_patcher_tpat);
  fts_client_mess_install(LOAD_PATCHER_DPAT_CODE, fts_mess_client_load_patcher_dpat);

  fts_client_mess_install(DOWNLOAD_PATCHER_CODE, fts_mess_client_download_patcher);

  fts_client_mess_install(OPEN_PATCHER_CODE, fts_mess_client_open_patcher);
  fts_client_mess_install(CLOSE_PATCHER_CODE, fts_mess_client_close_patcher);
  fts_client_mess_install(PATCHER_LOADED_CODE,  fts_mess_client_patcher_loaded);
  fts_client_mess_install(NEW_OBJECT_CODE,  fts_mess_client_new);
  fts_client_mess_install(REDEFINE_OBJECT_CODE,  fts_mess_client_redefine);
  fts_client_mess_install(REPLACE_OBJECT_CODE,  fts_mess_client_replace);
  fts_client_mess_install(FREE_OBJECT_CODE,  fts_mess_client_free);
  fts_client_mess_install(CONNECT_OBJECTS_CODE,  fts_mess_client_connect);
  fts_client_mess_install(DISCONNECT_OBJECTS_CODE, fts_mess_client_disconnect);
  fts_client_mess_install(MESSAGE_CODE, fts_mess_client_mess);
  fts_client_mess_install(NAMED_MESSAGE_CODE, fts_mess_client_nmess);
  fts_client_mess_install(PUTPROP_CODE,  fts_mess_client_put_prop);
  fts_client_mess_install(GETPROP_CODE,  fts_mess_client_get_prop);
}





