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


/* This file implement the class/metaclass structure */

#include <string.h>

#include <fts/fts.h>
#include <ftsprivate/class.h>
#include <ftsprivate/object.h>
#include <ftsprivate/package.h>
#include <ftsprivate/patcher.h>

const int fts_system_inlet = -1;
static int typeid = FTS_FIRST_OBJECT_TYPEID;

/* Return Status declarations */
fts_status_description_t fts_DuplicatedMetaclass = {"Duplicated metaclass"};
fts_status_description_t fts_ClassAlreadyInitialized = {"class already initialized"};
fts_status_description_t fts_InletOutOfRange = {"inlet out of range"};
fts_status_description_t fts_OutletOutOfRange = {"outlet out of range"};
fts_status_description_t fts_CannotInstantiate = {"Cannot instantiate class"};

/******************************************************************************/
/*                                                                            */
/*                MetaClass  Handling                                         */
/*                                                                            */
/******************************************************************************/

static fts_class_t *fts_class_new(fts_metaclass_t *mcl, int ac, const fts_atom_t *at);

static fts_metaclass_t*
fts_metaclass_new(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun, fts_equiv_fun_t equiv_fun)
{
  fts_metaclass_t *mcl = fts_zalloc(sizeof(fts_metaclass_t));

  mcl->name = name;
  mcl->instantiate_fun = instantiate_fun;
  mcl->equiv_fun = equiv_fun;
  mcl->selector = name;
  mcl->package = NULL;

  mcl->typeid = typeid++;

  /* for simple classes create class instance without arguments */
  if(equiv_fun == 0)
    fts_class_new(mcl, 0, 0);

  return mcl;
}

fts_metaclass_t *
fts_class_install(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
{
  fts_metaclass_t *mcl = fts_metaclass_new(name, instantiate_fun, 0);

  if(name != NULL)
    {
      if(fts_package_add_metaclass(fts_get_current_package(), mcl, name) != fts_ok)
	return 0;
    }

  return mcl;
}

/* for now just recreate the same metaclass and add it to the current package */
void
fts_class_alias(fts_metaclass_t *mcl, fts_symbol_t alias)
{
  fts_package_add_metaclass(fts_get_current_package(), mcl, alias);
}

static fts_metaclass_t *get_metaclass( fts_symbol_t package_name, fts_symbol_t class_name)
{
  fts_package_t *pkg;
  fts_metaclass_t *mcl;

  pkg = fts_package_get( package_name);
  if (pkg == NULL)
    return NULL;

  return fts_package_get_metaclass( pkg, class_name);
}

fts_metaclass_t *
fts_metaclass_get_by_name( fts_symbol_t package_name, fts_symbol_t class_name)
{
  fts_package_t *pkg;
  fts_metaclass_t *mcl;
  fts_iterator_t iter;

  if (package_name != NULL)
    return get_metaclass( package_name, class_name);

  /* ask the kernel package before any other package. The kernel
     classes should not be redefined anyway. If we search the kernel
     package before the required packages, we avoid the loading of all
     (required) packages to find the patcher class.  */
  pkg = fts_get_system_package();

  mcl = fts_package_get_metaclass(pkg, class_name);
  if (mcl != NULL)
    return mcl;

  /* ask the current package */
  pkg = fts_get_current_package();
  mcl = fts_package_get_metaclass(pkg, class_name);
  if (mcl != NULL)
    return mcl;

  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &iter);

  while ( fts_iterator_has_more( &iter)) 
    {
      fts_atom_t a;

      fts_iterator_next( &iter, &a);

      mcl = get_metaclass( fts_get_symbol( &a), class_name);
      if (mcl != NULL)
	return mcl;
  }

  return NULL;
}

fts_object_t *fts_metaclass_new_instance( fts_metaclass_t *mcl, fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_symbol_t error;
  fts_class_t *cl = 0;
  fts_object_t *obj;

  if ( mcl->name == fts_s_patcher)
    {
      /* Patcher behave differently w.r.t. than other objects;
	 the metaclass do not depend on the arguments, but on the inlets/outlets.
	 New patchers are created with zero in zero outs, and changed later 
      */
      fts_atom_t a[2];

      fts_set_int(a, 0);
      fts_set_int(a + 1, 0);
      cl = fts_class_instantiate( patcher_metaclass, 2, a);
    }
  else
    cl = fts_class_instantiate( mcl, ac, at);

  if (!cl)
    return NULL;

  obj = fts_object_new( cl);
  obj->patcher = patcher;

  if ( fts_class_get_constructor(cl))
    fts_class_get_constructor(cl)(obj, fts_system_inlet, fts_s_init, ac, at);

  error = fts_object_get_error(obj);
  
  if (error)
    {
      fts_object_free(obj);
      
      return NULL;
    }

  if ( patcher != NULL)
    fts_patcher_add_object( patcher, obj);

  return obj;
}

/******************************************************************************/
/*                                                                            */
/*              Class  Creation, Definition And Handling                      */
/*                                                                            */
/******************************************************************************/


static void 
fts_class_register( fts_metaclass_t *mcl, int ac, const fts_atom_t *at, fts_class_t *cl)
{
  fts_class_t **p;
  fts_atom_t *store;
  int i;

  cl->ac = ac;
  cl->at = fts_malloc(ac * sizeof(fts_atom_t));
  cl->next = 0;

  store = (fts_atom_t *)cl->at;

  for(i=0; i<ac; i++)
    {
      store[i] = at[i];

      if(fts_is_object(at + i))
	{
	  /* NOTE: here we access the atom structure fields directly,
	     without using the macros, because the macro fts_set_object does not
	     accept to set the atom to type object and null value.
	     However, this is related to metaclass mechanism, which we hope to
	     remove soon.
	  */
	  store[i].type = fts_get_class( at+i);
	  fts_word_set_object( &store[i].value, 0);
	}
    }

  /* append class to end is instance list */
  p = &(mcl->inst_list);

  while(*p)
    p = &((*p)->next);

  *p = cl;
}


static fts_class_t *
fts_class_get( fts_metaclass_t *mcl, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = mcl->inst_list;

  while(cl)
    {
      if ((! mcl->equiv_fun) || mcl->equiv_fun(cl->ac, cl->at, ac, at))
	return cl;

      cl = cl->next;
    }

  return 0;
}

static fts_class_t *
fts_class_new(fts_metaclass_t *mcl, int ac, const fts_atom_t *at)
{
  fts_class_t *cl;
  fts_status_t s;

  cl = fts_zalloc(sizeof(fts_class_t));
  
  cl->constructor = 0;
  cl->deconstructor = 0;

  cl->properties  = 0;
  cl->daemons = 0;
  cl->user_data = 0;
  
  cl->mcl = mcl;
  s = mcl->instantiate_fun(cl, ac, at);
  
  if (s == fts_ok)
    {
      fts_atom_t a;
      
      fts_class_register(mcl, ac, at, cl);
      return cl;
    }
  else
    {
      fts_free(cl);
      return 0;
    }
}

fts_class_t *
fts_class_instantiate(fts_metaclass_t *mcl, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = fts_class_get(mcl, ac, at);

  if(!cl)
    cl = fts_class_new(mcl, ac, at);

  return cl;
}

fts_status_t 
fts_class_init( fts_class_t *cl, unsigned int size, int ninlets, int noutlets, void *user_data)
{
  int i;

  if (cl->size)
    return &fts_ClassAlreadyInitialized;

  cl->size = size;
  cl->heap = fts_heap_new(size);

  /* allocate system inlet declaration */
  cl->sysinlet = fts_zalloc(sizeof(fts_inlet_decl_t));

  fts_hashtable_init(&(cl->sysinlet->messhash), FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
  cl->sysinlet->anything = NULL;

  /* allocate inlet declarations */
  cl->ninlets = ninlets;
  if (ninlets)
    cl->inlets = fts_zalloc(ninlets * sizeof(fts_inlet_decl_t));

  /* init message hashtable and anything method for all inlets */
  for(i=0; i<ninlets; i++)
    {
      fts_hashtable_init(&(cl->inlets[i].messhash), FTS_HASHTABLE_SYMBOL, FTS_HASHTABLE_SMALL);
      cl->inlets[i].anything = NULL;
    }

  /* allocate outlet declarations */
  cl->noutlets = noutlets;
  if (noutlets)
    cl->outlets = fts_zalloc(noutlets * sizeof(fts_outlet_decl_t));

  cl->user_data = user_data;

  return fts_ok;
}

void
fts_method_define(fts_class_t *cl, int winlet, fts_symbol_t s, fts_method_t mth)
{
  if(s != NULL)
    {    
      fts_inlet_decl_t *in;
      
      if (winlet == fts_system_inlet)
	{
	  if(s == fts_s_init)
	    {
	      cl->constructor = mth;
	      return;
	    }
	  else if(s == fts_s_delete)
	    {
	      cl->deconstructor = mth;
	      return;
	    }
	  
	  in = cl->sysinlet;
	}
      else if (winlet < cl->ninlets && winlet >= 0)
	in = cl->inlets + winlet;
      else
	{
	  post("inlet number %d out of range [0..%d] for class `%s', method `%s'\n", winlet, cl->ninlets, fts_class_get_name(cl), s); 
	  return;
	}
      
      if(s == fts_s_anything)
	{
	  if(in->anything != NULL)
	    post("message \"%s\" doubly defined for class %s %d\n", s, fts_class_get_name(cl), winlet);
	  else
	    in->anything = mth;
	}
      else
	{
	  fts_atom_t k, a;

	  fts_set_symbol(&k, s);
	  if(fts_hashtable_get(&in->messhash, &k, &a))
	    post("message \"%s\" doubly defined for class %s %d\n", s, fts_class_get_name(cl), winlet);
	  else
	    {
	      fts_set_pointer(&a, mth);
	      fts_hashtable_put(&in->messhash, &k, &a);
	    }
	}
    }
  else
    post("method definition with NULL selector for class %s\n", winlet, fts_class_get_name(cl)); 
}

void
fts_outlet_type_define( fts_class_t *cl, int woutlet, fts_symbol_t s)
{
  fts_outlet_decl_t *out;

  if (woutlet >= cl->noutlets || woutlet < 0)
    {
      post("fts_outlet_type_define: outlet out of range #%d for class `%s'\n", woutlet, fts_class_get_name(cl));
      return;
    }
  out = &cl->outlets[woutlet];

  if (out->selector)
    {
      post("outlet %d doubly defined for class `%s'\n", woutlet, fts_class_get_name(cl));
      return;
    }

  out->selector = s;
}

static fts_inlet_decl_t *
class_get_inlet_decl(fts_class_t *cl, int inlet)
{
  fts_inlet_decl_t *in;

  if(inlet >= cl->ninlets)
    inlet = cl->ninlets - 1;
  
  if(inlet >= 0)
    return cl->inlets + inlet;

  return NULL;
}

static fts_outlet_decl_t *
class_get_outlet_decl(fts_class_t *cl, int outlet)
{
  if(outlet < 0)
    outlet = 0;
  else if(outlet >= cl->noutlets)
    outlet = cl->noutlets - 1;
  
  return cl->outlets + outlet;
}

static fts_method_t
inlet_get_method(fts_inlet_decl_t *in, fts_symbol_t s)
{
  fts_atom_t k, a;

  fts_set_symbol(&k, s);
  if(fts_hashtable_get(&in->messhash, &k, &a))
    return fts_get_pointer(&a);
  else
    return in->anything;
}

fts_method_t 
fts_class_get_method(fts_class_t *cl, fts_symbol_t s)
{
  fts_inlet_decl_t *in = cl->sysinlet;

  return inlet_get_method(in, s);
}

fts_method_t 
fts_class_inlet_get_method(fts_class_t *cl, int inlet, fts_symbol_t s)
{
  fts_inlet_decl_t *in = class_get_inlet_decl(cl, inlet);

  if(in != NULL)
    return inlet_get_method(in, s);

  return NULL;
}

fts_method_t 
fts_class_inlet_get_anything(fts_class_t *cl, int inlet)
{
  fts_inlet_decl_t *in = class_get_inlet_decl(cl, inlet);

  return in->anything;
}

int
fts_class_inlet_has_anything_only(fts_class_t *cl, int inlet)
{
  fts_inlet_decl_t *in = class_get_inlet_decl(cl, inlet);

  return (fts_hashtable_get_size(&in->messhash) == 0) && (in->anything != NULL);
}

fts_symbol_t 
fts_class_outlet_get_selector(fts_class_t *cl, int outlet)
{
  fts_outlet_decl_t *out = class_get_outlet_decl(cl, outlet);

  return out->selector;
}

/*****************************************************************************
 *
 *  equivalence function library
 *
 */
int
fts_arg_type_equiv(int ac0, const fts_atom_t *at0, int ac1,  const fts_atom_t *at1)
{
  int i;

  if (ac0 != ac1)
    return 0;

  for (i=0; i<ac0; i++)
    if (!fts_atom_same_type(at0 + i, at1 + i))
      return 0;

  return 1;
}

int fts_arg_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  int i;

  if (ac0 != ac1)
    return 0;

  for (i=0; i<ac0; i++)
    {
      if ( !fts_atom_same_type( at0 + i, at1 + i))
	return 0;

      if (fts_is_int(at0 + i))
	{
	  if (fts_get_int(at0 + i) != fts_get_int(at1 + i))
	    return 0;
	}
      else if (fts_is_float(at0 + i))
	{
	  if (fts_get_float(at0 + i) != fts_get_float(at1 + i))
	    return 0;
	}
      else if (fts_is_symbol(at0 + i))
	{
	  if (fts_get_symbol(at0 + i) != fts_get_symbol(at1 + i))
	    return 0;
	}
      else if (fts_is_string(at0 + i))
	{
	  if (strcmp(fts_get_string(at0 + i), fts_get_string(at1 + i)))
	    return 0;
	}
      else if (fts_is_pointer(at0 + i))
	{
	  if (fts_get_pointer(at0 + i) != fts_get_pointer(at1 + i))
	    return 0;
	}
    }

  return 1;
}

/* as the previous, but in case of float values, just test type equivalence;
   may be needed because the client protocol do not guarantee that two equal
   floats are actually float (see expr, that is broken anyway).
*/

int
fts_arg_equiv_or_float(int ac0, const fts_atom_t *at0, int ac1,  const fts_atom_t *at1)
{
  int i;

  if (ac0 != ac1)
    return 0;

  for (i=0; i<ac0; i++)
    {
      if ( !fts_atom_same_type( at0 + i, at1 + i))
	return 0;

      if (fts_is_int(at0 + i))
	{
	  if (fts_get_int(at0 + i) != fts_get_int(at1 + i))
	    return 0;
	}
      else if (fts_is_float(at0 + i))
	{
	  if (! fts_is_float(at1 + i))
	    return 0;
	}
      else if (fts_is_symbol(at0 + i))
	{
	  if (fts_get_symbol(at0 + i) != fts_get_symbol(at1 + i))
	    return 0;
	}
      else if (fts_is_string(at0 + i))
	{
	  if (strcmp(fts_get_string(at0 + i), fts_get_string(at1 + i)))
	    return 0;
	}
      else if (fts_is_pointer(at0 + i))
	{
	  if (fts_get_pointer(at0 + i) != fts_get_pointer(at1 + i))
	    return 0;
	}
    }

  return 1;
}

/* if there is no first arg, return 1 */
int
fts_first_arg_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  if (ac0 == 0  && ac1 == 0)
    return 1;
  else if (ac0 > 0 && ac1 > 0)
    {
      if ( !fts_atom_same_type( at0, at1))
	{
	  if (fts_is_int(at0))
	    {
	      if (fts_get_int(at0) == fts_get_int(at1))
		return 1;
	    }
	  else if (fts_is_float(at0))
	    {
	      if (fts_get_float(at0) == fts_get_float(at1))
		return 1;
	    }
	  else if (fts_is_symbol(at0))
	    {
	      if (fts_get_symbol(at0) == fts_get_symbol(at1))
		return 1;
	    }
	  else if (fts_is_string(at0))
	    {
	      if (! strcmp(fts_get_string(at0), fts_get_string(at1)))
		return 1;
	    }
	  else if (fts_is_pointer(at0))
	    {
	      if (fts_get_pointer(at0) == fts_get_pointer(at1))
		return 1;
	    }
	}

      return 0;
    }
  else
    return 0;
}

int
fts_narg_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return ac0 == ac1;
}

int
fts_never_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return 0;
}

/***********************************************************************
 *
 * Initialization
 *
 */

void fts_kernel_class_init( void)
{
}
