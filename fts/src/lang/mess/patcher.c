/*
 *                      Copyright (c) 1997 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *
 *
 */

/* #define DO_EXPRESSIONS */

/* THE REAL FTS 2.0 patcher class

The patcher class is a standard class, with real inlets and real
outlets; it keeps track of its content; inlets and outlets objects
have special and direct relationship with the patcher, that use them
as place holder for the connections.

The patcher class do not have variable number of inlet/outlet;
if the editor add a inlet or outlet, it replace the patcher object
with a new one; the patcher have, for now, two arguments, the number
of inputs and the number of outputs; in the future, it may handles the
types from the inputs.

Housekeeping of the patcher structure is integrated in the message system;
the patcher is not a standalone object anymore.

For now, until Input/output typing, all the inlets and outlaets are
DSP, and the patcher is integrated with the DSP compiler (to be
rewritten, i am afraid :-< ).

*/


/* The object Structures are defined in mess_sys.h
*/
#include "sys.h"
#include "lang.h"
#include "lang/mess/messP.h"

fts_metaclass_t *patcher_metaclass;
fts_metaclass_t *inlet_metaclass;
fts_metaclass_t *outlet_metaclass;



/* INlets; inlets are only placeholders for the internal connections
   they receive no messages, send no messages; the messages are sent
   by the patcher object on its outlet; for the DSP, they are like through.
*/

/* function and method to reposition an inlet, without using redefinition (bad
   for properties !!)
   */

void
fts_inlet_reposition(fts_object_t *o, int pos)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_set_int(&(o->argv[1]), pos);
  this->position = pos;

  if ((this->position >= 0) && (this->position < fts_object_get_inlets_number((fts_object_t *)patcher)))
    patcher->inlets[this->position] = this;
}

/* Arguments:
   arg 1 : position (number of inlet, optional; if missing, it is not assigned)
*/

static void
inlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if (fts_is_long(at+1))
    {
      this->position = fts_get_long_arg(ac, at, 1, 0);

      if (this->position < fts_object_get_inlets_number((fts_object_t *)patcher))
	patcher->inlets[this->position] = this;
    }
  else
    {
      /* compatibility with ".pat" files: inlet will be
	 redefined later
       */
      
      this->position = -1;
    }
}


static void
inlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this   = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if ((this->position >= 0) && (this->position < fts_object_get_inlets_number((fts_object_t *)patcher)))
    patcher->inlets[this->position] = 0;
}


/* Class instantiation */

static fts_status_t
inlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
  int argc;

  /* initialize the class */

  fts_class_init(cl, sizeof(fts_inlet_t),  1, 1, 0);

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  fts_method_define(cl, fts_SystemInlet, fts_s_init,   inlet_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, inlet_delete, 0, 0);

  return fts_Success;
}


/* Class/metaclass installation  */

static void
internal_inlet_config(void)
{
  fts_metaclass_create(fts_s_inlet, inlet_instantiate, fts_always_equiv);
  inlet_metaclass = fts_metaclass_get_by_name(fts_s_inlet);
}


/********************* outlet ************************/

/* Outlets, if they are assigned, send the message they receive directly
   thru the patcher outlet */


static void
outlet_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_outlet_send((fts_object_t *)patcher, this->position, s, ac, at);
}

/* function to reposition an outlet, without using redefinition (bad
   for properties !!)
   */

void
fts_outlet_reposition(fts_object_t *o, int pos)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  this->position = pos;
  fts_set_int(&(o->argv[1]), pos);

  if ((this->position >= 0) && (this->position < fts_object_get_outlets_number((fts_object_t *)patcher)))
    patcher->outlets[this->position] = this;
}


static void
outlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if (fts_is_long(at+1))
    {
      this->position = fts_get_long_arg(ac, at, 1, 0);

      if (this->position < fts_object_get_outlets_number((fts_object_t *)patcher))
	patcher->outlets[this->position] = this;
    }
  else
    {
      /* Compatibility with ".pat" files, it will be redefined
	 later */

      this->position = -1;
    }
}


static void
outlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this   = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if ((this->position >= 0) && (this->position < fts_object_get_outlets_number((fts_object_t *)patcher)))
    patcher->outlets[this->position] = 0;
}


/* Class instantiation */


static fts_status_t
outlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[5];
  int argc;

  /* initialize the class */

  fts_class_init(cl, sizeof(fts_outlet_t), 1, 1, 0);

  /* define the init system method */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, outlet_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, outlet_delete, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, outlet_anything);

  /* signal inlet (inlet is included to anything) */

  return fts_Success;
}

/* metaclass installation  */

static void
internal_outlet_config(void)
{
  fts_metaclass_create(fts_s_outlet, outlet_instantiate, fts_always_equiv);
  outlet_metaclass = fts_metaclass_get_by_name(fts_s_outlet);
}


/*
  the patcher class; the patcher send messages it receive on the inlets
  to the inlets outlets.

  The output messages are directly send to the patcher outlets by the outlet
  objects
*/


static void
patcher_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_patcher_t *this  = (fts_patcher_t *) o;

  if (this->inlets[winlet])
    fts_outlet_send((fts_object_t *)(this->inlets[winlet]), 0, s, ac, at);
}


/* The load init method;

   it send recursively load_init to all the included objects, *first* to subpatchers,
   and then to the other objects; this guarantee that any loadbang or similar
   objects are executed first in the external patches and then in the external ones

   The patchers have a protection against multiple load-init firing.
   */


static void
patcher_load_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;

  if (this->load_init_fired)
    return;

  this->load_init_fired = 1;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_patcher(p))
      fts_send_message(p, winlet, s, ac, at);

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_send_message(p, winlet, s, ac, at);

}


/* Open a patch;

   Set the open flag to 1, so that all the "updating"
   objects can be active.

   A "OPEN" message is sent to all the objects (but not the patchers)
   in the patch
   */


static void
patcher_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;

  this->open = 1;

  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      fts_object_send_properties(p);
    }

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_send_message(p, winlet, s, ac, at);
}


/* Close a patch;

   Set the open flag to 1, so that all the "updating"
   objects can be inactive.

   A "close" message is sent to all the objects (but not the patchers)
   in the patch
   */


static void
patcher_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;

  this->open = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_send_message(p, winlet, s, ac, at);
}

/* Methods: init put the pointers to zero */

static void
patcher_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  int ninlets, noutlets;

  fts_patcher_t *this = (fts_patcher_t *) o;

  /* get the name */

  this->name = fts_get_symbol_arg(ac, at, 1, fts_new_symbol("unnamed"));

  /* should use block allocation ?? */

  ninlets = fts_object_get_inlets_number(o);
  noutlets = fts_object_get_outlets_number(o);

  if (ninlets > 0)
    {
      this->inlets  = (fts_inlet_t **)  fts_block_alloc(sizeof(fts_inlet_t *) * ninlets);

      for (i = 0; i < ninlets; i++)
	this->inlets[i] = 0;
    }
  else
    this->inlets = 0;
  
  if (noutlets)
    {
      this->outlets = (fts_outlet_t **) fts_block_alloc(sizeof(fts_outlet_t *) * noutlets);

      for (i = 0; i < noutlets; i++)
	this->outlets[i] = 0;
    }
  else
    this->outlets = 0;

  this->objects = (fts_object_t *) 0;
  this->open    = 0;		/* start as closed */
  this->env     = 0;
  fts_patcher_set_standard(this);
}


/* static void */
void
patcher_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;

  /*
   * delete its content; each destroied object will take away himself
   * from the list, including inlets and outlets; this will undo the
   * connections also to the inlets and outlets.
   */

  while (this->objects)
    fts_object_delete(this->objects);

  /* Delete all the variables */

  fts_variable_env_clean(&(this->env));

  /* delete the inlets and inlets tables */

  if (this->inlets)
    fts_block_free((char *) this->inlets, sizeof(fts_inlet_t *) * fts_object_get_inlets_number((fts_object_t *) this));

  if (this->outlets)
    fts_block_free((char *) this->outlets, sizeof(fts_outlet_t *)*fts_object_get_outlets_number((fts_object_t *) this));
}

/* Error property handling */

static void
patcher_get_name(fts_daemon_action_t action, fts_object_t *obj,
		  int idx, fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_t *this = (fts_patcher_t *) obj;

  if (fts_patcher_is_standard(this))
    {
      fts_set_symbol(value, this->name);
    }
  else
    {
      /* Template, or error, use the class name as patcher name, for the moment */

      if ((obj->argc >= 3) &&
	  fts_is_symbol(&obj->argv[0]) &&
	  fts_is_symbol(&obj->argv[1])
	  && (fts_get_symbol(&obj->argv[1]) == fts_s_else))
	{
	  /* foo : <obj> syntax; extract the class name */

	  *value = obj->argv[2];
	}
      else
	{
	  *value = obj->argv[0];
	}
    }
}


static void
patcher_get_error(fts_daemon_action_t action, fts_object_t *obj,
		  int idx, fts_symbol_t property, fts_atom_t *value)
{
  if (fts_object_is_error(obj))
    fts_set_int(value, 1);
  else
    fts_set_int(value, 0);
}

/* Class instantiation */

static fts_status_t
patcher_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
  int ninlets;
  int noutlets;
  int i;

  ninlets  = fts_get_long_arg(ac, at, 2, 0);
  noutlets = fts_get_long_arg(ac, at, 3, 0);

  /* initialize the class */

  fts_class_init(cl, sizeof(fts_patcher_t), ninlets, noutlets, 0);

  /* define the init system method */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define_optargs(cl,fts_SystemInlet, fts_s_init, patcher_init, 4, a, 1);

  fts_method_define(cl,fts_SystemInlet, fts_s_delete, patcher_delete, 0, 0);

  for (i = 0; i < ninlets; i ++)
    fts_method_define_varargs(cl, i, fts_s_anything, patcher_anything);

  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("load_init"), patcher_load_init, 0, 0); 
  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("open"), patcher_open, 0, 0); 
  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("close"), patcher_close, 0, 0); 

  /* No methods really installed on the inlets  */

  /* daemon for error property */

  fts_class_add_daemon(cl, obj_property_get, fts_s_error, patcher_get_error);

  /* daemon for name property */

  fts_class_add_daemon(cl, obj_property_get, fts_s_name, patcher_get_name);

  return fts_Success;
}


/* 
 * Redefine a patcher inplace from a new description in term of "object" description.
 * 
 * For the moment, parse the arguments, and call fts_patcher redefine.
 * It should also reset the local patcher variables (only those defined by the 
 * previuos definition !!!, and parse the new ones).
 * Than, send back properties nins, nouts and name.
 */

void
fts_patcher_redefine_description(fts_patcher_t *this, int aoc, const fts_atom_t *aot)
{
#ifdef DO_EXPRESSIONS 
  fts_expression_state_t *e;
  int ac;
  fts_atom_t at[3]; /* Actually, the evaluated atom vector */
#else
#define at aot  
#define ac aoc
#endif

#ifdef DO_EXPRESSIONS 

  /* 1- unbound the patcher variable if any */

  /* To be implemented !!! */

  /* 2- delete  the patcher internal variable if any */

  fts_patcher_remove_variables(this);

  /* 3- eval the expression
     Ignore the errors, for the moment !!
     */

  e = fts_expression_eval((fts_object_t *)this, aoc, aot, 3, at);
  ac = fts_expression_get_count(e);
#endif

    /* 4- redefine the patcher */

    fts_patcher_redefine(this, fts_get_symbol(&at[0]),fts_get_int(&at[1]), fts_get_int(&at[2]));

    /* 5- set the new variables */

#ifdef DO_EXPRESSIONS
    fts_expression_assign_properties(e, (fts_object_t *)this);
#endif

    /* 6- Send the properties */

    fts_object_send_properties((fts_object_t *)this);
}

/*
 * Redefine a patcher "inplace" by changing directly the name, inlets and outlets
 * and *the class* of the object.
 * To do this, it heavily rely on the internals of the message system !!!
 * 
 * If the name argument is null, it do not change the name.
 * It update also the patcher description (argc, argv) , only if the patcher is not
 * an abstraction.
 *
 * Also, this function assume that the inlet/outlet housekeeping is done in the editor;
 * it do not delete redundant inlets or outlets.
 */

void
fts_patcher_redefine(fts_patcher_t *this, fts_symbol_t name, int new_ninlets, int new_noutlets)
{
  fts_object_t *obj_this = (fts_object_t *)this;
  int old_ninlets;
  int old_noutlets;

  old_ninlets = fts_object_get_inlets_number((fts_object_t *) this);
  old_noutlets = fts_object_get_outlets_number((fts_object_t *) this);

  /* Reallocate and copy the patcher inlets, incoming connections and inlets properties if needed */

  if (old_ninlets != new_ninlets)
    {
      if (new_ninlets == 0)
	{
	  /* No new inlets, but (since the != test is true) there
	     are old inlets to delete */

	  fts_block_free((char *)this->inlets, old_ninlets * sizeof(fts_inlet_t *));
	  fts_block_free((char *)obj_this->in_conn, old_ninlets * sizeof(fts_connection_t *));
	  fts_block_free((char *)obj_this->inlets_properties, old_ninlets * sizeof(fts_plist_t *));

	  this->inlets = 0;
	  obj_this->in_conn = 0;
	  obj_this->inlets_properties = 0;
	}
      else  if (old_ninlets > 0)
	{
	  /* There are new inlets: and there are  old inlets, so reallocate and move */

	  int i;
	  fts_inlet_t  **new_inlets;
	  fts_connection_t **new_in_conn;
	  fts_plist_t **new_inlets_properties;	

	  new_inlets  = (fts_inlet_t **)  fts_block_alloc(new_ninlets * sizeof(fts_inlet_t *));
	  new_in_conn = (fts_connection_t **) fts_block_zalloc(new_ninlets * sizeof(fts_connection_t *));
	  new_inlets_properties = (fts_plist_t **) fts_block_zalloc(new_ninlets * sizeof(fts_plist_t *));

	  for (i = 0; i < new_ninlets; i++)
	    {
	      if (i < old_ninlets)
		{
		  new_inlets[i] = this->inlets[i];
		  new_in_conn[i] = obj_this->in_conn[i];
		  new_inlets_properties[i] = obj_this->inlets_properties[i];
		}
	      else
		{
		  new_inlets[i] = 0;
		  new_in_conn[i] = 0;
		  new_inlets_properties[i] = 0;
		}
	    }

	  fts_block_free((char *)this->inlets, old_ninlets * sizeof(fts_inlet_t *));
	  fts_block_free((char *)obj_this->in_conn, old_ninlets * sizeof(fts_connection_t *));
	  fts_block_free((char *)obj_this->inlets_properties, old_ninlets * sizeof(fts_plist_t *));
	      
	  this->inlets = new_inlets;
	  obj_this->in_conn = new_in_conn;
	  obj_this->inlets_properties = new_inlets_properties;
	}
      else 
	{
	  int i;

	  /* There are new inlets, but there were no inlets before, so just allocate without
	     copying old stuff */

	  this->inlets = (fts_inlet_t **) fts_block_alloc(new_ninlets * sizeof(fts_inlet_t *));
	  obj_this->in_conn = (fts_connection_t **) fts_block_zalloc(new_ninlets * sizeof(fts_connection_t *));
	  obj_this->inlets_properties = (fts_plist_t **) fts_block_zalloc(new_ninlets * sizeof(fts_plist_t *));

	  for (i = 0; i < new_ninlets; i++)
	    {
	      this->inlets[i] = 0;
	      obj_this->in_conn[i] = 0;
	      obj_this->inlets_properties[i] = 0;
	    }
	}
    }

  /* If it is a standard patcher, change the description for ninlets */

  if (fts_patcher_is_standard(this))
    fts_set_int(&(obj_this->argv[2]), new_ninlets);

  /* Reallocate and copy the patcher outlets, incoming connections and outlets properties if needed */


  if (old_noutlets != new_noutlets)
    {
      if (new_noutlets == 0)
	{
	  /* No new outlets, but (since the != test is true) there
	     are old outlets to delete */

	  fts_block_free((char *)this->outlets, old_noutlets * sizeof(fts_outlet_t *));
	  fts_block_free((char *)obj_this->in_conn, old_noutlets * sizeof(fts_connection_t *));
	  fts_block_free((char *)obj_this->outlets_properties, old_noutlets * sizeof(fts_plist_t *));

	  this->outlets = 0;
	  obj_this->in_conn = 0;
	  obj_this->outlets_properties = 0;
	}
      else  if (old_noutlets > 0)
	{
	  /* There are new outlets: and there are no old outlets, so reallocate and move */

	  int i;
	  fts_outlet_t  **new_outlets;
	  fts_connection_t **new_out_conn;
	  fts_plist_t **new_outlets_properties;	

	  new_outlets  = (fts_outlet_t **)  fts_block_alloc(new_noutlets * sizeof(fts_outlet_t *));
	  new_out_conn = (fts_connection_t **) fts_block_zalloc(new_noutlets * sizeof(fts_connection_t *));
	  new_outlets_properties = (fts_plist_t **) fts_block_zalloc(new_noutlets * sizeof(fts_plist_t *));

	  for (i = 0; i < new_noutlets; i++)
	    {
	      new_outlets[i] = 0;

	      if (i < old_noutlets)
		{
		  new_out_conn[i] = obj_this->out_conn[i];
		  new_outlets_properties[i] = obj_this->outlets_properties[i];
		}
	      else
		{
		  new_out_conn[i] = 0;
		  new_outlets_properties[i] = 0;
		}
	    }

	  fts_block_free((char *)this->outlets, old_noutlets * sizeof(fts_outlet_t *));
	  fts_block_free((char *)obj_this->out_conn, old_noutlets * sizeof(fts_connection_t *));
	  fts_block_free((char *)obj_this->outlets_properties, old_noutlets * sizeof(fts_plist_t *));
	      
	  this->outlets = new_outlets;
	  obj_this->out_conn = new_out_conn;
	  obj_this->outlets_properties = new_outlets_properties;
	}
      else 
	{
	  int i;

	  /* There are new outlets, but there were no outlets before, so just allocate without
	     copying old stuff */

	  this->outlets = (fts_outlet_t **) fts_block_alloc(new_noutlets * sizeof(fts_outlet_t *));
	  obj_this->out_conn = (fts_connection_t **) fts_block_zalloc(new_noutlets*sizeof(fts_connection_t *));
	  obj_this->outlets_properties = (fts_plist_t **) fts_block_zalloc(new_noutlets*sizeof(fts_plist_t *));

	  for (i = 0; i < new_noutlets; i++)
	    {
	      this->outlets[i] = 0;
	      obj_this->out_conn[i] = 0;
	      obj_this->outlets_properties[i] = 0;
	    }
	}
    }

  /* If it is a standard patcher abstraction, change the description for noutlets */

  if (fts_patcher_is_standard(this))
    fts_set_int(&(obj_this->argv[3]), new_noutlets);

  /* Set the new name if not null */

  if (name != 0)
    {
      this->name = name;

      if (fts_patcher_is_standard(this))
	fts_set_symbol(&(obj_this->argv[1]), name);
    }

  {
    fts_object_t *p;

    /* Reinstall the inlets */

    for (p = this->objects; p ; p = p->next_in_patcher)
      if (fts_object_is_inlet(p))
	fts_inlet_reposition(p, ((fts_inlet_t *)p)->position);

    /* Reinstall the outlets */

    for (p = this->objects; p ; p = p->next_in_patcher)
      if (fts_object_is_outlet(p))
	fts_outlet_reposition(p, ((fts_outlet_t *)p)->position);
  }

  /*
   * Finally , change the patcher class (of course, not the metaclass); also,
   * no init method needed here; we could conditionally use the description
   * if the object is not an abstraction, but is probabily not worth doing it.
   */

  {
    fts_atom_t description[4];

    fts_set_symbol(&description[0], fts_s_patcher);
    fts_set_symbol(&description[1], this->name);
    fts_set_int(&description[2], new_ninlets);
    fts_set_int(&description[3], new_noutlets);

    obj_this->cl = fts_class_instantiate(4, description);
  }
}

/* Functions for object management; register and remove  an object in a patcher. */

void
fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj)
{
  obj->patcher = this;
  obj->next_in_patcher = this->objects;
  this->objects = obj;
}

void
fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj)
{
  fts_object_t **p;		/* indirect precursor */

  for (p = &(this->objects); *p; p = &((*p)->next_in_patcher))
    if (*p == obj)
      {
	*p = obj->next_in_patcher;
	obj->patcher = 0;
	return;
      }
}

/* Functions for direct .pat loading support */

static int
fts_patcher_count_inlet_objects(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_inlet(p))
      i++;

  return i;
}

static int
fts_patcher_count_outlet_objects(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_outlet(p))
      i++;

  return i;
}

/*
 * Support for .pat parsing.
 * 
 * This function take all the declared inlets and outlets
 * and assign them to the patcher in the correct order (based on the x position),
 * setting also the inlet and outlet status.
 * It assume that the patcher have no inlets and outlets,
 * allocate the new ones, and *change* the class of the patcher,
 * using the fts_patcher_redefine function.
 * 
 * The new_name may be null; in this case, the name is not changed at all.
 */

void
fts_patcher_reassign_inlets_outlets_name(fts_patcher_t *this, fts_symbol_t new_name)
{
  fts_object_t *p;
  int i, j;
  int ninlets;
  int noutlets;

  /* Compute number of inlets and outlets */

  ninlets = fts_patcher_count_inlet_objects(this);
  noutlets = fts_patcher_count_outlet_objects(this);

  /* make a redefine the old patcher */

  fts_patcher_redefine(this, new_name, ninlets, noutlets);

  /* Store the inlets in the inlet arrays; not so redundant as may seems,
     because patcher_redefine do not reposition inlets with 
     no position.
   */

  i = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_inlet(p))
      this->inlets[i++] = (fts_inlet_t *) p;

  /* Store the outlets in the oulet arrays */

  i = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_outlet(p))
	this->outlets[i++] = (fts_outlet_t *) p;

  /* Sort the inlets  based on their x property*/

  for (i = 1; i < ninlets; i++)
    for (j = 0; j < i; j++)
      {
	fts_atom_t aix, ajx;
	int ix, jx;

	fts_object_get_prop((fts_object_t *)this->inlets[i], fts_s_x, &aix);
	fts_object_get_prop((fts_object_t *)this->inlets[j], fts_s_x, &ajx);

	ix = fts_get_int(&aix);
	jx = fts_get_int(&ajx);

	if (jx > ix)
	  {
	    fts_inlet_t *tmp;

	    tmp = this->inlets[i];
	    this->inlets[i] = this->inlets[j];
	    this->inlets[j] = tmp;
	  }
      }

  /* Sort the outlets  based on their x property*/

  for (i = 1; i < noutlets; i++)
    for (j = 0; j < i; j++)
      {
	fts_atom_t aix, ajx;
	int ix, jx;

	fts_object_get_prop((fts_object_t *)this->outlets[i], fts_s_x, &aix);
	fts_object_get_prop((fts_object_t *)this->outlets[j], fts_s_x, &ajx);

	ix = fts_get_int(&aix);
	jx = fts_get_int(&ajx);

	if (jx > ix)
	  {
	    fts_outlet_t *tmp;

	    tmp = this->outlets[i];
	    this->outlets[i] = this->outlets[j];
	    this->outlets[j] = tmp;
	  }
      }

  /* redefine all the inlets */

  for (i = 0; i < ninlets; i++)
    fts_inlet_reposition((fts_object_t *) this->inlets[i], i);

  /* redefine all the outlets */

  for (i = 0; i < noutlets; i++)
    fts_outlet_reposition((fts_object_t *) this->outlets[i], i);
}


/* accessing inlets and outlets in patcher */

fts_object_t *
fts_patcher_get_inlet(fts_object_t *patcher, int inlet)
{
  fts_patcher_t *this = (fts_patcher_t *) patcher;

  if (inlet < fts_object_get_inlets_number(this))
    return (fts_object_t *) this->inlets[inlet];
  else
    return 0;
}

fts_object_t *
fts_patcher_get_outlet(fts_object_t *patcher, int outlet)
{
  fts_patcher_t *this = (fts_patcher_t *) patcher;

  if (outlet < fts_object_get_outlets_number(this))
    return (fts_object_t *) this->outlets[outlet];
  else
    return 0;
}

/* Class/metaclass installation  */

static int
fts_patcher_equiv(int ac0, const fts_atom_t *at0, int ac1,  const fts_atom_t *at1)
{
  return ((fts_get_int(&at0[2]) == fts_get_int(&at1[2])) &&
	  (fts_get_int(&at0[3]) == fts_get_int(&at1[3])));
}


static void
internal_patcher_config(void)
{
  fts_metaclass_create(fts_s_patcher, patcher_instantiate, fts_patcher_equiv);
  patcher_metaclass = fts_metaclass_get_by_name(fts_s_patcher);
}


/* The root patcher: it created here, and accessed thru the function
   fts_get_root_patcher(); the root patcher have by definition an ID 
   of 1 !!!
   */

fts_object_t *fts_root_patcher;

void fts_create_root_patcher()
{
  fts_atom_t description[4];
  fts_object_t *patcher;

  fts_set_symbol(&description[0], fts_s_patcher);
  fts_set_symbol(&description[1], fts_new_symbol("root"));
  fts_set_int(&description[2], 0);
  fts_set_int(&description[3], 0);

  fts_root_patcher = fts_object_new((fts_patcher_t *)0, 1, 4, description);
}

static void fts_delete_root_patcher()
{
  fts_object_delete(fts_root_patcher);
}

fts_object_t *fts_get_root_patcher()
{
  return fts_root_patcher;
}



/* Global configuration function; it call the configuration 
   function of each class */


void fts_patcher_init(void)
{
  internal_patcher_config();
  internal_inlet_config();
  internal_outlet_config();
}


void
fts_patcher_shutdown(void)
{
  fts_delete_root_patcher();
}



