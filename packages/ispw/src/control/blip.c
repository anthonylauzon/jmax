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

