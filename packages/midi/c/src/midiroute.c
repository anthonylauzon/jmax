/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * See file COPYING.LIB for further informations on licensing terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#ifndef WIN32
#include <unistd.h>
#endif
#if HAVE_ALLOCA_H
#include <alloca.h>
#endif

static void
midiroute_input(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_midievent_t *event = (fts_midievent_t *)fts_get_object(at);

  fts_outlet_object(o, fts_midievent_get_type(event), (fts_object_t *)event);
}

static void
midiroute_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_set_outlets_number(o, n_midi_types);
}

static void
midiroute_instantiate(fts_class_t *cl)
{
  fts_class_init(cl, sizeof(fts_object_t), midiroute_init, NULL);

  fts_class_inlet(cl, 0, fts_midievent_type, midiroute_input);
  fts_class_outlet(cl, 0, fts_midievent_type);
}

void
midiroute_config(void)
{
  fts_class_install(fts_new_symbol("midiroute"), midiroute_instantiate);
}
