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

For the moment, no DSP implementation !!!
*/

/* The object Structures are defined in mess_sys.h
*/

#include "sys.h"
#include "lang.h"

/* Forward declarations */

fts_metaclass_t *patcher_metaclass;
fts_metaclass_t *inlet_metaclass;
fts_metaclass_t *outlet_metaclass;

/* INlets; inlets are only placeholders for the internal connections
   they receive no messages, send no messages; the messages are sent
   by the patcher object on its outlet; for the DSP, they are like through.
*/

static void
inlet_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  /* If the operation is a nop (i.e. a copy on the same buffer),
     do not generate it */

  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    {
      fts_set_symbol( argv,   fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_long( argv+2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
    }
}


/* Arguments:
   arg 1 : position (number of inlet, optional; if missing, it is not assigned)
*/

static void
inlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if (ac == 2)
    {
      this->position = fts_get_long_arg(ac, at, 1, 0);
      patcher->inlets[this->position] = this;
    }
  else
    {
      /* compatibility with ".pat" files: inlet will be
	 redefined later
       */
      
      this->position = -1;
    }

  dsp_list_insert(o); /* put object in list */
}


static void
inlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this   = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if (this->position >= 0)
    patcher->inlets[this->position] = 0;

  dsp_list_remove(o);
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
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init,   inlet_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, inlet_delete, 0, 0);

  a[0] = fts_s_ptr; 
  fts_method_define(cl, fts_SystemInlet, fts_s_put, inlet_put, 1, a);

  /* signal inlet (outlet is included to anything) */

  dsp_sig_inlet(cl, 0);

  return fts_Success;
}


/* Class/metaclass installation  */

static void
internal_inlet_config(void)
{
  inlet_metaclass = fts_metaclass_create(fts_new_symbol("inlet"), inlet_instantiate, fts_always_equiv);
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


static void
outlet_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  /* If the operation is a nop (i.e. a copy on the same buffer),
     do not generate it */

  if (fts_dsp_get_input_name(dsp, 0) != fts_dsp_get_output_name(dsp, 0))
    {
      fts_set_symbol( argv,   fts_dsp_get_input_name(dsp, 0));
      fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
      fts_set_long( argv+2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(ftl_sym.cpy.f, 3, argv);
    }
}


static void
outlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if (ac == 2)
    {
      this->position = fts_get_long_arg(ac, at, 1, 0);
      patcher->outlets[this->position] = this;
    }
  else
    {
      /* Compatibility with ".pat" files, it will be redefined
	 later */

      this->position = -1;
    }

  dsp_list_insert(o); /* put object in list */
}


void
outlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this   = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  if (this->position >= 0)
    patcher->outlets[this->position] = 0;

  dsp_list_remove(o);
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
  a[1] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, outlet_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, outlet_delete, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, outlet_anything);

  a[0] = fts_s_ptr; 
  fts_method_define(cl, fts_SystemInlet, fts_s_put, outlet_put, 1, a);

  /* signal inlet (inlet is included to anything) */

  return fts_Success;
}

/* metaclass installation  */

static void
internal_outlet_config(void)
{
  outlet_metaclass = fts_metaclass_create(fts_new_symbol("outlet"),outlet_instantiate, fts_always_equiv);
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



static void
patcher_replace(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *new = (fts_patcher_t *) o;
  fts_patcher_t *old  = (fts_patcher_t *) fts_get_object(&at[0]);
  fts_object_t *p;
  int i;

  /* first, delete the inlets that will not be used in the new patcher,
     if any
   */

  for (i = fts_object_get_inlets_number((fts_object_t *) new);
       i < fts_object_get_inlets_number((fts_object_t *) old);
       i++)
    fts_object_delete((fts_object_t *) (old->inlets[i]));
    
  /* Move the other inlets to the new patcher (if any) */

  for (i = 0; i < fts_object_get_inlets_number((fts_object_t *) new); i++)
    {

      if (i < fts_object_get_inlets_number((fts_object_t *) old))
	{
	  new->inlets[i] = old->inlets[i];
	  old->inlets[i] = 0;
	}
      else
	new->inlets[i] = 0;
    }

  /* second, delete the outlets that will not be used in the new patcher;
   */

  for (i = fts_object_get_outlets_number((fts_object_t *) new);
       i < fts_object_get_outlets_number((fts_object_t *) old);
       i++)
    fts_object_delete((fts_object_t *) (old->outlets[i]));

  /* Move the other outlets to the new patcher, leaving to zero
     the non-initialized inlets.
   */

  for (i = 0; i < fts_object_get_outlets_number((fts_object_t *) new); i++)
    {
      if (i < fts_object_get_outlets_number((fts_object_t *) old))
	{
	  new->outlets[i] = old->outlets[i];
	  old->outlets[i] = 0;
	}
      else
	new->outlets[i] = 0;
    }
  
  /* move all the contained objects */

  new->objects = old->objects;
  old->objects = 0;

  /* change the patcher pointer in the contained objects */
  
  for (p = new->objects; p ; p = p->next_in_patcher)
    p->patcher = new;
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
      fts_message_send(p, winlet, s, ac, at);

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_message_send(p, winlet, s, ac, at);
}


/* Open a patch;

   this send the inlet/outlet information to the client for all the objects
   in the patcher, and set the open flag to 1, so that all the "updating"
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
      fts_object_property_changed(p, fts_s_ninlets);
      fts_object_property_changed(p, fts_s_noutlets);
    }

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_message_send(p, winlet, s, ac, at);
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

  this->open = 1;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_message_send(p, winlet, s, ac, at);
}

/* Methods: init put the pointers to zero */

static void
patcher_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  int ninlets, noutlets;

  fts_patcher_t *this = (fts_patcher_t *) o;

  /* get the name */

  this->name = fts_get_symbol_arg(ac, at, 1, 0);

  /* should use block allocation ?? */

  ninlets = fts_object_get_inlets_number(o);
  noutlets = fts_object_get_outlets_number(o);

  if (ninlets > 0)
    {
      this->inlets  = (fts_inlet_t **)  fts_malloc(sizeof(fts_inlet_t *) * ninlets);

      for (i = 0; i < ninlets; i++)
	this->inlets[i] = 0;
    }
  else
    this->inlets = 0;
  
  if (noutlets)
    {
      this->outlets = (fts_outlet_t **) fts_malloc(sizeof(fts_outlet_t *) * noutlets);

      for (i = 0; i < noutlets; i++)
	this->outlets[i] = 0;
    }
  else
    this->outlets = 0;

  this->objects = (fts_object_t *) 0;
  this->open    = 0;		/* start as closed */
}


static void
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

  /* delete the inlets and inlets tables */

  if (this->inlets)
    fts_free(this->inlets);

  if (this->outlets)
    fts_free(this->outlets);
}

/* Class instantiation */

static fts_status_t
patcher_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[4];
  int ninlets;
  int noutlets;
  int i;

  ninlets  = fts_get_long(at + 2);
  noutlets = fts_get_long(at + 3);

  /* initialize the class */

  fts_class_init(cl, sizeof(fts_patcher_t), ninlets, noutlets, 0);

  /* define the init system method */

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_int;
  a[3] = fts_s_int;
  fts_method_define(cl,fts_SystemInlet, fts_s_init, patcher_init, 4, a);

  fts_method_define(cl,fts_SystemInlet, fts_s_delete, patcher_delete, 0, 0);

  a[0] = fts_s_object;
  fts_method_define(cl,fts_SystemInlet, fts_s_replace, patcher_replace, 1, a); 

  for (i = 0; i < ninlets; i ++)
    fts_method_define_varargs(cl, i, fts_s_anything, patcher_anything);

  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("load_init"), patcher_load_init, 0, 0); 
  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("open"), patcher_open, 0, 0); 
  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("close"), patcher_close, 0, 0); 

  /* No methods really installed on the inlets  */

  return fts_Success;
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
  patcher_metaclass = fts_metaclass_create(fts_new_symbol("patcher"), patcher_instantiate, fts_patcher_equiv);
}

/* Global configuration function; it call the configuration 
   function of each class */

void
fts_patcher_init(void)
{
  internal_patcher_config();
  internal_inlet_config();
  internal_outlet_config();
}

