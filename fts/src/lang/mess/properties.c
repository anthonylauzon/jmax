/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */

/* 
   Object dynamic properties.

   A property list is associated to every object, that can
   store there properties needed by rest of the sytem.
   
   A list is associated also the class, and by default,
   if a property is not found in the object, is looked for in the
   class itself; the property list should be initialized in
   the class instantiation function.

   Also inlets and outlets have properties, both at the object and
   at the class level.

   Properties are not intended as substitute of object memory;
   they should *not* be used during standard control computation (their
   use is memory and computing intensive).

   The DSP compiler will make large use of properties.

   DAEMON: a daemon is a function that is called when an object property is
   put, get or removed; the daemon is called with the object, 
   the property name and value as argument; daemon are defined at the *class* level;
   daemons are installed on specific properties.

   When getting a property from an object, the order is:

   1- Look if the object have the property set
   2- If not, look in the class
   3- If not, look if there are class get daemons for that property
   4- If not, return the void atom.

   When putting the property in an object, the order is:

   1- Look if there are class put daemons for that property
   2- If not, put the property in the object plist.

   When removing an property from an object, the order is:

   1- Look if there are class remove daemons for that property
   2- If not, remove the property from the object plist.

   For every object function, there is a companion started with "_"
   that do not call daemons; these companions can be used by the
   daemons to put/remove/get the property after processing, avoiding
   recursive daemon calling.

 */

#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"

static const fts_atom_t *fts_class_get_prop(fts_class_t *cl, fts_symbol_t property);

static fts_heap_t *daemon_heap;
static fts_heap_t *plist_cell_heap;
static fts_heap_t *plist_heap;


/** Local and naive implementation of property lists 
    to use in storing properties
    */


struct fts_plist_cell {
  fts_symbol_t property;
  fts_atom_t value;
  struct fts_plist_cell *next;
};


static fts_plist_t *
fts_plist_new( void)
{
  fts_plist_t *tmp = (fts_plist_t *)fts_heap_alloc(plist_heap);

  tmp->head = 0;

  return tmp;
}


static void 
fts_plist_free( fts_plist_t *plist)
{
  struct fts_plist_cell *c, *next;

  for( c = plist->head; c; c = next)
    {
      next = c->next;
      fts_heap_free((char *)c, plist_cell_heap);
    }

  plist->head = 0;
  fts_heap_free( (char *)plist, plist_heap);
}


static const fts_atom_t *
fts_plist_get( const fts_plist_t *plist, fts_symbol_t property)
{
  struct fts_plist_cell *c;

  for( c = plist->head; c; c = c->next)    
    if (c->property == property)
      return &(c->value);

  return 0;
}


static void
fts_plist_put( fts_plist_t *plist, fts_symbol_t property, const fts_atom_t *value)
{
  struct fts_plist_cell *prev, *current, *insert;

  /* See if the property is already there */

  for( current = plist->head; current; current = current->next)
    if (current->property == property)
      {
	current->value = *value;
	return;
      }

  /* It is not there, add a new list entry */
	  
  prev = 0;
  for( current = plist->head; current; current = current->next )
    prev = current;

  insert = (struct fts_plist_cell *) fts_heap_alloc(plist_cell_heap);

  insert->property =  property;
  insert->value = *value;
  insert->next = 0;

  if (prev)
    prev->next = insert;
  else
    plist->head = insert;
}


static void
fts_plist_remove( fts_plist_t *plist, fts_symbol_t property)
{
  struct fts_plist_cell *prev, *current, *next;
  int found;

  prev = 0;
  found = 0;
  for( current = plist->head; current; current = next)
    {
      next = current->next;
      if (current->property == property)
	{
	  if (prev)
	    prev->next = next;
	  else
	    plist->head = next;

	  fts_heap_free((char *)current, plist_cell_heap);
	}
      else
	prev = current;
    }
}



/* Daemon list manipulation */

/* Private type to store the Daemon list */

typedef struct daemon_list
{
  fts_symbol_t           property;
  fts_daemon_action_t    action;
  fts_property_daemon_t  daemon;

  struct daemon_list   *next;
} fts_daemon_entry_t;


static void
fts_property_daemon_list_add(fts_daemon_entry_t **list,
			     fts_daemon_action_t  action,
			     fts_symbol_t         property,
			     fts_property_daemon_t daemon)
{
  fts_daemon_entry_t *entry = (fts_daemon_entry_t *) fts_heap_alloc(daemon_heap);

  entry->action   = action;
  entry->property = property;
  entry->daemon = daemon;
  entry->next   = *list;
  
  *list = entry;
}

static void
fts_property_daemon_list_remove(fts_daemon_entry_t **list,
				fts_daemon_action_t  action, 
				fts_symbol_t         property,
				fts_property_daemon_t daemon)
{
  fts_daemon_entry_t **p; /* indirect precursor */

  for (p = list; *p ; p = &((*p)->next))
    {
      if (((*p)->action == action) && ((*p)->daemon == daemon) && ((*p)->property == property))
	{
	  fts_daemon_entry_t *old;

	  old = *p;
	  *p = (*p)->next;

	  fts_heap_free((char *) old, daemon_heap);	  
	}
    }
}


static fts_property_daemon_t
fts_property_daemon_list_get(fts_daemon_entry_t **list,
			     fts_daemon_action_t action,
			     fts_symbol_t        property)
{
  fts_daemon_entry_t *p; 

  for (p = *list; p ; p = p->next)
    {
      if ((p->action == action) && (p->property == property))
	return p->daemon;
    }

  return 0;
}





/* Object put */

void
_fts_object_put_prop(fts_object_t *obj, fts_symbol_t property, const fts_atom_t *value)
{
  if (! obj->properties)
    obj->properties = fts_plist_new();

  fts_plist_put(obj->properties, property, value);
}


void
fts_object_put_prop(fts_object_t *obj, fts_symbol_t property, const fts_atom_t *value)
{
  fts_property_daemon_t d;

  d = fts_property_daemon_list_get(&(obj->cl->daemons), obj_property_put, property);
  
  if (d)
    (* d)(obj_property_put, obj, property, (fts_atom_t *)value);
  else
    _fts_object_put_prop(obj, property, value);
}


/* Object remove properties */

void
_fts_object_remove_prop(fts_object_t *obj, fts_symbol_t property)
{
  if (obj->properties)
    fts_plist_remove(obj->properties, property);
}


void
fts_object_remove_prop(fts_object_t *obj, fts_symbol_t property)
{
  fts_property_daemon_t d;


  d = fts_property_daemon_list_get(&(obj->cl->daemons), obj_property_remove, property);

  if (d)
    (* d)(obj_property_remove, obj, property, 0);
  else
    _fts_object_remove_prop(obj, property);
}


/* Object get properties */

void
_fts_object_get_prop(fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  const fts_atom_t *ret;

  ret = 0;

  if (obj->properties)
    ret = fts_plist_get(obj->properties, property);

  if (ret)
    *value = *ret;
  else
    {
      ret = fts_class_get_prop(obj->cl, property);

      if (ret)
	*value = *ret;
      else
	fts_set_void(value);
    }
}


void
fts_object_get_prop(fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  const fts_atom_t *ret;
  fts_property_daemon_t d;

  ret = 0;

  if (obj->properties)
    ret = fts_plist_get(obj->properties, property);

  if (ret)
    {
      *value = *ret;
    }
  else
    {
    
      ret = fts_class_get_prop(obj->cl, property);

      if (ret)
	{
	  *value = *ret;
	}
      else
	{
	  d = fts_property_daemon_list_get(&(obj->cl->daemons), obj_property_get, property);

	  if (d)
	    (* d)(obj_property_remove, obj, property, value);
	  else
	    fts_set_void(value);
	}
    }
}


/* Class version

   the class set will *not* run the daemons, of course.

   Also the is remove is provided for completness, but should not
   be used after class initialization.
*/


void
fts_class_put_prop(fts_class_t *cl, fts_symbol_t property, const fts_atom_t *value)
{
  if (! cl->properties)
    cl->properties = fts_plist_new();

  fts_plist_put(cl->properties, property, value);
}

void
fts_class_remove_prop(fts_class_t *cl, fts_symbol_t property)
{
  if (cl->properties)
    fts_plist_remove(cl->properties, property);
}

/* null if no prop */

static const fts_atom_t *
fts_class_get_prop(fts_class_t *cl, fts_symbol_t property)
{
  if (cl->properties)
    return fts_plist_get(cl->properties, property);
  else
    return 0;
}

/* 
   Daemon handling
 */

/* class daemons */


void
fts_class_add_daemon(fts_class_t *cl,
		     fts_daemon_action_t  action,
		     fts_symbol_t        property,
		     fts_property_daemon_t daemon)
{
  fts_property_daemon_list_add(&(cl->daemons), action, property, daemon);
}



void
fts_class_remove_daemon(fts_class_t *cl, 
			fts_daemon_action_t  action, 
			fts_symbol_t        property,
			fts_property_daemon_t daemon)
{
  fts_property_daemon_list_remove(&(cl->daemons), action, property, daemon);
}


/*
  Function to remove all the properties from an object, and to clean
  up its property related functions; private to the message system,
  it is called just before freeing an object
  */
  
void
fts_properties_free(fts_object_t *obj)
{
  if (obj->properties)
    fts_plist_free(obj->properties);
}


/* init function for the whole thing */

void fts_properties_init(void)
{
  daemon_heap  = fts_heap_new(sizeof(struct daemon_list));
  plist_cell_heap   = fts_heap_new(sizeof(struct fts_plist_cell));
  plist_heap   = fts_heap_new(sizeof(fts_plist_t));
}


