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
#define FTS_OBJECT_BITS_CLIENT 8
#define FTS_OBJECT_BITS_ID (32 - FTS_OBJECT_BITS_STATUS-FTS_OBJECT_BITS_CLIENT)

struct fts_object {
  fts_class_t *cl;
  
  struct { 
    unsigned int status:FTS_OBJECT_BITS_STATUS;

    /* IDs for the client communication */
    int client_id:FTS_OBJECT_BITS_CLIENT;
    int id:FTS_OBJECT_BITS_ID;
  } flag; 
  
  /* reference counter */
  int refcnt;

  /* message cache */
  fts_symbol_t cache_selector;
  fts_class_t *cache_type;
  int cache_varargs;
  fts_method_t cache_method;

  /* name definition */
  fts_definition_t *definition;
  
  /* patcher data */
  fts_object_patcher_data_t *patcher_data;
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
#define fts_object_refer(o) (((fts_object_t *)(o))->refcnt++)
#define fts_object_release(o) ((--(((fts_object_t *)(o))->refcnt) > 0)? 0: (fts_object_destroy((fts_object_t *)(o)), 0))
#define fts_object_has_only_one_reference(o) (((fts_object_t *)(o))->refcnt == 1)

/* client id */
#define fts_object_get_id(o) ((o)->flag.id)
#define fts_object_has_id(o) (fts_object_get_id(o) > FTS_NO_ID)
#define fts_object_get_client_id(o) ((o)->flag.client_id)

/* class */
#define fts_object_get_class(o) ((o)->cl)
#define fts_object_get_class_name(o) (fts_class_get_name((o)->cl))

/* object name */
FTS_API void fts_object_set_name(fts_object_t *obj, fts_symbol_t sym);
FTS_API fts_symbol_t fts_object_get_name(fts_object_t *obj);
FTS_API void fts_object_update_name(fts_object_t *obj);
extern void fts_object_remove_name(fts_object_t *obj);

/* message cache */
#define fts_object_message_cache_get_selector(o) ((o)->cache_selector)
#define fts_object_message_cache_get_type(o) ((o)->cache_type)
#define fts_object_message_cache_get_varargs(o) ((o)->cache_varargs)
#define fts_object_message_cache_get_method(o) ((o)->cache_method)

#define fts_object_message_cache_set_selector(o, s) ((o)->cache_selector = (s))
#define fts_object_message_cache_set_type(o, t) ((o)->cache_type = (t))
#define fts_object_message_cache_set_varargs(o, v) ((o)->cache_varargs = (v))
#define fts_object_message_cache_set_method(o, m) ((o)->cache_method = (m))

#endif
