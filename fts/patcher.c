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
 */

/**************************************************************************
 *
 *  THE REAL FTS 2.0 patcher class
 *
 *  The patcher class is a standard class, with real inlets and real
 *  outlets; it keeps track of its content; inlets and outlets objects
 *  have special and direct relationship with the patcher, that use them
 *  as place holder for the connections.
 *
 *  The patcher class do not have variable number of inlet/outlet;
 *  if the editor add a inlet or outlet, it replace the patcher object
 *  with a new one; the patcher have, for now, two arguments, the number
 *  of inputs and the number of outputs; in the future, it may handles the
 *  types from the inputs.
 *
 *  Inlets are only placeholders for the internal connections
 *  they receive no messages, send no messages; the messages are sent
 *  by the patcher object on its outlet; for the DSP, they are like through.
 *
 *  Housekeeping of the patcher structure is integrated in the message system;
 *  the patcher is not a standalone object anymore.
 *
 *  For now, until Input/output typing, all the inlets and outlets are
 *  DSP, and the patcher is integrated with the DSP compiler.
 *
 *  (the object structures are defined in mess_sys.h)
 *
 */

#include <stdarg.h>
#include <stdio.h>

#include <fts/fts.h>
#include <ftsprivate/client.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/expression.h>
#include <ftsprivate/loader.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/bmaxhdr.h>
#include <ftsprivate/saver.h>
#include <ftsprivate/template.h>
#include <ftsprivate/variable.h>
#include <ftsprivate/label.h>

fts_metaclass_t *patcher_metaclass = 0;
fts_class_t *inlet_class = 0;
fts_class_t *outlet_class = 0;

fts_symbol_t sym_showObject = 0;
fts_symbol_t sym_stopWaiting = 0;
fts_symbol_t sym_setDirty = 0;

fts_symbol_t sym_redefineStart = 0;
fts_symbol_t sym_setWX = 0;
fts_symbol_t sym_setWY = 0;
fts_symbol_t sym_setWW = 0;
fts_symbol_t sym_setWH = 0;
fts_symbol_t sym_setPatcherBounds = 0;
fts_symbol_t sym_addObject = 0;
fts_symbol_t sym_addConnection = 0;
fts_symbol_t sym_redefineConnection = 0;
fts_symbol_t sym_releaseConnection = 0;
fts_symbol_t sym_redefineObject = 0;
fts_symbol_t sym_objectRedefined = 0;
fts_symbol_t sym_setRedefined = 0;
fts_symbol_t sym_blip = 0;
fts_symbol_t sym_setDescription;
fts_symbol_t sym_setSaved;
fts_symbol_t sym_endUpload;
fts_symbol_t sym_startPaste;
fts_symbol_t sym_endPaste;
fts_symbol_t sym_noHelp;

#define set_editor_open(q) ((q)->editor_open = 1)
#define set_editor_close(q) ((q)->editor_open = 0)
#define editor_is_open(q) ((q)->editor_open != 0)
#define fts_patcher_is_dirty(q)    ((q)->dirty != 0)

static void
printf_mess(const char *msg, int ac, const fts_atom_t *av)
{
  fprintf(stderr, "%s (%d args): ", msg, ac);
  /*fprintf_atoms(stderr, ac, av);*/
  fprintf(stderr, "\n");
}

extern fts_status_t receive_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);
extern fts_status_t send_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);

/*************************************************************
 *
 *  inlet class
 *
 *  the inlet has one mandatory argument, the position (inlet number):
 *    -2: the inlet is not assigned (compatibility with the .pat parser) it will be assigned later.
 *    -1: the inlet is assigned to the next unused number in the existing inlet list.
 */

static int 
inlet_get_next_position(fts_patcher_t *patcher, fts_inlet_t *this)
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

static void 
inlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  int pos = fts_get_int_arg(ac, at, 1, -1);

  /* Initialize to a non valid value */
  this->position = -1;
  this->next = 0;

  if (pos == -2)
    {
      /* OFF inlets: inlet will be redefined later (.pat parsing) */
      return;
    }
  else if (pos < 0)
    fts_patcher_inlet_reposition(o, inlet_get_next_position(patcher, this));
  else
    fts_patcher_inlet_reposition(o, pos);
}

static void 
inlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_inlet_t *this   = (fts_inlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_patcher_remove_inlet(patcher, this);

  if (! patcher->deleted)
    fts_patcher_trim_number_of_inlets(patcher);
}

static void 
inlet_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  fts_atom_t xa, ya, wa;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);

  fprintf( file, "#P inlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
}

static fts_status_t
inlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 0 || fts_is_int(at))
    {
      /* initialize the class */
      fts_class_init(cl, sizeof(fts_inlet_t), 1, 1, 0);
      
      /* define the system methods */
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, inlet_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, inlet_delete);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_save_dotpat, inlet_save_dotpat); 
      
      fts_class_define_thru(cl, 0);
      
      inlet_class = cl;

      return fts_Success;
    }
  else if(ac == 1 && (fts_is_symbol(at) || fts_is_label(at)))
    return receive_instantiate(cl, ac, at);
  else
    return &fts_CannotInstantiate;
}

/*************************************************************
 *
 *  outlet class
 *
 */

/* if outlets are assigned, send the message they receive directly thru the patcher outlet */
static void 
outlet_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_outlet_send((fts_object_t *)patcher, this->position, s, ac, at);
}

static int 
outlet_get_next_position(fts_patcher_t *patcher, fts_outlet_t *this)
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

static void
outlet_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);
  int pos = fts_get_int_arg(ac, at, 1, -1);

  this->position = -1;      
  this->next = 0;      

  if (pos == -2)
    {
      /* OFF inlets: inlet will be redefined later (.pat parsing) */
      return;
    }
  else if (pos < 0)
    fts_patcher_outlet_reposition(o, outlet_get_next_position(patcher, this));
  else
    fts_patcher_outlet_reposition(o, pos);
}

static void
outlet_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this   = (fts_outlet_t *) o;
  fts_patcher_t  *patcher = fts_object_get_patcher(o);

  fts_patcher_remove_outlet(patcher, this);

  if (! patcher->deleted)
    fts_patcher_trim_number_of_outlets(patcher);
}

static void 
outlet_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *file;
  fts_atom_t xa, ya, wa;

  file = (FILE *)fts_get_pointer( at);

  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);

  fprintf( file, "#P outlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
}

static void
outlet_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_t *this  = (fts_outlet_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  fts_patcher_t *patcher;

  patcher = fts_object_get_patcher(this);

  propagate_fun(propagate_context, (fts_object_t *)patcher, this->position);
}

static fts_status_t 
outlet_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 0 || fts_is_int(at))
    {
      fts_class_init(cl, sizeof(fts_outlet_t), 1, 1, 0);
      
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, outlet_init);
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, outlet_delete);
      
      fts_method_define_varargs( cl, fts_SystemInlet, fts_s_save_dotpat, outlet_save_dotpat); 
      
      fts_class_define_thru(cl, outlet_propagate_input);
      
      fts_method_define_varargs(cl, 0, fts_s_anything, outlet_anything);
      
      outlet_class = cl;
      
      return fts_Success;
    }
  else if(ac == 1 && (fts_is_symbol(at) || fts_is_label(at)))
    return send_instantiate(cl, ac, at);
  else
    return &fts_CannotInstantiate;
}

/*************************************************************
 *
 *  inlet/outlet house keeping
 *
 */

void 
fts_patcher_add_inlet(fts_patcher_t *patcher, fts_inlet_t *this)
{
  if (this->position >= 0)
    {
      this->next = patcher->inlets[this->position];
      patcher->inlets[this->position] = this;
    }
}

void 
fts_patcher_add_outlet(fts_patcher_t *patcher, fts_outlet_t *this)
{
  if (this->position >= 0)
    {
      this->next = patcher->outlets[this->position];
      patcher->outlets[this->position] = this;
    }
}

void 
fts_patcher_remove_inlet(fts_patcher_t *patcher, fts_inlet_t *this)
{
  /* remove the inlet from its previous position (ignore the case where the inlet was not there yet) */
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

void 
fts_patcher_remove_outlet(fts_patcher_t *patcher, fts_outlet_t *this)
{
  /* remove the outlet from its previous position (ignore the case where the outlet was not there yet) */
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

static void 
patcher_position_inlet(fts_patcher_t *patcher, fts_inlet_t *this, int pos)
{
  fts_object_t *o = (fts_object_t *) this;

  /* Remove the inlet from the patcher */
  fts_patcher_remove_inlet(patcher, this);

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
  fts_patcher_add_inlet(patcher, this);
}

static void 
patcher_position_outlet(fts_patcher_t *patcher, fts_outlet_t *this, int pos)
{
  fts_object_t *o = (fts_object_t *)this;

  /* Remove the outlet from the patcher */
  fts_patcher_remove_outlet(patcher, this);

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
  fts_patcher_add_outlet(patcher, this);
}

/* remove the last empty inlets in a patcher */
void 
fts_patcher_trim_number_of_inlets(fts_patcher_t *patcher)
{
  int new_inlets;

  new_inlets = fts_object_get_inlets_number((fts_object_t *) patcher);
     
  while ((new_inlets > 0) && (patcher->inlets[new_inlets - 1] == 0))
    new_inlets--;

  fts_patcher_redefine_number_of_inlets(patcher, new_inlets);
}

/* remove the last empty inlets in a patcher */
void 
fts_patcher_trim_number_of_outlets(fts_patcher_t *patcher)
{
  int new_outlets;

  new_outlets = fts_object_get_outlets_number((fts_object_t *) patcher);

  while ((new_outlets > 0) && (patcher->outlets[new_outlets - 1] == 0))
    new_outlets--;

  fts_patcher_redefine_number_of_outlets(patcher, new_outlets);
}

void 
fts_patcher_inlet_reposition(fts_object_t *o, int pos)
{
  fts_inlet_t *this  = (fts_inlet_t *) o;

  if (pos != this->position)
    {
      fts_patcher_t  *patcher = fts_object_get_patcher(o);

      /* add inlets if needed */
      if (pos >= fts_object_get_inlets_number((fts_object_t *)patcher))
	fts_patcher_redefine_number_of_inlets(patcher, pos + 1);

      /* set the inlet position */
      patcher_position_inlet(patcher, this, pos);

      /* trim the number of inlets if needed 
       * (this in general avoid redefining the patched twice while changing the last inlet) 
       */
      fts_patcher_trim_number_of_inlets(patcher);
    }
}

void 
fts_patcher_outlet_reposition(fts_object_t *o, int pos)
{
  fts_outlet_t *this  = (fts_outlet_t *) o;

  if (pos != this->position)
    {
      fts_patcher_t  *patcher = fts_object_get_patcher(o);

      /* Add outlets if needed */
      if (pos >= fts_object_get_outlets_number((fts_object_t *)patcher))
	fts_patcher_redefine_number_of_outlets(patcher, pos + 1);

      /* set the outlet position */
      patcher_position_outlet(patcher, this, pos);

      /* trim the number of outlets */
      fts_patcher_trim_number_of_outlets(patcher);
    }
}

/*************************************************************
 *
 *  patcher class
 *
 *  the patcher send messages it receive on the inlets to the inlets outlets,
 *  output messages are directly send to the patcher outlets by the outlet objects
 */
static void
patcher_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_patcher_t *this  = (fts_patcher_t *) o;
  fts_inlet_t *i;

  for (i = this->inlets[winlet]; i; i = i->next)
    fts_outlet_send((fts_object_t *)i, 0, s, ac, at);
}

/* load init:
 * The patcher sends recursively load_init to all the included objects, *first* to subpatchers,
 * and then to the other objects; this guarantee that any loadbang or similar
 * objects are executed first in the external patches and then in the external ones
 * (patchers have a protection against multiple load-init firing)
 *
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

/* open a patch;
 * Set the open flag to 1, so that all the "updating" objects can be active.
 * A "OPEN" message is sent to all the objects (but not the patchers) in the patch
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

/* close a patch;
 * Set the open flag to 0, so that all the "updating" objects can be inactive.
 * A "close" message is sent to all the objects (but not the patchers) in the patch
 */
static void
patcher_close(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;

  this->open = 0;
}

static void
patcher_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  set_editor_open(this);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
patcher_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  set_editor_close(this);
}

static void 
patcher_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;
  
  if(editor_is_open(this))
    {
      set_editor_close(this);
      fts_client_send_message((fts_object_t *)this, fts_s_closeEditor, 0, 0);  
    }
  
  p = this->objects;
  while (p)
    {
      fts_send_message(p, fts_SystemInlet, fts_s_closeEditor, 0, 0);      
      p = p->next_in_patcher;
    }
}

static void
show_object(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *obj = fts_get_object(at);

  if(fts_object_is_in_patcher(obj, this))
    {
      if(!editor_is_open(this))
	set_editor_open(this);

      fts_client_send_message(o, fts_s_openEditor, 0, 0);	  
      fts_client_send_message(o, sym_showObject, 1, at);
    }
}

static void
stop_waiting(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_client_send_message(o, sym_stopWaiting, 0, 0);
}

static int
fts_atom_is_subsequence(int sac, const fts_atom_t *sav, int ac, const fts_atom_t *av)
{
  int i,j;

  for (i = 0; i < (ac - sac + 1); i++)
    if (fts_atom_equals(&sav[0], &av[i]))
      {
	/* Found the beginning, test the rest */
	
	for (j = 1; j < sac; j++)
	  if (! fts_atom_equals(&sav[j], &av[j + i]))
	    return 0;

	return 1;
      }

  return 0;
}

/* the find engines */
static void
patcher_find(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *p;
  fts_status_t ret;

  /* see if this should be added */
  if (fts_atom_is_subsequence(ac - 1, at + 1, o->argc, o->argv))
    {
      /* add only if the patcher is not the root patcher or a patcher in the root patcher */
      if((this != fts_get_root_patcher())&&(o->patcher != fts_get_root_patcher()))
	fts_objectset_add(set, o);
    }
  /* look if the objects in the patchers are to be found */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_object(p))
	{
	  if (! fts_object_is_error(p))
	    {
	      /* send the find message to the object; if the object do not implement it, do the standard check */
	      ret = fts_send_message(p, winlet, s, ac, at);
	      
	      if (ret == &fts_MethodNotFound)
	        if (fts_atom_is_subsequence(ac - 1, at + 1, p->argc, p->argv))
		  {
		    fts_objectset_add(set, p);
		  }
	    }
	  else
	    {
	      /* for error objects do the check */
	      if (fts_atom_is_subsequence(ac - 1, at + 1, p->argc, p->argv))
		{
		  fts_objectset_add(set, p);
		}
	    }
	}
    }

  /* do the recursive calls  */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_standard_patcher(p) ||
	  fts_object_is_template(p) ||
	  fts_object_is_abstraction(p))
	fts_send_message(p, winlet, s, ac, at);
    }
}

/* the find errors engines */
static void
patcher_find_errors(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *p;

  /* look if the objects in the patchers are to be found */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      /* check if it have the error property on */
      fts_atom_t value;

      fts_object_get_prop(p, fts_s_error, &value);
      
      if (fts_is_int(&value))
	{
	  int v = fts_get_int(&value);
	  
	  if (v)
	    {
	      fts_objectset_add(set, p);
	    }
	}
    }

  /* do the recursive calls  */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_standard_patcher(p) ||
	  fts_object_is_template(p) ||
	  fts_object_is_abstraction(p))
	fts_send_message(p, winlet, s, ac, at);
    }
}

static void
patcher_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  int n = fts_get_int(at + 2);
  fts_inlet_t *inlet;

  inlet = this->inlets[n];
  while(inlet)
    {
      propagate_fun(propagate_context, (fts_object_t *)inlet, 0);
      inlet = inlet->next;
    }
}

void 
fts_patcher_set_template(fts_patcher_t *this, fts_template_t *template)
{
  this->type = fts_p_template;
  this->template = template;

  fts_template_add_instance(template, (fts_object_t *) this);
}

/* Methods: init put the pointers to zero */
static void 
patcher_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int i;
  int ninlets, noutlets;
  fts_atom_t va;
  fts_patcher_t *this = (fts_patcher_t *) o;
  
  ac--;
  at++;

  fts_env_init(&(this->env), (fts_object_t *) this);
  fts_patcher_set_standard(this);

  /* Define the "args" variable */
  this->args = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
  fts_object_refer( this->args);

  fts_variable_define( this, fts_s_args);
  fts_set_tuple( &va, this->args);
  fts_variable_restore(this, fts_s_args, &va, o);

  /* should use block allocation ?? */
  ninlets = fts_object_get_inlets_number(o);
  noutlets = fts_object_get_outlets_number(o);

  if (ninlets > 0)
    {
      this->inlets = (fts_inlet_t **)fts_malloc(sizeof(fts_inlet_t *) * ninlets);

      for (i=0; i<ninlets; i++)
	this->inlets[i] = 0;
    }
  else
    this->inlets = 0;

  if (noutlets)
    {
      this->outlets = (fts_outlet_t **) fts_malloc(sizeof(fts_outlet_t *) * noutlets);

      for (i=0; i<noutlets; i++)
	this->outlets[i] = 0;
    }
  else
    this->outlets = 0;

  this->objects = (fts_object_t *) 0;
  this->open        = 0; /* start as closed */
  this->editor_open = 0; /* start with editor closed */  
  this->deleted     = 0;
  this->dirty       = 0; /* start as saved */
}

/* delete patcher:
 *
 * Contained objects are deleted in the reverse order (last first);
 * it purpose is to reduce the number of redefinitions happening
 * and errors objects instantiated during a delete; locally to a patcher,
 * objects using variables come after objects defining the same variable,
 * so by deleting in the reverse order, we avoid redefinition within the
 * patchers; of course, if a variable is used somewhere else,
 * the user may be recomputed during delete.
 *
 * Also, since the list is can be indirectly reordered by a recompute
 * (i.e. a deletion), the last member is recomputed at each
 * interaction; slow, but the only alternative is to use a dynamic array
 * instead of a list. We will do it if it is a problem.
 */

static void 
patcher_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;

  if(editor_is_open(this))
    {
      set_editor_close(this);
      fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
    }

  /* If it is a template, remove it from the template instance list */
  if (fts_patcher_is_template(this))
    fts_template_remove_instance(this->template, (fts_object_t *) this);

  /* Set the deleted and reset the open flag */
  this->deleted = 1;
  this->open = 0;

  while (this->objects)
    {
      /* find last object in patcher */
      p = this->objects;
      while (p->next_in_patcher)
	p = p->next_in_patcher;

      fts_object_delete_from_patcher(p);
    }

  /* delete all the variables */
  fts_object_release( this->args);

  fts_variables_undefine(this, (fts_object_t *)this);

  /* delete the inlets and inlets tables */
  if (this->inlets)
    fts_free( this->inlets);

  if (this->outlets)
    fts_free( this->outlets);
}

static void fts_patcher_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_atom_t a[4];
  fts_object_t *p;

  if(!fts_object_is_error((fts_object_t *)this))
    {
      fts_object_get_prop((fts_object_t *)this, fts_s_wx, a);
      fts_object_get_prop((fts_object_t *)this, fts_s_wy, a+1);
      fts_object_get_prop((fts_object_t *)this, fts_s_ww, a+2);
      fts_object_get_prop((fts_object_t *)this, fts_s_wh, a+3);
      
      if( fts_get_int( a) && fts_get_int( a+1) && fts_get_int( a+2) && fts_get_int( a+3))
	fts_client_send_message((fts_object_t *)this, sym_setPatcherBounds, 4, a);
      
      for (p = this->objects; p ; p = p->next_in_patcher)
	fts_client_upload_object( p, -1);
      
      for (p = this->objects; p ; p = p->next_in_patcher)
	{
	  int outlet;
	  
	  for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	    {
	      fts_connection_t *c;
	      
	      for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
		{
		  ((fts_object_t *)c)->patcher = this;
		  fts_client_upload_object( (fts_object_t *)c, -1);
		}
	    }
	}

      fts_client_send_message((fts_object_t *)this, sym_endUpload, 0, 0);
    }
}

/***********************************************************************
 *
 *  .pat file format saving
 *
 */
static void patcher_save_dotpat_internal( FILE *file, fts_patcher_t *patcher);
static void patcher_save_dotpat( FILE *file, fts_patcher_t *patcher);
static void fts_patcher_save_as_dotpat( fts_symbol_t filename, fts_patcher_t *patcher);

static int get_int_property( fts_object_t *object, fts_symbol_t property_name)
{
  fts_atom_t a;

  fts_object_get_prop( object, property_name, &a);
  return fts_get_int( &a);
}

static void 
patcher_save_dotpat_object_geometry( FILE *file, fts_object_t *object)
{
  fprintf( file, "%d ", get_int_property( object, fts_s_x));     /* x */
  fprintf( file, "%d ", get_int_property( object, fts_s_y));     /* y */
  fprintf( file, "%d ", get_int_property( object, fts_s_width)); /* width */
  fprintf( file, "%d", 1); /* font_index, a voir */
}

static void 
patcher_save_dotpat_atoms( FILE *file, int ac, const fts_atom_t *at)
{
  int i;

  for ( i = 0; i < ac; i++)
    {
      if (fts_is_int( at + i ))
	fprintf( file, " %d", fts_get_int( at + i ));
      else if (fts_is_float( at + i ))
	fprintf( file, " %f", fts_get_float( at + i ));
      else if (fts_is_symbol( at + i ))
	fprintf( file, " %s", fts_get_symbol( at + i ) );
    }
}

static int 
find_object_index_in_patcher( fts_object_t *object)
{
  int index = 0;
  fts_object_t *o;
  fts_patcher_t *patcher;

  patcher = fts_object_get_patcher( object);
  for ( o = patcher->objects; o ; o = o->next_in_patcher)
    {
      if ( object == o)
	return index;

      index++;
    }

  return -1;
}

static void 
fts_patcher_save_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int type = fts_get_int(at);
  fts_symbol_t filename = fts_get_symbol(at+1);
  
  if(type)
    fts_save_patcher_as_bmax(filename, o);
  else
    fts_patcher_save_as_dotpat(filename, (fts_patcher_t *)o);

  fts_patcher_set_dirty((fts_patcher_t *)o, 0);

  fts_client_send_message(o, sym_setSaved, ac, at);
}

static void 
patcher_save_objects( FILE *file, fts_object_t *object)
{
  while ( object)
    {
      if ( fts_object_is_standard_patcher( object) )
	{
	  patcher_save_dotpat( file, (fts_patcher_t *)object);

	  fprintf( file, "#P newobj ");
	  patcher_save_dotpat_object_geometry( file, object);
	  fprintf( file, " patcher ");
	  patcher_save_dotpat_atoms( file, object->argc - 1, object->argv + 1);
	  fprintf( file, ";\n");
	}
      else if ( ! fts_object_is_patcher( object)
		&& fts_class_has_method( fts_object_get_class(object), fts_SystemInlet, fts_s_save_dotpat) )
	{
 	  fts_atom_t a;

	  fts_set_pointer( &a, file);
	  fts_send_message( object, fts_SystemInlet, fts_s_save_dotpat, 1, &a);
	}
      else
	{
	  fprintf( file, "#P newex ");
	  patcher_save_dotpat_object_geometry( file, object);
	  patcher_save_dotpat_atoms( file, object->argc, object->argv);
	  fprintf( file, ";\n");
	}

      object = object->next_in_patcher;
    }
}

/* This auxiliary function is used to save the connections in reverse order.
 * There is no requested order for connections saving, but MAX/Opcode
 * saves in reverse order, so in order to ease text comparison of files,
 * we can save in reverse order.
 *
 * (apparently, the connection saving order depends from the MAX version...)
 */
static void patcher_save_connections( FILE *file, fts_object_t *object, int patcher_object_count)
{
  int object_index = 0;

  while ( object)
    {
      int outlet;

      for ( outlet = 0; outlet < fts_object_get_outlets_number( object); outlet++)
	{
	  fts_connection_t *c;

	  for ( c = object->out_conn[outlet]; c ; c = c->next_same_src)
	    {
	      fprintf( file, "#P connect %d %d %d %d;\n", 
		       patcher_object_count - 1 - object_index,
		       outlet, 
		       patcher_object_count - 1 - find_object_index_in_patcher( c->dst),
		       c->winlet);
	    }
	}

      object_index++;

      object = object->next_in_patcher;
    }
}

static void 
patcher_save_dotpat( FILE *file, fts_patcher_t *patcher)
{
  int x_left, y_top, x_right, y_bottom;

  /* save window properties */
  x_left = get_int_property( (fts_object_t *)patcher, fts_s_wx);
  y_top = get_int_property( (fts_object_t *)patcher, fts_s_wy);
  x_right = x_left + get_int_property( (fts_object_t *)patcher, fts_s_ww);
  y_bottom = y_top + get_int_property( (fts_object_t *)patcher, fts_s_wh);;

  fprintf( file, "#N vpatcher %d %d %d %d;\n", x_left, y_top, x_right, y_bottom);

  /* save objects */
  patcher_save_objects( file, patcher->objects);

  /* save connections */
  patcher_save_connections( file, patcher->objects, fts_patcher_get_objects_count( patcher));

  fprintf( file, "#P pop;\n");
}

/* save a patcher in .pat format:
 * at[0]: filename (the name of the destination file)
 *
 * (note: this method does not check for nor add a .pat suffix to the file name,
 * it is left to the user interface)
*/
static void 
patcher_save_dotpat_file(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_save_as_dotpat( fts_get_symbol( at), (fts_patcher_t *)o);
}

static void 
fts_patcher_save_as_dotpat(fts_symbol_t filename, fts_patcher_t *patcher)
{
  FILE *file;

  file = fopen( filename, "wb");

  if ( file != NULL)
    {
      fprintf( file, "max v2;\n");
      patcher_save_dotpat( file, patcher);
      fclose( file);
    }
  else
    {
      /* this should be a dialog opened in the user interface */
      post( "Error: cannot open %s for saving\n", filename );
    }
}

/* **********************************************************************
 *
 * Properties
 *
 */

static void 
patcher_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_property_changed(o, fts_s_patcher_type);
}

/* daemon to get the patcher_type property */
static void 
patcher_get_patcher_type(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
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

/*
 * The remote functions
 */
static void fts_patcher_start_updates( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_open(o, fts_SystemInlet, fts_s_open, 0, 0);
}

static void fts_patcher_stop_updates( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_close(o, fts_SystemInlet, fts_s_close, 0, 0);
}

/**
 * This function send all the objects and connections in the patcher that have not
 * yet been uploaded; it is usefull after paste operations.
 */
static void fts_patcher_update( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *p;
  fts_atom_t a[2];

  /* upload all the not uploaded objects */

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (!fts_object_has_id(p))
      fts_client_upload_object( p, -1);

  /* For each object, for each outlet, upload all the not uploaded connections */

  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    if (!fts_object_has_id((fts_object_t *)c))
	      {
		((fts_object_t *)c)->patcher = this;
		fts_client_upload_object((fts_object_t *)c, -1);
	      }
	}
    }
}

/* Handle geometric properties */
static void fts_patcher_set_wx( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_wx, at);
}
static void fts_patcher_set_wy( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_wy, at);
}
static void fts_patcher_set_wh( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_wh, at);
  /*fts_patcher_set_dirty((fts_patcher_t *)o, 1);*/
}
static void fts_patcher_set_ww( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_ww, at);
  /*  fts_patcher_set_dirty((fts_patcher_t *)o, 1);*/
}

static void 
fts_patcher_open_help_patch( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *obj = fts_get_object(&at[0]);
  fts_package_t *pkg = fts_object_get_package(obj);
  fts_symbol_t dir = fts_package_get_dir( pkg);
  fts_symbol_t class_name = fts_object_get_class_name(obj);
  fts_symbol_t file_name;
  fts_patcher_t *help_patch;
  char path[256];

  snprintf(path, 256, "%s%c%s%c%s%s", dir, fts_file_separator, "help", fts_file_separator, class_name, ".help.jmax");
  file_name = fts_new_symbol_copy(path);

  fts_log("[patcher] help %s\n", file_name);

  help_patch = fts_client_get_patcher_by_file_name(file_name);

  if(help_patch)
    patcher_open_editor((fts_object_t *)help_patch, 0, 0, 0, 0);
  else
    help_patch = fts_client_load_patcher(file_name, (fts_object_t *)fts_get_root_patcher(), fts_get_client_id(o));

  if(!help_patch)
    fts_client_send_message(o, sym_noHelp, 1, at); 
}

static void 
fts_patcher_add_object_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *obj = fts_eval_object_description((fts_patcher_t *)this, ac - 2, at + 2);
  fts_atom_t a[1];
  fts_set_int(a, -1);

  fts_object_put_prop(obj, fts_s_x, at);
  fts_object_put_prop(obj, fts_s_y, at+1);
  fts_object_put_prop(obj, fts_s_layer, a);

  fts_client_upload_object(obj, -1);

  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void 
fts_patcher_upload_child( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_upload_object( o, fts_get_object(&at[0]));
}

void 
fts_patcher_upload_object(fts_object_t *this, fts_object_t *obj)
{
  fts_atom_t a[9];

  if(fts_object_get_class_name(obj) == fts_s_connection)
    {      
      fts_client_start_message( this, sym_addConnection);
      fts_client_add_int( this, fts_get_object_id(obj));
      fts_client_add_object( this, ((fts_connection_t *)obj)->src);
      fts_client_add_int( this, ((fts_connection_t *)obj)->woutlet);
      fts_client_add_object( this, ((fts_connection_t *)obj)->dst);
      fts_client_add_int( this, ((fts_connection_t *)obj)->winlet);
      fts_client_add_int( this, ((fts_connection_t *)obj)->type);
      fts_client_done_message( this);
    }
  else
    {
      fts_object_get_prop(obj, fts_s_x, a);
      fts_object_get_prop(obj, fts_s_y, a+1);
      fts_object_get_prop(obj, fts_s_width, a+2);
      fts_object_get_prop(obj, fts_s_height, a+3);
      fts_object_get_prop(obj, fts_s_ninlets, a+4);
      fts_object_get_prop(obj, fts_s_noutlets, a+5);
      fts_object_get_prop(obj, fts_s_layer, a+6);
      fts_object_get_prop(obj, fts_s_error, a+7);
      
      fts_client_start_message( this, sym_addObject);
      fts_client_add_int( this, fts_get_object_id(obj));
      fts_client_add_int( this, fts_get_int(a));
      fts_client_add_int( this, fts_get_int(a+1));
      fts_client_add_int( this, fts_get_int(a+2));      
      fts_client_add_int( this, fts_get_int(a+3));
      fts_client_add_int( this, fts_get_int(a+4));
      fts_client_add_int( this, fts_get_int(a+5));
      fts_client_add_int( this, fts_get_int(a+6));
      fts_client_add_int( this, fts_get_int(a+7));
      
      if(fts_object_is_error(obj))
	{
	  fts_object_get_prop(obj, fts_s_error_description, a+8);
	  fts_client_add_string( this, fts_get_string(a+8));
	}
      else
	fts_client_add_symbol( this, fts_object_get_class_name( obj));
      
      /*fts_client_add_int( this, fts_object_is_template(obj));*/

      fts_client_add_atoms( this, obj->argc, obj->argv);
      fts_client_done_message( this);

      fts_object_get_prop(obj, fts_s_font, a);

      if( fts_get_symbol(a))
	{
	  fts_object_get_prop(obj, fts_s_fontSize, a+1);
	  fts_object_get_prop(obj, fts_s_fontStyle, a+2);
	  fts_client_send_message(obj, fts_s_setFont, 3, a);
	}

      fts_send_message(obj, fts_SystemInlet, fts_s_send_properties, 0, 0);
    }
}

void 
fts_patcher_redefine_connection(fts_object_t *patcher, fts_connection_t *c)
{
  fts_client_start_message( patcher, sym_redefineConnection);
  fts_client_add_object( patcher, (fts_object_t *)c);
  fts_client_add_object( patcher, c->src);
  fts_client_add_int( patcher, c->woutlet);
  fts_client_add_object( patcher, c->dst);
  fts_client_add_int( patcher, c->winlet);
  fts_client_add_int( patcher, c->type);
  fts_client_done_message( patcher);
}

void 
fts_patcher_release_connection(fts_object_t *patcher, fts_connection_t *c)
{
  fts_atom_t a[1];

  fts_set_object(a, (fts_object_t *)c);
  fts_client_send_message( patcher, sym_releaseConnection, 1, a);
}

static void 
fts_patcher_redefine_object_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *obj;
  fts_atom_t a[1];
  int do_var = 0;

  fts_object_t *oldobj = fts_get_object(&at[0]);
      
  obj = fts_object_redefine(oldobj, -1, 1, ac - 1, at + 1);
      
  fts_client_upload_object(obj, -1);

  /* qui dovrebbe uploadare le connessioni sull'oggetto */
  fts_object_upload_connections(obj);

  fts_set_object(a, obj);
  fts_client_send_message(o, sym_objectRedefined, 1, a);

  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void 
fts_patcher_redefine_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  
  if (ac >= 0)
    {
      fts_atom_t argv[512];
      int argc;

      if (fts_object_description_defines_variable(ac, at))
	{
	  fts_set_symbol(&argv[0], fts_s_patcher); 

	  /* copy arguments (ignoring variable) */
	  for (argc = 1; (argc < ac - 1) && (argc < 512) ; argc++) 
	    argv[argc] = at[argc+1];
	}
      else
	{
	  /* Plain syntax */
	  fts_set_symbol(&argv[0], fts_s_patcher);

	  for (argc = 1; (argc <= ac) && (argc < 512) ; argc++)
	    argv[argc] = at[argc-1];
	}

      fts_patcher_redefine(this, argc, argv);

      fts_client_send_message((fts_object_t *)this, sym_setDescription, argc - 1, argv + 1);
    
      fts_patcher_set_dirty((fts_patcher_t *)o, 1);
    }
  else
    printf_mess("System Error in FOS message REDEFINE PATCHER: bad args", ac, at);
}

static void 
fts_patcher_delete_objects_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac > 0)
    {
      fts_object_t *obj;
      int i;
  
      for(i=0; i<ac; i++)
	{
	  obj = fts_get_object(&at[i]);
	  if (obj)
	      fts_object_delete_from_patcher(obj);
	  else
	    fts_log("[patcher] delete_objects_from_client: System Error deleting a non existing object\n");
	}
      
        fts_patcher_set_dirty((fts_patcher_t *)o, 1);
    }
}

static void 
fts_patcher_add_connection_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *obj;
  fts_atom_t a[6];

  if((ac == 4) &&
     fts_is_object(&at[0]) &&
     fts_is_int(&at[1]) &&
     fts_is_object(&at[2]) &&
     fts_is_int(&at[3]))
    {
      int inlet, outlet;
      int id;
      fts_object_t *from, *to;
      fts_status_t ret;
      fts_connection_t *connection;

      from   = fts_get_object( at);
      outlet = fts_get_int( at+1);
      to     = fts_get_object( at+2);
      inlet  = fts_get_int( at+3);

      if (to && from)
	{
	  connection = fts_connection_new( -1, from, outlet, to, inlet);
		  
	  if (! connection)
	    {
	      return;
	    }

	  ((fts_object_t *)connection)->patcher = this;
 	  fts_client_upload_object((fts_object_t *)connection, -1);
	}
      
      fts_patcher_set_dirty((fts_patcher_t *)o, 1);
    }
}

static void 
fts_patcher_delete_connection_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_connection_t *c = (fts_connection_t *)fts_get_object( at);

  if (c)
    {
      fts_connection_delete(c);
      fts_patcher_set_dirty((fts_patcher_t *)o, 1);
    }
}

static void fts_patcher_paste( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_object_t *p;
  fts_atom_t a[2];

  fts_object_t *clipboard = fts_get_object( at);
  int dx = fts_get_int(at+1);
  int dy = fts_get_int(at+2);

  fts_set_object(a, o);
  fts_send_message( clipboard, fts_SystemInlet, fts_s_paste, 1, a);   

  /* upload all the not uploaded objects */

  fts_client_send_message((fts_object_t *)this, sym_startPaste, 0, 0);

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (!fts_object_has_id(p))
      {
	fts_object_get_prop(p, fts_s_x, a);
	fts_object_get_prop(p, fts_s_y, a+1);

	fts_set_int(a, fts_get_int(a) + dx);
	fts_set_int(a+1, fts_get_int(a+1) + dy);

	fts_object_put_prop(p, fts_s_x, a);
	fts_object_put_prop(p, fts_s_y, a+1);

	fts_client_upload_object( p, -1);
      }
  /* For each object, for each outlet, upload all the not uploaded connections */

  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
	{
	  fts_connection_t *c;

	  for (c = p->out_conn[outlet]; c ; c = c->next_same_src)
	    if (!fts_object_has_id((fts_object_t *)c))
	      {
		((fts_object_t *)c)->patcher = this;
		fts_client_upload_object((fts_object_t *)c, -1);
	      }
	}
    }
  
  fts_client_send_message((fts_object_t *)this, sym_endPaste, 0, 0);
}

/****************************************************************/
/****************************************************************/

/* daemon for getting the property "state". */
static void
patcher_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_atom_t *v;

  v = fts_variable_get_value( (fts_patcher_t *)obj, fts_s_export);

  if(v)
    *value = *v;
  else
    fts_set_object(value, obj);
}

/* daemon for setting the number of inlets */
static void
patcher_set_ninlets(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_redefine_number_of_inlets((fts_patcher_t *)obj, fts_get_int(value));
}

/* daemon for setting the number of outlets */
static void
patcher_set_noutlets(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_redefine_number_of_outlets((fts_patcher_t *)obj, fts_get_int(value));
}

static fts_status_t
patcher_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  int ninlets = fts_get_int_arg(ac, at, 0, 0);
  int noutlets = fts_get_int_arg(ac, at, 1, 0);
  int i;

  /* initialize the class */
  fts_class_init(cl, sizeof(fts_patcher_t), ninlets, noutlets, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, patcher_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, patcher_delete);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload, fts_patcher_upload);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_upload_child, fts_patcher_upload_child);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find, patcher_find);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_find_errors, patcher_find_errors);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_send_properties, patcher_send_properties); 

  fts_class_define_thru(cl, patcher_propagate_input);

  for (i = 0; i < ninlets; i ++)
    fts_method_define_varargs(cl, i, fts_s_anything, patcher_anything);

  fts_method_define_varargs(cl,fts_SystemInlet, fts_new_symbol("load_init"), patcher_load_init); 
  fts_method_define_varargs(cl,fts_SystemInlet, fts_s_open, patcher_open); 
  fts_method_define_varargs(cl,fts_SystemInlet, fts_s_close, patcher_close); 

  fts_method_define_varargs(cl,fts_SystemInlet, fts_new_symbol("open_help_patch"), fts_patcher_open_help_patch); 

  fts_method_define_varargs(cl,fts_SystemInlet, fts_s_save, fts_patcher_save_from_client); 

  fts_method_define_varargs(cl,fts_SystemInlet, fts_s_paste, fts_patcher_paste); 

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_openEditor, patcher_open_editor);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_destroyEditor, patcher_destroy_editor);
  fts_method_define_varargs(cl,fts_SystemInlet, fts_s_closeEditor, patcher_close_editor); 
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("show_object"), show_object);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("stop_waiting"), stop_waiting);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("start_updates"), fts_patcher_start_updates);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("stop_updates"), fts_patcher_stop_updates);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("patcher_update"), fts_patcher_update);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_wx"), fts_patcher_set_wx);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_wy"), fts_patcher_set_wy);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_ww"), fts_patcher_set_ww);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("set_wh"), fts_patcher_set_wh);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_object"), fts_patcher_add_object_from_client);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("delete_objects"), 
			    fts_patcher_delete_objects_from_client);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("add_connection"), 
			    fts_patcher_add_connection_from_client);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("delete_connection"),
			    fts_patcher_delete_connection_from_client);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("redefine_object"),
			    fts_patcher_redefine_object_from_client);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_new_symbol("redefine_patcher"),
			    fts_patcher_redefine_from_client);

  fts_method_define_varargs( cl, fts_SystemInlet, fts_new_symbol("save_dotpat_file"), patcher_save_dotpat_file); 

  /* daemon for properties */
  fts_class_add_daemon(cl, obj_property_get, fts_s_patcher_type, patcher_get_patcher_type);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, patcher_get_state);

  fts_class_add_daemon(cl, obj_property_put, fts_s_ninlets, patcher_set_ninlets);
  fts_class_add_daemon(cl, obj_property_put, fts_s_noutlets, patcher_set_noutlets);

  return fts_Success;
}

/*************************************************************
 *
 *  patcher re-definition
 *
 *  fts_patcher_redefine()
 *
 *    Redefine a patcher inplace from a new description in term of "object" description.
 *  
 *    Actually handle the variables; here ins and outs cannot be changed
 *    (for patchers, they do not depend on the description).
 *
 *  fts_patcher_redefine_number_of_inlets()
 *
 *    Redefine a patcher "inplace" by changing directly the inlets and outlets
 *    and *the class* of the object.
 *    To do this, it heavily rely on the internals of the message system; the patcher
 *    is a primitive special class for the message system.
 * 
 */

/* assign variables to a patcher as found by the expression parser */
void 
fts_patcher_assign_variable(fts_symbol_t name, fts_atom_t *value, void *data)
{
  fts_patcher_t *this = (fts_patcher_t *)data;

  if(! fts_variable_is_suspended(this, name))
    fts_variable_define(this, name);

  fts_variable_restore(this, name, value, (fts_object_t *)this);
}

fts_patcher_t *
fts_patcher_redefine(fts_patcher_t *this, int aoc, const fts_atom_t *aot)
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

  /* if the old patcher defines a variable, and the new definition
   * doesn't define the same variable, delete the variable,
   * if the new object defines the same variable, just suspend it.
   */
  if (var)
    {
      if (obj->varname == var)
	{
	  fts_variable_suspend(obj->patcher, obj->varname);
	  fts_variable_undefine(obj->patcher, obj->varname, obj);
	}
      else
	{
	  /* if the variable already exists in this local context, make an double definition patcher  */
	  if (! fts_variable_can_define(obj->patcher, var))
	    {
	      fts_variable_define(obj->patcher, var);
	      fts_variables_undefine_suspended(this, obj);
	      fts_object_set_error(obj, "Variable %s already defined", var);

	      if (fts_object_has_id(obj))
		{
		  fts_object_property_changed(obj, fts_s_error);
		  fts_object_property_changed(obj, fts_s_error_description);
		}

	      return this;
	    }
	  else if ( ! fts_variable_is_suspended(obj->patcher, var))
	    {
	      /* define the variable, suspended
	       * (this will also steal all the objects referring to the same variable name
	       * in the local scope from any variable defined outside the scope)
	       */
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

      /* set error to expression error */
      fts_object_set_error(obj, fts_expression_get_msg(e), fts_expression_get_err_arg(e));
    }
  else
    {
      /* Set the error property to zero */
      fts_set_int(&a, 0);
      fts_object_put_prop(obj, fts_s_error, &a);

      /* reallocate the args */
      fts_tuple_set( this->args, ac - 1, at + 1);

      /* set the new variables */
      fts_expression_map_to_assignements(e, fts_patcher_assign_variable, (void *) this);

      fts_set_tuple(&a, this->args);
      fts_variable_restore(this, fts_s_args, &a, obj);

      /* register the patcher as user of the used variables */
      fts_expression_add_variables_user(e, obj);

      /* undefine all the locals that are still suspended  */
      fts_variables_undefine_suspended(this, obj);
    }

  /* recover the variable this patcher define, if any */
  if (var != 0)
    {
      fts_set_object(&a, obj);

      fts_variable_restore(obj->patcher, var, &a, obj);
      obj->varname = var;
    }

  /* free the expression state structure */
  fts_expression_state_free(e);

  /* inform the UI that the name is probabily changed (the type cannot change) and the error property too. */

  if (fts_object_has_id(obj))
    {
      fts_object_property_changed(obj, fts_s_error);
      fts_object_property_changed(obj, fts_s_error_description);
    }

  return this;
}

void 
fts_patcher_redefine_number_of_inlets(fts_patcher_t *this, int new_ninlets)
{
  fts_object_t *obj_this = (fts_object_t *)this;
  int old_ninlets;

  old_ninlets = fts_object_get_inlets_number((fts_object_t *) this);

  if (old_ninlets == new_ninlets)
    return;

  /* delete all the connections that will not be pertinent any more */
  fts_object_trim_inlets_connections(obj_this, new_ninlets);

  /* reallocate and copy the patcher inlets, incoming connections and inlets properties if needed */
  if (new_ninlets == 0)
    {
      /* no new inlets, but old inlets to delete */

      fts_free( this->inlets);
      fts_free( obj_this->in_conn);

      this->inlets = 0;
      obj_this->in_conn = 0;
    }
  else if (old_ninlets > 0)
    {
      /* there are new inlets and there are old inlets(reallocate and move) */
      int i;
      fts_inlet_t  **new_inlets;
      fts_connection_t **new_in_conn;

      new_inlets  = (fts_inlet_t **)  fts_malloc(new_ninlets * sizeof(fts_inlet_t *));
      new_in_conn = (fts_connection_t **) fts_calloc(new_ninlets * sizeof(fts_connection_t *));

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

      fts_free( this->inlets);
      fts_free( obj_this->in_conn);
	      
      this->inlets = new_inlets;
      obj_this->in_conn = new_in_conn;
    }
  else 
    {
      int i;
      
      /* there are new inlets, but there were no inlets before (just allocate without copying old stuff) */
      this->inlets = (fts_inlet_t **) fts_malloc(new_ninlets * sizeof(fts_inlet_t *));
      obj_this->in_conn = (fts_connection_t **) fts_calloc(new_ninlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_ninlets; i++)
	{
	  this->inlets[i] = 0;
	  obj_this->in_conn[i] = 0;
	}
    }

  /* change the patcher class */
  {
    fts_atom_t a[2];

    fts_set_int(a, new_ninlets);
    fts_set_int(a + 1, fts_object_get_outlets_number((fts_object_t *) this));
    obj_this->head.cl = fts_class_instantiate(patcher_metaclass, 2, a);
  }

  {
    fts_object_t *p;

    /* reinstall the inlets */
    for (p = this->objects; p ; p = p->next_in_patcher)
      if (fts_object_is_inlet(p))
	{
	  /* check the number (during object deleting we may find the object we are deleting in the list) */
	  fts_inlet_t *inlet = (fts_inlet_t *)p;

	  if (inlet->position < new_ninlets)
	    patcher_position_inlet(this, inlet, inlet->position);
	}
  }

  if (fts_object_has_id((fts_object_t *)this))
    fts_object_property_changed((fts_object_t *)this, fts_s_ninlets);
}

void 
fts_patcher_redefine_number_of_outlets(fts_patcher_t *this, int new_noutlets)
{
  fts_object_t *obj_this = (fts_object_t *)this;
  int old_noutlets;

  old_noutlets = fts_object_get_outlets_number((fts_object_t *) this);

  if (old_noutlets == new_noutlets)
    return;

  /* delete all the connections that will not be pertinent any more */
  fts_object_trim_outlets_connections(obj_this, new_noutlets);

  /* reallocate and copy the patcher outlets, incoming connections and outlets properties if needed */
  if (new_noutlets == 0)
    {
      /* no new outlets, but old outlets to delete */
      fts_free( this->outlets);
      fts_free( obj_this->out_conn);

      this->outlets = 0;
      obj_this->out_conn = 0;
    }
  else  if (old_noutlets > 0)
    {
      /* new outlets, but no old outlets (reallocate and move) */
      int i;
      fts_outlet_t  **new_outlets;
      fts_connection_t **new_out_conn;

      new_outlets  = (fts_outlet_t **)  fts_malloc(new_noutlets * sizeof(fts_outlet_t *));
      new_out_conn = (fts_connection_t **) fts_calloc(new_noutlets * sizeof(fts_connection_t *));

      for (i = 0; i < new_noutlets; i++)
	{
	  new_outlets[i] = 0;

	  if (i < old_noutlets)
	    new_out_conn[i] = obj_this->out_conn[i];
	  else
	    new_out_conn[i] = 0;
	}

      fts_free( this->outlets);
      fts_free( obj_this->out_conn);
	      
      this->outlets = new_outlets;
      obj_this->out_conn = new_out_conn;
    }
  else 
    {
      int i;

      /* new outlets, but no old outlets before (just allocate without copying old stuff) */
      this->outlets = (fts_outlet_t **) fts_malloc(new_noutlets * sizeof(fts_outlet_t *));
      obj_this->out_conn = (fts_connection_t **) fts_calloc(new_noutlets*sizeof(fts_connection_t *));

      for (i = 0; i < new_noutlets; i++)
	{
	  this->outlets[i] = 0;
	  obj_this->out_conn[i] = 0;
	}
    }

  /* change the patcher class */
  {
    fts_atom_t a[2];

    fts_set_int(a, fts_object_get_inlets_number((fts_object_t *) this));
    fts_set_int(a + 1, new_noutlets);
    obj_this->head.cl = fts_class_instantiate(patcher_metaclass, 3, a);
  }

  {
    fts_object_t *p;

    /* reinstall the outlets */
    for (p = this->objects; p ; p = p->next_in_patcher)
      if (fts_object_is_outlet(p))
	{
	  /* check the number (during object deleting we may find the object we are deleting in the list */
	  fts_outlet_t *outlet = (fts_outlet_t *)p;

	  if (outlet->position < new_noutlets)
	    patcher_position_outlet(this, outlet, outlet->position);
	}
  }

  if (fts_object_has_id((fts_object_t *)this))
    fts_object_property_changed((fts_object_t *)this, fts_s_noutlets);
}

/*************************************************************
 *
 *  add/remove objects
 *
 *    An object is always added to the end; this is to reduce (or suppress) the
 *    number of redefinitions at loading time (by definition, a non error object
 *    is defined after all the variables it define, so it will be *after* the
 *    variables in the list).
 *
 */

void 
fts_patcher_add_object(fts_patcher_t *this, fts_object_t *obj)
{
  fts_object_t **p; /* indirect precursor */

  for (p = &(this->objects); *p; p = &((*p)->next_in_patcher))
    {
    }

  *p = obj;
}

void 
fts_patcher_remove_object(fts_patcher_t *this, fts_object_t *obj)
{
  fts_object_t **p; /* indirect precursor */

  for (p = &(this->objects); *p; p = &((*p)->next_in_patcher))
    if (*p == obj)
      {
	*p = obj->next_in_patcher;
	return;
      }
}

int 
fts_patcher_get_objects_count(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;	

  for (p = this->objects; p; p = p->next_in_patcher)
    i++;

  return i;
}

/*************************************************************
 *
 *  patcher .pat format support
 *
 */

static int 
patcher_count_inlet_objects(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;

  for (p = this->objects; p; p = p->next_in_patcher)
    if (fts_object_is_inlet(p))
      i++;

  return i;
}

static int 
patcher_count_outlet_objects(fts_patcher_t *this)
{
  int i = 0;
  fts_object_t *p;
  
  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_outlet(p))
      i++;

  return i;
}

/*
 * take all the declared inlets and outlets and assign them to the patcher 
 * in the correct order (based on the x position),
 * setting also the inlet and outlet status.
 *
 * assume that the patcher have no inlets and outlets,
 * allocate the new ones, and *change* the class of the patcher,
 * using the fts_patcher_redefine function
 * 
 * use the  *let_set_position functions to avoid redefining the nins/nouts
 * of the patcher during parsing
 *
 * (note that the following code work because .pat do not have
 * multiple inlet/outlet objects connected to the same inlet)
 */

void 
fts_patcher_reassign_inlets_outlets(fts_patcher_t *this)
{
  fts_object_t *p;
  int i, j;
  int ninlets;
  int noutlets;

  /* compute number of inlets and outlets */
  ninlets = patcher_count_inlet_objects(this);
  noutlets = patcher_count_outlet_objects(this);

  /* make a redefine the old patcher */
  fts_patcher_redefine_number_of_inlets(this,  ninlets);
  fts_patcher_redefine_number_of_outlets(this, noutlets);

  /* store the inlets in the inlet arrays 
   * (not so redundant as may seems: patcher_redefine do not reposition inlets without position)
   */
  i = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_inlet(p))
      {
	this->inlets[i] = (fts_inlet_t *) p;
	i++;
      }

  /* store the outlets in the oulet arrays */
  i = 0;

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (fts_object_is_outlet(p))
      {
	this->outlets[i] = (fts_outlet_t *) p;
	i++;
      }

  /* sort the inlets based on their x property*/
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

  /* sort the outlets based on their x property*/
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

  /* first, update the position field of all the inlets, then reposition them
   * (the position field must be updated for all the inlets before starting to reposition them,
   * because is used by the reposition function to remove the inlet first)
   */

  for (i = 0; i < ninlets; i++)
    this->inlets[i]->position = i; 

  /* redefine all the inlets */
  for (i = 0; i < ninlets; i++)
    patcher_position_inlet(this, this->inlets[i], i);

  /* same for the outlets */
  for (i = 0; i < noutlets; i++)
    this->outlets[i]->position = i; 

  /* redefine all the outlets */
  for (i = 0; i < noutlets; i++)
    patcher_position_outlet(this, this->outlets[i], i);
}

void 
fts_patcher_blip(fts_patcher_t *this, const char *msg)
{
  if (fts_patcher_is_open(this))
    {
      fts_atom_t a[1];
  
      fts_set_string(&a[0], (char *)msg);
      fts_client_send_message((fts_object_t *)this, sym_blip, 1, a);
    }
  else if (fts_object_get_patcher((fts_object_t *)this))
    {
      fts_patcher_blip(fts_object_get_patcher((fts_object_t *)this), msg);
    }
}

/* set a patch as dirty or as saved: if this patcher is a sub-patcher propagate the set_dirty to his father
 * until a firts level patche is reached.
 * A "setDirty" message is sent to the client after is_dirty flag changed
 */
void 
fts_patcher_set_dirty(fts_patcher_t *this, int is_dirty)
{
  if( ((fts_object_t *)this)->patcher != fts_get_root_patcher())
    fts_patcher_set_dirty( ((fts_object_t *)this)->patcher, is_dirty);
  else
    {
      if(this->dirty != is_dirty)
	{
	  this->dirty = is_dirty;

	  if (editor_is_open(this))
	    {
	      fts_atom_t a[1];
  
	      fts_set_int(&a[0], is_dirty);
	      fts_client_send_message((fts_object_t *)this, sym_setDirty, 1, a);
	    }
	}
    }
}


/*************************************************************
 *
 *  root patcher
 *
 *  (note: the root patcher has by definition the ID "1")
 *
 */
static fts_patcher_t *fts_root_patcher;

static void
fts_create_root_patcher()
{
  fts_atom_t a[1];

  fts_set_symbol(a, fts_new_symbol("root"));
  fts_root_patcher = (fts_patcher_t *)fts_object_create(patcher_metaclass, 1, a);
  fts_object_set_id((fts_object_t *)fts_root_patcher, 1);  
}

static void fts_delete_root_patcher(void)
{
  fts_object_destroy((fts_object_t *)fts_root_patcher);
}

fts_patcher_t *
fts_get_root_patcher(void)
{
  return fts_root_patcher;
}

/*************************************************************
 *
 *  module init
 *
 */

/* converter/doctor for the old patcher format */
static fts_object_t *
patcher_doctor(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_atom_t a[2];
  fts_object_t *obj;

  fts_set_symbol(&a[0], fts_s_patcher);

  if (ac >= 2)
    {
      a[1] = at[1];
      fts_object_new_to_patcher(patcher, 2, a, &obj);
      fts_object_set_description(obj, 2, a);

      if (ac >= 3)
	fts_object_put_prop(obj, fts_s_ninlets, &at[2]);

      if (ac >= 4)
	fts_object_put_prop(obj, fts_s_noutlets, &at[3]);
    }
  else
    fts_object_new_to_patcher(patcher, 1, at , &obj);

  return obj;
}

/***********************************************************************
 *
 * Initialization/shutdown
 *
 */

void fts_kernel_patcher_init(void)
{
  fts_atom_t a;

  sym_showObject = fts_new_symbol("showObject");
  sym_stopWaiting = fts_new_symbol("stopWaiting");
  sym_setDirty = fts_new_symbol("setDirty");

  sym_redefineStart = fts_new_symbol("redefineStart");
  sym_setWX = fts_new_symbol("setWX");
  sym_setWY = fts_new_symbol("setWY");
  sym_setWW = fts_new_symbol("setWW");
  sym_setWH = fts_new_symbol("setWH");
  sym_setPatcherBounds = fts_new_symbol("setPatcherBounds");
  sym_addObject = fts_new_symbol("addObject");
  sym_addConnection = fts_new_symbol("addConnection");
  sym_redefineConnection = fts_new_symbol("redefineConnection");
  sym_releaseConnection = fts_new_symbol("releaseConnection");
  sym_redefineObject = fts_new_symbol("redefineObject");
  sym_objectRedefined = fts_new_symbol("objectRedefined");
  sym_setRedefined = fts_new_symbol("setRedefined");
  sym_endUpload = fts_new_symbol("endUpload");
  sym_blip = fts_new_symbol("setMessage");
  sym_setDescription = fts_new_symbol("setDescription");
  sym_setSaved = fts_new_symbol("setSaved");
  sym_startPaste = fts_new_symbol("startPaste");
  sym_endPaste = fts_new_symbol("endPaste");
  sym_noHelp = fts_new_symbol("noHelp");

  /*fts_register_object_doctor(fts_new_symbol("patcher"), patcher_doctor);*/

  patcher_metaclass = fts_metaclass_install(fts_s_patcher, patcher_instantiate, fts_arg_equiv);
  fts_class_instantiate(patcher_metaclass, 0, 0);

  fts_metaclass_install(fts_s_inlet, inlet_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_s_outlet, outlet_instantiate, fts_arg_type_equiv);

  fts_create_root_patcher();
}

void fts_kernel_patcher_shutdown(void)
{
  fts_delete_root_patcher();
}
