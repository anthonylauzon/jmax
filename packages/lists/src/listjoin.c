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
 * Based on Max/ISPW by Miller Puckette.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */

#include "fts.h"

typedef struct
{
  fts_atom_t *at;
  int ac;
  int alloc;
} list_t;
  
typedef struct 
{
  fts_object_t o;
  list_t list;
  list_t store;
} listjoin_t;

#define LIST_ALLOC_BLOCK 32

/*********************************************
 *
 *  utils
 *
 */

static list_t*
list_realloc_empty(list_t *list, int ac)
{
  int alloc = list->alloc;

  if(list->at) 
    fts_free(list->at);

  while(alloc < ac)
    alloc += LIST_ALLOC_BLOCK;

  list->at = (fts_atom_t *) fts_malloc(alloc * sizeof(fts_atom_t));
  list->ac = ac;
  list->alloc = alloc;

  return list;
}


/*********************************************
 *
 *  methods
 *
 */

static void
listjoin_store_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int i;

  if(this->store.alloc < ac)
    list_realloc_empty(&(this->store), ac);
  else
    this->store.ac = ac;

  memcpy((char *)this->store.at, at, ac * sizeof(fts_atom_t)); /* copy incomming list to store */
}

static void
listjoin_append(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int total;

  total = this->store.ac + ac;

  if(this->list.alloc < total)
    list_realloc_empty(&(this->list), total);
  else
    this->list.ac = total;

  /* copy incomming list */
  memcpy((char *)this->list.at, at, ac * sizeof(fts_atom_t)); 
  /* append stored list */
  memcpy((char *)(this->list.at + ac), this->store.at, this->store.ac * sizeof(fts_atom_t)); 

  fts_outlet_send(o, 0, fts_s_list, this->list.ac, this->list.at);
}


static void
listjoin_prepend(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;
  int total;

  total = this->store.ac + ac;

  if(this->list.alloc < total)
    list_realloc_empty(&(this->list), total);
  else
    this->list.ac = total;

  /* copy stored list */
  memcpy((char *)this->list.at, this->store.at, this->store.ac * sizeof(fts_atom_t)); 
  /* append incomming list */
  memcpy((char *)(this->list.at + this->store.ac), at, ac * sizeof(fts_atom_t)); 

  fts_outlet_send(o, 0, fts_s_list, this->list.ac, this->list.at);
}

/*********************************************
 *
 *  object
 *
 */

static void
listjoin_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  this->list.at = (fts_atom_t *) fts_malloc(LIST_ALLOC_BLOCK * sizeof(fts_atom_t));
  this->list.at = 0;
  this->list.ac = 0;
  this->list.alloc = 0;

  this->store.at = 0;
  this->store.ac = 0;
  this->store.alloc = 0;

  if(ac > 1)
    listjoin_store_list(o, 0, 0, ac-1, at+1);
}

static void
listjoin_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listjoin_t *this = (listjoin_t *)o;

  fts_free(this->list.at);
  fts_free(this->store.at);
}

/*********************************************
 *
 *  class
 *
 */

static void
listjoin_common_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  fts_atom_t class_name;

  /* initialization */
  fts_class_init(cl, sizeof(listjoin_t), 2, 1, 0); 

  /* system methods */
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, listjoin_init);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, listjoin_delete, 0, 0);

  /* user methods */
  fts_method_define_varargs(cl, 1, fts_s_list, listjoin_store_list);

  /* outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);
}

static fts_status_t
listappend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  listjoin_common_instantiate(cl, ac, at);

  fts_method_define_varargs(cl, 0, fts_s_list, listjoin_append);

  return fts_Success;
}

static fts_status_t
listprepend_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  listjoin_common_instantiate(cl, ac, at);

  fts_method_define_varargs(cl, 0, fts_s_list, listjoin_prepend);

  return fts_Success;
}

void
listjoin_config(void)
{
  fts_class_install(fts_new_symbol("listappend"), listappend_instantiate);
  fts_class_install(fts_new_symbol("listprepend"), listprepend_instantiate);
}







