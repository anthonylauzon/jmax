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


/* 
   The selection; it is an object representing a set of objects and connections
   (connections must refer to objects in the selection).
   Implements methods to add and remove a connection, and to copy the selection
   to a clipboard object.

   Warning: at the moment, this object do *not* implement Cut like
   operation; those operations must be implemented on the client side,
   beacause we don't still have a way to propagate "delete" events up
   to the client.

   Warning: also, objects are not automatically deleted from the selection
   when deleted; this is a low level service, not the implementation of the
   whole selection UI; so, the selection must be clean before deleting objects.

   All the methods are implemented on the system inlet.

   Objects and connections are kept in an array; the array is not kept
   compact, there may be holes (null pointers) in between.
*/

#define INITIAL_SELECTION_SIZE 16

#include <fts/fts.h>
#include <ftsprivate/selection.h>

static void
selection_object_size_to_fit(fts_selection_t *this)
{
  int i;
  int new_size;
  fts_object_t **new_objects;

  new_size = this->objects_size * 2;
  new_objects = (fts_object_t **) fts_malloc(new_size * sizeof(fts_object_t *));

  for (i = 0; i < this->objects_size; i++)
    new_objects[i] = this->objects[i];

  for (i = this->objects_size; i < new_size; i++)
    new_objects[i] = 0;

  this->objects_size = new_size;

  fts_free(this->objects);
  this->objects = new_objects;
}

int
fts_selection_contains_object(fts_selection_t *sel, fts_object_t *o)
{
  int i;
  for (i = 0; i < sel->objects_size; i++)
    if (sel->objects[i] == o)
      {
	return 1;
      }
  return 0;
}

int
fts_selection_connection_ends_selected(fts_selection_t *sel, fts_connection_t *c)
{
  if(fts_selection_contains_object(sel, c->src) && fts_selection_contains_object(sel, c->dst))
    return 1;
  else
    return 0;
}

void
fts_selection_add_object( fts_selection_t *sel, fts_object_t *obj)
{
  int i;

  sel->objects_count++;

  if (sel->objects_count > sel->objects_size)
    selection_object_size_to_fit(sel);

  for (i = 0; i < sel->objects_size; i++)
    if (sel->objects[i] == 0)
      {
	sel->objects[i] = obj;
	return;
      }
}

static void
selection_add(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_selection_add_object( (fts_selection_t *) o, fts_get_object(at));
}

static void
selection_remove(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_selection_t *this  = (fts_selection_t *) o;
  int i;
  
  this->objects_count--;

  for (i = 0; i < this->objects_size; i++)
    if (this->objects[i] == fts_get_object(at))
      {
	this->objects[i] = 0;
	return;
      }
}


static void
selection_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_selection_t *this  = (fts_selection_t *) o;
  int i;
  
  this->objects_count = 0;
  for (i = 0; i < this->objects_size; i++)
    this->objects[i] = 0;
}

static void
selection_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_selection_t *this  = (fts_selection_t *) o;
  int i;

  this->objects_size = INITIAL_SELECTION_SIZE;
  this->objects_count = 0;
  this->objects = (fts_object_t **) fts_malloc(this->objects_size * sizeof(fts_object_t *));

  for (i = 0; i < this->objects_size; i++)
    this->objects[i] = 0;
}

static void
selection_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_selection_t *this  = (fts_selection_t *) o;

  fts_free(this->objects);
}


static void
selection_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_selection_t), selection_init, selection_delete);

  fts_class_message_varargs(cl, fts_new_symbol("add"),  selection_add);
  fts_class_message_varargs(cl, fts_new_symbol("remove"), selection_remove);

  fts_class_message_varargs(cl, fts_s_clear, selection_clear);
}


/***********************************************************************
 *
 * Initialization
 *
 */

void fts_selection_config( void)
{
  fts_class_install(fts_new_symbol("__selection"), selection_instantiate);
}



