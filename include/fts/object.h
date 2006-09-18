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

#ifndef _FTS_OBJECT_H_
#define _FTS_OBJECT_H_

#define FTS_NO_ID -1

#define FTS_OBJECT_BITS_STATUS 2
#define FTS_OBJECT_BITS_CLIENT 4
#define FTS_OBJECT_BITS_ID (32 - FTS_OBJECT_BITS_STATUS - FTS_OBJECT_BITS_CLIENT)

typedef void (*fts_object_listener_callback_t)(fts_object_t *o, void *l, fts_symbol_t s, int ac, const fts_atom_t *at);

typedef struct
{
  fts_object_t *container;
} fts_context_t;

typedef struct fts_object_listener
{
  void *listener;
  fts_object_listener_callback_t callback;
  struct fts_object_listener *next;
} fts_object_listener_t;

struct fts_object {
  fts_class_t *cl;
  
  struct { 
    unsigned int status:FTS_OBJECT_BITS_STATUS;
    int client_id:FTS_OBJECT_BITS_CLIENT;
    int id:FTS_OBJECT_BITS_ID;
  } flag; 
  
  int refcnt; /* reference counter */

  fts_context_t *context; /* (back) pointer to container (or container related data structure) */
  fts_object_listener_t *listeners;
};

/**
 * Create an instance of a class.
 *
 * A new instance of the class is created and initialized.
 * If parent is not NULL, the created instance will be added as child to the parent object.
 *
 * @fn fts_object_t *fts_object_create( fts_class_t *cl, int ac, const fts_atom_t *at)
 * @param cl the class to instantiate
 * @param ac argument count
 * @param at the arguments
 * @return the created object, NULL if instantiation failed
 */
FTS_API fts_object_t *fts_object_create(fts_class_t *cl, int ac, const fts_atom_t *at);
FTS_API void fts_object_destroy(fts_object_t *obj);

/* garbage collector handling */
#ifdef DEBUG_REF_COUNT
FTS_API void fts_object_refer(fts_object_t *obj);
#else
#define fts_object_refer(o) (((fts_object_t *)(o))->refcnt++)
#endif

#define fts_object_release(o) ((--(((fts_object_t *)(o))->refcnt) > 0)? 0: (fts_object_destroy((fts_object_t *)(o)), 0))
#define fts_object_has_only_one_reference(o) (((fts_object_t *)(o))->refcnt == 1)

/* set name and persistence by container */
FTS_API void fts_object_set_name(fts_object_t *obj, fts_symbol_t name);
FTS_API void fts_object_set_persistence(fts_object_t *obj, int persistence);
FTS_API void fts_object_set_dirty(fts_object_t *obj);
FTS_API void fts_object_set_state_dirty(fts_object_t *obj);

/* standard name and persistence methods */
FTS_API fts_method_status_t fts_object_name(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
FTS_API fts_method_status_t fts_object_persistence(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

/* client */
#define fts_object_get_id(o) ((o)->flag.id)
#define fts_object_get_client_id(o) ((o)->flag.client_id)
#define fts_object_has_client(o) (fts_object_get_client_id(o) > FTS_NO_ID)
FTS_API void fts_object_upload(fts_object_t *obj);

/* class */
#define fts_object_get_class(o) ((o)->cl)
#define fts_object_get_class_name(o) (fts_class_get_name((o)->cl))
#define fts_object_is_a(o, c) ((o)->cl == (c))

/* context & container*/
#define fts_object_get_context(o) ((o)->context)
#define fts_object_set_context(o, c) ((o)->context = c)
#define fts_object_get_container(o) (((o)->context != NULL)? ((o)->context->container): NULL)

/* object listeners */
FTS_API void fts_object_add_listener(fts_object_t *o, void *listener, fts_object_listener_callback_t callback);
FTS_API void fts_object_remove_listener(fts_object_t *o, void *listener);
FTS_API void fts_object_call_listeners(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at);

#define fts_object_changed(o) fts_object_call_listeners((o), fts_s_state, 0, NULL)

/** try import handlers from class with the given arguments until one returns true */
FTS_API fts_method_status_t fts_object_import(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
FTS_API fts_method_status_t fts_object_import_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

/** try export handlers from class with the given arguments until one returns true */
FTS_API fts_method_status_t fts_object_export(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);
FTS_API fts_method_status_t fts_object_export_as(fts_object_t *o, fts_symbol_t s, int ac, const fts_atom_t *at, fts_atom_t *ret);

#endif  /* _FTS_OBJECT_H_ */
