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

typedef struct {
  fts_object_t o;
  int noutlets;
} bangbang_t;

static void bangbang_bang( fts_object_t *object, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bangbang_t *this = (bangbang_t *)object;
  int i;

  for ( i = this->noutlets - 1; i >= 0; i--)
    fts_outlet_send( object, i, fts_s_bang, 0, 0);
}

static void bangbang_init( fts_object_t *object, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  bangbang_t *this = (bangbang_t *)object;

  this->noutlets = fts_get_int_arg( ac, at, 1, 2);
}

static fts_status_t bangbang_instantiate(fts_class_t *class, int ac, const fts_atom_t *at)
{
  int i;
  int noutlets;
  fts_type_t t[2];

  post( "Instantiating class `SRCbangbang' of package `sources'\n");

  if ((ac >= 2)  && fts_is_int( &(at[1])))
    noutlets = fts_get_int( &(at[1]));
  else
    noutlets = 2;

  fts_class_init(class, sizeof(bangbang_t), 1, noutlets, 0);

  fts_method_define(class, 0, fts_s_bang, bangbang_bang, 0, 0);

  t[0] = fts_t_symbol;
  t[1] = fts_t_int;
  fts_method_define_optargs( class, fts_SystemInlet, fts_s_init, bangbang_init, 2, t, 1);

  for (i = 0; i < noutlets; i++)
    fts_outlet_type_define( class, i, fts_s_bang, 0, 0);

  return fts_Success;
}

static int bangbang_equiv( int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  if (ac0 == 1 && ac1 == 1 
      && fts_is_int(at0) && fts_is_int(at1)
      && fts_get_int(at0) == fts_get_int(at1))
    return 1;
  else
    return 0;
}

void src_bangbang_config(void)
{
  post( "Installing class `SRCbangbang' of package `sources'\n");

  /* Uses a bizarre class name no to overwrite the "bangbang" standard class */
  fts_metaclass_install( fts_new_symbol( "SRCbangbang"), bangbang_instantiate, bangbang_equiv);
}
