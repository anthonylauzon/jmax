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
 */

#include <stdarg.h>
#include <stdio.h>
#include <limits.h>

#include <fts/fts.h>
#include <ftsprivate/client.h>
#include <ftsprivate/class.h>
#include <ftsprivate/connection.h>
#include <ftsprivate/errobj.h>
#include <ftsprivate/object.h>
#include <ftsprivate/patcher.h>
#include <ftsprivate/package.h>
#include <ftsprivate/bmaxfile.h>
#include <ftsprivate/template.h>
#include <ftsprivate/label.h>

fts_metaclass_t *patcher_metaclass = 0;
static fts_metaclass_t *patcher_inout_metaclass = 0;
static fts_metaclass_t *receive_metaclass = 0;
static fts_metaclass_t *send_metaclass = 0;

fts_symbol_t sym_showObject = 0;
fts_symbol_t sym_stopWaiting = 0;

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

/*************************************************************
 *
 *  inlet and outlet utilities
 *
 */

struct patcher_inout
{
  fts_object_t o;
  fts_channel_t channel;
  fts_patcher_t *patcher;
  int index;
  int position;
};

static int 
patcher_inouts_get_free_index(patcher_inout_t **inouts, int n)
{
  int i;

  for(i=0; i<n; i++)
    if(inouts[i] == NULL)
      return i;

  return i;
}

static void
patcher_inouts_shift_right(patcher_inout_t **inouts, int n, int index)
{
  int i;
  
  for(i=n-1; i>index; i--)
    {
      /* shift right and change index */
      inouts[i] = inouts[i - 1];
      inouts[i]->index = i;
    }

  /* make hole */
  inouts[index] = NULL;
}

static int
patcher_inouts_get_index_by_position(patcher_inout_t **inouts, int n, int position)
{
  int left_x = INT_MIN;
  int index = 0;
  int i;
  
  /* look for the right index regarding the x position */
  while(index < n && inouts[index] != NULL)
    {
      int right_x = inouts[index]->position;
      
      if(position >= left_x && position < right_x)
	break;
      
      /* pass on to next inout */
      left_x = right_x;
      index++;
    }
  
  return index;
}

static void 
patcher_redefine_number_of_inlets(fts_patcher_t *this, int n)
{
  fts_object_t *o = (fts_object_t *)this;
  fts_atom_t a;
  int i;
  
  this->inlets = (patcher_inout_t **)fts_realloc(this->inlets, n * sizeof(patcher_inout_t *));
  
  /* init new inlets */
  for(i=this->n_inlets; i<n; i++)
    this->inlets[i] = NULL;
  
  /* delete all the connections that will not be pertinent any more */
  fts_object_set_inlets_number(o, n);
  
  fts_set_int(&a, n);
  
  /* update gui */
  if (fts_object_has_id((fts_object_t *)this))
    fts_client_send_message((fts_object_t *)this, fts_s_n_inlets, 1, &a);
  
  this->n_inlets = n;
}

static void 
patcher_redefine_number_of_outlets(fts_patcher_t *this, int n)
{
  fts_object_t *o = (fts_object_t *)this;
  fts_atom_t a;
  int i;
  
  this->outlets = (patcher_inout_t **)fts_realloc(this->outlets, n * sizeof(patcher_inout_t *));
  
  /* init new outlets */
  for(i=this->n_outlets; i<n; i++)
    this->outlets[i] = NULL;
  
  /* delete all the connections that will not be pertinent any more */
  fts_object_set_outlets_number(o, n);
  
  fts_set_int(&a, n);
  
  /* update gui */
  if (fts_object_has_id((fts_object_t *)this))
    fts_client_send_message((fts_object_t *)this, fts_s_n_outlets, 1, &a);
  
  this->n_outlets = n;
}

/* remove the last empty inlets in a patcher */
static void 
patcher_trim_number_of_inlets(fts_patcher_t *patcher)
{
  int n_inlets = patcher->n_inlets;
     
  while (n_inlets > 0 && patcher->inlets[n_inlets - 1] == NULL)
    n_inlets--;

  if(n_inlets < patcher->n_inlets)
    patcher_redefine_number_of_inlets(patcher, n_inlets);
}

/* remove the last empty inlets in a patcher */
static void 
patcher_trim_number_of_outlets(fts_patcher_t *patcher)
{
  int n_outlets = patcher->n_outlets;

  while (n_outlets > 0 && (patcher->outlets[n_outlets - 1] == NULL))
    n_outlets--;

  if(n_outlets < patcher->n_outlets)
    patcher_redefine_number_of_outlets(patcher, n_outlets);
}

/*************************************************************
 *
 *  patcher inlet/outlet class
 *
 */

/* input of patcher outlet */
static void 
patcher_inout_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  patcher_inout_t *this  = (patcher_inout_t *) o;

  fts_outlet_send((fts_object_t *)this->patcher, this->index, s, ac, at);
}

static void
patcher_inout_add_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *this = (patcher_inout_t *) o;

  fts_channel_add_target(&this->channel, fts_get_object(at));
}

static void
patcher_inout_remove_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *this = (patcher_inout_t *) o;

  fts_channel_remove_target(&this->channel, fts_get_object(at));
}

static void 
patcher_inout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *this  = (patcher_inout_t *) o;

  fts_channel_init(&this->channel);
  this->patcher = NULL;
  this->index = 0;
  this->position = 0;
}

static void 
patcher_inout_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *this = (patcher_inout_t *)o;
  fts_patcher_t *patcher = this->patcher;

  /* remove from inlets or outlets array in patcher and trim in or outlets */
  if(patcher != NULL)
    {
      int i;

      /* search in inlet array */
      for(i=0; i<patcher->n_inlets; i++)
	{
	  if(patcher->inlets[i] == this)
	    {
	      patcher->inlets[i] = NULL;
	      
	      if (!patcher->deleted)
		patcher_trim_number_of_inlets(patcher);
	      
	      return;
	    }
	}
 
      /* search in outlet array */
      for(i=0; i<patcher->n_outlets; i++)
	{
	  if(patcher->outlets[i] == this)
	    {
	      patcher->outlets[i] = NULL;
	      
	      if (!patcher->deleted)
		patcher_trim_number_of_outlets(patcher);
	      
	      return;
	    }
	}
    }
}

static void
patcher_inout_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *this = (patcher_inout_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  fts_patcher_t *patcher = this->patcher;
  
  propagate_fun(propagate_context, (fts_object_t *)patcher, this->index);
}

static void
patcher_inout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(patcher_inout_t), patcher_inout_init, patcher_inout_delete);

  fts_class_message_varargs(cl, fts_s_propagate_input, patcher_inout_propagate_input);

  fts_class_message_varargs(cl, fts_s_input, patcher_inout_input);
  fts_class_message_varargs(cl, fts_s_add_listener, patcher_inout_add_listener);
  fts_class_message_varargs(cl, fts_s_remove_listener, patcher_inout_remove_listener);
}

static void
patcher_insert_inlet_hole(fts_patcher_t *patcher, int index)
{
  /* add an inlet */
  patcher_redefine_number_of_inlets(patcher, patcher->n_inlets + 1);
  
  /* make place for new inlet at given index */
  patcher_inouts_shift_right(patcher->inlets, patcher->n_inlets, index);
}

static void
patcher_insert_outlet_hole(fts_patcher_t *patcher, int index)
{
  /* add an outlet */
  patcher_redefine_number_of_outlets(patcher, patcher->n_outlets + 1);
  
  /* make place for new outlet at given index */
  patcher_inouts_shift_right(patcher->outlets, patcher->n_outlets, index);
}

static patcher_inout_t *
patcher_get_inlet(fts_patcher_t *patcher, int index)
{
  /* get free index if index is not specified */
  if(index < 0)
    index = patcher_inouts_get_free_index(patcher->inlets, patcher->n_inlets);

  /* extend array of inlets if needed */
  if(index >= patcher->n_inlets)
    patcher_redefine_number_of_inlets(patcher, index + 1);

  /* create inlet if needed */
  if(patcher->inlets[index] == NULL)
    {
      patcher_inout_t *inlet = (patcher_inout_t *)fts_object_create(patcher_inout_metaclass, 0, 0);
      
      inlet->patcher = patcher;
      inlet->index = index;

      patcher->inlets[index] = inlet;
    }

  return patcher->inlets[index];
}

static patcher_inout_t *
patcher_get_outlet(fts_patcher_t *patcher, int index)
{
  /* get free index if index is not specified */
  if(index < 0)
    index = patcher_inouts_get_free_index(patcher->outlets, patcher->n_outlets);

  /* extend array of outlets if needed */
  if(index >= patcher->n_outlets)
    patcher_redefine_number_of_outlets(patcher, index + 1);
  
  /* create outlet if needed */
  if(patcher->outlets[index] == NULL)
    {
      patcher_inout_t *outlet = (patcher_inout_t *)fts_object_create(patcher_inout_metaclass, 0, 0);
      
      outlet->patcher = patcher;
      outlet->index = index;

      patcher->outlets[index] = outlet;
    }

  return patcher->outlets[index];  
}

/*************************************************************
 *
 *  generic receicve/send objects
 *
 */

typedef struct
{
  fts_object_t o;
  fts_object_t *obj; /* source object */
} fts_receive_t;

static void
receive_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_receive_t *this = (fts_receive_t *) o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  fts_object_t *obj = NULL;

  if(ac == 0 || fts_is_number(at))
    {
      /* patcher inlet */
      int index = -1;
      int position = 0;
      patcher_inout_t *inlet;

      if(ac > 0)
	index = fts_get_number_int(at);
      
      if(index == -2)
	{
	  /* .pat support */
	  fts_atom_t ax;
	  int x;

	  /* get x position */
	  fts_object_get_prop(o, fts_s_x, &ax);
	  x = fts_get_int(&ax);

	  /* look for index regarding the x position */
	  index = patcher_inouts_get_index_by_position(patcher->inlets, patcher->n_inlets, x);

	  /* insert hole at index */
	  patcher_insert_inlet_hole(patcher, index);
	}

      inlet = patcher_get_inlet(patcher, index);
      inlet->position = position;

      obj = (fts_object_t *)inlet;
    }
  else if(ac == 1 && fts_is_symbol(at))
    {
      /* symbol: implicit label */
      fts_symbol_t name = fts_get_symbol(at);
      fts_label_t *label = fts_label_get_or_create(patcher, name);

      fts_variable_add_user(patcher, name, o);
      obj = (fts_object_t *)label;
    }
  else if(ac == 1 && fts_is_object(at))
    obj = fts_get_object(at);

  if(obj)
    {
      fts_atom_t a;

      /* add receive as listener of object */
      fts_set_object(&a, o);
      fts_send_message(obj, fts_s_add_listener, 1, &a);
      
      /* claim object */
      this->obj = obj;
      fts_object_refer(obj);
    }
  else
    fts_object_set_error(o, "Bad argument");
}

static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_receive_t *this = (fts_receive_t *) o;

  if(this->obj)
    {
      fts_atom_t a;
      
      /* remove receive as listener of object */
      fts_set_object(&a, o);
      fts_send_message(this->obj, fts_s_remove_listener, 1, &a);
      
      /* release object (also destroys patcher inlet) */
      fts_object_release(this->obj);
    }
}

static void 
receive_spost_description(fts_object_t *o, int wreceive, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_receive_t *this  = (fts_receive_t *)o;
  fts_metaclass_t *mcl = fts_object_get_metaclass(this->obj);

  if(mcl == patcher_inout_metaclass)
    {
      /* send inlet index as object description */
      patcher_inout_t *inout = (patcher_inout_t *)this->obj;
      fts_atom_t a;
      
      fts_set_int(&a, inout->index);
      fts_spost_object_description_args((fts_bytestream_t *)fts_get_object(at), 1, &a);
    }
  else
    fts_spost_object_description_args((fts_bytestream_t *)fts_get_object(at), o->argc - 1, o->argv + 1);
}

static void
receive_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_receive_t *this = (fts_receive_t *) o;
  fts_metaclass_t *mcl = fts_object_get_metaclass(this->obj);
  FILE *file = (FILE *)fts_get_pointer(at);
  fts_atom_t xa, ya, wa;
  
  fts_object_get_prop(o, fts_s_x, &xa);
  fts_object_get_prop(o, fts_s_y, &ya);
  fts_object_get_prop(o, fts_s_width, &wa);
  
  if(mcl == patcher_inout_metaclass)
    fprintf( file, "#P inlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
  else if(mcl == fts_label_metaclass)
    {
      /* save r object */
    }
}

static void
receive_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_receive_t),  receive_init, receive_delete);

  fts_class_message_varargs(cl, fts_s_save_dotpat, receive_save_dotpat);
  fts_class_message_varargs(cl, fts_s_spost_description, receive_spost_description); 
  
  fts_class_outlet_anything(cl, 0);
}

typedef struct
{
  fts_object_t o;
  fts_object_t *obj; /* target object */
  fts_method_t meth; /* target method */
} fts_send_t;

static void
send_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *this = (fts_send_t *) o;
  
  this->meth(this->obj, fts_system_inlet, s, ac, at);
}

static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *this = (fts_send_t *) o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  fts_object_t *obj = NULL;
  fts_method_t meth = NULL;

  if(ac == 0 || fts_is_number(at))
    {
      /* patcher outlet */
      int index = -1;
      int position = 0;
      patcher_inout_t *outlet;

      if(ac > 0)
	index = fts_get_number_int(at);
      
      if(index == -2)
	{
	  /* .pat support */
	  fts_atom_t ax;
	  int x;

	  /* get x position */
	  fts_object_get_prop(o, fts_s_x, &ax);
	  x = fts_get_int(&ax);

	  /* look for index regarding the x position */
	  index = patcher_inouts_get_index_by_position(patcher->outlets, patcher->n_outlets, x);

	  /* insert hole at index */
	  patcher_insert_outlet_hole(patcher, index);
	}

      outlet = patcher_get_outlet(patcher, index);
      outlet->position = position;

      obj = (fts_object_t *)outlet;
    }
  else if(ac == 1 && fts_is_symbol(at))
    {
      /* symbol: implicit label */
      fts_symbol_t name = fts_get_symbol(at);
      fts_label_t *label = fts_label_get_or_create(patcher, name);

      fts_variable_add_user(patcher, name, o);
      obj = (fts_object_t *)label;
    }
  else if(ac == 1 && fts_is_object(at))
    obj = fts_get_object(at);

  if(obj)
    {
      meth = fts_class_get_method(fts_object_get_class(obj), fts_s_input);
      
      if(!meth)
	{
	  fts_object_set_error(o, "Cannot connect to object");
	  return;
	}
      
      /* set object and method */
      this->obj = obj;
      this->meth = meth;
      
      /* claim object */
      fts_object_refer(obj);
    }
  else
    fts_object_set_error(o, "Bad argument");
}

static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *this = (fts_send_t *) o;

  /* simply release the object */
  if(this->obj)
    fts_object_release(this->obj);
}

static void
send_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *this = (fts_send_t *) o;
  fts_metaclass_t *mcl = fts_object_get_metaclass(this->obj);
  FILE *file = (FILE *)fts_get_pointer(at);
  fts_atom_t xa, ya, wa;
  
  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);
  
  if(mcl == patcher_inout_metaclass)
    fprintf( file, "#P outlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
  else
    {
      /* save s object */
    }
}

static void
send_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *this  = (fts_send_t *)o;

  fts_send_message(this->obj, fts_s_propagate_input, ac, at);
}

static void
send_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_send_t), send_init, send_delete);

  fts_class_message_varargs(cl, fts_s_propagate_input, send_propagate_input); 

  fts_class_message_varargs(cl, fts_s_save_dotpat, send_save_dotpat); 
  fts_class_message_varargs(cl, fts_s_spost_description, receive_spost_description); 

  fts_class_set_default_handler(cl, send_input);
  fts_class_inlet_anything(cl, 0);
  fts_dsp_declare_inlet(cl, 0);
  }

/*************************************************************
 *
 *  patcher class
 *
 *  the patcher send messages it receive on the inlets to the inlets outlets,
 *  output messages are directly send to the patcher outlets by the outlet objects
 */
static void
patcher_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_patcher_t *this  = (fts_patcher_t *) o;

  if(winlet < this->n_inlets)
    {
      patcher_inout_t *inlet = this->inlets[winlet];
      
      if(inlet)
	{
	  fts_objectlist_t *targets = fts_channel_get_targets(&inlet->channel);
	  fts_objectlist_cell_t *p;
	  
	  /* send input directly from output of inlet (receive) objects */
	  for (p=fts_objectlist_get_head(targets); p; p=fts_objectlist_get_next(p))
	    fts_outlet_send(fts_objectlist_get_object(p), 0, s, ac, at);
	}
    }
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
      fts_send_message(p, s, ac, at);

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (! fts_object_is_patcher(p))
      fts_send_message(p, s, ac, at);

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
      fts_client_send_message((fts_object_t *)this, fts_s_destroyEditor, 0, 0);  
    }
  
  p = this->objects;
  while (p)
    {
      fts_send_message(p, fts_s_closeEditor, 0, 0);      
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
	  if (!fts_object_is_error(p))
	    {
	      fts_method_t find_method = fts_class_get_method(fts_object_get_class(p), fts_s_find);

	      /* send the find message to the object; if the object do not implement it, do the standard check */
	      if(find_method != NULL)
		(*find_method)(p, fts_system_inlet, fts_s_find, ac, at);
	      else if (fts_atom_is_subsequence(ac - 1, at + 1, p->argc, p->argv))
		fts_objectset_add(set, p);
	    }
	  else if (fts_atom_is_subsequence(ac - 1, at + 1, p->argc, p->argv))
	    fts_objectset_add(set, p);
	}
    }

  /* do the recursive calls  */
  for (p = this->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_standard_patcher(p) ||
	  fts_object_is_template(p) ||
	  fts_object_is_abstraction(p))
	fts_send_message(p, s, ac, at);
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
	fts_send_message(p, s, ac, at);
    }
}

static void
patcher_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  int n = fts_get_int(at + 2);
  patcher_inout_t *inlet = this->inlets[n];

  if(inlet)
    {
      fts_objectlist_t *targets = fts_channel_get_targets(&inlet->channel);
      fts_objectlist_cell_t *p;
      
      for(p=fts_objectlist_get_head(targets); p; p=fts_objectlist_get_next(p))
	propagate_fun(propagate_context, fts_objectlist_get_object(p), 0);
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
  fts_patcher_t *this = (fts_patcher_t *) o;
  int n_inlets = fts_get_int_arg(ac, at, 0, 0);
  int n_outlets = fts_get_int_arg(ac, at, 1, 0);
  fts_atom_t va;
  int i;
  
  this->inlets = NULL;
  this->outlets = NULL;

  this->n_inlets = 0;
  this->n_outlets = 0;

#ifndef REIMPLEMENTING_VARIABLES
  fts_env_init(&this->env, (fts_object_t *) this);
#endif

  fts_patcher_set_standard(this);

  /* Define the "args" variable */
  this->args = (fts_tuple_t *)fts_object_create(fts_tuple_metaclass, ac, at);
  fts_object_refer(this->args);

#ifndef REIMPLEMENTING_VARIABLES
  fts_variable_define( this, fts_s_args);
  fts_set_object( &va, (fts_object_t *)this->args);
  fts_variable_restore(this, fts_s_args, &va, o);
#endif

  patcher_redefine_number_of_inlets(this, n_inlets);
  patcher_redefine_number_of_outlets(this, n_outlets);

  this->objects = (fts_object_t *)0;
  this->open = 0; /* start as closed */
  this->editor_open = 0; /* start with editor closed */  
  this->deleted = 0;
  this->dirty = 0; /* start as saved */
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

#ifndef REIMPLEMENTING_VARIABLES
  fts_variables_undefine(this, (fts_object_t *)this);
#endif

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
  fts_object_t *o;
  fts_patcher_t *patcher = fts_object_get_patcher(object);
  int index = 0;
  
  for(o=patcher->objects; o; o=o->next_in_patcher)
    {
      if (object == o)
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
		&& fts_class_get_method( fts_object_get_class(object), fts_s_save_dotpat) != NULL)
	{
 	  fts_atom_t a;

	  fts_set_pointer( &a, file);
	  fts_send_message( object, fts_s_save_dotpat, 1, &a);
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
	      if(fts_connection_get_type(c) > fts_c_hidden) /* don't save hidden connections */
		{
		  fprintf( file, "#P connect %d %d %d %d;\n", 
			   patcher_object_count - 1 - object_index,
			   outlet, 
			   patcher_object_count - 1 - find_object_index_in_patcher( c->dst),
			   c->winlet);
		}
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

/* 
 * save a patcher in .pat format:
 *
 * (note: this function does not check for nor add a .pat suffix to the file name,
 * it is left to the user interface)
*/

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

/***********************************************************************
 *
 * Properties
 *
 */

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

static void 
patcher_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if( !fts_object_is_error( o))
    {
      if( fts_patcher_is_template( (fts_patcher_t *)o) || fts_patcher_is_abstraction( (fts_patcher_t *)o))
	fts_spost_object_description( (fts_bytestream_t *)fts_get_object(at), o);
      else
	fts_spost_object_description_args( (fts_bytestream_t *)fts_get_object(at), o->argc-1, o->argv+1);
    }  
  else
    fts_spost_object_description( (fts_bytestream_t *)fts_get_object(at), o);
}

static void 
patcher_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post("coucou!!!\n");
}

/*
 * The remote functions
 */
static void fts_patcher_start_updates( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;
  fts_object_t *p;

  this->open = 1;

  for(p = this->objects; p ; p = p->next_in_patcher)
    {
      if(fts_class_get_method( fts_object_get_class(p), fts_s_update_real_time) != NULL)
	fts_update_request(p);
    }
}

static void fts_patcher_stop_updates( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *) o;

  this->open = 0;
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

static fts_patcher_t * get_patcher_by_file_name( fts_symbol_t file_name)
{
  fts_patcher_t *root = fts_get_root_patcher();  
  fts_object_t *p = 0;

  for (p = root->objects; p ; p = p->next_in_patcher)
    {
      if (fts_object_is_patcher(p))
	{
	  fts_patcher_t *patcher = (fts_patcher_t *)p;
	  fts_symbol_t patch_name = fts_patcher_get_file_name(patcher);
	  
	  if(file_name == patch_name)
	    return patcher;
	}
    }

  return 0;
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
  const char *help_name;
  char path[256];

  help_name = fts_package_get_help( pkg, class_name);
  if( help_name)
    snprintf(path, 256, "%s%c%s%c%s", dir, fts_file_separator, "help", fts_file_separator, help_name);
  else
    snprintf(path, 256, "%s%c%s%c%s%s", dir, fts_file_separator, "help", fts_file_separator, class_name, ".help.jmax");
  
  file_name = fts_new_symbol(path);

  fts_log("[patcher] help %s\n", file_name);

  help_patch = get_patcher_by_file_name(file_name);

  if(help_patch)
    patcher_open_editor((fts_object_t *)help_patch, 0, 0, 0, 0);
  else
    help_patch = fts_client_load_patcher(file_name, fts_get_client_id(o));

  if(!help_patch)
    fts_client_send_message(o, sym_noHelp, 1, at); 
}

static void 
fts_patcher_add_object_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = fts_eval_object_description((fts_patcher_t *)o, ac - 2, at + 2);
  fts_atom_t a[1];

  fts_object_put_prop(obj, fts_s_x, at);
  fts_object_put_prop(obj, fts_s_y, at+1);

  fts_set_int(a, -1);
  fts_object_put_prop(obj, fts_s_layer, a);

  fts_client_upload_object(obj, -1);

  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void 
fts_patcher_upload_child( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_upload_object( o, fts_get_object(&at[0]));
}

static fts_memorystream_t *patcher_memory_stream ;

static fts_memorystream_t * patcher_get_memory_stream()
{
  if(!patcher_memory_stream)
    patcher_memory_stream = (fts_memorystream_t *)fts_object_create( fts_memorystream_type, 0, 0);
  return patcher_memory_stream;
}

void 
fts_patcher_upload_object(fts_object_t *this, fts_object_t *obj)
{
  if(fts_object_get_class_name(obj) == fts_s_connection)
    {
      fts_connection_t *conn = (fts_connection_t *)obj;

      /* don't upload hidden connections */
      if(fts_connection_get_type(conn) <= fts_c_hidden)
	return;
      
      fts_client_start_message( this, sym_addConnection);
      fts_client_add_int( this, fts_get_object_id(obj));
      fts_client_add_object( this, conn->src);
      fts_client_add_int( this, conn->woutlet);
      fts_client_add_object( this, conn->dst);
      fts_client_add_int( this, conn->winlet);
      fts_client_add_int( this, conn->type);
      fts_client_done_message( this);
    }
  else
    {
      fts_method_t spost_method = fts_class_get_method(fts_object_get_class(obj), fts_s_spost_description);
      fts_memorystream_t *stream;
      fts_atom_t a[9];
      fts_atom_t b[1];

      fts_object_get_prop(obj, fts_s_x, a);
      fts_object_get_prop(obj, fts_s_y, a+1);
      fts_object_get_prop(obj, fts_s_width, a+2);
      fts_object_get_prop(obj, fts_s_height, a+3);

      fts_set_int(a+4, obj->n_inlets);
      fts_set_int(a+5, obj->n_outlets);

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
      
      fts_client_add_int( this, fts_object_is_template(obj));
      
      stream = patcher_get_memory_stream();
      fts_memorystream_reset( stream);
      
      fts_set_object( b, stream);

      if(spost_method != NULL)
	(*spost_method)(obj, fts_system_inlet, fts_s_spost_description, 1, b);
      else
	fts_spost_object_description((fts_bytestream_t *)stream, obj);

      fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
      fts_client_add_string( this, fts_memorystream_get_bytes( stream));
      
      fts_client_done_message( this);
      
      fts_object_get_prop(obj, fts_s_font, a);
      
      if( fts_get_symbol(a))
	{
	  fts_object_get_prop(obj, fts_s_fontSize, a+1);
	  fts_object_get_prop(obj, fts_s_fontStyle, a+2);
	  fts_client_send_message(obj, fts_s_setFont, 3, a);
	}

      /* send gui properties */
      fts_send_message(obj, fts_s_update_gui, 0, 0);

      /* add to real time update list */
      if(fts_class_get_method( fts_object_get_class(obj), fts_s_update_real_time) != NULL)
	fts_update_request(obj);
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
  fts_object_t *oldobj = fts_get_object(&at[0]);
  fts_object_t *obj = fts_object_redefine(oldobj, ac - 1, at + 1);
  fts_atom_t a[1];
  int do_var = 0;

  fts_client_upload_object(obj, -1);
  
  fts_object_upload_connections(obj);
  
  fts_set_object(a, obj);
  fts_client_send_message(o, sym_objectRedefined, 1, a);

  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void 
fts_patcher_redefine_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *this = (fts_patcher_t *)o;
  fts_memorystream_t *stream;  
  fts_atom_t a[1];

  if (ac >= 0)
    {
      fts_atom_t argv[512];
      int argc;

#ifndef REIMPLEMENTING_VARIABLES
      if (fts_object_description_defines_variable(ac, at))
	{
	  fts_set_symbol(&argv[0], fts_s_patcher); 

	  /* copy arguments (ignoring variable) */
	  for (argc = 1; (argc < ac - 1) && (argc < 512) ; argc++) 
	    argv[argc] = at[argc+1];
	}
      else
#endif
	{
	  /* Plain syntax */
	  fts_set_symbol(&argv[0], fts_s_patcher);

	  for (argc = 1; (argc <= ac) && (argc < 512) ; argc++)
	    argv[argc] = at[argc-1];
	}

      fts_patcher_redefine(this, argc, argv);

      stream = patcher_get_memory_stream();
      fts_memorystream_reset( stream);
      fts_spost_object_description_args( (fts_bytestream_t *)stream, argc - 1, argv + 1);
      fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
      fts_set_string( a,  fts_memorystream_get_bytes( stream));
      fts_client_send_message((fts_object_t *)this, sym_setDescription, 1, a);

      fts_patcher_set_dirty( this, 1);
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
	  obj = fts_get_object(at + i);

	  if (obj)
	    {
	      if (fts_object_has_id(obj))
		{
		  fts_update_reset(obj);
		  
		  fts_send_message( obj, fts_s_closeEditor, 0, 0);   

		  fts_client_release_object(obj);
		  fts_object_set_id(obj, FTS_DELETE);
		}
	    }
	  else
	    {
	      fts_log("[patcher] delete_objects_from_client: System Error deleting a non existing object\n");
	      return;
	    }
	}
      

      for(i=0; i<ac; i++)
	{
	  obj = fts_get_object(at + i);

	  if (obj)
	    fts_object_delete_from_patcher(obj);
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
	  connection = fts_connection_new(from, outlet, to, inlet, fts_c_anything);
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
  fts_send_message( clipboard, fts_s_paste, 1, a);   

  /* upload all the not uploaded objects */

  fts_client_send_message((fts_object_t *)this, sym_startPaste, 0, 0);

  for (p = this->objects; p ; p = p->next_in_patcher)
    if (!fts_object_has_id(p))
      {
	/* shift object during paste */
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
	    {
	      if( !fts_object_has_id( (fts_object_t *)c))
		{
		  ((fts_object_t *)c)->patcher = this;
		  fts_client_upload_object((fts_object_t *)c, -1);
		}
	    }
	}
    }
  
  fts_client_send_message((fts_object_t *)this, sym_endPaste, 0, 0);
}

/****************************************************************/
/****************************************************************/

/* daemon for setting the number of inlets */
static void
patcher_set_ninlets(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_t *this = (fts_patcher_t *)obj;
  int n_inlets = fts_get_int(value);

  if(n_inlets != this->n_inlets)
    patcher_redefine_number_of_inlets((fts_patcher_t *)obj, n_inlets);
}

/* daemon for setting the number of outlets */
static void
patcher_set_noutlets(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  fts_patcher_t *this = (fts_patcher_t *)obj;
  int n_outlets = fts_get_int(value);

  if(n_outlets != this->n_outlets)
    patcher_redefine_number_of_outlets((fts_patcher_t *)obj, n_outlets);
}

static void
patcher_instantiate(fts_class_t *cl)
{
  /* initialize the class */
  fts_class_init(cl, sizeof(fts_patcher_t), patcher_init, patcher_delete);

  fts_class_message_varargs(cl, fts_s_upload, fts_patcher_upload);
  fts_class_message_varargs(cl, fts_s_upload_child, fts_patcher_upload_child);

  fts_class_message_varargs(cl, fts_s_find, patcher_find);
  fts_class_message_varargs(cl, fts_s_find_errors, patcher_find_errors);

  fts_class_message_varargs(cl, fts_s_propagate_input, patcher_propagate_input);

  fts_class_message_varargs(cl, fts_new_symbol("load_init"), patcher_load_init); 
  fts_class_message_varargs(cl, fts_new_symbol("open_help_patch"), fts_patcher_open_help_patch); 
  fts_class_message_varargs(cl, fts_s_save, fts_patcher_save_from_client); 
  fts_class_message_varargs(cl, fts_s_paste, fts_patcher_paste); 

  fts_class_message_varargs(cl, fts_s_openEditor, patcher_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, patcher_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, patcher_close_editor); 
  fts_class_message_varargs(cl, fts_new_symbol("show_object"), show_object);
  fts_class_message_varargs(cl, fts_new_symbol("stop_waiting"), stop_waiting);

  fts_class_message_varargs(cl, fts_new_symbol("start_updates"), fts_patcher_start_updates);
  fts_class_message_varargs(cl, fts_new_symbol("stop_updates"), fts_patcher_stop_updates);
  fts_class_message_varargs(cl, fts_new_symbol("patcher_update"), fts_patcher_update);
  fts_class_message_varargs(cl, fts_new_symbol("set_wx"), fts_patcher_set_wx);
  fts_class_message_varargs(cl, fts_new_symbol("set_wy"), fts_patcher_set_wy);
  fts_class_message_varargs(cl, fts_new_symbol("set_ww"), fts_patcher_set_ww);
  fts_class_message_varargs(cl, fts_new_symbol("set_wh"), fts_patcher_set_wh);

  fts_class_message_varargs(cl, fts_new_symbol("add_object"), fts_patcher_add_object_from_client);

  fts_class_message_varargs(cl, fts_new_symbol("delete_objects"), fts_patcher_delete_objects_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("add_connection"), fts_patcher_add_connection_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("delete_connection"), fts_patcher_delete_connection_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("redefine_object"), fts_patcher_redefine_object_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("redefine_patcher"), fts_patcher_redefine_from_client);

  fts_class_message_varargs(cl, fts_s_spost_description, patcher_spost_description); 

  fts_class_set_default_handler(cl, patcher_input);
  fts_class_outlet_anything(cl, 0);
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
 */

/* assign variables to a patcher as found by the expression parser */
void 
fts_patcher_assign_variable(fts_symbol_t name, fts_atom_t *value, void *data)
{
#ifndef REIMPLEMENTING_VARIABLES
  fts_patcher_t *this = (fts_patcher_t *)data;

  if(! fts_variable_is_suspended(this, name))
    fts_variable_define(this, name);

  fts_variable_restore(this, name, value, (fts_object_t *)this);
#endif
}

fts_patcher_t *
fts_patcher_redefine(fts_patcher_t *this, int aoc, const fts_atom_t *aot)
{
  fts_object_t *obj;
  int ac;
  fts_atom_t at[1024];
  int rac;
  const fts_atom_t *rat;
  fts_atom_t a;
#ifndef REIMPLEMENTING_VARIABLES
  fts_oldexpression_state_t *e;

  obj = (fts_object_t *) this; 

  /* change the patcher definition */
  fts_object_set_description(obj, aoc, aot);

  /* check for the "var : <obj> syntax" and ignore the variable if any */
  if (fts_object_description_defines_variable(aoc, aot))
    {
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

  /* suspend  the patcher internal variables if any */
  fts_variables_suspend(this, obj);

  /* eval the expression */
  e = fts_oldexpression_eval(obj->patcher, rac, rat, 1024, at);
  ac = fts_oldexpression_get_result_count(e);

  if (fts_oldexpression_get_status(e) != FTS_OLDEXPRESSION_OK)
    {
      /* undefine all the variables, and set the error property */
      fts_variables_undefine_suspended(this, obj);

      /* set error to expression error */
      fts_object_set_error(obj, fts_oldexpression_get_msg(e), fts_oldexpression_get_err_arg(e));
    }
  else
    {
      /* Set the error property to zero */
      fts_set_int(&a, 0);
      fts_object_put_prop(obj, fts_s_error, &a);

      /* reallocate the args */
      fts_tuple_set( this->args, ac - 1, at + 1);

      /* set the new variables */
      fts_oldexpression_map_to_assignements(e, fts_patcher_assign_variable, (void *) this);

      fts_set_object(&a, (fts_object_t *)this->args);
      fts_variable_restore(this, fts_s_args, &a, obj);

      /* register the patcher as user of the used variables */
      fts_oldexpression_add_variables_user(e, obj);

      /* undefine all the locals that are still suspended  */
      fts_variables_undefine_suspended(this, obj);
    }

  /* free the expression state structure */
  fts_oldexpression_state_free(e);

#endif

  return this;
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
  fts_object_refer(obj);
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


/* set a patch as dirty or as saved: if this patcher is a sub-patcher propagate the set_dirty to his father
 * until a firts level patche is reached.
 * A "setDirty" message is sent to the client after is_dirty flag changed
 */
void 
fts_patcher_set_dirty(fts_patcher_t *this, int is_dirty)
{
  if(this == fts_get_root_patcher()) return;

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
	      fts_client_send_message((fts_object_t *)this, fts_s_set_dirty, 1, a);
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
  sym_setDescription = fts_new_symbol("setDescription");
  sym_setSaved = fts_new_symbol("setSaved");
  sym_startPaste = fts_new_symbol("startPaste");
  sym_endPaste = fts_new_symbol("endPaste");
  sym_noHelp = fts_new_symbol("noHelp");

  patcher_metaclass = fts_class_install(fts_s_patcher, patcher_instantiate);
  patcher_inout_metaclass = fts_class_install(NULL, patcher_inout_instantiate);

  receive_metaclass = fts_class_install(fts_s_receive, receive_instantiate);
  send_metaclass = fts_class_install(fts_s_send, send_instantiate);
  fts_class_alias(receive_metaclass, fts_s_inlet);
  fts_class_alias(send_metaclass, fts_s_outlet);

  /* historical aliases */
  fts_class_alias(receive_metaclass, fts_new_symbol("r"));
  fts_class_alias(receive_metaclass, fts_new_symbol("r~"));
  fts_class_alias(send_metaclass, fts_new_symbol("s"));
  fts_class_alias(send_metaclass, fts_new_symbol("s~"));

  fts_create_root_patcher();
}

void fts_kernel_patcher_shutdown(void)
{
  fts_delete_root_patcher();
}
