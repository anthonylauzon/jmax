/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */


/* WARNIG: code changed for compatibility with MAX Opcode; 
   exactly one post event is invoked for every message
   Also, printing is smoother on X window Max.
*/

#include <string.h>
#include "fts.h"
#include "data.h"

/**********************************************************************
 *
 *  object
 *
 */

typedef struct {
  fts_object_t o;
  fts_symbol_t prompt;
} post_obj_t;

static void
post_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post_obj_t *this = (post_obj_t *)o;

  this->prompt = fts_get_symbol_arg(ac, at, 1, 0);
}


/**********************************************************************
 *
 *  user methods
 *
 */

#define MAX_POST_DATA_ATOMS 1024

static void
post_obj_data(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post_obj_t *this = (post_obj_t *)o;
  fts_data_t *data = fts_get_data(at);
  fts_atom_t a[MAX_POST_DATA_ATOMS];
  int size = data_get_atoms(data, MAX_POST_DATA_ATOMS, a);
  
  if(this->prompt)
    post("%s: ", fts_symbol_name(this->prompt));
  
  post("{%s<%s> ", ((fts_data_is_const(data))? "const":""), fts_symbol_name(fts_data_get_class_name(data)));
  
  if(size <= MAX_POST_DATA_ATOMS)
    {
      post_atoms(size, a);
    }
  else
    {
      post_atoms(MAX_POST_DATA_ATOMS, a);
      post(" ... ");
    }
  
  post("}\n");

  fts_outlet_send(o, 0, s, ac, at);
}

static void
post_obj_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post_obj_t *this = (post_obj_t *)o;

  if(this->prompt)
    post("%s: ", fts_symbol_name(this->prompt));

  if(ac)
    {
      post("<%s> ", fts_symbol_name(s));
      post_atoms( ac, at);
      post("\n");
    }
  else
    post("%s\n", fts_symbol_name(s));

  fts_outlet_send(o, 0, s, ac, at);
}

/**********************************************************************
 *
 *  class
 *
 */

static fts_status_t
post_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *aat)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(post_obj_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, post_obj_init, 2, a, 1);

  fts_method_define_anything(cl, 0, post_obj_anything);

  a[0] = fts_s_data;
  fts_method_define(cl, 0, fts_s_atom_array, post_obj_data, 1, a);
  fts_method_define(cl, 0, fts_s_integer_vector, post_obj_data, 1, a);
  fts_method_define(cl, 0, fts_s_float_vector, post_obj_data, 1, a);

  return fts_Success;
}

void
post_obj_config(void)
{
  fts_class_install(fts_new_symbol("post"), post_obj_instantiate);
}
