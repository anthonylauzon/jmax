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

#include "fts.h"

/*------------------------- gfloat class -------------------------------------*/

typedef struct {
  fts_object_t o;
} ucs_t;


extern fts_status_t fts_ucs_execute_command_opcode(fts_symbol_t opcode, int argc, const fts_atom_t *argv);

static void
ucs_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_ucs_execute_command_opcode(s, ac, at);
}

static fts_status_t
ucs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(ucs_t), 1, 0, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, ucs_anything);

  return fts_Success;
}

void
ucs_config(void)
{
  fts_class_install(fts_new_symbol("ucs"),ucs_instantiate);
}

