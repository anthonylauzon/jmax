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
  int index;
} listelement_t;

static void
listelement_index(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_t *this = (listelement_t *)o;

  this->index = fts_get_long(at);
}

static void
listelement_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_t *this = (listelement_t *)o;
  int i;

  i = this->index;

  if(ac > i)
    {
      if (fts_is_data(at + i))
	{
	  fts_data_t *data = fts_get_data(at + i);
	  fts_outlet_send(o, 0, fts_data_get_class_name(data), 1, at + i);
	}
      else
	fts_outlet_send(o, 0, fts_get_type(at + 1), 1, at + i);
    }
}

static void
listelement_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  listelement_index(o, 0, 0, 1, at+1);
}

static fts_status_t
listelement_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(listelement_t), 2, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, listelement_init, 2, a);

  /* define the methods */

  fts_method_define_varargs(cl, 0, fts_s_list, listelement_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 1, fts_s_int, listelement_index, 1, a);

  return fts_Success;
}

void
listelement_config(void)
{
  fts_class_install(fts_new_symbol("listelement"), listelement_instantiate);
}





