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
#include <ftsprivate/package.h>

const int fts_SystemInlet = -1;

static int typeid = FTS_FIRST_OBJECT_TYPEID;

/* Return Status declarations */

fts_status_description_t fts_DuplicatedMetaclass = {"Duplicated metaclass"};
fts_status_description_t fts_ClassAlreadyInitialized = {"class already initialized"};
fts_status_description_t fts_InletOutOfRange = {"inlet out of range"};
fts_status_description_t fts_OutletOutOfRange = {"outlet out of range"};
fts_status_description_t fts_OutletAlreadyDefined = {"outlet already defined"};
fts_status_description_t fts_CannotInstantiate = {"Cannot instantiate class"};

/* Static  declarations  */
static fts_heap_t *class_mess_heap;

/* Forward declarations */

static fts_class_mess_t *fts_class_mess_create(fts_symbol_t s, fts_method_t mth,
					       int mandatory_args, int nargs, fts_symbol_t *arg_types);

static int fts_class_mess_exists(fts_inlet_decl_t *in, fts_class_mess_t *msg);


/******************************************************************************/
/*                                                                            */
/*                Utilities                                                   */
/*                                                                            */
/******************************************************************************/

static void fts_array_alloc( void **array, unsigned int size, unsigned int *nalloc, unsigned int wanted)
{
#define ALLOC_INC 8
  if (*nalloc < wanted)
    {
      unsigned int newalloc = wanted + ALLOC_INC;

      *array = fts_realloc(*array, newalloc*size);

      memset((char *)(*array)+(*nalloc)*size, 0, (newalloc-*nalloc)*size);
      *nalloc = newalloc;
    }
}


static void fts_atom_type_copy( int ac, fts_symbol_t *at, fts_symbol_t **sat)
{
  if (ac > 0)
    {
      int i;

      *sat = (fts_symbol_t *) fts_malloc( ac*sizeof(fts_symbol_t ) );

      for (i = 0; i < ac; i++)
	(*sat)[i]=at[i];
    }
  else
    *sat = 0;
}



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
  mcl->package = 0;

  mcl->typeid = typeid++;

  /* for simple classes create first class instance without arguments */
  if(equiv_fun == 0)
    fts_class_new(mcl, 0, 0);

  return mcl;
}

fts_metaclass_t *
fts_metaclass_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun, fts_equiv_fun_t equiv_fun)
{
  fts_metaclass_t *mcl = fts_metaclass_new(name, instantiate_fun, equiv_fun);

  if(name != NULL)
    {
      if(fts_package_add_metaclass(fts_get_current_package(), mcl) != fts_Success)
	return 0;
    }

  return mcl;
}

fts_metaclass_t *
fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
{
  return fts_metaclass_install( name, instantiate_fun, 0);
}

void 
fts_alias_install( fts_symbol_t alias_name, fts_symbol_t class_name)
{
  fts_package_add_alias(fts_get_current_package(), alias_name, class_name);
}

fts_metaclass_t*
fts_metaclass_get_by_name(fts_symbol_t name)
{
  fts_package_t *pkg;
  fts_metaclass_t *mcl;
  fts_iterator_t pkg_iter;
  fts_atom_t pkg_name;

  /* ask the kernel package before any other package. The kernel
     classes should not be redefined anyway. If we search the kernel
     package before the required packages, we avoid the loading of all
     (required) packages to find the patcher class.  */
  pkg = fts_get_system_package();

  mcl = fts_package_get_metaclass(pkg, name);
  if (mcl != NULL) {
    return mcl;
  }

  /* ask the current package */
  pkg = fts_get_current_package();
  mcl = fts_package_get_metaclass(pkg, name);
  if (mcl != NULL) {
    return mcl;
  }

  /* ask the required packages of the current package */
  fts_package_get_required_packages(pkg, &pkg_iter);

  while ( fts_iterator_has_more( &pkg_iter)) {
    fts_iterator_next( &pkg_iter, &pkg_name);
    pkg = fts_package_get(fts_get_symbol(&pkg_name));
    
    if (pkg == NULL) {
      continue;
    }

    mcl = fts_package_get_metaclass(pkg, name);
    if (mcl != NULL) {
      return mcl;
    }
  }

  return NULL;
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
  
  if (s == fts_Success)
    {
      fts_atom_t a;
      
      fts_class_register(mcl, ac, at, cl);
      
      /* put the ninlets and noutlets in the class */
      fts_set_int(&a, cl->ninlets);
      fts_class_put_prop(cl, fts_s_ninlets, &a);
      
      fts_set_int(&a, cl->noutlets);
      fts_class_put_prop(cl, fts_s_noutlets, &a);

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
  if (cl->size)
    return &fts_ClassAlreadyInitialized;

  cl->size = size;
  cl->heap = fts_heap_new(size);

  cl->sysinlet = fts_zalloc(sizeof(fts_inlet_decl_t));

  cl->ninlets = ninlets;
  if (ninlets)
    cl->inlets = fts_zalloc(ninlets * sizeof(fts_inlet_decl_t));

  cl->noutlets = noutlets;
  if (noutlets)
    cl->outlets = fts_zalloc(noutlets * sizeof(fts_outlet_decl_t));

  cl->user_data = user_data;

  return fts_Success;
}

fts_status_t
fts_method_define_optargs(fts_class_t *cl, int winlet, fts_symbol_t s, fts_method_t mth, int nargs, fts_symbol_t *arg_types, int mandatory_args)
{
  fts_inlet_decl_t *in;
  fts_class_mess_t *msg;

  if (winlet == fts_SystemInlet)
    {
      if(s == fts_s_init)
	{
	  cl->constructor = mth;
	  return fts_Success;
	}
      else if(s == fts_s_delete)
	{
	  cl->deconstructor = mth;
	  return fts_Success;
	}
	
      in = cl->sysinlet;
    }
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    {
      post("fts_method_define: inlet number %d out of range [0..%d] for class `%s', method `%s'\n", 
	   winlet, cl->ninlets, fts_class_get_name(cl),
	   (s ? s : "")); 

      return &fts_InletOutOfRange;
    }

  msg = fts_class_mess_create(s, mth, mandatory_args, nargs, arg_types);

  if (fts_class_mess_exists(in, msg))
    {
      post("fts_method_define: doubly defined method, class %s, inlet number %d message '%s'\n", 
	   fts_class_get_name(cl), winlet, (s? s: ""));
    }
  else
    {
      fts_array_alloc((void **)&in->messlist, sizeof(fts_class_mess_t *), &in->nalloc, in->nmess+1);
      in->messlist[in->nmess++] = msg;
    }

  return fts_Success;
}

fts_status_t 
fts_outlet_type_define_optargs( fts_class_t *cl, int woutlet, fts_symbol_t s, int ac, fts_symbol_t *at, int mandatory_args)
{
  fts_outlet_decl_t *out;

  if (woutlet >= cl->noutlets || woutlet < 0)
    {
      post("fts_outlet_type_define: outlet out of range #%d for class `%s'\n", woutlet,
	   fts_class_get_name(cl));
      return &fts_OutletOutOfRange;
    }
  out = &cl->outlets[woutlet];

  if (out->tmess.symb)
    {
      post("fts_outlet_type_define: outlet #%d already defined for class `%s'\n", woutlet,
	   fts_class_get_name(cl));
      return &fts_OutletAlreadyDefined;
    }

  out->tmess.symb = s;
  out->tmess.mandatory_args = mandatory_args;
  out->tmess.nargs = ac;
  fts_atom_type_copy(ac, at, &(out->tmess.arg_types));

  return fts_Success;
}

/******************************************************************************/
/*                                                                            */
/*                    Class_mess (Method) handling                            */
/*                                                                            */
/******************************************************************************/



static fts_class_mess_t *
fts_class_mess_create(fts_symbol_t s, fts_method_t mth, int mandatory_args, int nargs, fts_symbol_t *arg_types)
{
  fts_class_mess_t *cm = (fts_class_mess_t *) fts_heap_alloc(class_mess_heap);

  fts_atom_type_copy(nargs, arg_types, &(cm->tmess.arg_types));
  cm->tmess.mandatory_args = mandatory_args;
  cm->tmess.nargs = nargs;
  cm->tmess.symb = s;
  cm->mth = mth;

  return cm;
}

fts_class_mess_t *
fts_class_mess_inlet_get(fts_inlet_decl_t *in, fts_symbol_t s,  int *panything)
{
  fts_class_mess_t **messtable, *mess_anything = 0;
  int i;

  messtable = in->messlist;

  for (i = 0; i < (int)in->nmess; i++)
    {
      if (messtable[i]->tmess.symb == s)
	{
	  *panything = 0;
	  return messtable[i];
	}
      else if (messtable[i]->tmess.symb == fts_s_anything)
	mess_anything = messtable[i];
    }
  
  *panything = 1;
  return mess_anything;
}

/* first try of something new 22.3.2000
fts_class_mess_t *
fts_simple_class_mess_get(fts_inlet_decl_t *in, fts_symbol_t s)
{
  fts_class_mess_t **messtable = in->messlist;
  int i;

  for (i=0; i<in->nmess; i++)
    if (messtable[i]->tmess.symb == s)
      return messtable[i];
  
  return 0;
}
*/

fts_class_mess_t *
fts_class_mess_get(fts_class_t *cl, int winlet, fts_symbol_t s)
{
  fts_inlet_decl_t *in;
  int panything;

  if (winlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    return 0;

  return fts_class_mess_inlet_get(in, s, &panything);
}


static int
fts_class_mess_exists(fts_inlet_decl_t *in, fts_class_mess_t *msg)
{
  fts_class_mess_t **mess;
  fts_symbol_t s = msg->tmess.symb;
  int n, nmess = in->nmess;

  for (mess = in->messlist, n = 0; n < nmess; mess++, n++)
    if ((*mess)->tmess.symb == s)
      return 1;
  
  return 0;
}

fts_method_t fts_class_get_method( fts_class_t *cl, int inlet, fts_symbol_t s)
{
  fts_inlet_decl_t *in;
  fts_class_mess_t **mess;
  int i; 

  if (inlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (inlet < cl->ninlets && inlet >= 0)
    in = &cl->inlets[inlet];
  else
    return 0;

  mess = in->messlist;

  for( i = 0; i < in->nmess; i++)
    {
      if ((*mess)->tmess.symb == s)
	return (*mess)->mth;

      mess++; 
    }
  
  return 0;
}

/*****************************************************************************
 *
 *  "thru" classes
 *
 *  Thru classes don't type in- and outlets.
 *  The input propagates directly to the output or the output of another object.
 *
 *  Thru classes must implement a method fts_s_propagate_input.
 *  This message is send to each object while traversing the graph in order 
 *  propagate there input to their outputs (e.g. fork) or directly to the output 
 *  of other objects connected by index, name or variable (e.g. inlet/outlet, send/receive).
 *
 *  The called method will declare one by one the outlets to which the input 
 *  is propagated using the received function and context (structure).
 *    
 *     fts_propagate_fun_t propagate_fun = (fts_propagate_fun_t)fts_get_pointer(at + 0);
 *     void *propagate_context = (fts_dspgraph_t *)fts_get_pointer(at + 1);
 *
 *     propagate_fun(propagate_context, <object>, <outlet>);
 *
 */

void
fts_class_define_thru(fts_class_t *class, fts_method_t propagate_input)
{
  fts_atom_t a;

  if(propagate_input)
    fts_method_define_varargs(class, fts_SystemInlet, fts_s_propagate_input, propagate_input);

  fts_set_int(&a, 1);
  fts_class_put_prop(class, fts_s_thru, &a);
}

int
fts_class_is_thru(fts_class_t *class)
{
  return (fts_class_get_prop(class, fts_s_thru) != 0);
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
  class_mess_heap = fts_heap_new(sizeof(fts_class_mess_t));
}
