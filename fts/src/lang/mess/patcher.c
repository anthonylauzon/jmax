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

#include <stdarg.h>
#include <stdio.h>
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


static void fts_inlet_remove_from_patcher(fts_inlet_t *this, fts_patcher_t *patcher)
{
  /* Remove the inlet from its previous position;
     ignore the case where the inlet was not there yet
   */

  if ((this->position >= 0) && (this->position < fts_object_get_inlets_number((fts_object_t *)patcher)))
    {
      fts_inlet_t **p; /* indirect precursor */

      for (p = &(patcher->inlets[this->position]); *p ; p = &((*p)->next))
	if ((*p) == this)
	  {
	    (*p) = this->next;
	    break;
	  }
    }
}

/** This function remove the last empty inlets in a patcher */
static void fts_patcher_trim_number_of_inlets(fts_patcher_t *patcher)
{
  int new_inlets;

  new_inlets = fts_object_get_inlets_number((fts_object_t *) patcher);
     
  while ((new_inlets > 0) && (patcher->inlets[new_inlets - 1] == 0))
    new_inlets--;

  fts_patcher_redefine_number_of_inlets(patcher, new_inlets);
}


static void fts_inlet_add_to_patcher(fts_inlet_t *this, fts_patcher_t *patcher)
{
  if (this->position >= 0)
    {
      this->next = patcher->inlets[this->position];
      patcher->inlets[this->position] = this;
    }
}


static void fts_inlet_set_position(fts_patcher_t *patcher, fts_inlet_t *this, int pos)
{
  fts_object_t *o = (fts_object_t *) this;

  /* Remove the inlet from the patcher */
  fts_inlet_remove_from_patcher(this, patcher);

  /* Change the description  */
  if (o->argv)
    fts_set_int(&(o->argv[1]), pos);
  else
    {
      fts_atom_t a[2];

      fts_set_symbol(&(a[0]), fts_s_inlet);
      fts_set_int(&(a[1]), pos);
      fts_object_set_description(o, 2, a);
    }

  this->position = pos;
  fts_inlet_add_to_patcher(this, patcher);
}


void fts_inlet_reposition(fts_object_t *o, int pos)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;

  if (pos != this->position)
    {
      fts_patcher_t  *patcher = fts_object_get_patcher(o);

      /* Add inlets if needed */
      if (pos >= fts_object_get_inlets_number((fts_object_t *)patcher))
	fts_patcher_redefine_number_of_inlets(patcher, pos + 1);

      /* set the inlet position */
      fts_inlet_set_position(patcher, this, pos);

      /* At the end, trim the number of inlets if needed; this in general avoid redefining
	 the patched twice while changing the last inlet.
       */
      fts_patcher_trim_number_of_inlets(patcher);
    }
}


/* Arguments:
   arg 1 : position,  number of inlet, mandatory.
    
   If it is -2, the inlet is not assigned (compatibility with the .pat parser),
   it will be assigned later.

   If it is -1, the inlet is assigned to the next unused number in the existing 
   inlet list.
*/

static int inlet_get_next_position(fts_patcher_t *patcher, fts_inlet_t *this)
{
  int pos;

  pos = 0;

  while (1)
    {
      int found;
      fts_object_t *p;

      found = 1;

      for (p = patcher->objects; p ; p = p->next_in_patcher)
	if (fts_object_is_inlet(p) && ((fts_inlet_t *)p) != this)
	  if (((fts_inlet_t *) p)->position == pos)
	    found = 0;

      if (found)
	return pos;
      else
	pos++;
    }
}

fts_object_t *fts_inlet_get_next_inlet(fts_object_t *inlet)
{
  return (fts_object_t *) ((fts_inlet_t *)inlet)->next;
}


static void inlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);
  int pos = fts_get_long_arg(ac, at, 1, 0);

  /* Initialize to a non valid value */
  this->position = -1;
  this->next = 0;

  if (pos == -2)
    {
      /* OFF inlets: inlet will be
	 redefined later (.pat parsing).
       */
      return;
    }
  else if (pos == -1)
    fts_inlet_reposition(o, inlet_get_next_position(patcher, this));
  else
    fts_inlet_reposition(o, pos);
}


static void inlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this   = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_inlet_remove_from_patcher(this, patcher);

  if (! patcher->deleted)
    fts_patcher_trim_number_of_inlets(patcher);
}


/* Class instantiation */

static fts_status_t inlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */
  fts_class_init(cl, sizeof(fts_inlet_t),  1, 1, 0);

  /* define the system methods */
  a[0]= fts_s_symbol;
  a[1]= fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_init,   inlet_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, inlet_delete, 0, 0);

  return fts_Success;
}


/* Class/metaclass installation  */
static void internal_inlet_config(void)
{
  fts_class_install(fts_s_inlet, inlet_instantiate);
  inlet_metaclass = fts_metaclass_get_by_name(fts_s_inlet);
}


/********************* outlet ************************/

/* Outlets, if they are assigned, send the message they receive directly
   thru the patcher outlet */
static void outlet_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_outlet_send((fts_object_t *)patcher, this->position, s, ac, at);
}


/* function to reposition an outlet, without using redefinition (bad
   for properties !!)
   */
static void fts_outlet_remove_from_patcher(fts_outlet_t *this, fts_patcher_t *patcher)
{
  /* Remove the outlet from its previous position;
     ignore the case where the outlet was not there yet
   */
  if ((this->position >= 0) && (this->position < fts_object_get_outlets_number((fts_object_t *)patcher)))
    {
      fts_outlet_t **p; /* indirect precursor */

      for (p = &(patcher->outlets[this->position]); *p ; p = &((*p)->next))
	if ((*p) == this)
	  {
	    (*p) = this->next;
	    break;
	  }
    }
}

/** This function remove the last empty inlets in a patcher */
static void fts_patcher_trim_number_of_outlets(fts_patcher_t *patcher)
{
  int new_outlets;

  new_outlets = fts_object_get_outlets_number((fts_object_t *) patcher);

  while ((new_outlets > 0) && (patcher->outlets[new_outlets - 1] == 0))
    new_outlets--;

  fts_patcher_redefine_number_of_outlets(patcher, new_outlets);
}


static void fts_outlet_add_to_patcher(fts_outlet_t *this, fts_patcher_t *patcher)
{
  if (this->position >= 0)
    {
      this->next = patcher->outlets[this->position];
      patcher->outlets[this->position] = this;
    }
}


static void fts_outlet_set_position(fts_patcher_t *patcher, fts_outlet_t *this, int pos)
{
  fts_object_t *o = (fts_object_t *) this;

  /* Remove the outlet from the patcher */
  fts_outlet_remove_from_patcher(this, patcher);

  /* Change or create the description */
  if (o->argv)
    fts_set_int(&(o->argv[1]), pos);
  else
    {
      fts_atom_t a[2];

      fts_set_symbol(&(a[0]), fts_s_outlet);
      fts_set_int(&(a[1]), pos);
      fts_object_set_description(o, 2, a);
    }

  this->position = pos;
  fts_outlet_add_to_patcher(this, patcher);
}


void fts_outlet_reposition(fts_object_t *o, int pos)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;

  if (pos != this->position)
    {
      fts_patcher_t  *patcher = fts_object_get_patcher(o);

      /* Add outlets if needed */
      if (pos >= fts_object_get_outlets_number((fts_object_t *)patcher))
	fts_patcher_redefine_number_of_outlets(patcher, pos + 1);

       /* set the outlet position */
      fts_outlet_set_position(patcher, this, pos);

      /* trim the number of outlets */
      fts_patcher_trim_number_of_outlets(patcher);
    }
}


static int outlet_get_next_position(fts_patcher_t *patcher, fts_outlet_t *this)
{
  int pos;
  pos = 0;

  while (1)
    {
      int found;
      fts_object_t *p;

      found = 1;

      for (p = patcher->objects; p ; p = p->next_in_patcher)
	if (fts_object_is_outlet(p) && ((fts_outlet_t *) p) != this)
	  if (((fts_outlet_t *) p)->position == pos)
	    found = 0;

      if (found)
	return pos;
      else
	pos++;
    }
}

fts_object_t *fts_outlet_get_next_inlet(fts_object_t *outlet)
{
  return (fts_object_t *) ((fts_outlet_t *)outlet)->next;
}


static void
outlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  int pos = fts_get_long_arg(ac, at, 1, 0);

  this->position = -1;      
  this->next = 0;      

  if (pos == -2)
    {
      /* OFF inlets: inlet will be
	 redefined later (.pat parsing).
       */
      return;
    }
  else if (pos == -1)
    fts_outlet_reposition(o, outlet_get_next_position(patcher, this));
  else
    fts_outlet_reposition(o, pos);
}

static void
outlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this   = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_outlet_remove_from_patcher(this, patcher);

  if (! patcher->deleted)
    fts_patcher_trim_number_of_outlets(patcher);
}


/* Class instantiation */
static fts_status_t
outlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */
  fts_class_init(cl, sizeof(fts_outlet_t), 1, 1, 0);

  /* define the init system method */
  a[0]= fts_s_symbol;
  a[1]= fts_s_int;
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
  fts_class_install(fts_s_outlet, outlet_instantiate);
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
  fts_inlet_t *i;

  for (i = this->inlets[winlet]; i; i = i->next)
    fts_outlet_send((fts_object_t *)i, 0, s, ac, at);
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
      fts_object_send_ui_properties(p);
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
}

/* The find engines */
static void
patcher_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);
  fts_object_t *p;
  fts_status_t ret;

  /* See if this should be added */
  if (fts_atom_is_subsequence(ac - 1, at + 1, o->argc, o->argv))
    fts_object_set_add(set, o);

  /* First, look if the objects in the patchers are to be found */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_object(p))
	{
	  if (! fts_object_is_error(p))
	    {
	      /* Send the find message to the object; 
		 if the object do not implement it,
		 do the standard check */
	      ret = fts_send_message(p, winlet, s, ac, at);
	      
	      if (ret == &fts_MethodNotFound)
	        if (fts_atom_is_subsequence(ac - 1, at + 1, p->argc, p->argv))
		  fts_object_set_add(set, p);
	    }
	  else
	    {
	      /* For error objects do the check */
	      if (fts_atom_is_subsequence(ac - 1, at + 1, p->argc, p->argv))
		fts_object_set_add(set, p);
	    }
	}
    }

  /* Then, do the recursive calls  */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_standard_patcher(p) ||
	  fts_object_is_template(p) ||
	  fts_object_is_abstraction(p))
	fts_send_message(p, winlet, s, ac, at);
    }
}

/* The find errors engines */
static void
patcher_find_errors(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_set_t *set = (fts_object_set_t *)fts_get_data(at);
  fts_object_t *p;
  fts_status_t ret;

  /* First, look if the objects in the patchers are to be found */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      /* Check if it have  the error property on */
      fts_atom_t value;

      fts_object_get_prop(p, fts_s_error, &value);
      
      if (fts_is_int(&value))
	{
	  int v = fts_get_int(&value);
	  
	  if (v)
	    fts_object_set_add(set, p);
	}
      }

  /* Last, do the recursive calls  */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_standard_patcher(p) ||
	  fts_object_is_template(p) ||
	  fts_object_is_abstraction(p))
	fts_send_message(p, winlet, s, ac, at);
    }
}


void fts_patcher_set_template(fts_patcher_t *this, fts_template_t *template)
{
  this->type     = fts_p_template;
  this->template = template;

  fts_template_add_instance(template, (fts_object_t *) this);
}


/* Methods: init put the pointers to zero */
static void patcher_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  int ninlets, noutlets;
  fts_atom_t va;
  fts_patcher_t *this = (fts_patcher_t *) o;
  
  /* allocate the data */
  this->data = fts_patcher_data_new(this);

  fts_env_init(&(this->env), (fts_object_t *) this);
  fts_patcher_set_standard(this);

  /* Define the "args" variable */
  this->args = (fts_data_t *)fts_atom_array_new(ac - 1, at + 1);

  fts_variable_define(this, fts_s_args);
  fts_set_atom_array(&va, this->args);
  fts_variable_restore(this, fts_s_args, &va, o);

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
  this->deleted = 0;
}

/* Contained objects are deleted in the reverse order (last first);
   it purpose is to reduce the number of redefinitions happening
   and errors objects instantiated during a delete; locally to a patcher,
   objects using variables come after objects defining the same variable,
   so by deleting in the reverse order, we avoid redefinition within the
   patchers; of course, if a variable is used somewhere else,
   the user may be recomputed during delete.

   Also, since the list is can be indirectly reordered by a recompute
   (i.e. a deletion), the last member is recomputed at each
   interaction; slow, but the only alternative is to use a dynamic array
   instead of a list. We will do it if it is a problem.
   */

static void 
patcher_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;

  /* If the data is still there, delete it */
  if (this->data)
    fts_patcher_data_free(this->data);

  /* If it is a template, remove it from the template instance list */
  if (fts_patcher_is_template(this))
    fts_template_remove_instance(this->template, (fts_object_t *) this);

  /* Set the deleted and reset the open flag */
  this->deleted = 1;
  this->open    = 0;

  /*
   * delete its content; each destroied object will take away himself
   * from the list, including inlets and outlets; this will undo the
   * connections also to the inlets and outlets.
   */

  while (this->objects)
    {
      /* find last */
      p = this->objects;

      while (p->next_in_patcher)
	p = p->next_in_patcher;

      /* Delete it */
      fts_object_delete(p);

      /* Restart all over again; slow
	 but needed, see comment before the function */
    }

  /* Delete all the variables */
  fts_atom_array_delete((fts_atom_array_t *)this->args);
  fts_variables_undefine(this, (fts_object_t *)this);

  /* delete the inlets and inlets tables */
  if (this->inlets)
    fts_block_free((char *) this->inlets, sizeof(fts_inlet_t *) * fts_object_get_inlets_number((fts_object_t *) this));

  if (this->outlets)
    fts_block_free((char *) this->outlets, sizeof(fts_outlet_t *)*fts_object_get_outlets_number((fts_object_t *) this));
}

/*
  Function: patcher_open_and_save_dotpat
  Description:
    Saves a patcher in .pat format
  Arguments:
    at[0]  filename         the name of the destination file
  Note:
    This method does not check for nor add a .pat suffix to the file name.
    This is left to the user interface.
*/
static void patcher_save_dotpat_content( fts_patcher_t *patcher, FILE *file);

static void patcher_save_dotpat_file(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;

  post( "About to save %s\n", fts_symbol_name( fts_get_symbol( at)));

  file = fopen( fts_symbol_name( fts_get_symbol( at)), "w");

  if ( file != NULL)
    {
      fprintf( file, "max v2;\n");

      patcher_save_dotpat_content( (fts_patcher_t *)o, file);

      fclose( file);
    }
  else
    {
      /* This should be a dialog opened in the user interface */
      post( "Error: cannot open %s for saving\n", fts_symbol_name( fts_get_symbol(at)) );
    }
}

static void patcher_save_dotpat_description( fts_object_t *object, FILE *file, const char *command)
{
  int x, y, w, font_index;
  fts_atom_t a;
  int i;

  fts_object_get_prop( object, fts_s_x, &a);
  x = fts_get_int( &a);
  fts_object_get_prop( object, fts_s_y, &a);
  y = fts_get_int( &a);
  fts_object_get_prop( object, fts_s_width, &a);
  w = fts_get_int( &a);
  font_index = 1;

  fprintf( file, "#P %s %d %d %d %d ", command, x, y, w, font_index);

  for ( i = 0; i < object->argc; i++)
    {
      fts_atom_t *a;

      a = object->argv + i;

      if (fts_is_int(a))
	fprintf( file, "%d", fts_get_int( a));
      else if (fts_is_float(a))
	fprintf( file, "%f", fts_get_float( a));
      else if (fts_is_symbol(a))
	fprintf( file, "%s", fts_symbol_name( fts_get_symbol( a)) );

      if ( i < object->argc - 1)
	fprintf( file, " ");
    }

  fprintf( file, ";\n");
}

static void patcher_save_dotpat_content( fts_patcher_t *patcher, FILE *file)
{
  int x_left, y_top, x_right, y_bottom;
  fts_atom_t a;
  fts_object_t *object;

  fts_object_get_prop( (fts_object_t *)patcher, fts_s_wx, &a);
  x_left = fts_get_int( &a);
  fts_object_get_prop( (fts_object_t *)patcher, fts_s_wy, &a);
  y_top = fts_get_int( &a);
  fts_object_get_prop( (fts_object_t *)patcher, fts_s_ww, &a);
  x_right = x_left + fts_get_int( &a);
  fts_object_get_prop( (fts_object_t *)patcher, fts_s_wh, &a);
  y_bottom = y_top + fts_get_int( &a);

  /* Save window properties */
  fprintf( file, "#N vpatcher %d %d %d %d;\n", x_left, y_top, x_right, y_bottom);

  /* Save objects */
  for ( object = patcher->objects; object ; object = object->next_in_patcher)
    {
      if ( fts_object_handle_message( object, fts_SystemInlet, fts_new_symbol( "save_dotpat")))
	{
 	  fts_atom_t a;

	  fts_set_ptr( &a, file);
	  fts_message_send( object, fts_SystemInlet, fts_new_symbol( "save_dotpat"), 1, &a);
	}
      else
	patcher_save_dotpat_description( object, file, "newex");
    }

  /* Save connections */

  fprintf( file, "#P pop;\n");
}

static void patcher_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  fts_patcher_t *patcher;

  patcher = (fts_patcher_t *)o;
  file = (FILE *)fts_get_ptr( at);

  if ( ! fts_patcher_is_abstraction( patcher) && ! fts_patcher_is_template( patcher) )
    patcher_save_dotpat_description( o, file, "newex");
  else
    {
      patcher_save_dotpat_content( (fts_patcher_t *)o, file);

      patcher_save_dotpat_description( o, file, "newobj");
    }
}


static void patcher_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_patcher_type);
}


/* Daemon to get the data property; the data property of a patcher is itself */
static void patcher_get_data(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_atom_t a;
  fts_patcher_t *this = (fts_patcher_t *) obj;

  fts_object_get_prop( obj, fts_new_symbol( "no_upload"), &a);

  if ( fts_is_int( &a) && fts_get_int( &a))
    fts_set_void( value);
  else if (! fts_patcher_is_error(this))
    fts_set_data(value, (fts_data_t *) (this->data));
  else
    fts_set_void(value);
}


/* Daemon to get the patcher_type property */
static void patcher_get_patcher_type(fts_daemon_action_t action, fts_object_t *obj,
				     fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_t *this = (fts_patcher_t *) obj;


  if (fts_patcher_is_abstraction(this))
    fts_set_symbol(value, fts_s_abstraction);
  else if (fts_patcher_is_template(this))
    fts_set_symbol(value, fts_s_template);
  else if (fts_patcher_is_error(this))
    fts_set_void(value);
  else
    fts_set_symbol(value, fts_s_patcher);
}

/* Daemon for getting the property "state". */
static void
patcher_get_state(fts_daemon_action_t action, fts_object_t *obj,
		  fts_symbol_t property, fts_atom_t *value)
{
  fts_set_object(value, obj);
}

/* Daemon for setting the number of inlets */
static void
patcher_set_ninlets(fts_daemon_action_t action, fts_object_t *obj,
		    fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_redefine_number_of_inlets((fts_patcher_t *)obj, fts_get_int(value));
}

/* Daemon for setting the number of outlets */
static void
patcher_set_noutlets(fts_daemon_action_t action, fts_object_t *obj,
		     fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_redefine_number_of_outlets((fts_patcher_t *)obj, fts_get_int(value));
}

/* Class instantiation */
static fts_status_t
patcher_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int ninlets;
  int noutlets;
  int i;
  fts_type_t t[1];

  ninlets  = fts_get_long_arg(ac, at, 1, 0);
  noutlets = fts_get_long_arg(ac, at, 2, 0);

  /* initialize the class */
  fts_class_init(cl, sizeof(fts_patcher_t), ninlets, noutlets, 0);

  /* define the init system method */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, patcher_init);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, patcher_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find, patcher_find);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_errors, patcher_find_errors);
  fts_method_define(cl, fts_SystemInlet, fts_s_send_properties, patcher_send_properties, 0, 0); 

  for (i = 0; i < ninlets; i ++)
    fts_method_define_varargs(cl, i, fts_s_anything, patcher_anything);

  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("load_init"), patcher_load_init, 0, 0); 
  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("open"), patcher_open, 0, 0); 
  fts_method_define(cl,fts_SystemInlet, fts_new_symbol("close"), patcher_close, 0, 0); 

  t[0] = fts_t_symbol;
  fts_method_define( cl, fts_SystemInlet, fts_new_symbol("save_dotpat_file"), patcher_save_dotpat_file, 1, t); 
  t[0] = fts_t_ptr;
  fts_method_define( cl, fts_SystemInlet, fts_new_symbol("save_dotpat"), patcher_save_dotpat, 1, t); 

  /* daemon for properties */
  fts_class_add_daemon(cl, obj_property_get, fts_s_data, patcher_get_data);
  fts_class_add_daemon(cl, obj_property_get, fts_s_patcher_type, patcher_get_patcher_type);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, patcher_get_state);

  fts_class_add_daemon(cl, obj_property_put, fts_s_ninlets, patcher_set_ninlets);
  fts_class_add_daemon(cl, obj_property_put, fts_s_noutlets, patcher_set_noutlets);

  return fts_Success;
}


/* 
 * Redefine a patcher inplace from a new description in term of "object" description.
 * 
 * Actually handle the variables; here ins and outs cannot be changed
 * (for patchers, they do not depend on the description).
 *
 */

/* Utility functions to assign variables to a patcher as found by the expression parser */
void fts_patcher_assign_variable(fts_symbol_t name, fts_atom_t *value, void *data)
{
  fts_patcher_t *this = (fts_patcher_t *)data;

  if (! fts_variable_is_suspended(this, name))
    fts_variable_define(this, name);

  fts_variable_restore(this, name, value, (fts_object_t *)this);
}


/* This function get a new full description of the patcher, i.e. [<varname> : ] jpatcher args */
static fts_symbol_t fts_patcher_make_error_msg(const char *format, ...)
{
  va_list ap;
  char buf[1024];

  va_start(ap, format);
  vsprintf(buf, format, ap);
  va_end(ap);

  return fts_new_symbol_copy(buf);
}


fts_patcher_t *fts_patcher_redefine(fts_patcher_t *this, int aoc, const fts_atom_t *aot)
{
  fts_object_t *obj;
  fts_expression_state_t *e;
  fts_symbol_t var = 0;
  int ac;
  fts_atom_t at[1024];
  int rac;
  const fts_atom_t *rat;
  fts_atom_t a;

  obj = (fts_object_t *) this; 

  /* change the patcher definition */
  fts_object_set_description(obj, aoc, aot);

  /* check for the "var : <obj> syntax" and ignore the variable if any */
  if (fts_object_description_defines_variable(aoc, aot))
    {
      /* var = fts_get_symbol(&aot[0]); */
      rat = aot + 2;
      rac = aoc - 2;
    }
  else
    {
      rat = aot;
      rac = aoc;
    }

  /* change the patcher definition */
  fts_object_set_description(obj, rac, rat);

  /* if the old patcherr define a variable, and the new definition
     do not define  the same variable, delete the variable;
     if the new object define the same variable,  just suspend it.
     */
  if (var)
    {
      if (obj->varname == var)
	{
	  fts_variable_suspend(obj->patcher, obj->varname);
	  fts_variable_undefine(obj->patcher, obj->varname, obj); /* ??/ */
	}
      else
	{
	  /* If the variable already exists in this local context, make an double definition patcher  */
	  if (! fts_variable_can_define(obj->patcher, var))
	    {
	      fts_variable_define(obj->patcher, var);

	      fts_variables_undefine_suspended(this, obj);

	      fts_set_int(&a, 1);
	      fts_object_put_prop(obj, fts_s_error, &a);

	      fts_set_symbol(&a, fts_patcher_make_error_msg("Variable %s already defined", var));
	      fts_object_put_prop(obj, fts_s_error_description, &a);

	      if (fts_object_has_id(obj))
		{
		  fts_object_property_changed(obj, fts_s_error);
		  fts_object_property_changed(obj, fts_s_error_description);
		}

	      return this;
	    }
	  else if ( ! fts_variable_is_suspended(obj->patcher, var))
	    {
	      /* Define the variable, suspended;
		 this will also steal all the objects referring to the same variable name
		 in the local scope from any variable defined outside the scope */
	      fts_variable_define(obj->patcher, var);
	    }
	}
    }
  else if (obj->varname)
    {
      fts_variable_undefine(obj->patcher, obj->varname, obj);
      obj->varname = 0;
    }

  /* suspend  the patcher internal variables if any */
  fts_variables_suspend(this, obj);

  /* eval the expression */
  e = fts_expression_eval(obj->patcher, rac, rat, 1024, at);
  ac = fts_expression_get_result_count(e);

  if (fts_expression_get_status(e) != FTS_EXPRESSION_OK)
    {
      /* undefine all the variables, and set the error property */
      fts_variables_undefine_suspended(this, obj);

      fts_set_int(&a, 1);
      fts_object_put_prop(obj, fts_s_error, &a);

      fts_set_symbol(&a, fts_patcher_make_error_msg(fts_expression_get_msg(e), fts_expression_get_err_arg(e)));
      fts_object_put_prop(obj, fts_s_error_description, &a);
    }
  else
    {
      /* Set the error property to zero */
      fts_set_int(&a, 0);
      fts_object_put_prop(obj, fts_s_error, &a);

      /* reallocate the atom array */
      fts_atom_array_delete((fts_atom_array_t *)this->args);

      this->args = (fts_data_t *)fts_atom_array_new(ac - 1, at + 1);

      /* set the new variables */
      fts_expression_map_to_assignements(e, fts_patcher_assign_variable, (void *) this);

      fts_set_atom_array(&a, this->args);
      fts_variable_restore(this, fts_s_args, &a, obj);

      /* register the patcher as user of the used variables */
      fts_expression_add_variables_user(e, obj);

      /* undefine all the locals that are still suspended  */
      fts_variables_undefine_suspended(this, obj);
    }

  /* Recover the variable this patcher define, if any */
  if (var != 0)
    {
      fts_set_object(&a, obj);

      fts_variable_restore(obj->patcher, var, &a, obj);
      obj->varname = var;
    }

  /* Free the expression state structure */
  fts_expression_state_free(e);

  /* Inform the UI that the name is probabily changed (the type cannot change);
     and the error property too. */

  if (fts_object_has_id(obj))
    {
      fts_object_property_changed(obj, fts_s_error);
      fts_object_property_changed(obj, fts_s_error_description);
    }

  return this;
}

/*
 * Redefine a patcher "inplace" by changing directly the inlets and outlets
 * and *the class* of the object.
 * To do this, it heavily rely on the internals of the message system; the patcher
 * is a primitive special class for the message system.
 * 
 */
void fts_patcher_redefine_number_of_inlets(fts_patcher_t *this, int new_ninlets)
{
  fts_object_t *obj_this = (fts_object_t *)this;
  int old_ninlets;

  old_ninlets = fts_object_get_inlets_number((fts_object_t *) this);

  if (old_ninlets == new_ninlets)
    return;

  /* Delete all the connections that will not be pertinent any more */
  fts_object_trim_inlets_connections(obj_this, new_ninlets);

  /* Reallocate and copy the patcher inlets, incoming connections and inlets properties if needed */
  if (new_ninlets == 0)
    {
      /* No new inlets, but (since the != test is true) there
	 are old inlets to delete */

      fts_block_free((char *)this->inlets, old_ninlets * sizeof(fts_inlet_t *));
      fts_block_free((char *)obj_this->in_conn, old_ninlets * sizeof(fts_connection_t *));

      this->inlets = 0;
      obj_this->in_conn = 0;
    }
  else  if (old_ninlets > 0)
    {
      /* There are new inlets: and there are  old inlets, so reallocate and move */
      int i;
      fts_inlet_t  **new_inlets;
      fts_connection_t **new_in_conn;

      new_inlets  = (fts_inlet_t **)  fts_block_alloc(new_ninlets * sizeof(fts_inlet_t *));
      new_in_conn = (fts_connection_t **) fts_block_zalloc(new_ninlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_ninlets; i++)
	{
	  if (i < old_ninlets)
	    {
	      new_inlets[i] = this->inlets[i];
	      new_in_conn[i] = obj_this->in_conn[i];
	    }
	  else
	    {
	      new_inlets[i] = 0;
	      new_in_conn[i] = 0;
	    }
	}

      fts_block_free((char *)this->inlets, old_ninlets * sizeof(fts_inlet_t *));
      fts_block_free((char *)obj_this->in_conn, old_ninlets * sizeof(fts_connection_t *));
	      
      this->inlets = new_inlets;
      obj_this->in_conn = new_in_conn;
    }
  else 
    {
      int i;
      
      /* There are new inlets, but there were no inlets before, so just allocate without copying old stuff */
      this->inlets = (fts_inlet_t **) fts_block_alloc(new_ninlets * sizeof(fts_inlet_t *));
      obj_this->in_conn = (fts_connection_t **) fts_block_zalloc(new_ninlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_ninlets; i++)
	{
	  this->inlets[i] = 0;
	  obj_this->in_conn[i] = 0;
	}
    }

  /*
   *  change the patcher class (of course, not the metaclass).
   */
  {
    fts_atom_t a[3];

    fts_set_symbol(&a[0], fts_s_patcher);
    fts_set_int(&a[1], new_ninlets);
    fts_set_int(&a[2], fts_object_get_outlets_number((fts_object_t *) this));
    obj_this->head.cl = fts_class_instantiate(3, a);
  }

  {
    fts_object_t *p;

    /* Reinstall the inlets */
    for (p = this->objects; p ; p = p->next_in_patcher)
      if (fts_object_is_inlet(p))
	{
	  /* We check the number, because during object deleting 
	     we may find the object we are deleting in the list */
	  fts_inlet_t *inlet = (fts_inlet_t *)p;

	  if (inlet->position < new_ninlets)
	    fts_inlet_set_position(this, inlet, inlet->position);
	}
  }


  if (fts_object_has_id((fts_object_t *)this))
    fts_object_property_changed((fts_object_t *)this, fts_s_ninlets);
}

void fts_patcher_redefine_number_of_outlets(fts_patcher_t *this, int new_noutlets)
{
  fts_object_t *obj_this = (fts_object_t *)this;
  int old_noutlets;

  old_noutlets = fts_object_get_outlets_number((fts_object_t *) this);

  if (old_noutlets == new_noutlets)
    return;

  /* Delete all the connections that will not be pertinent any more */
  fts_object_trim_outlets_connections(obj_this, new_noutlets);

  /* Reallocate and copy the patcher outlets, incoming connections and outlets properties if needed */
  if (new_noutlets == 0)
    {
      /* No new outlets, but there
	 are old outlets to delete */
      fts_block_free((char *)this->outlets, old_noutlets * sizeof(fts_outlet_t *));
      fts_block_free((char *)obj_this->out_conn, old_noutlets * sizeof(fts_connection_t *));

      this->outlets = 0;
      obj_this->out_conn = 0;
    }
  else  if (old_noutlets > 0)
    {
      /* There are new outlets: and there are no old outlets, so reallocate and move */
      int i;
      fts_outlet_t  **new_outlets;
      fts_connection_t **new_out_conn;

      new_outlets  = (fts_outlet_t **)  fts_block_alloc(new_noutlets * sizeof(fts_outlet_t *));
      new_out_conn = (fts_connection_t **) fts_block_zalloc(new_noutlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_noutlets; i++)
	{
	  new_outlets[i] = 0;

	  if (i < old_noutlets)
	    new_out_conn[i] = obj_this->out_conn[i];
	  else
	    new_out_conn[i] = 0;
	}

      fts_block_free((char *)this->outlets, old_noutlets * sizeof(fts_outlet_t *));
      fts_block_free((char *)obj_this->out_conn, old_noutlets * sizeof(fts_connection_t *));
	      
      this->outlets = new_outlets;
      obj_this->out_conn = new_out_conn;
    }
  else 
    {
      int i;

      /* There are new outlets, but there were no outlets before, so just allocate without
	 copying old stuff */
      this->outlets = (fts_outlet_t **) fts_block_alloc(new_noutlets * sizeof(fts_outlet_t *));
      obj_this->out_conn = (fts_connection_t **) fts_block_zalloc(new_noutlets*sizeof(fts_connection_t *));

      for (i = 0; i < new_noutlets; i++)
	{
	  this->outlets[i] = 0;
	  obj_this->out_conn[i] = 0;
	}
    }

  /*
   *  change the patcher class (of course, not the metaclass).
   */
  {
    fts_atom_t a[3];

    fts_set_symbol(&a[0], fts_s_patcher);
    fts_set_int(&a[1], fts_object_get_inlets_number((fts_object_t *) this));
    fts_set_int(&a[2], new_noutlets);
    obj_this->head.cl = fts_class_instantiate(3, a);
  }

  {
    fts_object_t *p;

    /* Reinstall the outlets */
    for (p = this->objects; p ; p = p->next_in_patcher)
      if (fts_object_is_outlet(p))
	{
	  /* We check the number, because during object deleting 
	     we may find the object we are deleting in the list
	     */

	  fts_outlet_t *outlet = (fts_outlet_t *)p;

	  if (outlet->position < new_noutlets)
	    fts_outlet_set_position(this, outlet, outlet->position);
	}
  }

  if (fts_object_has_id((fts_object_t *)this))
    fts_object_property_changed((fts_object_t *)this, fts_s_noutlets);
}

/* Functions for object management; register and remove  an object in a patcher.
   An object is always added to the end; this is to reduce (or suppress) the
   number of redefinitions at loading time (by definition, a non error object
   is defined after all the variables it define, so it will be *after* the
   variables in the list).
 */
void fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj)
{
  fts_object_t **p; /* indirect precursor */

  obj->patcher = this;

  for (p = &(this->objects); *p; p = &((*p)->next_in_patcher))
    {
    }

  *p = obj;
}

void fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj)
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

int fts_patcher_get_objects_count(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;	

  for (p = this->objects; p; p = p->next_in_patcher)
    i++;

  return i;
}


/* Functions for direct .pat loading support */
static int fts_patcher_count_inlet_objects(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_inlet(p))
      i++;

  return i;
}

static int fts_patcher_count_outlet_objects(fts_patcher_t *this)
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
 * This functions take all the declared inlets and outlets
 * and assign them to the patcher in the correct order (based on the x position),
 * setting also the inlet and outlet status.
 * It assume that the patcher have no inlets and outlets,
 * allocate the new ones, and *change* the class of the patcher,
 * using the fts_patcher_redefine function.
 * 
 * It use the  *let_set_position functions to avoid redefining the nins/nouts
 * of the patcher during parsing.
 *
 * Note that the following code work because .pat do not have
 * multiple inlet/outlet objects connected to the same inlet.
 */

void fts_patcher_reassign_inlets_outlets(fts_patcher_t *this)
{
  fts_object_t *p;
  int i, j;
  int ninlets;
  int noutlets;

  /* Compute number of inlets and outlets */
  ninlets = fts_patcher_count_inlet_objects(this);
  noutlets = fts_patcher_count_outlet_objects(this);

  /* make a redefine the old patcher */
  fts_patcher_redefine_number_of_inlets(this,  ninlets);
  fts_patcher_redefine_number_of_outlets(this, noutlets);

  /* Store the inlets in the inlet arrays; not so redundant as may seems,
     because patcher_redefine do not reposition inlets with 
     no position.
   */
  i = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_inlet(p))
      {
	this->inlets[i] = (fts_inlet_t *) p;
	i++;
      }

  /* Store the outlets in the oulet arrays */
  i = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_outlet(p))
      {
	this->outlets[i] = (fts_outlet_t *) p;
	i++;
      }

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

  /* First, update the position field of all the inlets,
     then reposition them; the position field must be updated
     for all the inlets before starting to reposition them,
     because is used by the reposition function to remove
     the inlet first.
     */

  for (i = 0; i < ninlets; i++)
    this->inlets[i]->position = i; 

  /* redefine all the inlets */
  for (i = 0; i < ninlets; i++)
    fts_inlet_set_position(this, this->inlets[i], i);

  /* Same things for the outlets */
  for (i = 0; i < noutlets; i++)
    this->outlets[i]->position = i; 

  /* redefine all the outlets */
  for (i = 0; i < noutlets; i++)
    fts_outlet_set_position(this, this->outlets[i], i);
}

/* accessing inlets and outlets in patcher */
fts_object_t *fts_patcher_get_inlet(fts_object_t *patcher, int inlet)
{
  fts_patcher_t *this = (fts_patcher_t *) patcher;

  if (inlet < fts_object_get_inlets_number(this))
    return (fts_object_t *) this->inlets[inlet];
  else
    return 0;
}

fts_object_t *fts_patcher_get_outlet(fts_object_t *patcher, int outlet)
{
  fts_patcher_t *this = (fts_patcher_t *) patcher;

  if (outlet < fts_object_get_outlets_number(this))
    return (fts_object_t *) this->outlets[outlet];
  else
    return 0;
}


void fts_patcher_blip(fts_patcher_t *this, const char *msg)
{
  if (fts_patcher_is_open(this))
    {
      if (this->data)
	fts_patcher_data_blip((fts_data_t *)this->data, msg);
    }
  else if (fts_object_get_patcher((fts_object_t *)this))
    {
      fts_patcher_blip(fts_object_get_patcher((fts_object_t *)this), msg);
    }
}

/* Class/metaclass installation  */
static void internal_patcher_config(void)
{
  fts_metaclass_install(fts_s_patcher, patcher_instantiate, fts_arg_equiv);
  patcher_metaclass = fts_metaclass_get_by_name(fts_s_patcher);
}


/* The root patcher: it created here, and accessed thru the function
   fts_get_root_patcher(); the root patcher have by definition an ID 
   of 1 !!!
   */
static fts_patcher_t *fts_root_patcher;

void fts_create_root_patcher()
{
  fts_atom_t description[2];
  fts_object_t *patcher;

  fts_set_symbol(&description[0], fts_s_patcher);
  fts_set_symbol(&description[1], fts_new_symbol("root"));

  fts_root_patcher = (fts_patcher_t *) fts_eval_object_description((fts_patcher_t *)0, 2, description);

  fts_object_set_id((fts_object_t *)fts_root_patcher, 1);
}

static void fts_delete_root_patcher(void)
{
  fts_object_delete((fts_object_t *) fts_root_patcher);
}

fts_patcher_t *fts_get_root_patcher(void)
{
  return fts_root_patcher;
}

/* Converted/Doctor for the old patcher format */
static fts_object_t *patcher_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_atom_t a[2];
  fts_object_t *obj;

  fts_set_symbol(&a[0], fts_s_patcher);

  if (ac >= 2)
    {
      a[1] = at[1];
      fts_object_new(patcher, 2, a, &obj);
      fts_object_set_description(obj, 2, a);

      if (ac >= 3)
	fts_object_put_prop(obj, fts_s_ninlets, &at[2]);

      if (ac >= 4)
	fts_object_put_prop(obj, fts_s_noutlets, &at[3]);
    }
  else
    fts_object_new(patcher, 1, at , &obj);

  return obj;
}


/* Global configuration function; it call the configuration 
   function of each class */
void fts_patcher_init(void)
{
  fts_register_object_doctor(fts_new_symbol("patcher"), patcher_doctor);
  internal_patcher_config();
  internal_inlet_config();
  internal_outlet_config();
}


void
fts_patcher_shutdown(void)
{
  fts_delete_root_patcher();
}




