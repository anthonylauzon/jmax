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
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell, Miller Puckette.
 *
 */



#include <limits.h>
#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  fts_atom_t state;
} change_t;

static void
change_atom(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(!fts_atom_equals(at, &this->state))
    {
      fts_atom_assign(&this->state, at);
      fts_outlet_atom(o, 0, at);
    }
}

static void
change_anything(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(ac > 0 && s == fts_get_selector(at))
    change_atom(o, 0, 0, ac, at);
  else
    fts_object_signal_runtime_error(o, "Don't understand message %s", s);
}

static void
change_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(ac > 0)
    fts_atom_assign(&this->state, at);
}

static void
change_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  fts_set_void(&this->state);
  change_set(o, 0, 0, 1, at);  
}

static void
change_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  fts_atom_void(&this->state);
}

static fts_status_t
change_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(change_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, change_init);
  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_delete, change_delete);

  fts_method_define_varargs(cl, 0, fts_s_anything, change_anything);
  fts_method_define_varargs(cl, 0, fts_s_set, change_set);

  fts_outlet_type_define_varargs(cl, 0, fts_s_int);

  return fts_Success;
}

void
change_config(void)
{
  fts_class_install(fts_new_symbol("change"), change_instantiate);
}

