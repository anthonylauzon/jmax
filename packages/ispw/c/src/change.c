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



#include <fts/fts.h>

typedef struct 
{
  fts_object_t o;
  int state;
} change_t;

static void
change_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;
  int n = fts_get_number_int(at);

  if (n != this->state)
    {
      this->state = n;
      fts_outlet_int(o, 0, n);
    }
}

static void
change_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  if(ac > 0 && fts_is_number(at))
     this->state = fts_get_number_int(at);
}

static void
change_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  change_t *this = (change_t *)o;

  ac--;
  at++;

  this->state = 0;

  change_set(o, 0, 0, 1, at);  
}

static fts_status_t
change_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_class_init(cl, sizeof(change_t), 1, 1, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, change_init);

  fts_method_define_varargs(cl, 0, fts_s_int, change_int);
  fts_method_define_varargs(cl, 0, fts_s_float, change_int);
  fts_method_define_varargs(cl, 0, fts_s_set, change_set);

  fts_outlet_type_define_varargs(cl, 0, fts_s_int);

  return fts_Success;
}

void
change_config(void)
{
  fts_class_install(fts_new_symbol("change"),change_instantiate);
}

