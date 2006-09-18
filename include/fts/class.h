/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/* "system methods" every class should implement */
typedef void (*fts_instantiate_fun_t)(fts_class_t *);
typedef unsigned int (*fts_class_hash_function_t)( const fts_atom_t *);
typedef int (*fts_class_equals_function_t)( const fts_object_t *, const fts_object_t *);
typedef void (*fts_class_description_function_t)(fts_object_t *obj, fts_array_t *array);
typedef void (*fts_class_copy_function_t)( const fts_object_t *, fts_object_t *);
typedef void (*fts_class_array_function_t)(fts_object_t *obj, fts_array_t *array);
typedef void (*fts_class_spost_function_t)(fts_object_t *obj, fts_bytestream_t *stream);
typedef fts_object_t *(*fts_class_guiobject_function_t)(fts_object_t *obj);

typedef struct fts_class_outlet fts_class_outlet_t;

FTS_API void fts_class_instantiate(fts_class_t *cl);

/* Predefined class ids */
#define FTS_FIRST_OBJECT_TYPEID   16

/**************************************************
 *
 *  class documentation
 *
 */
typedef struct fts_class_doc_line
{
  fts_symbol_t name;
  const char *args;
  const char *comment;
  struct fts_class_doc_line *next;
} fts_class_doc_t;

#define fts_class_doc_get_name(l) ((l)->name)
#define fts_class_doc_get_args(l) ((l)->args)
#define fts_class_doc_get_comment(l) ((l)->comment)
#define fts_class_doc_get_next(l) ((l)->next)

FTS_API void fts_class_doc(fts_class_t *cl, fts_symbol_t name, const char *args, const char *comment);
FTS_API void fts_class_doc_post(fts_class_t *cl);
/* fts_class_doc_get appends doc atoms to array */
FTS_API int  fts_class_doc_get (fts_class_t *cl, fts_array_t *output);

/**************************************************
 *
 *  the class structure
 *
 *  N.B.: IF THIS STRUCTURE CHANGES, THE MACRO FTS_PRIMITIVE_CLASS IN
 *        atom.c MUST BE ADAPTED!
 */

struct fts_class
{
  fts_object_t head;
  
  fts_symbol_t name; /* name of the class, i.e. the first name used to register it */
  int type_id; /* a type id that separates primitive types from objects: lower values are primitive types */
  fts_class_t *super_class;
  
  /* The hash function and equality function for this class */
  fts_class_hash_function_t hash_function;
  fts_class_equals_function_t equals_function;
  fts_class_description_function_t description_function;
  fts_class_copy_function_t copy_function;
  fts_class_array_function_t array_function;
  fts_class_spost_function_t spost_function;
  fts_class_guiobject_function_t guiobject_function;
  fts_hashtable_t import_handlers; /* table of import handlers */
  fts_hashtable_t export_handlers; /* table of export handlers */
  
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
  
  fts_class_doc_t *doc;
};

#define fts_class_get_name(C) ((C)->name)
#define fts_class_get_super(C) ((C)->super_class)

#define fts_class_get_constructor(c) ((c)->constructor)
#define fts_class_get_deconstructor(c) ((c)->deconstructor)
#define fts_class_get_input_handler(c) ((c)->input_handler)

#define fts_class_get_hash_function(cl) ((cl)->hash_function)
#define fts_class_get_equals_function(cl) ((cl)->equals_function)
#define fts_class_get_description_function(cl) ((cl)->description_function)
#define fts_class_get_copy_function(cl) ((cl)->copy_function)
#define fts_class_get_array_function(cl) ((cl)->array_function)
#define fts_class_get_spost_function(cl) ((cl)->spost_function)
#define fts_class_get_guiobject_function(cl) ((cl)->guiobject_function)

#define fts_class_get_import_handlers(c) (&(c)->import_handlers)
#define fts_class_get_export_handlers(c) (&(c)->export_handlers)

#define fts_class_set_super(C, s) ((C)->super_class = s)

#define fts_class_set_hash_function( cl, f) ((cl)->hash_function = (f))
#define fts_class_set_equals_function( cl, f) ((cl)->equals_function = (f))
#define fts_class_set_description_function(cl, f) ((cl)->description_function = (f))
#define fts_class_set_copy_function( cl, f) ((cl)->copy_function = (f))
#define fts_class_set_array_function( cl, f) ((cl)->array_function = (f))
#define fts_class_set_spost_function( cl, f) ((cl)->spost_function = (f))
#define fts_class_set_guiobject_function(cl, f) ((cl)->guiobject_function = (f))

#define fts_class_is_primitive(CL) ((CL)->type_id < FTS_FIRST_OBJECT_TYPEID)

#define fts_class_get_doc(C) ((C)->doc)

FTS_API void fts_class_get_messages(const fts_class_t *cl, fts_iterator_t *i);

/**
* Get a method of a class by its message symbol and argument type
 *
 * @fn void fts_class_get_method(fts_class_t *cl, fts_symbol_t s, fts_class_t *type)
 * @param cl the class
 * @param s message symbol
 * @param type class of argument
 * @return the method or NULL if failed
 */
FTS_API fts_method_t fts_class_get_method(fts_class_t *cl, fts_symbol_t s, fts_class_t *type);

/**
* Get the a non-varargs method of a class by its message symbol
 *
 * @fn void fts_class_get_method_novarargs(fts_class_t *cl, fts_symbol_t s, fts_class_t *type)
 * @param cl the class
 * @return the method or NULL if failed
 * @param s message symbol
 */
FTS_API fts_method_t fts_class_get_method_novarargs(fts_class_t *cl, fts_symbol_t s, fts_class_t *type);

/**
* Get the varargs method of a class by its message symbol
 *
 * @fn void fts_class_get_method_varargs(fts_class_t *cl, fts_symbol_t s, fts_class_t *type)
 * @param cl the class
 * @return the method or NULL if failed
 * @param s message symbol
 */
FTS_API fts_method_t fts_class_get_method_varargs(fts_class_t *cl, fts_symbol_t s);

FTS_API const int fts_system_inlet;

/* Status return values */
FTS_API fts_status_description_t fts_ClassAlreadyInitialized;

/**
* Class API
 *
 * @defgroup class_api class API
 */

/**
* Install a class
 * Create a class (without initializing) and register it by name in the current package.
 *
 * @fn fts_class_t *fts_class_install(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
 * @param name the name (NULL for unregistered classes)
 * @param instantiate_fun class initialization function
 * @return the class (handle)
 * @ingroup class_api
 */
FTS_API fts_class_t *fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun);

/**
* Create an alias for a class
 *
 * @fn void fts_class_alias(fts_class_t *cl, fts_symbol_t alias)
 * @param cl the class
 * @param alias alias name
 * @ingroup class_api
 */
FTS_API void fts_class_alias(fts_class_t *cl, fts_symbol_t alias);

/**
* Initialize a class
 *
 * @fn void fts_class_init(fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor)
 * @param cl the class
 * @param size the size of its associated C structure (must have fts_object_t as first entry)
 * @param constructor constructor method
 * @param deconstructor deconstructor method
 * @ingroup class_api
 */
FTS_API void fts_class_init(fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor);

/**
* Set input handler
 * If set the input handler is called for any input of an object comming into an inlet.
 *
 * @fn void fts_class_input_handler(fts_class_t *cl, fts_method_t method)
 * @param cl the class
 * @param method input handler method
 * @ingroup class_api
 */
FTS_API void fts_class_input_handler(fts_class_t *cl, fts_method_t method);

/** add import handler to table of handlers to try 
*
*  An import handler is called with the object to import into, and
*  the list of filename and arguments.
*/
FTS_API void fts_class_import_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t meth);
#define fts_class_import_handler_default(c, m) fts_class_import_handler(c, fts_s_default, m)

/** add export handler to table of handlers to try 
*
*  An export handler is called with the object to export from, and
*  the list of filename and arguments.
*/
FTS_API void fts_class_export_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t meth);
#define fts_class_export_handler_default(c, m) fts_class_export_handler(c, fts_s_default, m)

/**
* Register a method for a given message and argument type (of a single argument or void).
 *
 * @fn void fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t mth)
 * @param cl the class
 * @param s message symbol
 * @param type class of argument (NULL for a single argument of any type, fts_void_class for a void method)
 * @param mth the method
 * @ingroup class_api
 */
FTS_API void fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t mth);

/* marcros for most popular message types */
#define fts_class_message_void(c, s, m) fts_class_message((c), (s), fts_void_class, (m))
#define fts_class_message_int(c, s, m) fts_class_message((c), (s), fts_int_class, (m))
#define fts_class_message_float(c, s,  m) fts_class_message((c), (s), fts_float_class, (m))
#define fts_class_message_number(c, s,  m) do{ \
  fts_class_message((c), (s), fts_int_class, (m)); \
    fts_class_message((c), (s), fts_float_class, (m));} while(0)
#define fts_class_message_symbol(c, s,  m) fts_class_message((c), (s), fts_symbol_class, (m))
#define fts_class_message_varargs(c, s,  m) fts_class_message((c), (s), NULL, (m))

/* deprecated */
#define fts_class_message_atom(c, s,  m) fts_class_message((c), (s), NULL, (m))

/**
* Declare an inlet and register a method for a given argument type (of a single argument or void).
 *
 * @fn void fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t mth)
 * @param cl the class
 * @param winlet index of the inlet
 * @param type class of argument (NULL for a single argument of any type, fts_void_class for a void method)
 * @param mth the method
 * @ingroup class_api
 */
FTS_API void fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t mth);

/* marcros for most popular inlet types */
#define fts_class_inlet_bang(c, i, m) fts_class_inlet((c), (i), fts_void_class, (m))
#define fts_class_inlet_void(c, i, m) fts_class_inlet((c), (i), fts_void_class, (m))
#define fts_class_inlet_int(c, i, m) fts_class_inlet((c), (i), fts_int_class, (m))
#define fts_class_inlet_float(c, i, m) fts_class_inlet((c), (i), fts_float_class, (m))
#define fts_class_inlet_number(c, i, m) do{ \
  fts_class_inlet((c), (i), fts_int_class, (m)); \
    fts_class_inlet((c), (i), fts_float_class, (m));} while(0)
#define fts_class_inlet_symbol(c, i, m) fts_class_inlet((c), (i), fts_symbol_class, (m))
#define fts_class_inlet_varargs(c, i, m) fts_class_inlet((c), (i), NULL, (m))

/* deprecated */
#define fts_class_inlet_atom(c, i, m) fts_class_inlet((c), (i), NULL, (m))

/**
* Declare an empty inlet.
 *
 * @fn void fts_class_inlet_thru(fts_class_t *cl, int winlet)
 * @param cl the class
 * @param winlet index of the inlet
 * @ingroup class_api
 */
FTS_API void fts_class_inlet_thru(fts_class_t *cl, int winlet);

/**
* Declare an outlet of the given type.
 *
 * @fn void fts_class_outlet(fts_class_t *cl, int woutlet, fts_class_t *type)
 * @param cl the class
 * @param woutlet index of the outlet
 * @param type of the outlet (NULL for an untyped outlet)
 * @ingroup class_api
 */
FTS_API void fts_class_outlet(fts_class_t *cl, int woutlet, fts_class_t *type);

/* marcros for most popular outlet types */
#define fts_class_outlet_bang(c, i) fts_class_outlet((c), (i), fts_void_class)
#define fts_class_outlet_void(c, i) fts_class_outlet((c), (i), fts_void_class)
#define fts_class_outlet_int(c, i) fts_class_outlet((c), (i), fts_int_class)
#define fts_class_outlet_float(c, i) fts_class_outlet((c), (i), fts_float_class)
#define fts_class_outlet_number(c, i) do{ \
  fts_class_outlet((c), (i), fts_int_class); \
    fts_class_outlet((c), (i), fts_float_class);} while(0)
#define fts_class_outlet_symbol(c, i) fts_class_outlet((c), (i), fts_symbol_class)

#define fts_class_outlet_varargs(c, i) fts_class_outlet((c), (i), NULL)
#define fts_class_outlet_thru(c, i) fts_class_outlet((c), (i), NULL)

/* deprecated */
#define fts_class_outlet_atom(c, i) fts_class_outlet((c), (i), NULL)

/*****************************************************************************
*
*  "thru" classes
*
*  Thru classes propagate their input directly to the output or the output of another object.
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
