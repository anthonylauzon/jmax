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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 */

/**
 * Object
 *
 * The FTS object
 *
 * @defgroup object FTS object
 * @ingroup fts_system
 */

#ifndef _FTS_OBJECT_H_
#define _FTS_OBJECT_H_

/** 
 * @def FTS_NO_ID -1
 * @brief object id value for not defined id. 
 * @ingroup object
 */
#define FTS_NO_ID -1

#define FTS_OBJECT_BITS_STATUS 2
#define FTS_OBJECT_BITS_CLIENT 4
#define FTS_OBJECT_BITS_ID (32 - FTS_OBJECT_BITS_STATUS - FTS_OBJECT_BITS_CLIENT)

/**
 * @typedef void (*fts_object_listener_callback_t)(fts_object_t *o, void *l, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @brief fts object listener callback
 * @param o the object
 * @param l the listener
 * @param s selector
 * @param ac args count
 * @param at arguments
 * @ingroup object
 */
typedef void (*fts_object_listener_callback_t)(fts_object_t *o, void *l, fts_symbol_t s, int ac, const fts_atom_t *at);

/**
 * @typedef struct fts_context fts_context_t
 * @brief fts object context
 * @ingroup object
 */
/**
 * @struct fts_context
 * @brief fts object context struct
 * @ingroup object
 */
typedef struct fts_context
{
  fts_object_t *container;/**< container ... */
} fts_context_t;

/**
 * @typedef struct fts_object_listener fts_object_listener_t
 * @brief fts object listener
 * @ingroup object
 */
/**
 * @struct fts_object_listener
 * @brief fts object listener struct
 * @ingroup object
 */
typedef struct fts_object_listener
{
  void *listener; /**< listener ... */
  fts_object_listener_callback_t callback; /**< object listener callback ... */
  struct fts_object_listener *next; /**< next listener ... */
} fts_object_listener_t;

/**
 * @struct fts_object
 * @brief fts object struct
 * @ingroup object
 */

struct fts_object {
  fts_class_t *cl;/**< class ... */
  
  struct { 
    unsigned int status:FTS_OBJECT_BITS_STATUS;/**<  status ... */
    int client_id:FTS_OBJECT_BITS_CLIENT;/**< client id ... */
    int id:FTS_OBJECT_BITS_ID; /**< object id ... */
  } flag; /**< flag ... */
  
  int refcnt; /**< reference counter */

  fts_context_t *context; /**< (back) pointer to container (or container related data structure) */
  fts_object_listener_t *listeners;/**< object listener ...*/
};

/**
 * @fn fts_object_t *fts_object_create( fts_class_t *cl, int ac, const fts_atom_t *at)
 * @brief  create an instance of a class
 * @param cl the class to instantiate
 * @param ac argument count
 * @param at the arguments
 * @return the created object, NULL if instantiation failed
 * @ingroup object 
 *
 * A new instance of the class is created and initialized.
 * If parent is not NULL, the created instance will be added as child to the parent object.
 */
FTS_API fts_object_t *fts_object_create(fts_class_t *cl, int ac, const fts_atom_t *at);
/**
 * @fn void fts_object_destroy(fts_object_t *obj)
 * @brief object destroy function
 * @param obj the object
 * @ingroup object 
 */
FTS_API void fts_object_destroy(fts_object_t *obj);

/* garbage collector handling */
#if defined(DEBUG_REF_COUNT)  ||  defined(AVOID_MACROS) 
/**
 * @fn void fts_object_refer(fts_object_t *obj)
 * @brief increment reference count
 * @param obj the object
 * @ingroup object 
 */
FTS_API void fts_object_refer(fts_object_t *obj);
#else
#define fts_object_refer(o) (((fts_object_t *)(o))->refcnt++)
#endif

#ifdef AVOID_MACROS
/**
 * @fn void fts_object_release(fts_object_t *obj)
 * @brief decrement reference count
 * @param obj the object
 * @ingroup object 
 *
 * if reference count become negative the object is destroyed ...
 */
FTS_API void fts_object_release(fts_object_t *o);
/**
 * @fn int fts_object_has_only_one_reference(fts_object_t *o)
 * @brief tell if object's reference count == 1 
 * @param obj the object
 * @ingroup object 
 */
FTS_API int fts_object_has_only_one_reference(fts_object_t *o);
#else
#define fts_object_release(o) ((--(((fts_object_t *)(o))->refcnt) > 0)? 0: (fts_object_destroy((fts_object_t *)(o)), 0))
#define fts_object_has_only_one_reference(o) (((fts_object_t *)(o))->refcnt == 1)
#endif

/* set name and persistence by container */
/**
 * @fn void fts_object_set_name(fts_object_t *obj, fts_symbol_t name)
 * @brief set object name
 * @param obj the object
 * @param name the name
 * @ingroup object 
 */
FTS_API void fts_object_set_name(fts_object_t *obj, fts_symbol_t name);
/**
 * @fn void fts_object_set_persistence(fts_object_t *obj, int persistence)
 * @brief set object name
 * @param obj the object
 * @param int persistence
 * @ingroup object 
 */
FTS_API void fts_object_set_persistence(fts_object_t *obj, int persistence);
/**
 * @fn void  fts_object_set_dirty(fts_object_t *obj)
 * @brief set object's root patcher as dirty (to be saved) 
 * @param obj the object
 * @ingroup object 
 */
FTS_API void fts_object_set_dirty(fts_object_t *obj);
/**
 * @fn void  fts_object_set_state_dirty(fts_object_t *obj)
 * @brief set object's root patcher as dirty (to be saved) according to obj persistence
 * @param obj the object
 * @ingroup object 
 */
FTS_API void fts_object_set_state_dirty(fts_object_t *obj);

/* standard name and persistence methods */
/**
 * @fn  fts_method_status_t fts_object_name(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief standard name method
 * @param o the object
 * @param s selector
 * @param ac arguments count
 * @param at arguments
 * @param ret return value atom
 * @return status
 * @ingroup object 
 */
FTS_API fts_method_status_t fts_object_name(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
/**
 * @fn  fts_method_status_t fts_object_name(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief standard persistence method
 * @param o the object
 * @param s selector
 * @param ac arguments count
 * @param at arguments
 * @param ret return value atom
 * @return status
 * @ingroup object 
 */
FTS_API fts_method_status_t fts_object_persistence(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

#ifdef AVOID_MACROS
/**
 * @fn int fts_object_get_id(fts_object_t *o)
 * @brief get object id
 * @param obj the object
 * @return the id
 * @ingroup object 
 */
FTS_API int fts_object_get_id(fts_object_t *o);
/**
 * @fn int fts_object_get_client_id(fts_object_t *o)
 * @brief get client id fro given object
 * @param obj the object
 * @return the client id
 * @ingroup object 
 */
FTS_API int fts_object_get_client_id(fts_object_t *o);
/**
 * @fn int fts_object_has_client(fts_object_t *o)
 * @brief tell if object has a client id
 * @param obj the object
 * @return 1 if true, 0 if false
 * @ingroup object 
 */
FTS_API int fts_object_has_client(fts_object_t *o);
#else
/* client */
#define fts_object_get_id(o) ((o)->flag.id)
#define fts_object_get_client_id(o) ((o)->flag.client_id)
#define fts_object_has_client(o) (fts_object_get_client_id(o) > FTS_NO_ID)
#endif

/**
 * @fn void fts_object_upload(fts_object_t *obj)
 * @brief upload object content to client
 * @param obj the object
 * @ingroup object 
 */
FTS_API void fts_object_upload(fts_object_t *obj);

#ifdef AVOID_MACROS
/**
 * @fn fts_class_t *fts_object_get_class(fts_object_t *o)
 * @brief get object class
 * @param obj the object
 * @return object class
 * @ingroup object 
 */
FTS_API fts_class_t *fts_object_get_class(fts_object_t *o);
/**
 * @fn fts_symbol_t fts_object_get_class_name(fts_object_t *o)
 * @brief get object class name
 * @param obj the object
 * @return object class name
 * @ingroup object 
 */
FTS_API fts_symbol_t fts_object_get_class_name(fts_object_t *o);
/**
 * @fn int fts_object_is_a(fts_object_t *o, fts_class_t *c)
 * @brief tell if object is an instance of given class
 * @param obj the object
 * @param c the class
 * @return 1 if true, 0 if false
 * @ingroup object 
 */
FTS_API int fts_object_is_a(fts_object_t *o, fts_class_t *c);

/* context & container*/
/**
 * @fn fts_context_t *fts_object_get_context(fts_object_t *o)
 * @brief get object context
 * @param obj the object
 * @return object context
 * @ingroup object 
 */
FTS_API fts_context_t *fts_object_get_context(fts_object_t *o);
/**
 * @fn void fts_object_set_context(fts_object_t *o, fts_context_t *c)
 * @brief set object context
 * @param obj the object
 * @param c the context
 * @ingroup object 
 */
FTS_API void fts_object_set_context(fts_object_t *o, fts_context_t *c);
/**
 * @fn fts_object_t *fts_object_get_container(fts_object_t *o)
 * @brief get object container
 * @param obj the object
 * @return object container
 * @ingroup object 
 */
FTS_API fts_object_t *fts_object_get_container(fts_object_t *o);
#else
/* class */
#define fts_object_get_class(o) ((o)->cl)
#define fts_object_get_class_name(o) (fts_class_get_name((o)->cl))
#define fts_object_is_a(o, c) ((o)->cl == (c))

/* context & container*/
#define fts_object_get_context(o) ((o)->context)
#define fts_object_set_context(o, c) ((o)->context = c)
#define fts_object_get_container(o) (((o)->context != NULL)? ((o)->context->container): NULL)
#endif

/* object listeners */
/**
 * @fn void fts_object_add_listener(fts_object_t *o, void *listener, fts_object_listener_callback_t callback)
 * @brief add listener to given object
 * @param o the object
 * @param listener the object listener
 * @param callback the fts_object_listener_callaback
 * @ingroup object 
 */
FTS_API void fts_object_add_listener(fts_object_t *o, void *listener, fts_object_listener_callback_t callback);
/**
 * @fn void fts_object_remove_listener(fts_object_t *o, void *listener)
 * @brief remove given listener from object
 * @param obj the object
 * @param listener the listener
 * @ingroup object 
 */
FTS_API void fts_object_remove_listener(fts_object_t *o, void *listener);
/**
 * @fn void fts_object_call_listeners(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at)
 * @brief notify all object listeners of given object
 * @param obj the object
 * @param s selector
 * @param ac arguments count
 * @param at arguments
 * @ingroup object 
 */
FTS_API void fts_object_call_listeners(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at);

#ifdef AVOID_MACROS
/**
 * @fn void fts_object_changed(fts_object_t *o)
 * @brief notify listeners that object is changed
 * @param obj the object
 * @ingroup object 
 */
FTS_API void fts_object_changed(fts_object_t *o);
#else
#define fts_object_changed(o) fts_object_call_listeners((o), fts_s_state, 0, NULL)
#endif

/** try import handlers from class with the given arguments until one returns true */
/**
 * @fn fts_method_status_t fts_object_import(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief import method
 * @param o the object
 * @param s selector
 * @param ac args count
 * @param at args
 * @param ret return atom
 * @ingroup object 
 * @return status
 *
 * try import handlers from class with the given arguments until one returns true
 */
FTS_API fts_method_status_t fts_object_import(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
/**
 * @fn fts_method_status_t fts_object_import_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief import as method
 * @param o the object
 * @param s selector
 * @param ac args count
 * @param at args
 * @param ret return atom
 * @ingroup object 
 * @return status
 *
 * try import handlers from class with the given arguments until one returns true
 */
FTS_API fts_method_status_t fts_object_import_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

/** try export handlers from class with the given arguments until one returns true */
/**
 * @fn fts_method_status_t fts_object_export(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief import method
 * @param o the object
 * @param s selector
 * @param ac args count
 * @param at args
 * @param ret return atom
 * @ingroup object 
 * @return status
 *
 * try export handlers from class with the given arguments until one returns true
 */
FTS_API fts_method_status_t fts_object_export(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
/**
 * @fn fts_method_status_t fts_object_export_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret)
 * @brief export as method
 * @param o the object
 * @param s selector
 * @param ac args count
 * @param at args
 * @param ret return atom
 * @ingroup object 
 * @return status
 *
 * try export handlers from class with the given arguments until one returns true
 */
FTS_API fts_method_status_t fts_object_export_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

#endif  /* _FTS_OBJECT_H_ */
