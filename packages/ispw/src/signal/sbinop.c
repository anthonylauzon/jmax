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

#include "fts.h"

typedef struct
{
  fts_object_t o;

  fts_symbol_t operator;
  fts_symbol_t value;

} sbinop_t;


static void
sbinop_symbol_1(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->operator = fts_get_symbol(at);
}


static void
sbinop_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = fts_get_symbol_arg(ac, at, 0, 0);
}


static void
sbinop_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (ac >= 2 && fts_is_symbol(&at[1]))
    sbinop_symbol_1(o, winlet, s, 1, at + 1);

  if (ac >= 1 && fts_is_symbol(&at[0]))
    fts_message_send(o, 0, fts_s_int, 1, at);
}


static void
sbinop_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = 0;
  this->operator = fts_get_symbol_arg(ac, at, 1, 0);
}


static fts_status_t
sbinop_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at,
		   fts_method_t bang_meth,
		   fts_method_t symbol_meth)
{
  if((ac == 2) && fts_is_symbol(&at[1]))
    {
      fts_symbol_t a[2];

      fts_class_init(cl, sizeof(sbinop_t), 2, 1, 0);

      /* system inlet */

      a[0] = fts_s_symbol;
      a[1] = fts_s_symbol;
      fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sbinop_init, 2, a, 1);

      /* inlet #0 */

      a[0] = fts_s_symbol;
      fts_method_define(cl, 0, fts_new_symbol("set"), sbinop_set, 1, a);

      a[0] = fts_s_symbol;
      fts_method_define(cl, 0, fts_s_symbol, symbol_meth, 1, a);

      fts_method_define(cl, 0, fts_s_bang, bang_meth, 0, 0);

      fts_method_define_varargs(cl, 0, fts_s_list, sbinop_list);

      /* inlet #1 */

      a[0] = fts_s_symbol;
      fts_method_define(cl, 1, fts_s_symbol, sbinop_symbol_1, 1, a);

      /* outlet #0 */

      a[0] = fts_s_int;
      fts_outlet_type_define(cl, 0, fts_s_int, 1, a);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static void
see_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = fts_get_symbol(at);
  fts_outlet_int(o, 0, this->value == this->operator);
}

static void
see_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  fts_outlet_int(o, 0, this->value == this->operator);
}


static fts_status_t
see_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return sbinop_instantiate(cl, ac, at, see_bang, see_symbol);
}


static void
sne_symbol(fts_object_t *o, int winlet, fts_symbol_t s, int ac,  const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  this->value = fts_get_symbol(at);
  fts_outlet_int(o, 0, this->value != this->operator);
}


static void
sne_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sbinop_t *this = (sbinop_t *)o;

  fts_outlet_int(o, 0, this->value != this->operator);
}


static fts_status_t
sne_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return sbinop_instantiate(cl, ac, at, sne_bang, sne_symbol);
}


void
sbinop_config(void)
{
  /* integer components of generic multiclasses */

  fts_metaclass_install(fts_new_symbol("=="), see_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(fts_new_symbol("!="), sne_instantiate, fts_arg_type_equiv);
}





