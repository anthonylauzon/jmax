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
 */

#include <fts/fts.h>
#include <string.h>

static fts_symbol_t arch_sym;

typedef struct 
{
  fts_object_t _o;

} arch_t;


static void
arch_bang_mth(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_outlet_send(o, 0, arch_sym, 0, 0);
}

static fts_status_t
arch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(arch_t), 1, 1, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, arch_bang_mth);
  
  return fts_Success;
}

void
arch_config(void)
{
  arch_sym = fts_new_symbol(FTS_ARCH_NAME);

  fts_class_install(fts_new_symbol("arch"),arch_instantiate);
}
