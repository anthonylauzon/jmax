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

#include <string.h>
#include "fts.h"

typedef struct
{
  fts_object_t o;

  fts_symbol_t prompt;
} blip_t;

static fts_symbol_t sym_blip = 0;

static char buf[1024];

/**********************************************************************
 *
 *  object
 *
 */

static void
blip_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  blip_t *this = (blip_t *)o;

  this->prompt = 0;
}


/**********************************************************************
 *
 *  user methods
 *
 */

static void
blip_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  blip_t *this = (blip_t *)o;

  if (this->prompt)
    fts_object_blip(o, "%s: bang", fts_symbol_name(this->prompt));
  else
    fts_object_blip(o, "bang");
}


static void
blip_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  blip_t *this = (blip_t *)o;

  sprintf_atoms(buf, ac, at);

  if (this->prompt)
    fts_object_blip(o, "%s: [ %s ]", fts_symbol_name(this->prompt), buf);
  else
    fts_object_blip(o, "[ %s ]", buf);
}

static void
blip_atoms(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  blip_t *this = (blip_t *)o;

  sprintf_atoms(buf, ac, at);

  if (this->prompt)
    fts_object_blip(o, "%s: %s", fts_symbol_name(this->prompt), buf);
  else
    fts_object_blip(o, "%s", buf);
}


static void
blip_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  blip_t *this = (blip_t *)o;

  if (this->prompt)
    {
      if (ac)
	{
	  sprintf_atoms(buf, ac, at);
	  fts_object_blip(o, "%s: %s %s", fts_symbol_name(this->prompt), fts_symbol_name(s), buf);
	}
      else
	fts_object_blip(o, "%s %s", fts_symbol_name(this->prompt), fts_symbol_name(s));
    }
  else
    if (ac)
      {
	sprintf_atoms(buf, ac, at);
	fts_object_blip(o, "%s %s", fts_symbol_name(s), buf);
      }

}

/**********************************************************************
 *
 *  class
 *
 */

static fts_status_t
blip_instantiate(fts_class_t *cl, int ac, const fts_atom_t *aat)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(blip_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, blip_init, 2, a, 1);

  fts_method_define_bang(cl, 0, blip_bang);
  fts_method_define_int(cl, 0, blip_atoms);
  fts_method_define_float(cl, 0, blip_atoms);
  fts_method_define_symbol(cl, 0, blip_atoms);
  fts_method_define_list(cl, 0, blip_list);
  fts_method_define_anything(cl, 0, blip_anything);

  return fts_Success;
}

void
blip_config(void)
{
  fts_class_install(fts_new_symbol("blip"), blip_instantiate);
}

