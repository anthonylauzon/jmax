/*
 *                      Copyright (c) 1997 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *
 *  Eric Viara for Ircam, January 1995
 *  The real time team, Ircam, 96, 97
 */

/* This file implement the class/metaclass structure */


#include "sys.h"
#include "lang/mess.h"
#include "lang/mess/messP.h"
#include "lang/utils.h"

/* Return Status declarations */

fts_status_description_t fts_DuplicatedMetaclass = {"Duplicated metaclass"};
fts_status_description_t fts_ClassAlreadyInitialized = {"class already initialized"};
fts_status_description_t fts_InletOutOfRange = {"inlet out of range"};
fts_status_description_t fts_OutletOutOfRange = {"outlet out of range"};
fts_status_description_t fts_OutletAlreadyDefined = {"outlet already defined"};
fts_status_description_t fts_CannotInstantiate = {"Cannot instantiate class"};


/* Static  declarations  */

static fts_hash_table_t fts_metaclass_table;
static fts_hash_table_t fts_metaclass_alias_table;
static fts_heap_t *class_mess_heap;

/* Forward declarations */

static fts_class_mess_t *fts_class_mess_create(fts_symbol_t s, fts_method_t mth,
					       int mandatory_args, int nargs, fts_symbol_t *arg_types);



/******************************************************************************/
/*                                                                            */
/*                Init                                                        */
/*                                                                            */
/******************************************************************************/

/* initialize the class/metaclasses related data structures */

const int fts_SystemInlet = -1;

void
fts_classes_init(void)
{
  /* Initialize the heaps */

  fts_hash_table_init(&fts_metaclass_table);
  fts_hash_table_init(&fts_metaclass_alias_table);
  class_mess_heap = fts_heap_new(sizeof(fts_class_mess_t));
}

/******************************************************************************/
/*                                                                            */
/*                Utilities                                                   */
/*                                                                            */
/******************************************************************************/

static void
fts_array_alloc(void **array, int size, int *nalloc, int wanted)
{
#define ALLOC_INC 8
  if (*nalloc < wanted)
    {
      int newalloc = wanted + ALLOC_INC;
      if (*array)
	*array = fts_realloc(*array, newalloc*size);
      else
	*array = fts_malloc(newalloc*size);

      memset((char *)(*array)+(*nalloc)*size, 0, (newalloc-*nalloc)*size);
      *nalloc = newalloc;
    }
}



static void
fts_atom_type_copy(int ac, fts_symbol_t *at, fts_symbol_t **sat)
{
  if (ac > 0)
    {
      int i;

      *sat = (fts_symbol_t *) fts_block_alloc(ac*sizeof(fts_symbol_t ));

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

fts_status_t
fts_metaclass_create(fts_symbol_t name,
		     fts_method_instantiate_t mth_instantiate,
		     fts_method_equiv_t mth_equiv)
{
  fts_atom_t data;
  fts_metaclass_t *mcl;

  mcl = fts_zalloc(sizeof(fts_metaclass_t));

  mcl->mth_instantiate = mth_instantiate;
  mcl->mth_equiv = mth_equiv;
  mcl->name = name;

  if (fts_hash_table_lookup(&fts_metaclass_table, name, &data))
    {
      return &fts_DuplicatedMetaclass;
    }
  else
    {
      fts_set_ptr(&data, mcl);
      fts_hash_table_insert(&fts_metaclass_table, name, &data);
    }

  return fts_Success;
}


void
fts_metaclass_alias(fts_symbol_t new_name, fts_symbol_t old_name)
{
  fts_atom_t data;

  if ((fts_hash_table_lookup(&fts_metaclass_alias_table, new_name, &data)) ||
      (fts_hash_table_lookup(&fts_metaclass_table, new_name, &data)))
    return;			/* error: duplicated meta class */
  else
    {
      fts_set_symbol(&data, old_name);
      fts_hash_table_insert(&fts_metaclass_alias_table, new_name, &data);
    }
}


static fts_symbol_t 
fts_metaclass_get_real_name(fts_symbol_t name)
{
  fts_atom_t data;

  if (fts_hash_table_lookup(&fts_metaclass_alias_table, name, &data))
    return (fts_symbol_t )fts_get_symbol(&data);
  else
    return name;
}


fts_metaclass_t *
fts_metaclass_get_by_name(fts_symbol_t name)
{
  fts_atom_t data;

  if (fts_hash_table_lookup(&fts_metaclass_table, fts_metaclass_get_real_name(name), &data))
    return (fts_metaclass_t *) fts_get_ptr(&data);
  else
    return (fts_metaclass_t *) 0;
}



/******************************************************************************/
/*                                                                            */
/*              Class  Creation, Definition And Handling                      */
/*                                                                            */
/******************************************************************************/


static void
fts_class_register(fts_metaclass_t *mcl, int ac, const fts_atom_t *at, fts_class_t *cl)
{
  cl->ac = ac;
  cl->at = fts_malloc(ac * sizeof(fts_atom_t));
  memcpy((char *)cl->at, at, ac * sizeof(fts_atom_t));

  cl->next = mcl->inst_list;
  mcl->inst_list = cl;
}


static fts_class_t *
fts_class_get(fts_metaclass_t *mcl, int ac, const fts_atom_t *at)
{
  fts_class_t *cl = mcl->inst_list;

  while(cl)
    {
      if ((! mcl->mth_equiv) || mcl->mth_equiv(cl->ac, cl->at, ac, at))
	return cl;

      cl = cl->next;
    }

  return 0;
}

/* Note that the generity discrimination is made procedurally
   by the class instantiation functions, that should return
   an error if the arguments are not ok.
   *We cannot use init arguments because the classes do not exists yet*.
 */


fts_class_t *
fts_class_instantiate(int ac, const fts_atom_t *at)
{
  fts_metaclass_t *mcl;
  fts_class_t *cl;

  mcl = fts_metaclass_get_by_name(fts_get_symbol(&at[0]));

  if (! mcl)
    return 0;

  cl = fts_class_get(mcl, ac, at);

  if (cl)
    return cl;
  else
    {
      fts_status_t s;

      cl = fts_zalloc(sizeof(fts_class_t));

      cl->properties  = 0;
      cl->daemons = 0;

      cl->mcl = mcl;
      s = mcl->mth_instantiate(cl, ac, at);

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
	fts_free(cl);
    }

  return 0;
}


fts_status_t
fts_class_init(fts_class_t *cl, unsigned int size, int ninlets, int noutlets, void *user_data)
{
  if (cl->size)
    return &fts_ClassAlreadyInitialized;

  cl->size = size;

  cl->sysinlet = fts_zalloc(sizeof(fts_inlet_decl_t));

  cl->ninlets = ninlets;
  if (ninlets)
    cl->inlets = fts_zalloc(ninlets*sizeof(fts_inlet_decl_t));

  cl->noutlets = noutlets;
  if (noutlets)
    cl->outlets = fts_zalloc(noutlets*sizeof(fts_outlet_decl_t));

  cl->user_data = user_data;

  return fts_Success;
}


fts_status_t
fts_method_define_optargs(fts_class_t *cl, int winlet, fts_symbol_t s,
			  fts_method_t mth, 
			  int nargs, fts_symbol_t *arg_types,
			  int mandatory_args)
{
  fts_inlet_decl_t *in;
  fts_class_mess_t *msg;

  if (winlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    {
      post("fts_method_define: inlet number %d out of range [0..%d] for class `%s', method `%s'\n", 
	   winlet, cl->ninlets, fts_symbol_name(fts_get_class_name(cl)),
	   (s ? fts_symbol_name(s) : "")); 

      return &fts_InletOutOfRange;
    }

  msg = fts_class_mess_create(s, mth, mandatory_args, nargs, arg_types);

  if (fts_class_mess_exists(in, msg))
    {
      post("fts_method_define: doubly defined method, class %s, inlet number %d message `%s'\n", 
	   fts_symbol_name(fts_get_class_name(cl)), winlet, (s ? fts_symbol_name(s) : ""));
    }
  else
    {
      fts_array_alloc((void **)&in->messlist, sizeof(fts_class_mess_t *),
		      &in->nalloc, in->nmess+1);
      in->messlist[in->nmess++] = msg;
    }

  return fts_Success;
}


fts_status_t
fts_outlet_type_define_optargs(fts_class_t *cl, int woutlet, fts_symbol_t s,
			       int ac, fts_symbol_t *at,
			       int mandatory_args)
{
  fts_outlet_decl_t *out;

  if (woutlet >= cl->noutlets || woutlet < 0)
    {
      post("fts_outlet_type_define: outlet out of range #%d for class `%s'\n", woutlet,
	   fts_symbol_name(fts_get_class_name(cl)));
      return &fts_OutletOutOfRange;
    }
  out = &cl->outlets[woutlet];

  if (out->tmess.symb)
    {
      post("fts_outlet_type_define: outlet #%d already defined for class `%s'\n", woutlet,
	   fts_symbol_name(fts_get_class_name(cl)));
      return &fts_OutletAlreadyDefined;
    }

  out->tmess.symb = s;
  out->tmess.mandatory_args = mandatory_args;
  out->tmess.nargs = ac;
  fts_atom_type_copy(ac, at, &(out->tmess.arg_types));

  return fts_Success;
}


fts_symbol_t 
fts_get_class_name(fts_class_t *cl)
{
  return cl->mcl->name;
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

  for (i = 0; i < in->nmess; i++)
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


fts_class_mess_t *
fts_class_mess_get(fts_class_t *cl, int winlet, fts_symbol_t s, int *panything)
{
  fts_inlet_decl_t *in;

  if (winlet == fts_SystemInlet)
    in = cl->sysinlet;
  else if (winlet < cl->ninlets && winlet >= 0)
    in = &cl->inlets[winlet];
  else
    return 0;

  return fts_class_mess_inlet_get(in, s, panything);
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


/******************************************************************************/
/*                                                                            */
/*                    Equivalence Function Library                            */
/*                                                                            */
/******************************************************************************/

int
fts_arg_type_equiv(int ac0, const fts_atom_t *at0, int ac1,  const fts_atom_t *at1)
{
  int n;

  if (ac0 != ac1)
    return 0;

  for (n = 0; n < ac0; n++, at0++, at1++)
    if (fts_get_type(at0) != fts_get_type(at1))
      return 0;

  return 1;
}



int fts_arg_equiv(int ac0, const fts_atom_t *at0, int ac1, const
fts_atom_t *at1) { int n;

  if (ac0 != ac1)
    return 0;

  for (n = 0; n < ac0; n++, at0++, at1++)
    {
      if (fts_get_type(at0) != fts_get_type(at1))
	return 0;

      if (fts_is_long(at0))
	{
	  if (fts_get_long(at0) != fts_get_long(at1))
	    return 0;
	}
      else if (fts_is_float(at0))
	{
	  if (fts_get_float(at0) != fts_get_float(at1))
	    return 0;
	}
      else if (fts_is_symbol(at0))
	{
	  if (fts_get_symbol(at0) != fts_get_symbol(at1))
	    return 0;
	}
      else if (fts_is_string(at0))
	{
	  if (strcmp(fts_get_string(at0), fts_get_string(at1)))
	    return 0;
	}
      else if (fts_is_ptr(at0))
	{
	  if (fts_get_ptr(at0) != fts_get_ptr(at1))
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
  int n;

  if (ac0 != ac1)
    return 0;

  for (n = 0; n < ac0; n++, at0++, at1++)
    {
      if (fts_get_type(at0) != fts_get_type(at1))
	return 0;

      if (fts_is_long(at0))
	{
	  if (fts_get_long(at0) != fts_get_long(at1))
	    return 0;
	}
      else if (fts_is_float(at0))
	{
	  if (! fts_is_float(at1))
	    return 0;
	}
      else if (fts_is_symbol(at0))
	{
	  if (fts_get_symbol(at0) != fts_get_symbol(at1))
	    return 0;
	}
      else if (fts_is_string(at0))
	{
	  if (strcmp(fts_get_string(at0), fts_get_string(at1)))
	    return 0;
	}
      else if (fts_is_ptr(at0))
	{
	  if (fts_get_ptr(at0) != fts_get_ptr(at1))
	    return 0;
	}
    }

  return 1;
}

/* if there is no first arg, return 1 */

int
fts_first_arg_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  if (ac0 == 1  && ac1 == 1)
    return 1;
  else if (ac0 > 1  && ac1 > 1)
    {
      at0++;
      at1++;

      if (fts_get_type(at0) == fts_get_type(at1))
	{
	  if (fts_is_long(at0))
	    {
	      if (fts_get_long(at0) == fts_get_long(at1))
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
	  else if (fts_is_ptr(at0))
	    {
	      if (fts_get_ptr(at0) == fts_get_ptr(at1))
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


int
fts_always_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return 1;
}
