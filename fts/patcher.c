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

#include <stdlib.h>
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
#include <ftsprivate/variable.h>

fts_class_t *patcher_class = 0;
static fts_class_t *patcher_inout_class = 0;
static fts_class_t *receive_class = 0;
static fts_class_t *send_class = 0;

static fts_symbol_t sym_set_title = 0;
static fts_symbol_t sym_showObject = 0;
static fts_symbol_t sym_stopWaiting = 0;
static fts_symbol_t sym_redefineStart = 0;
static fts_symbol_t sym_setWX = 0;
static fts_symbol_t sym_setWY = 0;
static fts_symbol_t sym_setWW = 0;
static fts_symbol_t sym_setWH = 0;
static fts_symbol_t sym_setPatcherBounds = 0;
static fts_symbol_t sym_addObject = 0;
static fts_symbol_t sym_addConnection = 0;
static fts_symbol_t sym_redefineConnection = 0;
static fts_symbol_t sym_redefineObject = 0;
static fts_symbol_t sym_objectRedefined = 0;
static fts_symbol_t sym_setRedefined = 0;
static fts_symbol_t sym_setDescription;
static fts_symbol_t sym_setSaved;
static fts_symbol_t sym_startPaste;
static fts_symbol_t sym_endPaste;
static fts_symbol_t sym_noHelp;

#define set_editor_open(q) ((q)->editor_open = 1)
#define set_editor_close(q) ((q)->editor_open = 0)
#define editor_is_open(q) ((q)->editor_open != 0)
#define fts_patcher_is_dirty(q)    ((q)->dirty != 0)

static fts_memorystream_t *patcher_memory_stream ;

/*************************************************************
*
*  patcher objects and utilities
*
*/

void
fts_patcher_add_object(fts_patcher_t *self, fts_object_t *obj)
{
  fts_object_t **p;

  /* create patcher data */
  fts_object_get_patcher_data(obj);
  
  /* add object to list of objects in patcher */
  for (p = &(self->objects); *p; p = &(fts_object_get_next_in_patcher(*p)))
    {
      if (*p == obj)
	return;
    }

  *p = obj;
  fts_object_set_next_in_patcher(obj, NULL);

  /* claim object */
  fts_object_refer(obj);
}

void
fts_patcher_remove_object(fts_patcher_t *self, fts_object_t *obj)
{
  fts_object_t **p;

  for (p = &(self->objects); *p; p = &(fts_object_get_next_in_patcher(*p)))
    {
      if (*p == obj)
	{
	  /* remove object from list */
	  *p = fts_object_get_next_in_patcher(obj);

	  fts_object_set_next_in_patcher(obj, NULL);

	  fts_object_release(obj);

	  return;
	}
    }
}

int
fts_patcher_get_objects_count(fts_patcher_t *self)
{
  int i = 0;
  fts_object_t *p;

  for (p = self->objects; p; p = fts_object_get_next_in_patcher(p))
    i++;

  return i;
}

/* set a patch as dirty or as saved */
void
fts_patcher_set_dirty(fts_patcher_t *self, int is_dirty)
{
  if (self == fts_get_root_patcher())
    return;

  if ( fts_object_get_patcher( (fts_object_t *)self) != fts_get_root_patcher()
       && !fts_object_is_template( (fts_object_t *)self))
  {
    fts_patcher_set_dirty( fts_object_get_patcher((fts_object_t *)self), is_dirty);
  }
  else
  {
    if (self->dirty != is_dirty)
    {
      self->dirty = is_dirty;

      if (editor_is_open(self))
      {
        fts_atom_t a[1];

        fts_set_int(&a[0], is_dirty);
        fts_client_send_message((fts_object_t *)self, fts_s_set_dirty, 1, a);
      }
    }
  }
}

void
fts_patcher_set_template( fts_patcher_t *self, fts_template_t *template)
{
  self->type = fts_p_template;
  self->template = template;
}

/*************************************************************
*
*  patcher inlet and outlet utilities
*
*/
struct patcher_inout
{
  fts_object_t o;
  fts_channel_t channel;
  fts_patcher_t *patcher;
  int index;
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

static void
patcher_redefine_number_of_inlets(fts_patcher_t *self, int n)
{
  fts_object_t *o = (fts_object_t *)self;
  fts_atom_t a;
  int i;

  if ( n > 0)
    self->inlets = (patcher_inout_t **)fts_realloc(self->inlets, n * sizeof(patcher_inout_t *));
  else
  {
    fts_free( self->inlets);
    self->inlets = NULL;
  }

  /* init new inlets */
  for(i=self->n_inlets; i<n; i++)
    self->inlets[i] = NULL;

  /* delete all the connections that will not be pertinent any more */
  fts_object_set_inlets_number(o, n);

  fts_set_int(&a, n);

  /* update gui */
  if (fts_object_has_id((fts_object_t *)self))
    fts_client_send_message((fts_object_t *)self, fts_s_n_inlets, 1, &a);

  self->n_inlets = n;
}

static void
patcher_redefine_number_of_outlets(fts_patcher_t *self, int n)
{
  fts_object_t *o = (fts_object_t *)self;
  fts_atom_t a;
  int i;

  if ( n > 0)
    self->outlets = (patcher_inout_t **)fts_realloc(self->outlets, n * sizeof(patcher_inout_t *));
  else
  {
    fts_free( self->outlets);
    self->outlets = NULL;
  }

  /* init new outlets */
  for(i=self->n_outlets; i<n; i++)
    self->outlets[i] = NULL;

  /* delete all the connections that will not be pertinent any more */
  fts_object_set_outlets_number(o, n);

  fts_set_int(&a, n);

  /* update gui */
  if (fts_object_has_id((fts_object_t *)self))
    fts_client_send_message((fts_object_t *)self, fts_s_n_outlets, 1, &a);

  self->n_outlets = n;
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
  patcher_inout_t *self  = (patcher_inout_t *) o;

  fts_outlet_send((fts_object_t *)self->patcher, self->index, s, ac, at);
}

static void
patcher_inout_add_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *self = (patcher_inout_t *) o;

  fts_channel_add_target(&self->channel, fts_get_object(at));
}

static void
patcher_inout_remove_listener(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *self = (patcher_inout_t *) o;

  fts_channel_remove_target(&self->channel, fts_get_object(at));
}

static void
patcher_inout_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *self  = (patcher_inout_t *) o;

  fts_channel_init(&self->channel);
  self->patcher = NULL;
  self->index = 0;
}

static void
patcher_inout_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  patcher_inout_t *self = (patcher_inout_t *)o;
  fts_patcher_t *patcher = self->patcher;

  /* remove from inlets or outlets array in patcher and trim in or outlets */
  if(patcher != NULL)
  {
    int i;

    /* search in inlet array */
    for(i=0; i<patcher->n_inlets; i++)
    {
      if(patcher->inlets[i] == self)
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
      if(patcher->outlets[i] == self)
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
  patcher_inout_t *self = (patcher_inout_t *)o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  fts_patcher_t *patcher = self->patcher;

  propagate_fun(propagate_context, (fts_object_t *)patcher, self->index);
}

static void
patcher_inout_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(patcher_inout_t), patcher_inout_init, patcher_inout_delete);

  fts_class_message_varargs(cl, fts_s_propagate_input, patcher_inout_propagate_input);

  fts_class_message_varargs(cl, fts_s_add_listener, patcher_inout_add_listener);
  fts_class_message_varargs(cl, fts_s_remove_listener, patcher_inout_remove_listener);

  fts_class_input_handler(cl, patcher_inout_input);
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
    patcher_inout_t *inlet = (patcher_inout_t *)fts_object_create(patcher_inout_class, 0, 0);

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
    patcher_inout_t *outlet = (patcher_inout_t *)fts_object_create(patcher_inout_class, 0, 0);

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
  fts_receive_t *self = (fts_receive_t *) o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  fts_object_t *obj = NULL;

  if(ac == 0 || fts_is_number(at))
  {
    patcher_inout_t *in;
    fts_atom_t a[3];

    /* patcher inlet */
    if(ac > 0)
      in = patcher_get_inlet(patcher, fts_get_number_int(at));
    else
      in = patcher_get_inlet(patcher, -1);

    fts_set_symbol(a + 0, fts_s_colon);
    fts_set_symbol(a + 1, fts_s_receive);
    fts_set_int(a + 2, in->index);
    fts_object_set_description(o, 3, a);

    obj = (fts_object_t *)in;
  }
  else if(ac == 1 && fts_is_symbol(at))
  {
    /* inlet label */
    fts_symbol_t name = fts_get_symbol(at);
    fts_label_t *label = fts_label_get_or_create(patcher, name);
    fts_atom_t a[3];

    fts_set_symbol(a + 0, fts_s_colon);
    fts_set_symbol(a + 1, fts_s_receive);
    fts_set_symbol(a + 2, fts_object_get_name((fts_object_t *)label));
    fts_object_set_description(o, 3, a);

    obj = (fts_object_t *)label;
  }
  else if(ac == 1 && fts_is_object(at))
    obj = fts_get_object(at);

  if(obj)
  {
    fts_atom_t a;

    /* add receive as listener of object */
    fts_set_object(&a, o);
    fts_send_message_varargs(obj, fts_s_add_listener, 1, &a);

    /* claim object */
    self->obj = obj;
    fts_object_refer(obj);
  }
  else
    fts_object_error(o, "bad argument");
}

static void
receive_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_receive_t *self = (fts_receive_t *) o;

  if(self->obj)
  {
    fts_atom_t a;

    /* remove receive as listener of object */
    fts_set_object(&a, o);
    fts_send_message_varargs(self->obj, fts_s_remove_listener, 1, &a);

    /* release object (also destroys patcher inlet) */
    fts_object_release(self->obj);
  }
}

static void
receive_spost_description(fts_object_t *o, int wreceive, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  int descr_ac = fts_object_get_description_size(o);
  fts_atom_t *descr_at = fts_object_get_description_atoms(o);
  
  fts_spost_object_description_args((fts_bytestream_t *)fts_get_object(at), descr_ac - 2, descr_at + 2);
}

static void
receive_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_receive_t *self = (fts_receive_t *) o;
  fts_class_t *cl = fts_object_get_class(self->obj);
  FILE *file = (FILE *)fts_get_pointer(at);
  fts_atom_t xa, ya, wa;

  fts_object_get_prop(o, fts_s_x, &xa);
  fts_object_get_prop(o, fts_s_y, &ya);
  fts_object_get_prop(o, fts_s_width, &wa);

  if(cl == patcher_inout_class)
    fprintf( file, "#P inlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
  else if(cl == fts_label_class)
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

  fts_class_outlet_thru(cl, 0);
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
  fts_send_t *self = (fts_send_t *) o;

  if(self->meth)
    self->meth(self->obj, fts_system_inlet, s, ac, at);
  else if(s == NULL)
    fts_send_message(self->obj, fts_s_send, ac, at);
  else
    fts_send_message(self->obj, s, ac, at);
}

static void
send_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *self = (fts_send_t *) o;
  fts_patcher_t *patcher = fts_object_get_patcher(o);
  fts_object_t *obj = NULL;
  fts_method_t meth = NULL;

  if(ac == 0 || fts_is_number(at))
  {
    patcher_inout_t *out;
    fts_atom_t a[3];

    /* patcher outlet */
    if(ac > 0)
      out = patcher_get_outlet(patcher, fts_get_number_int(at));
    else
      out = patcher_get_outlet(patcher, -1);

    fts_set_symbol(a + 0, fts_s_colon);
    fts_set_symbol(a + 1, fts_s_send);
    fts_set_int(a + 2, out->index);
    fts_object_set_description(o, 3, a);

    obj = (fts_object_t *)out;
  }
  else if(ac == 1 && fts_is_symbol(at))
  {
    /* outlet label */
    fts_symbol_t name = fts_get_symbol(at);
    fts_label_t *label = fts_label_get_or_create(patcher, name);
    fts_atom_t a[3];

    fts_set_symbol(a + 0, fts_s_colon);
    fts_set_symbol(a + 1, fts_s_send);
    fts_set_symbol(a + 2, fts_object_get_name((fts_object_t *)label));
    fts_object_set_description(o, 3, a);

    obj = (fts_object_t *)label;
  }
  else if(ac == 1 && fts_is_object(at))
    obj = fts_get_object(at);

  if(obj)
  {
    fts_class_t *cl = fts_object_get_class(obj);
    int varargs;

    /* try to get an input handler */
    meth = fts_class_get_input_handler(cl);

    if(!meth)
    {
      /* if there is no input handler try to get at least an atom method for "send"  */
      if(fts_class_get_method(cl, fts_s_send, NULL, &varargs) == NULL)
      {
        fts_object_error(o, "cannot connect to %s object", fts_class_get_name(cl));
        return;
      }
    }
    
    /* set object and method */
    self->obj = obj;
    self->meth = meth;

    /* claim object */
    fts_object_refer(obj);
  }
  else
    fts_object_error(o, "bad argument");
}

static void
send_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *self = (fts_send_t *) o;

  /* simply release the object */
  if(self->obj)
    fts_object_release(self->obj);
}

static void
send_save_dotpat(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *self = (fts_send_t *) o;
  fts_class_t *cl = fts_object_get_class(self->obj);
  FILE *file = (FILE *)fts_get_pointer(at);
  fts_atom_t xa, ya, wa;

  fts_object_get_prop( o, fts_s_x, &xa);
  fts_object_get_prop( o, fts_s_y, &ya);
  fts_object_get_prop( o, fts_s_width, &wa);

  if(cl == patcher_inout_class)
    fprintf( file, "#P outlet %d %d %d;\n", fts_get_int( &xa), fts_get_int( &ya), fts_get_int( &wa));
  else
  {
    /* save s object */
  }
}

static void
send_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_send_t *self  = (fts_send_t *)o;

  fts_send_message_varargs(self->obj, fts_s_propagate_input, ac, at);
}

static void
send_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_send_t), send_init, send_delete);

  fts_class_message_varargs(cl, fts_s_propagate_input, send_propagate_input);

  fts_class_message_varargs(cl, fts_s_save_dotpat, send_save_dotpat);
  fts_class_message_varargs(cl, fts_s_spost_description, receive_spost_description);

  fts_class_input_handler(cl, send_input);
  fts_class_inlet_thru(cl, 0);
}

/*************************************************************
*
*  patcher methods
*
*/
static void
patcher_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  fts_patcher_t *self  = (fts_patcher_t *) o;

  if(winlet < self->n_inlets)
  {
    patcher_inout_t *inlet = self->inlets[winlet];

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

static void
patcher_set_arguments( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  fts_memorystream_t *stream = patcher_memory_stream;
  fts_atom_t a;
  int i;

  /* set description */
  self->description_ac = ac;
  self->description_at = fts_realloc(self->description_at, sizeof(fts_atom_t) * ac);

  for(i=0; i<ac; i++)
    self->description_at[i] = at[i];

  /* update string at patcher */
  if(fts_object_has_id(o))
  {
    fts_memorystream_reset( stream);
    fts_spost_object_description_args( (fts_bytestream_t *)stream, ac, (fts_atom_t *)at);
    fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
    fts_set_string(&a,  fts_memorystream_get_bytes( stream));
    fts_client_send_message((fts_object_t *)self, sym_setDescription, 1, &a);

    fts_patcher_set_dirty(self, 1);
  }
}

static void
patcher_load_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_object_t *p;

  if (self->load_init_fired)
    return;

  self->load_init_fired = 1;

  /* send load_init to patcher objects */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
    if (fts_object_is_patcher(p))
      fts_send_message_varargs(p, s, ac, at);

  /* send load_init to all simple objects */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
    if (!fts_object_is_patcher(p))
      fts_send_message_varargs(p, s, ac, at);
}

static void
patcher_loaded(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;

  self->file_name = fts_get_symbol( at);
  patcher_load_init( o, 0, 0, 0, 0);
}

static void
patcher_open_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  set_editor_open(self);
  fts_client_send_message(o, fts_s_openEditor, 0, 0);
}

static void
patcher_destroy_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  set_editor_close(self);
}

static void
patcher_close_editor(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_object_t *p;

  if(editor_is_open(self))
  {
    set_editor_close(self);
    fts_client_send_message((fts_object_t *)self, fts_s_destroyEditor, 0, 0);
  }

  p = self->objects;
  while (p)
  {
    fts_send_message_varargs(p, fts_s_closeEditor, 0, 0);
    p = fts_object_get_next_in_patcher(p);
  }
}

/* tool panel support */
static void
patcher_show_object(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  fts_object_t *obj = fts_get_object(at);

  if(fts_object_is_in_patcher(obj, self))
  {
    if(!editor_is_open(self))
      set_editor_open(self);

    fts_client_send_message(o, fts_s_openEditor, 0, 0);
    fts_client_send_message(o, sym_showObject, 1, at);
  }
}

static void
patcher_stop_waiting(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_client_send_message(o, sym_stopWaiting, 0, 0);
}

fts_patcher_t * 
fts_patcher_get_by_file_name( fts_symbol_t file_name)
{
  fts_patcher_t *root = fts_get_root_patcher();
  fts_object_t *p = 0;

  for (p = root->objects; p ; p = fts_object_get_next_in_patcher(p))
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
patcher_open_help_patch( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = fts_get_object(&at[0]);
  fts_package_t *pkg = fts_object_get_package(obj);
  fts_symbol_t dir;
  fts_symbol_t class_name;
  fts_symbol_t file_name;
  fts_patcher_t *help_patch;
  const char *help_name;
  char path[256];

  if( fts_object_is_error( obj) && (pkg == NULL))
    return;

  dir = fts_package_get_dir( pkg);
  class_name = fts_object_get_class_name(obj);

  help_name = fts_package_get_help( pkg, class_name);
  if( help_name)
    snprintf(path, 256, "%s%c%s%c%s", dir, fts_file_separator, "help", fts_file_separator, help_name);
  else
    snprintf(path, 256, "%s%c%s%c%s%s", dir, fts_file_separator, "help", fts_file_separator, class_name, ".help.jmax");
  
  file_name = fts_new_symbol(path);
      
  help_patch = fts_patcher_get_by_file_name(file_name);

  if(help_patch)
    patcher_open_editor((fts_object_t *)help_patch, 0, 0, 0, 0);
  else
    help_patch = fts_client_load_patcher(file_name, fts_get_client_id(o));

  if(!help_patch)
    fts_client_send_message(o, sym_noHelp, 1, at); 
}

/* find utility */
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
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *p;

  /* see if self should be added */
  if (fts_atom_is_subsequence(ac - 1, at + 1, fts_object_get_description_size(o), fts_object_get_description_atoms(o)))
  {
    /* add only if the patcher is not the root patcher or a patcher in the root patcher */
    if((self != fts_get_root_patcher())&&(fts_object_get_patcher(o) != fts_get_root_patcher()))
      fts_objectset_add(set, o);
  }
  /* look if the objects in the patchers are to be found */
  for(p = self->objects; p; p = fts_object_get_next_in_patcher(p))
  {
    if(!fts_object_is_patcher(p))
    {
      if(!fts_object_is_error(p))
      {
        fts_method_t find_method = fts_class_get_method_varargs(fts_object_get_class(p), fts_s_find);

        /* send the find message to the object; if the object do not implement it, do the standard check */
        if(find_method != NULL)
          (*find_method)(p, fts_system_inlet, fts_s_find, ac, at);
        else if (fts_atom_is_subsequence(ac - 1, at + 1, fts_object_get_description_size(p), fts_object_get_description_atoms(p)))
          fts_objectset_add(set, p);
      }
      else if (fts_atom_is_subsequence(ac - 1, at + 1, fts_object_get_description_size(p), fts_object_get_description_atoms(p)))
        fts_objectset_add(set, p);
    }
  }

  /* do the recursive calls  */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
  {
    if (fts_object_is_standard_patcher(p) ||
        fts_object_is_template(p) ||
        fts_object_is_abstraction(p))
      fts_send_message_varargs(p, s, ac, at);
  }
}

/* the find errors engines */
static void
patcher_find_errors(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_objectset_t *set = (fts_objectset_t *)fts_get_object(at);
  fts_object_t *p;

  /* look if the objects in the patchers are to be found */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
  {
    if(fts_object_is_error(p))
      fts_objectset_add(set, p);
  }

  /* do the recursive calls  */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
  {
    if (fts_object_is_standard_patcher(p) ||
        fts_object_is_template(p) ||
        fts_object_is_abstraction(p))
      fts_send_message_varargs(p, s, ac, at);
  }
}

/*************************************************************
*
*  patcher upload and updates
*
*/
static void
patcher_upload( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  fts_atom_t a[4];
  fts_object_t *p;

  if(!fts_object_is_error((fts_object_t *)self))
  {
    fts_object_get_prop((fts_object_t *)self, fts_s_wx, a);
    fts_object_get_prop((fts_object_t *)self, fts_s_wy, a+1);
    fts_object_get_prop((fts_object_t *)self, fts_s_ww, a+2);
    fts_object_get_prop((fts_object_t *)self, fts_s_wh, a+3);

    if( fts_get_int( a) && fts_get_int( a+1) && fts_get_int( a+2) && fts_get_int( a+3))
      fts_client_send_message((fts_object_t *)self, sym_setPatcherBounds, 4, a);

    for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
      fts_object_upload( p);

    for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
    {
      int outlet;

      for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
      {
        fts_connection_t *c;

        for (c = fts_object_get_outlet_connections(p, outlet); c ; c = c->next_same_src)
          fts_object_upload( (fts_object_t *)c);
      }
    }

    fts_client_send_message((fts_object_t *)self, fts_s_end_upload, 0, 0);
  }
}

static void
patcher_start_updates( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_object_t *p;

  self->open = 1;

  for(p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
  {
    if(fts_class_get_method_varargs(fts_object_get_class(p), fts_s_update_real_time) != NULL)
      fts_update_request(p);
  }
}

static void
patcher_stop_updates( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;

  self->open = 0;
}

/**
* This function send all the objects and connections in the patcher that have not
 * yet been uploaded; it is usefull after paste operations.
 */
static void
patcher_update( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  fts_object_t *p;

  /* upload all the not uploaded objects */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
    if (!fts_object_has_id(p))
      fts_object_upload( p);

  /* for each object, for each outlet, upload all the not uploaded connections */
  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
  {
    int outlet;

    for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
    {
      fts_connection_t *c;

      for (c = fts_object_get_outlet_connections(p, outlet); c ; c = c->next_same_src)
        if (!fts_object_has_id((fts_object_t *)c))
          fts_object_upload((fts_object_t *)c);
    }
  }
}

static void
patcher_set_wx( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_wx, at);
}

static void
patcher_set_wy( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_wy, at);
}

static void
patcher_set_ww( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_ww, at);
  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void
patcher_set_wh( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_put_prop(o, fts_s_wh, at);
  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void
patcher_upload_child( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = fts_get_object(at);

  if(fts_object_get_class_name(obj) == fts_s_connection)
    {
      fts_connection_t *conn = (fts_connection_t *)obj;

      /* don't upload hidden connections */
      if(fts_connection_get_type(conn) <= fts_c_hidden)
	return;

      fts_client_start_message( o, sym_addConnection);
      fts_client_add_int( o, fts_get_object_id(obj));
      fts_client_add_object( o, conn->src);
      fts_client_add_int( o, conn->woutlet);
      fts_client_add_object( o, conn->dst);
      fts_client_add_int( o, conn->winlet);
      fts_client_add_int( o, conn->type);
      fts_client_done_message( o);
    }
  else
    {
      fts_class_t *class;
      fts_method_t spost_method;
      fts_memorystream_t *stream;
      fts_atom_t a_x, a_y, a_w, a_h, a_layer, a_font[3];
      fts_atom_t b[1];

      fts_object_get_prop(obj, fts_s_x, &a_x);
      fts_object_get_prop(obj, fts_s_y, &a_y);
      fts_object_get_prop(obj, fts_s_width, &a_w);
      fts_object_get_prop(obj, fts_s_height, &a_h);

      fts_object_get_prop(obj, fts_s_layer, &a_layer);

      fts_client_start_message( o, sym_addObject);
      fts_client_add_int( o, fts_get_object_id(obj));
      fts_client_add_int( o, fts_get_int(&a_x));
      fts_client_add_int( o, fts_get_int(&a_y));
      fts_client_add_int( o, fts_get_int(&a_w));
      fts_client_add_int( o, fts_get_int(&a_h));
      fts_client_add_int( o, fts_object_get_inlets_number(obj));
      fts_client_add_int( o, fts_object_get_outlets_number(obj));
      fts_client_add_int( o, fts_get_int(&a_layer));

      if(fts_object_is_error(obj))
	{
	  fts_client_add_symbol( o, fts_error_object_get_description((fts_error_object_t *)obj));

	  class = fts_error_object_get_class((fts_error_object_t *)obj);
	  /* ??? */
	  fts_class_instantiate(class);
	}
      else
	{
	  fts_client_add_symbol( o, fts_s_no_error);

	  class = fts_object_get_class(obj);
	}

      if(fts_class_get_name(class) != NULL)
	fts_client_add_symbol( o, fts_class_get_name(class));
      else
	{
	  if(fts_object_get_description_size(obj) > 0 && fts_is_symbol(fts_object_get_description_atoms(obj)))	  
	    fts_client_add_symbol( o, fts_get_symbol(fts_object_get_description_atoms(obj)));
	  else
	    fts_client_add_symbol( o, fts_s_error);
	}

      fts_client_add_int( o, fts_object_is_template(obj));

      stream = patcher_memory_stream;
      fts_memorystream_reset( stream);

      fts_set_object( b, stream);

      spost_method = fts_class_get_method_varargs(class, fts_s_spost_description);

      if(spost_method != NULL)
	(*spost_method)(obj, fts_system_inlet, fts_s_spost_description, 1, b);
      else
	fts_spost_object_description((fts_bytestream_t *)stream, obj);

      fts_bytestream_output_char((fts_bytestream_t *)stream,'\0');
      fts_client_add_string( o, fts_memorystream_get_bytes( stream));

      fts_client_done_message( o);

      fts_object_get_prop(obj, fts_s_font, a_font + 0);

      if(fts_get_symbol(a_font))
	{
	  fts_object_get_prop(obj, fts_s_fontSize, a_font + 1);
	  fts_object_get_prop(obj, fts_s_fontStyle, a_font + 2);
	  fts_client_send_message(obj, fts_s_setFont, 3, a_font);
	}

      /* send gui properties */
      fts_send_message_varargs(obj, fts_s_update_gui, 0, 0);

      /* add to real time update list */
      if(fts_class_get_method_varargs( fts_object_get_class(obj), fts_s_update_real_time) != NULL)
	fts_update_request(obj);
    }
}

/***********************************************************************
*
*  add objects from client
*
*/
static void
patcher_add_object_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *obj = fts_eval_object_description((fts_patcher_t *)o, ac - 2, at + 2);
  fts_atom_t a[1];

  fts_object_put_prop(obj, fts_s_x, at);
  fts_object_put_prop(obj, fts_s_y, at+1);

  fts_set_int(a, -1);
  fts_object_put_prop(obj, fts_s_layer, a);

  fts_object_upload(obj);

  fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

static void
patcher_redefine_object_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_t *old = fts_get_object(at);
  int dsp_restart = fts_dsp_is_active() && fts_is_dsp_object(old);
  fts_object_t *obj;
  fts_atom_t a;

  if(dsp_restart)
    fts_dsp_desactivate();

  obj = fts_object_redefine(old, ac - 1, at + 1);

  fts_set_object(&a, obj);
  fts_client_send_message(o, sym_objectRedefined, 1, &a);

  fts_patcher_set_dirty((fts_patcher_t *)o, 1);

  if(dsp_restart)
    fts_dsp_activate();
}

static void
patcher_delete_objects_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;

  if (ac > 0)
  {
    int dsp_restart = 0;
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

          fts_send_message_varargs( obj, fts_s_closeEditor, 0, 0);

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
      {
        if(fts_dsp_is_active() && fts_is_dsp_object(obj))
        {
          fts_dsp_desactivate();
          dsp_restart = 1;
        }

        /* release object */
        fts_patcher_remove_object(self, obj);
      }
    }

    fts_patcher_set_dirty((fts_patcher_t *)o, 1);

    if(dsp_restart == 1)
      fts_dsp_activate();
  }
}

static void
patcher_add_connection_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if((ac == 4) &&
     fts_is_object(&at[0]) &&
     fts_is_int(&at[1]) &&
     fts_is_object(&at[2]) &&
     fts_is_int(&at[3]))
  {
    int inlet, outlet;
    fts_object_t *from, *to;
    fts_connection_t *connection;

    from   = fts_get_object( at);
    outlet = fts_get_int( at+1);
    to     = fts_get_object( at+2);
    inlet  = fts_get_int( at+3);

    if (to && from)
    {
      connection = fts_connection_new(from, outlet, to, inlet, fts_c_anything);
      
      if(!connection)
        return;

      fts_object_upload((fts_object_t *)connection);
    }

    fts_patcher_set_dirty((fts_patcher_t *)o, 1);
  }
}

static void
patcher_delete_connection_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_connection_t *connection = (fts_connection_t *)fts_get_object( at);

  if (connection)
  {
    int dsp_restart = 0;

    if(fts_connection_get_type(connection) == fts_c_audio_active)
    {
      fts_dsp_desactivate();
      dsp_restart = 1;
    }

    fts_connection_delete(connection);
    fts_patcher_set_dirty((fts_patcher_t *)o, 1);

    if(dsp_restart == 1)
      fts_dsp_activate();
  }
}

static void
patcher_paste( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *)o;
  fts_object_t *clipboard = fts_get_object( at);
  int dx = fts_get_int(at+1);
  int dy = fts_get_int(at+2);
  fts_atom_t a[2];
  fts_object_t *p;
  int uploaded = 0;

  fts_set_object(a, o);
  fts_send_message_varargs(clipboard, fts_s_paste, 1, a);

  /* upload all the not uploaded objects */
  fts_client_send_message((fts_object_t *)self, sym_startPaste, 0, 0);

  for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
    if (!fts_object_has_id(p))
    {
      /* shift object during paste */
      fts_object_get_prop(p, fts_s_x, a);
      fts_object_get_prop(p, fts_s_y, a+1);

      fts_set_int(a, fts_get_int(a) + dx);
      fts_set_int(a+1, fts_get_int(a+1) + dy);

      fts_object_put_prop(p, fts_s_x, a);
      fts_object_put_prop(p, fts_s_y, a+1);

      fts_object_upload( p);
      uploaded = 1;
    }

      /* For each object, for each outlet, upload all the not uploaded connections */
      for (p = self->objects; p ; p = fts_object_get_next_in_patcher(p))
      {
        int outlet;

        for (outlet = 0; outlet < fts_object_get_outlets_number(p); outlet++)
        {
          fts_connection_t *c;

          for (c = fts_object_get_outlet_connections(p, outlet); c ; c = c->next_same_src)
          {
            if(!fts_object_has_id( (fts_object_t *)c))
            {
              fts_object_upload((fts_object_t *)c);
              uploaded = 1;
            }
          }
        }
      }

      fts_client_send_message((fts_object_t *)self, sym_endPaste, 0, 0);

  if(uploaded)
    fts_patcher_set_dirty((fts_patcher_t *)o, 1);
}

/***********************************************************************
*
*  .pat file format saving and loading
*
*/
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

  for(o=patcher->objects; o; o = fts_object_get_next_in_patcher(o))
  {
    if (object == o)
      return index;

    index++;
  }

  return -1;
}

static void
patcher_save_from_client( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
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
      patcher_save_dotpat_atoms( file, fts_object_get_description_size(object) - 1, fts_object_get_description_atoms(object) + 1);
      fprintf( file, ";\n");
    }
    else if ( ! fts_object_is_patcher( object)
              && fts_class_get_method_varargs( fts_object_get_class(object), fts_s_save_dotpat) != NULL)
    {
      fts_atom_t a;

      fts_set_pointer( &a, file);
      fts_send_message_varargs( object, fts_s_save_dotpat, 1, &a);
    }
    else
    {
      fprintf( file, "#P newex ");
      patcher_save_dotpat_object_geometry( file, object);
      patcher_save_dotpat_atoms( file, fts_object_get_description_size(object), fts_object_get_description_atoms(object));
      fprintf( file, ";\n");
    }

    object = fts_object_get_next_in_patcher(object);
  }
}

/* This auxiliary function is used to save the connections in reverse order.
* There is no requested order for connections saving, but MAX/Opcode
* saves in reverse order, so in order to ease text comparison of files,
* we can save in reverse order.
*
* (apparently, the connection saving order depends from the MAX version...)
*/
static void
patcher_save_connections( FILE *file, fts_object_t *object, int patcher_object_count)
{
  int object_index = 0;

  while ( object)
  {
    int outlet;

    for ( outlet = 0; outlet < fts_object_get_outlets_number( object); outlet++)
    {
      fts_connection_t *c;

      for ( c = fts_object_get_outlet_connections(object, outlet); c ; c = c->next_same_src)
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

    object = fts_object_get_next_in_patcher(object);
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

/* .pat loading: reorder inlets and outlets regarding their x position in the patcher */
static int
patcher_objects_compare_x(const void *left, const void *right)
{
  fts_object_t *obj_left = *((fts_object_t **)left);
  fts_object_t *obj_right = *((fts_object_t **)right);
  int x_left, x_right;
  fts_atom_t a;

  fts_object_get_prop(obj_left, fts_s_x, &a);
  x_left = fts_get_int(&a);

  fts_object_get_prop(obj_right, fts_s_x, &a);
  x_right = fts_get_int(&a);

  return x_left - x_right;
}

#define MAX_INOUTS 256

void
fts_patcher_order_inoutlets_regarding_position(fts_patcher_t *self)
{
  fts_receive_t *receives[MAX_INOUTS];
  fts_send_t *sends[MAX_INOUTS];
  fts_object_t *p;
  int n_ins = 0;
  int n_outs = 0;
  int i;

  /* put all inlet receives in an array */
  for (p=self->objects; p; p=fts_object_get_next_in_patcher(p))
  {
    if(fts_object_get_class(p) == receive_class)
    {
      fts_receive_t *receive = (fts_receive_t *)p;

      if(fts_object_get_class(receive->obj) == patcher_inout_class && n_ins < MAX_INOUTS)
      {
        receives[n_ins] = receive;
        n_ins++;
      }
    }
    else if(fts_object_get_class(p) == send_class)
    {
      fts_send_t *send = (fts_send_t *)p;

      if(fts_object_get_class(send->obj) == patcher_inout_class && n_outs < MAX_INOUTS)
      {
        sends[n_outs] = send;
        n_outs++;
      }
    }
  }

  /* sort receives and assign inlets */
  if(n_ins > 1)
  {
    patcher_redefine_number_of_inlets(self, n_ins);

    /* sort receives */
    qsort((void *)receives, n_ins, sizeof(fts_object_t *), patcher_objects_compare_x);

    /* assign new inlet to receive */
    for(i=1; i<n_ins; i++)
      receives[i]->obj = (fts_object_t *)patcher_get_inlet(self, i);
  }

  /* sort sends and assign outlets */
  if(n_outs > 1)
  {
    patcher_redefine_number_of_outlets(self, n_outs);

    /* sort sends */
    qsort((void *)sends, n_outs, sizeof(fts_object_t *), patcher_objects_compare_x);

    /* assign new outlet to receive */
    for(i=1; i<n_outs; i++)
      sends[i]->obj = (fts_object_t *)patcher_get_outlet(self, i);
  }
}

/***********************************************************************
*
*  patcher class and system methods
*
*/

static void
patcher_propagate_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
  void *propagate_context = fts_get_pointer(at + 1);
  int n = fts_get_int(at + 2);
  patcher_inout_t *inlet = self->inlets[n];

  if(inlet)
  {
    fts_objectlist_t *targets = fts_channel_get_targets(&inlet->channel);
    fts_objectlist_cell_t *p;

    for(p=fts_objectlist_get_head(targets); p; p=fts_objectlist_get_next(p))
      propagate_fun(propagate_context, fts_objectlist_get_object(p), 0);
  }
}

static void
patcher_spost_description(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;

  if(fts_patcher_is_template( (fts_patcher_t *)o) || fts_patcher_is_abstraction( (fts_patcher_t *)o))
    fts_spost_object_description( (fts_bytestream_t *)fts_get_object(at), o);
  else if(self->description_ac > 0)
    fts_spost_object_description_args((fts_bytestream_t *)fts_get_object(at), self->description_ac, self->description_at);
}

static void
patcher_dump(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;
  fts_dumper_t *dumper = (fts_dumper_t *)fts_get_object(at);
  fts_message_t *mess = fts_dumper_message_new( dumper, fts_s_set_arguments);

  fts_message_append(mess, self->description_ac, self->description_at);
  fts_dumper_message_send(dumper, mess);
}

static void
patcher_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;

  /* raw patcher arguments (the FTS object description stays always "jpatcher") */
  self->description_ac = 0;
  self->description_at = NULL;

  self->inlets = NULL;
  self->outlets = NULL;

  self->n_inlets = 0;
  self->n_outlets = 0;

  fts_object_set_inlets_number(o, 0);
  fts_object_set_outlets_number(o, 0);
  
  fts_patcher_set_standard(self);

  /* set arguments */
  if(ac > 0)
  {
    fts_definition_t *def = fts_definition_get(self, fts_s_args);
    fts_atom_t a;

    self->args = (fts_tuple_t *)fts_object_create(fts_tuple_class, ac, at);
    fts_object_refer(self->args);

    /* set new definiton */
    fts_set_object(&a, (fts_object_t *)self->args);
    fts_definition_update(def, &a);

    /* store definition in object */
    fts_object_set_definition((fts_object_t *)self->args, def);

    fts_patcher_set_template(self, NULL);
  }
  else
    self->args = NULL;
    
  /* init object list */
  self->objects = NULL;

  /* init flags */
  self->open = 0; /* start as closed */
  self->editor_open = 0; /* start with editor closed */
  self->deleted = 0;
  self->dirty = 0; /* start as saved */
}

static void
patcher_delete_objects( fts_object_t *obj)
{
  if (obj == NULL)
    return;

  patcher_delete_objects( fts_object_get_next_in_patcher(obj) );

  fts_object_set_next_in_patcher(obj, NULL);
  fts_object_release( obj);
}

static void
patcher_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_patcher_t *self = (fts_patcher_t *) o;

  if(editor_is_open(self))
  {
    set_editor_close(self);
    fts_client_send_message(o, fts_s_destroyEditor, 0, 0);
  }

  /* If it is a template, remove it from the template instance list */
  if (fts_patcher_is_template(self))
    fts_template_remove_instance(fts_patcher_get_template(self), (fts_object_t *) self);

  /* Set the deleted and reset the open flag */
  self->deleted = 1;
  self->open = 0;

  patcher_delete_objects( self->objects);
  self->objects = NULL;

  /* delete arguments */
  if(self->args != NULL)
    fts_object_release(self->args);
  
  /* delete the inlet and outlet tables */
  if (self->inlets)
    fts_free( self->inlets);

  if (self->outlets)
    fts_free( self->outlets);
}

static void
patcher_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_patcher_t), patcher_init, patcher_delete);

  fts_class_message_varargs(cl, fts_s_find, patcher_find);
  fts_class_message_varargs(cl, fts_s_find_errors, patcher_find_errors);

  fts_class_message_varargs(cl, fts_s_dump, patcher_dump);
  fts_class_message_varargs(cl, fts_s_propagate_input, patcher_propagate_input);
  fts_class_message_varargs(cl, fts_s_spost_description, patcher_spost_description);

  fts_class_message_varargs(cl, fts_s_loaded, patcher_loaded);
  fts_class_message_varargs(cl, fts_new_symbol("load_init"), patcher_load_init);

  fts_class_message_varargs(cl, fts_new_symbol("open_help_patch"), patcher_open_help_patch);
  fts_class_message_varargs(cl, fts_s_save, patcher_save_from_client);
  fts_class_message_varargs(cl, fts_s_paste, patcher_paste);

  fts_class_message_varargs(cl, fts_s_set_arguments, patcher_set_arguments);
  fts_class_message_varargs(cl, fts_s_openEditor, patcher_open_editor);
  fts_class_message_varargs(cl, fts_s_destroyEditor, patcher_destroy_editor);
  fts_class_message_varargs(cl, fts_s_closeEditor, patcher_close_editor);
  fts_class_message_varargs(cl, fts_new_symbol("show_object"), patcher_show_object);
  fts_class_message_varargs(cl, fts_new_symbol("stop_waiting"), patcher_stop_waiting);

  fts_class_message_varargs(cl, fts_s_upload, patcher_upload);
  fts_class_message_varargs(cl, fts_s_upload_child, patcher_upload_child);

  fts_class_message_varargs(cl, fts_new_symbol("start_updates"), patcher_start_updates);
  fts_class_message_varargs(cl, fts_new_symbol("stop_updates"), patcher_stop_updates);
  fts_class_message_varargs(cl, fts_new_symbol("patcher_update"), patcher_update);
  fts_class_message_varargs(cl, fts_new_symbol("set_wx"), patcher_set_wx);
  fts_class_message_varargs(cl, fts_new_symbol("set_wy"), patcher_set_wy);
  fts_class_message_varargs(cl, fts_new_symbol("set_ww"), patcher_set_ww);
  fts_class_message_varargs(cl, fts_new_symbol("set_wh"), patcher_set_wh);

  fts_class_message_varargs(cl, fts_new_symbol("add_object"), patcher_add_object_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("delete_objects"), patcher_delete_objects_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("add_connection"), patcher_add_connection_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("delete_connection"), patcher_delete_connection_from_client);
  fts_class_message_varargs(cl, fts_new_symbol("redefine_object"), patcher_redefine_object_from_client);

  fts_class_input_handler(cl, patcher_input);
  fts_class_inlet_thru(cl, 0);
  fts_class_outlet_thru(cl, 0);
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

  fts_root_patcher = (fts_patcher_t *)fts_object_create(patcher_class, 1, a);

  fts_object_refer((fts_object_t *)fts_root_patcher);
}

static void fts_delete_root_patcher(void)
{
  /* should be destroyed here! */
  fts_object_set_id( (fts_object_t *)fts_root_patcher, FTS_NO_ID);
  fts_object_release((fts_object_t *)fts_root_patcher);
  fts_root_patcher = NULL;
}

fts_patcher_t *
fts_get_root_patcher(void)
{
  return fts_root_patcher;
}

/***********************************************************************
*
*  top level & variables
*
*/

fts_patcher_t *
fts_patcher_get_scope(fts_patcher_t *patcher)
{
  if(patcher != NULL)
  {
    fts_patcher_t *parent = fts_object_get_patcher((fts_object_t *)patcher);

    while(parent != NULL && parent != fts_root_patcher && fts_patcher_is_template(patcher) == NULL)
    {
      patcher = parent;
      parent = fts_object_get_patcher((fts_object_t *)patcher);
    }

    return patcher;
  }

  return fts_root_patcher;
}

/***********************************************************************
*
* initialization/shutdown
*
*/

void fts_kernel_patcher_init(void)
{
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
  sym_redefineObject = fts_new_symbol("redefineObject");
  sym_objectRedefined = fts_new_symbol("objectRedefined");
  sym_setRedefined = fts_new_symbol("setRedefined");
  sym_setDescription = fts_new_symbol("setDescription");
  sym_setSaved = fts_new_symbol("setSaved");
  sym_startPaste = fts_new_symbol("startPaste");
  sym_endPaste = fts_new_symbol("endPaste");
  sym_noHelp = fts_new_symbol("noHelp");

  patcher_class = fts_class_install(fts_s_jpatcher, patcher_instantiate);
  patcher_inout_class = fts_class_install(NULL, patcher_inout_instantiate);

  receive_class = fts_class_install(fts_s_receive, receive_instantiate);
  send_class = fts_class_install(fts_s_send, send_instantiate);
  fts_class_alias(receive_class, fts_s_inlet);
  fts_class_alias(send_class, fts_s_outlet);

  /* historical aliases */
  fts_class_alias(receive_class, fts_new_symbol("r"));
  fts_class_alias(receive_class, fts_new_symbol("r~"));
  fts_class_alias(send_class, fts_new_symbol("s"));
  fts_class_alias(send_class, fts_new_symbol("s~"));

  patcher_memory_stream = (fts_memorystream_t *)fts_object_create( fts_memorystream_class, 0, 0);
}

void fts_patcher_config( void)
{
  fts_create_root_patcher();
}

void fts_kernel_patcher_shutdown(void)
{
  fts_delete_root_patcher();
}

/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
