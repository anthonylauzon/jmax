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

typedef struct {
  fts_object_t o;
  fts_symbol_t prompt;
} print_t;

static fts_symbol_t sym_print = 0;

/**********************************************************************
 *
 *  object
 *
 */

static void
print_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  this->prompt = fts_get_symbol_arg(ac, at, 1, fts_new_symbol("print"));
}


/**********************************************************************
 *
 *  user methods
 *
 */

static void
print_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: bang\n", fts_symbol_name(this->prompt));
}


static void
print_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: [ ", fts_symbol_name(this->prompt));
  post_atoms(ac, at);
  post("]\n");
}

static void
print_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: ", fts_symbol_name(this->prompt));
  post_atoms(ac, at);
  post("\n");
}


static void
print_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  print_t *this = (print_t *)o;

  post("%s: ", fts_symbol_name(this->prompt));

  if (ac)
    {
      post("%s ", fts_symbol_name(s));
      post_atoms( ac, at);
      post("\n");
    }
  else
    post("%s\n", fts_symbol_name(s));
}

/**********************************************************************
 *
 *  class
 *
 */

static fts_status_t
print_instantiate(fts_class_t *cl, int ac, const fts_atom_t *aat)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(print_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, print_init, 2, a, 1);

  fts_method_define_bang(cl, 0, print_bang);
  fts_method_define_int(cl, 0, print_atoms);
  fts_method_define_float(cl, 0, print_atoms);
  fts_method_define_symbol(cl, 0, print_atoms);
  fts_method_define_list(cl, 0, print_list);
  fts_method_define_anything(cl, 0, print_anything);

  return fts_Success;
}

void
print_config(void)
{
  fts_class_install(fts_new_symbol("print"),print_instantiate);
}

