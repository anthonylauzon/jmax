/*
 * jMax
 * Copyright (C) 1999 by IRCAM
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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 *
 */


#include "fts.h"

typedef struct loadbang_t {
  fts_object_t _o;
} loadbang_t;

static void
loadbang_load_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_bang(o, 0);
}

static fts_status_t
loadbang_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(loadbang_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("load_init"), loadbang_load_init, 0, 0);

  fts_outlet_type_define(cl, 0, fts_s_bang, 0, 0);

  return fts_Success;
}


void
loadbang_config(void)
{
  fts_class_install(fts_new_symbol("loadbang"),loadbang_instantiate);
}
