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
  int max_size;
} listreverse_t;

static void
listreverse_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;
  int i, j;

  if(ac > this->max_size)
    {
      fts_free(this->list);
      this->list = (fts_atom_t *) fts_malloc(ac * sizeof(fts_atom_t));
      this->max_size = ac;
    }

  for(i=0, j=ac-1; i<ac; i++, j--)
    this->list[i] = at[j];

  this->size = ac;
  
  fts_outlet_send(o, 0, fts_s_list, this->size, this->list);
}

static void
listreverse_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;

  this->size = 0;
  this->max_size = 0;
}

static void
listreverse_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listreverse_t *this = (listreverse_t *)o;

  fts_free(this->list);
}

static fts_status_t
listreverse_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listreverse_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, listreverse_init, 1, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, listreverse_delete, 0, 0);

  /* define the methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listreverse_list);

  /* Type the outlet */

  fts_outlet_type_define_varargs(cl, 0,	fts_s_list);

  return fts_Success;
}

void
listreverse_config(void)
{
  fts_class_install(fts_new_symbol("listreverse"), listreverse_instantiate);
}
