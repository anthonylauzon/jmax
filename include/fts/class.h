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
typedef unsigned int (*fts_hash_function_t)( const fts_atom_t *);
typedef int (*fts_equals_function_t)( const fts_atom_t *, const fts_atom_t *);

typedef struct fts_class_outlet fts_class_outlet_t;

/* Predefined typeids */
#define FTS_FIRST_OBJECT_TYPEID   16

struct fts_class {
  fts_object_t head;

  fts_symbol_t name; /* name of the class, i.e. the first name used to register it */

  /* A type id that separates primitive types from objects: lower values are primitive types */
  int typeid;

  /* The hash function and equality function for this class */
  fts_hash_function_t hash_function;
  fts_equals_function_t equals_function;

  fts_instantiate_fun_t instantiate_fun;

  fts_method_t constructor;
  fts_method_t deconstructor;

  fts_package_t *package;

  fts_hashtable_t *methods;

  int ninlets;
  fts_method_t input_handler;

  int noutlets;
  int out_alloc;
  fts_class_outlet_t *outlets;

  int size;
  fts_heap_t *heap;

  /* property list handling */
  fts_plist_t *properties;		/* class' dynamic properties */

  struct daemon_list *daemons;
};

#define fts_class_get_name(C) ((C)->name)

#define fts_class_get_constructor(c) ((c)->constructor)
#define fts_class_get_deconstructor(c) ((c)->deconstructor)

/* Class hash function and equality function */
#define fts_class_get_hash_function(cl) ((cl)->hash_function)
#define fts_class_get_equals_function(cl) ((cl)->equals_function)

#define fts_class_set_hash_function( cl, fun) ((cl)->hash_function = fun)
#define fts_class_set_equals_function( cl, fun) ((cl)->equals_function = fun)

FTS_API fts_class_t *fts_class_get_by_name( fts_symbol_t package_name, fts_symbol_t class_name);

FTS_API fts_class_t *fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun);
FTS_API void fts_class_alias(fts_class_t *cl, fts_symbol_t alias);

#define fts_class_is_primitive(CL) ((CL)->typeid < FTS_FIRST_OBJECT_TYPEID)

FTS_API void fts_class_init(fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor);

/* default input handler */
#define fts_class_get_input_handler(c) ((c)->input_handler)
#define fts_class_input_handler(c, m) ((c)->input_handler = (m))

FTS_API void fts_class_default_error_handler(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

/* message definition */
FTS_API void fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t mth);
FTS_API void fts_class_message_varargs(fts_class_t *cl, fts_symbol_t s, fts_method_t mth);

/* marcros for most popular message types */
#define fts_class_message_void(c, s, m) fts_class_message((c), (s), fts_void_class, (m))
#define fts_class_message_int(c, s, m) fts_class_message((c), (s), fts_int_class, (m))
#define fts_class_message_float(c, s,  m) fts_class_message((c), (s), fts_float_class, (m))
#define fts_class_message_number(c, s,  m) do{ \
  fts_class_message((c), (s), fts_int_class, (m)); \
    fts_class_message((c), (s), fts_float_class, (m));} while(0)
#define fts_class_message_symbol(c, s,  m) fts_class_message((c), (s), fts_symbol_class, (m))
#define fts_class_message_atom(c, s,  m) fts_class_message((c), (s), NULL, (m))

/* inlet definition */
FTS_API void fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t mth);
FTS_API void fts_class_inlet_varargs(fts_class_t *cl, int winlet, fts_method_t mth);
FTS_API void fts_class_inlet_thru(fts_class_t *cl, int winlet);

/* outlet definition */
FTS_API void fts_class_outlet(fts_class_t *cl, int woutlet, fts_class_t *type);

/* marcros for most popular inlet types */
#define fts_class_inlet_bang(c, i, m) fts_class_inlet((c), (i), fts_void_class, (m))
#define fts_class_inlet_void(c, i, m) fts_class_inlet((c), (i), fts_void_class, (m))
#define fts_class_inlet_int(c, i, m) fts_class_inlet((c), (i), fts_int_class, (m))
#define fts_class_inlet_float(c, i, m) fts_class_inlet((c), (i), fts_float_class, (m))
#define fts_class_inlet_number(c, i, m) do{ \
  fts_class_inlet((c), (i), fts_int_class, (m)); \
  fts_class_inlet((c), (i), fts_float_class, (m));} while(0)
#define fts_class_inlet_symbol(c, i, m) fts_class_inlet((c), (i), fts_symbol_class, (m))
#define fts_class_inlet_atom(c, i, m) fts_class_inlet((c), (i), NULL, (m))

/* marcros for most popular outlet types */
#define fts_class_outlet_bang(c, i) fts_class_outlet((c), (i), fts_void_class)
#define fts_class_outlet_void(c, i) fts_class_outlet((c), (i), fts_void_class)
#define fts_class_outlet_int(c, i) fts_class_outlet((c), (i), fts_int_class)
#define fts_class_outlet_float(c, i) fts_class_outlet((c), (i), fts_float_class)
#define fts_class_outlet_number(c, i) do{ \
  fts_class_outlet((c), (i), fts_int_class); \
  fts_class_outlet((c), (i), fts_float_class);} while(0)
#define fts_class_outlet_symbol(c, i) fts_class_outlet((c), (i), fts_symbol_class)
#define fts_class_outlet_atom(c, i) fts_class_outlet((c), (i), NULL)

#define fts_class_outlet_varargs(c, i) fts_class_outlet((c), (i), NULL)
#define fts_class_outlet_message(c, i) fts_class_outlet((c), (i), NULL)
#define fts_class_outlet_thru(c, i) fts_class_outlet((c), (i), NULL)

/* get (message) methods */
FTS_API fts_method_t fts_class_get_method(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, int *varargs);
FTS_API fts_method_t fts_class_get_method_varargs(fts_class_t *cl, fts_symbol_t s);

FTS_API const int fts_system_inlet;

/* Status return values */
FTS_API fts_status_description_t fts_ClassAlreadyInitialized;
FTS_API fts_status_description_t fts_InletOutOfRange;
FTS_API fts_status_description_t fts_OutletOutOfRange;
FTS_API fts_status_description_t fts_CannotInstantiate;

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
