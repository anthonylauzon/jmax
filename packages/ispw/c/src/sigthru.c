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

typedef struct sigthrough
{
  fts_object_t  o;
} sigthru_t;

static void
sigthru_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *av)
{
  fts_outlet_send(o, 1, s, ac, av); 
}

static fts_status_t
sigthru_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(sigthru_t), 1, 2, 0);

  fts_method_define_varargs(cl, 0, fts_s_anything, sigthru_anything);

  return fts_Success;
}

void
sigthru_config(void)
{
  fts_class_install(fts_new_symbol("thru~"), sigthru_instantiate);
}
