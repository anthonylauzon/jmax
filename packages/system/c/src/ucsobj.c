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

#include <fts/fts.h>

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

