/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

typedef struct 
{
  fts_object_t o;
  fts_atom_t *list;
  int size;
  int alloc;
} list_t;


/************************************************
 *
 *    object
 *
 */
 
static void
list_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_t *this = (list_t *)o;

  this->size = 0;
}

static void
list_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_t *this = (list_t *)o;

  fts_free(this->list);
}

static void
list_resize_buffer(list_t *this, int size)
{
  if(size > this->alloc)
    {
      fts_free(this->list);
      this->list = (fts_atom_t *) fts_malloc(size * sizeof(fts_atom_t));
      this->alloc = size;
    }

  this->size = size;
}

/************************************************
 *
 *  user methods
 *
 */

static void
list_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_t *this = (list_t *)o;

  fts_outlet_send(o, 0, fts_s_list, this->size, this->list);
}

static void
list_list_store(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_t *this = (list_t *)o;
  int i;

  list_resize_buffer(this, ac);

  for(i=0; i<ac; i++)
    this->list[i] = at[i];
}

static void
list_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  list_t *this = (list_t *)o;

  list_list_store(o, 0, 0, ac, at);
  list_bang(o, 0, 0, 0, 0);
}

/************************************************
 *
 *    class
 *
 */

static fts_status_t
list_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(list_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, list_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, list_delete, 0, 0);

  /* user methods */

  fts_method_define(cl, 0, fts_s_bang, list_bang, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_list, list_list);
  fts_method_define_varargs(cl, 1, fts_s_list, list_list_store);

  /* type the outlet */
  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
list_config(void)
{
  fts_class_install(fts_new_symbol("list"), list_instantiate);
}
