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

typedef void (*fts_instantiate_fun_t)(fts_class_t *);
typedef int (*fts_equiv_fun_t)(int, const fts_atom_t *, int, const fts_atom_t *);

typedef struct fts_class_outlet fts_class_outlet_t;

/* Predefined typeids */
#define FTS_FIRST_OBJECT_TYPEID   16

struct fts_metaclass 
{
  fts_symbol_t name; /* name of the metaclass, i.e. the first name used to register it */

  /* Selector used if an instance of this class is send in a message.
     Defaults to class name.
     Set only for metaclasses representing primitive types.
  */
  fts_symbol_t selector; 
  
  /* A type id that separates primitive types from objects: lower values are primitive types */
  int typeid;

  fts_instantiate_fun_t instantiate_fun;
  fts_equiv_fun_t equiv_fun;
  fts_class_t *inst_list; /* instance data base */
  
  fts_package_t *package; /* home package of the metaclass */
  struct fts_metaclass *next; /* next metaclass with the same name in the same package */
};

struct fts_class 
{
  /* Object management */
  fts_metaclass_t *mcl;

  fts_method_t constructor;
  fts_method_t deconstructor;

  fts_hashtable_t messages;

  int ninlets;
  fts_hashtable_t inlets;
  fts_method_t default_handler;

  int noutlets;
  int out_alloc;
  fts_class_outlet_t *outlets;

  int size;
  fts_heap_t *heap;

  /* Class Instance Data Base support */
  int ac;
  const fts_atom_t *at;
  fts_class_t *next;

  /* property list handling */
  fts_plist_t *properties;		/* class' dynamic properties */

  struct daemon_list *daemons;
};


/* Status return values */
FTS_API fts_status_description_t fts_ClassAlreadyInitialized;
FTS_API fts_status_description_t fts_InletOutOfRange;
FTS_API fts_status_description_t fts_OutletOutOfRange;
FTS_API fts_status_description_t fts_CannotInstantiate;

/**
 * Create an instance of a class.
 *
 * A new instance of the class is created and initialized.
 * If parent patcher is not NULL, the created instance will be added as child to the parent patcher.
 *
 * @fn fts_object_t *fts_metaclass_new_instance( fts_metaclass_t *cl, fts_patcher_t *parent, int ac, const fts_atom_t *at)
 * @param cl the class to instantiate
 * @param parent the parent of the created object
 * @param ac argument count
 * @param at the arguments
 * @return the created object, NULL if instantiation failed
 */
FTS_API fts_object_t *fts_metaclass_new_instance( fts_metaclass_t *cl, fts_patcher_t *parent, int ac, const fts_atom_t *at);
FTS_API fts_metaclass_t *fts_metaclass_get_by_name( fts_symbol_t package_name, fts_symbol_t class_name);

FTS_API fts_metaclass_t *fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun);
FTS_API void fts_class_alias(fts_metaclass_t *mcl, fts_symbol_t alias);

#define fts_metaclass_is_primitive(MCL) ((MCL)->typeid < FTS_FIRST_OBJECT_TYPEID)

FTS_API fts_status_t fts_class_init( fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor);

/* default input handler */
#define fts_class_get_default_handler(c) ((c)->default_handler)
#define fts_class_set_default_handler(c, m) ((c)->default_handler = (m))

FTS_API void fts_class_default_error_handler(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/* method definition */
FTS_API void fts_class_message_varargs(fts_class_t *cl, fts_symbol_t s, fts_method_t mth);
FTS_API void fts_class_inlet(fts_class_t *cl, int winlet, fts_metaclass_t *type, fts_method_t mth);
FTS_API void fts_class_inlet_anything(fts_class_t *cl, int woutlet);

/* outlet definition */
FTS_API void fts_class_outlet(fts_class_t *cl, int woutlet, fts_metaclass_t *type);
FTS_API void fts_class_outlet_message(fts_class_t *cl, int woutlet, fts_symbol_t selector);
FTS_API void fts_class_outlet_anything(fts_class_t *cl, int woutlet);

/* marcros for most popular inlet types */
#define fts_class_inlet_int(c, i, m) fts_class_inlet((c), (i), fts_t_int, (m))
#define fts_class_inlet_float(c, i, m) fts_class_inlet((c), (i), fts_t_float, (m))
#define fts_class_inlet_number(c, i, m) do{ \
  fts_class_inlet((c), (i), fts_t_int, (m)); \
  fts_class_inlet((c), (i), fts_t_float, (m));} while(0)
#define fts_class_inlet_symbol(c, i, m) fts_class_inlet((c), (i), fts_t_symbol, (m))
#define fts_class_inlet_varargs(c, i, m) fts_class_inlet((c), (i), NULL, (m))

/* marcros for most popular outlet types */
#define fts_class_outlet_int(c, i) fts_class_outlet((c), (i), fts_t_int)
#define fts_class_outlet_float(c, i) fts_class_outlet((c), (i), fts_t_float)
#define fts_class_outlet_number(c, i) do{ \
  fts_class_outlet((c), (i), fts_t_int); \
  fts_class_outlet((c), (i), fts_t_float);} while(0)
#define fts_class_outlet_symbol(c, i) fts_class_outlet((c), (i), fts_t_symbol)
#define fts_class_outlet_varargs(c, i) fts_class_outlet((c), (i), NULL)

/* marcros for most popular outlet messages */
#define fts_class_outlet_bang(c, i) fts_class_outlet_message((c), (i), fts_s_bang)

#define fts_class_get_name(C) ((C)->mcl->name)
#define fts_metaclass_get_name(C) ((C)->name)

FTS_API fts_method_t fts_class_get_method(fts_class_t *cl, fts_symbol_t s);
#define fts_class_get_constructor(c) ((c)->constructor)
#define fts_class_get_deconstructor(c) ((c)->deconstructor)

FTS_API const int fts_system_inlet;

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

typedef void (*fts_propagate_fun_t)(void *ptr, fts_object_t *object, int outlet);
