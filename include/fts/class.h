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

/**
 * Class
 *
 * The FTS class
 *
 * @defgroup class class
 * @ingroup fts_obj_class_method
 */

/* "system methods" every class should implement */
/** 
 * @var typedef void (*fts_instantiate_fun_t)(fts_class_t *)
 * @brief fts_instantiate_fun_t ...
 * @ingroup class
 */
typedef void (*fts_instantiate_fun_t)(fts_class_t *);
/** 
 * @var typedef unsigned int (*fts_class_hash_function_t)( const fts_atom_t *)
 * @brief fts_class_hash_function_t ...
 * @ingroup class
 */
typedef unsigned int (*fts_class_hash_function_t)( const fts_atom_t *);
/** 
 * @var typedef int (*fts_class_equals_function_t)( const fts_object_t *, const fts_object_t *)
 * @brief fts_class_equals_function_t ...
 * @ingroup class
 */
typedef int (*fts_class_equals_function_t)( const fts_object_t *, const fts_object_t *);
/** 
 * @var typedef void (*fts_class_description_function_t)(fts_object_t *obj, fts_array_t *array)
 * @brief fts_class_description_function_t ...
 * @ingroup class
 */
typedef void (*fts_class_description_function_t)(fts_object_t *obj, fts_array_t *array);
/** 
 * @var typedef void (*fts_class_copy_function_t)( const fts_object_t *, fts_object_t *)
 * @brief fts_class_copy_function_t ...
 * @ingroup class
 */
typedef void (*fts_class_copy_function_t)( const fts_object_t *, fts_object_t *);
/** 
 * @var typedefvoid (*fts_class_array_function_t)(fts_object_t *obj, fts_array_t *array)
 * @brief fts_class_array_function_t ...
 * @ingroup class
 */
typedef void (*fts_class_array_function_t)(fts_object_t *obj, fts_array_t *array);
/** 
 * @var typedef void (*fts_class_spost_function_t)(fts_object_t *obj, fts_bytestream_t *stream)
 * @brief fts_class_spost_function_t ...
 * @ingroup class
 */
typedef void (*fts_class_spost_function_t)(fts_object_t *obj, fts_bytestream_t *stream);
/** 
 * @var typedef fts_object_t *(*fts_class_guiobject_function_t)(fts_object_t *obj)
 * @brief fts_class_guiobject_function_t ...
 * @ingroup class
 */
typedef fts_object_t *(*fts_class_guiobject_function_t)(fts_object_t *obj);
/** 
 * @var struct fts_class_outlet fts_class_outlet_t
 * @brief fts_class_outlet_t ...
 * @ingroup class
 */
typedef struct fts_class_outlet fts_class_outlet_t;
/**
 * @fn void fts_class_instantiate(fts_class_t *cl)
 * @brief fts_class init function
 * @param cl the class
 * @ingroup class
 */ 
FTS_API void fts_class_instantiate(fts_class_t *cl);

/* Predefined class ids */
#define FTS_FIRST_OBJECT_TYPEID   16

/**************************************************
 *
 *  class documentation
 *
 */

/**
 * @typedef struct fts_class_doc_line fts_class_doc_t
 * @brief fts class documentation
 * @ingroup class
 */
/**
 * @struct fts_class_doc_line
 * @brief fts class documentation struct
 * @ingroup class
 */
typedef struct fts_class_doc_line
{
  fts_symbol_t name; /**< class doc name */
  const char *args; /**< arguments */
  const char *comment;/**< documentation line */
  struct fts_class_doc_line *next;/** pointer to next doc line */ 
} fts_class_doc_t;

#ifdef AVOID_MACROS
/**
 * @fn fts_symbol_t fts_class_doc_get_name(fts_class_doc_t *l) 
 * @brief returns class doc name as fts_symbol_t
 * @param l class doc
 * @return class doc name as fts_symbol_t
 * @ingroup class
 */
fts_symbol_t fts_class_doc_get_name(fts_class_doc_t *l);
/**
 * @fn char *fts_class_doc_get_args(fts_class_doc_t *l)
 * @brief returns class doc args
 * @param l class doc
 * @return class doc args
 * @ingroup class
 */
char *fts_class_doc_get_args(fts_class_doc_t *l);
/**
 * @fn char *fts_class_doc_get_comment(fts_class_doc_t *l)
 * @brief returns class doc comment
 * @param l class doc
 * @return class doc comment
 * @ingroup class
 */
char *fts_class_doc_get_comment(fts_class_doc_t *l);
/**
 * @fn fts_class_doc_t * fts_class_doc_get_next(fts_class_doc_t *l)
 * @brief returns next class doc
 * @param l class doc
 * @return next class doc
 * @ingroup class
 */
fts_class_doc_t * fts_class_doc_get_next(fts_class_doc_t *l);
#else
#define fts_class_doc_get_name(l) ((l)->name)
#define fts_class_doc_get_args(l) ((l)->args)
#define fts_class_doc_get_comment(l) ((l)->comment)
#define fts_class_doc_get_next(l) ((l)->next)
#endif
/**
 * @fn void fts_class_doc(fts_class_t *cl, fts_symbol_t name, const char *args, const char *comment)
 * @brief set documentation for given class 
 * @param cl the class
 * @param name the class doc name
 * @param args the class doc args
 * @param args the class doc comment
 * @ingroup class
 */
FTS_API void fts_class_doc(fts_class_t *cl, fts_symbol_t name, const char *args, const char *comment);
/**
 * @fn void fts_class_doc_post(fts_class_t *cl)
 * @brief post documentation of given class 
 * @param cl the class
 * @ingroup class
 */
FTS_API void fts_class_doc_post(fts_class_t *cl);
/**
 * @fn int fts_class_doc_get (fts_class_t *cl, fts_array_t *output)
 * @brief appends doc atoms to array 
 * @param cl the class
 * @param output the array
 * @return ....
 * @ingroup class
 */
FTS_API int fts_class_doc_get (fts_class_t *cl, fts_array_t *output);

/**************************************************
 *
 *  the class structure
 *
 *  N.B.: IF THIS STRUCTURE CHANGES, THE MACRO FTS_PRIMITIVE_CLASS IN
 *        atom.c MUST BE ADAPTED!
 */

/**
 * @struct fts_class
 * @brief tha class structure
 * @ingroup class
 */
struct fts_class
{
  fts_object_t head; /** head ...*/
  
  fts_symbol_t name; /**< name of the class, i.e. the first name used to register it */
  int type_id; /**< a type id that separates primitive types from objects: lower values are primitive types */
  fts_class_t *super_class;/**< superclass ... */
  
  /* The hash function and equality function for this class */
  fts_class_hash_function_t hash_function;/**< hash_function ... */
  fts_class_equals_function_t equals_function;/**< equals_function ... */
  fts_class_description_function_t description_function;/**< description_function ... */
  fts_class_copy_function_t copy_function;/**< copy_function ... */
  fts_class_array_function_t array_function;/**< array_function ... */
  fts_class_spost_function_t spost_function;/**< spost_function ... */
  fts_class_guiobject_function_t guiobject_function;/**< guiobject_function ... */
  fts_hashtable_t import_handlers; /**< table of import handlers */
  fts_hashtable_t export_handlers; /**< table of export handlers */
  
  fts_instantiate_fun_t instantiate_fun;/**< instantiate_fun ... */
  
  fts_method_t constructor;/**< constructor ... */
  fts_method_t deconstructor;/**< deconstructor ... */
  
  fts_package_t *package;/**< package ... */
  
  fts_hashtable_t *methods;/**< methods ... */
  
  int ninlets;/**< ninlets ... */
  fts_method_t input_handler;/**< input_handler ... */
  
  int noutlets;/**< noutlets ... */
  int out_alloc;/**< out_alloc ... */
  fts_class_outlet_t *outlets;/**< outlets ... */
  
  int size;/**< size ... */
  fts_heap_t *heap;/**< heap ... */
  
  fts_class_doc_t *doc;/**< doc ... */
};

#ifdef AVOID_MACROS
/**
 * @fn fts_symbol_t fts_class_get_name(fts_class_t *c)
 * @brief get class name
 * @param c the ftmext_class
 * @return class_name as fts_symbol_t
 * @ingroup class
 */
fts_symbol_t fts_class_get_name(fts_class_t *c);
/**
 * @fn fts_class_t *fts_class_get_super(fts_class_t *c)
 * @brief get super class of given class \n --> fts_class_t * fts_class_get_super(fts_class_t *c)
 * @param c the class
 * @return super class
 * @ingroup class
 */
fts_class_t *fts_class_get_super(fts_class_t *c);

/**
 * @fn fts_method_t fts_class_get_constructor(fts_class_t *c)
 * @brief get decobstructor of given class
 * @param c the class
 * @return constructor method
 * @ingroup class
 */
fts_method_t fts_class_get_constructor(fts_class_t *c);
/**
 * @fn fts_method_t fts_class_get_deconstructor(fts_class_t *c)
 * @brief get decobstructor of given class
 * @param c the class
 * @return deconstructor method
 * @ingroup class
 */
fts_method_t fts_class_get_deconstructor(fts_class_t *c);
/**
 * @fn fts_method_t fts_class_get_input_handler(fts_class_t *c) 
 * @brief get input handler of given class
 * @param c the class
 * @return input handler
 * @ingroup class
 */
fts_method_t fts_class_get_input_handler(fts_class_t *c);

/**
 * @fn fts_class_hash_function_t fts_class_get_hash_function(fts_class_t *cl) 
 * @brief get hash function of given class
 * @param c the class
 * @return hash function
 * @ingroup class
 */
fts_class_hash_function_t fts_class_get_hash_function(fts_class_t *cl);
/**
 * @fn fts_class_equals_function_t fts_class_get_equals_function(fts_class_t *cl) 
 * @brief get equals function of given class
 * @param c the class
 * @return equals function
 * @ingroup class
 */
fts_class_equals_function_t fts_class_get_equals_function(fts_class_t *cl);
/**
 * @fn fts_class_description_function_t fts_class_get_description_function(fts_class_t *cl)
 * @brief get description function of given class
 * @param c the class
 * @return description function
 * @ingroup class
 */
fts_class_description_function_t fts_class_get_description_function(fts_class_t *cl);
/**
 * @fn fts_class_copy_function_t fts_class_get_copy_function(fts_class_t *cl)
 * @brief get copy function of given class
 * @param c the class
 * @return copy function
 * @ingroup class
 */
fts_class_copy_function_t fts_class_get_copy_function(fts_class_t *cl);
/**
 * @fn fts_class_array_function_t fts_class_get_array_function(fts_class_t *cl) 
 * @brief get array function of given class
 * @param c the class
 * @return array function
 * @ingroup class
 */
fts_class_array_function_t fts_class_get_array_function(fts_class_t *cl);
/**
 * @fn fts_class_spost_function_t fts_class_get_spost_function(fts_class_t *cl)
 * @brief get spost function of given class
 * @param c the class
 * @return spost function
 * @ingroup class
 */
fts_class_spost_function_t fts_class_get_spost_function(fts_class_t *cl);
/**
 * @fn fts_class_guiobject_function_t fts_class_get_guiobject_function(fts_class_t *cl)
 * @brief get guiobject function of given class
 * @param c the class
 * @return guiobject function
 * @ingroup class
 */
fts_class_guiobject_function_t fts_class_get_guiobject_function(fts_class_t *cl)

/**
 * @fn fts_hashtable_t fts_class_get_import_handlers(fts_class_t *cl)
 * @brief get import function of given class
 * @param c the class
 * @return hashtable of import handlers
 * @ingroup class
 */
fts_hashtable_t fts_class_get_import_handlers(fts_class_t *cl)
/**
 * @fn fts_hashtable_t fts_class_get_export_handlers(fts_class_t *cl)
 * @brief get export function of given class
 * @param c the class
 * @return hashtable of export handlers
 * @ingroup class
 */
fts_hashtable_t fts_class_get_export_handlers(fts_class_t *cl);

/**
 * @fn void fts_class_set_super(fts_class_t *cl, fts_class_t *s)
 * @brief set super class for given class
 * @param c the class
 * @param c the super class
 * @ingroup class
 */
void fts_class_set_super(fts_class_t *cl, fts_class_t *s);

/**
 * @fn void fts_class_set_hash_function(fts_class_t *cl, fts_class_hash_function_t *f)
 * @brief set hash function to given class
 * @param c the class
 * @param f hash function
 * @ingroup class
 */
void fts_class_set_hash_function(fts_class_t *cl, fts_class_hash_function_t *f);
/**
 * @fn void fts_class_set_equals_function(fts_class_t *cl, fts_class_equals_function_t *f)
 * @brief set equals function to given class
 * @param c the class
 * @param f equals function
 * @ingroup class
 */
void fts_class_set_equals_function(fts_class_t *cl, fts_class_equals_function_t *f);
/**
 * @fn void fts_class_set_description_function(fts_class_t *cl, fts_class_description_function_t *f)
 * @brief set description function to given class
 * @param c the class
 * @param f description function
 * @ingroup class
 */
void fts_class_set_description_function(fts_class_t *cl, fts_class_description_function_t *f);
/**
 * @fn void fts_class_set_copy_function(fts_class_t *cl, fts_class_copy_function_t *f)
 * @brief set copy function to given class
 * @param c the class
 * @param f copy function
 * @ingroup class
 */
void fts_class_set_copy_function(fts_class_t *cl, fts_class_copy_function_t *f);
/**
 * @fn void fts_class_set_array_function(fts_class_t *cl, fts_class_array_function_t *f)
 * @brief set array function to given class
 * @param c the class
 * @param f copy function
 * @ingroup class
 */
void fts_class_set_array_function(fts_class_t *cl, fts_class_array_function_t *f);
/**
 * @fn void fts_class_set_spost_function(fts_class_t *cl, fts_class_spost_function_t *f)
 * @brief set spost function to given class
 * @param c the class
 * @param f spost function
 * @ingroup class
 */
void fts_class_set_spost_function(fts_class_t *cl, fts_class_spost_function_t *f);
/**
 * @fn void fts_class_set_guiobject_function(fts_class_t *cl, fts_class_guiobject_function_t *f)
 * @brief set guiobject function to given class
 * @param c the class
 * @param f guiobject function
 * @ingroup class
 */
void fts_class_set_guiobject_function(fts_class_t *cl, fts_class_guiobject_function_t *f);

/**
 * @fn int fts_class_is_primitive(fts_class_t *cl)
 * @brief tells if given class is a primitive class
 * @param c the class
 * @return 1 if yes, 0 if not
 * @ingroup class
 */
int fts_class_is_primitive(fts_class_t *cl);

/**
 * @fn fts_class_doc_t *fts_class_get_doc(fts_class_t *cl)
 * @brief get class documentation
 * @param c the class
 * @return class documentation
 * @ingroup class
 */
fts_class_doc_t *fts_class_get_doc(fts_class_t *cl);
#else
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
#define fts_class_set_array_function( cl, f) ((cl)->array_function = (f)
#define fts_class_set_spost_function( cl, f) ((cl)->spost_function = (f))
#define fts_class_set_guiobject_function(cl, f) ((cl)->guiobject_function = (f))
#define fts_class_is_primitive(CL) ((CL)->type_id < FTS_FIRST_OBJECT_TYPEID)
#define fts_class_get_doc(C) ((C)->doc)
#endif

/**
 * @fn void fts_class_get_messages(const fts_class_t *cl, fts_iterator_t *i)
 * @brief get messages of given class as iterator
 * @param cl the class
 * @param i iterator
 * @ingroup class
 */
FTS_API void fts_class_get_messages(const fts_class_t *cl, fts_iterator_t *i);

/**
 * Get a method of a class by its message symbol and argument type
 *
 * @fn void fts_class_get_method(fts_class_t *cl, fts_symbol_t s, fts_class_t *type)
 * @brief get class method by selector and arg type 
 * @param cl the class
 * @param s message symbol
 * @param type class of argument
 * @return the method or NULL if failed
 * @ingroup class
 */
FTS_API fts_method_t fts_class_get_method(fts_class_t *cl, fts_symbol_t s, fts_class_t *type);

/**
 * Get the a non-varargs method of a class by its message symbol
 *
 * @fn void fts_class_get_method_novarargs(fts_class_t *cl, fts_symbol_t s, fts_class_t *type)
 * @brief get non-varargs method by selector
 * @param cl the class
 * @param s message symbol
 * @param type class of argument
 * @return the method or NULL if failed
 * @ingroup class
 */
FTS_API fts_method_t fts_class_get_method_novarargs(fts_class_t *cl, fts_symbol_t s, fts_class_t *type);

/**
 * Get the varargs method of a class by its message symbol
 *
 * @fn void fts_class_get_method_varargs(fts_class_t *cl, fts_symbol_t s, fts_class_t *type)
 * @brief get varargs method by selector
 * @param cl the class
 * @param s message symbol
 * @return the method or NULL if failed
 * @ingroup class
 */
FTS_API fts_method_t fts_class_get_method_varargs(fts_class_t *cl, fts_symbol_t s);

/** 
 * @var const int fts_system_inlet
 * @brief system inlet num ... 
 * @ingroup class
 */
FTS_API const int fts_system_inlet;

/** 
 * @var fts_status_description_t fts_ClassAlreadyInitialized
 * @brief status return values
 * @ingroup class
 */
FTS_API fts_status_description_t fts_ClassAlreadyInitialized;

/**
 * Install a class
 * Create a class (without initializing) and register it by name in the current package.
 *
 * @fn fts_class_t *fts_class_install(fts_symbol_t name, fts_instantiate_fun_t instantiate_fun)
 * @brief install a class
 * @param name the name (NULL for unregistered classes)
 * @param instantiate_fun class initialization function
 * @return the class (handle)
 * @ingroup class
 */
FTS_API fts_class_t *fts_class_install( fts_symbol_t name, fts_instantiate_fun_t instantiate_fun);

/**
 * Create an alias for a class
 *
 * @fn void fts_class_alias(fts_class_t *cl, fts_symbol_t alias)
 * @brief create alias for given class
 * @param cl the class
 * @param alias alias name
 * @ingroup class
 */
FTS_API void fts_class_alias(fts_class_t *cl, fts_symbol_t alias);

/**
 * Initialize a class
 *
 * @fn void fts_class_init(fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor)
 * @breif class init function
 * @param cl the class
 * @param size the size of its associated C structure (must have fts_object_t as first entry)
 * @param constructor constructor method
 * @param deconstructor deconstructor method
 * @ingroup class
 */
FTS_API void fts_class_init(fts_class_t *cl, unsigned int size, fts_method_t constructor, fts_method_t deconstructor);

/**
 * If set the input handler is called for any input of an object comming into an inlet.
 *
 * @fn void fts_class_input_handler(fts_class_t *cl, fts_method_t method)
 * @brief set input handler
 * @param cl the class
 * @param method input handler method
 * @ingroup class
 */
FTS_API void fts_class_input_handler(fts_class_t *cl, fts_method_t method);

/**
 * Add import handler to table of handlers to try.
 * An import handler is called with the object to import into, and the list of filename and arguments.
 *
 * @fn void fts_class_import_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t method)
 * @brief add import handler
 * @param cl the class
 * @param suffix suffix
 * @param method import handler method
 * @ingroup class
 */
FTS_API void fts_class_import_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t meth);

#ifdef AVOID_MACROS
/**
 * @fn void fts_class_import_handler_default(fts_class_t *c, fts_method_t m) 
 * @brief add default import handler
 * @param c the class
 * @param m import handler method
 * @ingroup class
 */
void fts_class_import_handler_default(fts_class_t *c, fts_method_t m);
#else
#define fts_class_import_handler_default(c, m) fts_class_import_handler(c, fts_s_default, m)
#endif
                                              
/**
 * Add export handler to table of handlers to try.
 * An export handler is called with the object to import from, and the list of filename and arguments.
 *
 * @fn void fts_class_export_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t method)
 * @brief add export handler
 * @param cl the class
 * @param suffix suffix
 * @param method export handler method
 * @ingroup class
 */
FTS_API void fts_class_export_handler(fts_class_t *cl, fts_symbol_t suffix, fts_method_t meth);

#ifdef AVOID_MACROS
/**
 * @fn void fts_class_export_handler_default(fts_class_t *c, fts_method_t m) 
 * @brief add default export handler
 * @param c the class
 * @param m export handler method
 * @ingroup class
 */
void fts_class_export_handler_default(fts_class_t *c, fts_method_t m);
#else
#define fts_class_export_handler_default(c, m) fts_class_export_handler(c, fts_s_default, m)                                              
#endif
                                              
/**
 * Register a method for a given message and argument type (of a single argument or void).
 *
 * @fn void fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t mth)
 * @brief register method for given message and argument type (single argument or void).
 * @param cl the class
 * @param s message symbol
 * @param type class of argument (NULL for a single argument of any type, fts_void_class for a void method)
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_message(fts_class_t *cl, fts_symbol_t s, fts_class_t *type, fts_method_t mth);

/* marcros for most popular message types */

#ifdef AVOID_MACROS
/**
 * @fn void fts_class_message_void(fts_class_t *c, fts_symbol_t s, fts_method_t m)
 * @brief register void message
 * @param c the class
 * @param s message name
 * @param m method
 * @ingroup class
 */
FTS_API void fts_class_message_void(fts_class_t *c, fts_symbol_t s, fts_method_t m);
/**
 * @fn void fts_class_message_int(fts_class_t *c, fts_symbol_t s, fts_method_t m)
 * @brief register int message
 * @param c the class
 * @param s message name
 * @param m method
 * @ingroup class
 */
FTS_API void fts_class_message_int(fts_class_t *c, fts_symbol_t s, fts_method_t m);
/**
 * @fn void fts_class_message_float(fts_class_t *c, fts_symbol_t s, fts_method_t m)
 * @brief register float message
 * @param c the class
 * @param s message name
 * @param m method
 * @ingroup class
 */
FTS_API void fts_class_message_float(fts_class_t *c, fts_symbol_t s, fts_method_t m);
/**
 * @fn void fts_class_message_number(fts_class_t *c, fts_symbol_t s, fts_method_t m)
 * @brief register number message
 * @param c the class
 * @param s message name
 * @param m method
 * @ingroup class
 */
FTS_API void fts_class_message_number(fts_class_t *c, fts_symbol_t s, fts_method_t m);
/**
 * @fn void fts_class_message_symbol(fts_class_t *c, fts_symbol_t s, fts_method_t m) 
 * @brief register symbol message
 * @param c the class
 * @param s message name
 * @param m method
 * @ingroup class
 */
FTS_API void fts_class_message_symbol(fts_class_t *c, fts_symbol_t s, fts_method_t m);
/**
 * @def void fts_class_message_varargs(fts_class_t *c, fts_symbol_t s, fts_method_t m) 
 * @brief register varargs message 
 * @param c the class
 * @param s message name
 * @param m method
 * @ingroup class
 */
FTS_API void fts_class_message_varargs(fts_class_t *c, fts_symbol_t s, fts_method_t m);
#else
#define fts_class_message_void(c, s, m) fts_class_message((c), (s), fts_void_class, (m))
#define fts_class_message_int(c, s, m) fts_class_message((c), (s), fts_int_class, (m))
#define fts_class_message_float(c, s,  m) fts_class_message((c), (s), fts_float_class, (m))
#define fts_class_message_number(c, s,  m) do{ \
  fts_class_message((c), (s), fts_int_class, (m)); \
    fts_class_message((c), (s), fts_float_class, (m));} while(0)
#define fts_class_message_symbol(c, s,  m) fts_class_message((c), (s), fts_symbol_class, (m))
#define fts_class_message_varargs(c, s,  m) fts_class_message((c), (s), NULL, (m))
#endif

/* deprecated */
#define fts_class_message_atom(c, s,  m) fts_class_message((c), (s), NULL, (m))

/**
 * Declare an inlet and register a method for a given argument type (of a single argument or void).
 *
 * @fn void fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t mth)
 * @brief declare inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param type class of argument (NULL for a single argument of any type, fts_void_class for a void method)
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet(fts_class_t *cl, int winlet, fts_class_t *type, fts_method_t mth);

/* marcros for most popular inlet types */
#ifdef AVOID_MACROS
/**
 * @fn void fts_class_inlet_bang(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare bang inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_bang(fts_class_t *cl, int winlet, fts_method_t mth);
/**
 * @fn void fts_class_inlet_void(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare void inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_void(fts_class_t *cl, int winlet, fts_method_t mth);
/**
 * @fn void fts_class_inlet_int(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare int inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_int(fts_class_t *cl, int winlet, fts_method_t mth);
/**
 * @fn void fts_class_inlet_float(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare float inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_float(fts_class_t *cl, int winlet, fts_method_t mth);
/**
 * @fn void fts_class_inlet_number(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare number inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_number(fts_class_t *cl, int winlet, fts_method_t mth);
/**
 * @fn void fts_class_inlet_symbol(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare symbol inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_symbol(fts_class_t *cl, int winlet, fts_method_t mth);
/**
 * @fn void fts_class_inlet_varargs(fts_class_t *cl, int winlet, fts_method_t mth)
 * @brief declare varargs inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @param mth the method
 * @ingroup class
 */
FTS_API void fts_class_inlet_varargs(fts_class_t *cl, int winlet, fts_method_t mth);
#else
#define fts_class_inlet_bang(c, i, m) fts_class_inlet((c), (i), fts_void_class, (m))
#define fts_class_inlet_void(c, i, m) fts_class_inlet((c), (i), fts_void_class, (m))
#define fts_class_inlet_int(c, i, m) fts_class_inlet((c), (i), fts_int_class, (m))
#define fts_class_inlet_float(c, i, m) fts_class_inlet((c), (i), fts_float_class, (m))
#define fts_class_inlet_number(c, i, m) do{ \
  fts_class_inlet((c), (i), fts_int_class, (m)); \
    fts_class_inlet((c), (i), fts_float_class, (m));} while(0)
#define fts_class_inlet_symbol(c, i, m) fts_class_inlet((c), (i), fts_symbol_class, (m))
#define fts_class_inlet_varargs(c, i, m) fts_class_inlet((c), (i), NULL, (m))
#endif

/* deprecated */
#define fts_class_inlet_atom(c, i, m) fts_class_inlet((c), (i), NULL, (m))

/**
 * Declare an empty inlet.
 *
 * @fn void fts_class_inlet_thru(fts_class_t *cl, int winlet)
 * @brief declare an empty inlet
 * @param cl the class
 * @param winlet index of the inlet
 * @ingroup class
 */
FTS_API void fts_class_inlet_thru(fts_class_t *cl, int winlet);

/**
 * Declare an outlet of the given type.
 *
 * @fn void fts_class_outlet(fts_class_t *cl, int woutlet, fts_class_t *type)
 * @brief declare outlet of given type
 * @param cl the class
 * @param woutlet index of the outlet
 * @param type of the outlet (NULL for an untyped outlet)
 * @ingroup class
 */
FTS_API void fts_class_outlet(fts_class_t *cl, int woutlet, fts_class_t *type);

/* marcros for most popular outlet types */
#ifdef AVOID_MACROS
/**
 * @fn void fts_class_outlet_bang(fts_class_t *cl, int woutlet)
 * @brief declare bang outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_bang(fts_class_t *cl, int woutlet);
/**
 * @fn void fts_class_outlet_void(fts_class_t *cl, int woutlet)
 * @brief declare void outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_void(fts_class_t *cl, int woutlet);
/**
 * @fn void fts_class_outlet_int(fts_class_t *cl, int woutlet)
 * @brief declare int outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_int(fts_class_t *cl, int woutlet);
/**
 * @fn void fts_class_outlet_float(fts_class_t *cl, int woutlet)
 * @brief declare float outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_float(fts_class_t *cl, int woutlet);
/**
 * @fn void fts_class_outlet_number(fts_class_t *cl, int woutlet)
 * @brief declare number outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_number(fts_class_t *cl, int woutlet);
/**
 * @fn void fts_class_outlet_symbol(fts_class_t *cl, int woutlet)
 * @brief declare symbol outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_symbol(fts_class_t *cl, int woutlet);
/**
* @fn void fts_class_outlet_varargs(fts_class_t *cl, int woutlet)
 * @brief declare varargs outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_varargs(fts_class_t *cl, int woutlet);
/**
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
 * @fn void fts_class_outlet_thru(fts_class_t *cl, int woutlet)
 * @brief declare thru outlet
 * @param cl the class
 * @param woutlet index of the outlet
 * @ingroup class
 */
FTS_API void fts_class_outlet_thru(fts_class_t *cl, int woutlet);
#else
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
#endif

/* deprecated */
#define fts_class_outlet_atom(c, i) fts_class_outlet((c), (i), NULL)

/**
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
 * @var void (*fts_propagate_fun_t)(void *ptr, fts_object_t *object, int outlet)
 * @brief fts_propagate_fun_t ...
 * @param ptr ...
 * @param object ...
 * @param outlet ...
 * @ingroup class
 */
typedef void (*fts_propagate_fun_t)(void *ptr, fts_object_t *object, int outlet);
