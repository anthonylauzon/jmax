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

typedef fts_status_t (*fts_instantiate_fun_t)(fts_class_t *, int, const fts_atom_t *);
typedef int (*fts_equiv_fun_t)(int, const fts_atom_t *, int, const fts_atom_t *);

typedef struct fts_inlet_decl fts_inlet_decl_t;
typedef struct fts_outlet_decl fts_outlet_decl_t;

/* Predefined typeids */
#define FTS_FIRST_OBJECT_TYPEID   16

struct fts_metaclass {
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

struct fts_class {
  /* Object management */
  fts_metaclass_t *mcl;
  fts_inlet_decl_t *sysinlet;

  fts_method_t constructor;
  fts_method_t deconstructor;

  int ninlets;
  fts_inlet_decl_t *inlets;

  int noutlets;
  fts_outlet_decl_t *outlets;

  int size;
  fts_heap_t *heap;

  /* Class Instance Data Base support */
  int ac;
  const fts_atom_t *at;
  fts_class_t *next;

  /* property list handling */
  fts_plist_t *properties;		/* class' dynamic properties */

  struct daemon_list *daemons;

  /* User data */
  void *user_data;
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


/* Meta classes functions */
FTS_API fts_metaclass_t *fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun);
FTS_API void fts_class_alias(fts_metaclass_t *mcl, fts_symbol_t alias);

#define fts_metaclass_get_selector(MCL) ((MCL)->selector ? (MCL)->selector : (MCL)->name)
#define fts_metaclass_is_primitive(MCL) ((MCL)->typeid < FTS_FIRST_OBJECT_TYPEID)

/* Class functions  and macros */
#define FTS_VAR_ARGS  -1

FTS_API fts_status_t fts_class_init( fts_class_t *, unsigned int, int ninlets, int noutlets, void *);
FTS_API fts_class_t *fts_class_instantiate(fts_metaclass_t *mcl, int ac, const fts_atom_t *at);

FTS_API fts_metaclass_t *fts_metaclass_get_by_name( fts_symbol_t package_name, fts_symbol_t class_name);

/* method definition */
FTS_API void fts_method_define(fts_class_t *cl, int winlet, fts_symbol_t s, fts_method_t fun);
#define fts_method_define_varargs(class, winlet, s, fun)  \
  fts_method_define(class, winlet, s, fun)

#define fts_method_define_int(class, winlet, fun) \
  fts_method_define(class, winlet, fts_s_int, fun);

#define fts_method_define_float(class, winlet, fun) \
  fts_method_define(class, winlet, fts_s_float, fun);

#define fts_method_define_number(class, winlet, fun) \
  fts_method_define(class, winlet, fts_s_int, fun); \
  fts_method_define(class, winlet, fts_s_float, fun);

#define fts_method_define_symbol(class, winlet, fun) \
  fts_method_define(class, winlet, fts_s_symbol, fun);

#define fts_method_define_bang(class, winlet, fun) \
  fts_method_define(class, winlet, fts_s_bang, fun);

#define fts_method_define_anything(class, winlet, fun) \
  fts_method_define(class, winlet, fts_s_anything, fun);

/* outlet type definition */
FTS_API void fts_outlet_type_define(fts_class_t *cl, int woutlet, fts_symbol_t s);
#define fts_outlet_type_define_varargs(class, woutlet, s)  \
          fts_outlet_type_define(class, woutlet, s) 

#define fts_class_get_name(C) ((C)->mcl->name)

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

/*****************************************************************************
 *
 *  equivalence function library
 *
 */
FTS_API int fts_arg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_arg_equiv_or_float( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_first_arg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_narg_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_never_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);
FTS_API int fts_arg_type_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1);

