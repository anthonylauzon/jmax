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

typedef struct {
  fts_object_t o;   /* MUST BE FIRST STRUCTURE MEMBER */
  int n;            /* the state of the object */
} integer_t;

/* -------------------- the integer object methods -------------------- */

static void integer_init( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *)o;

  this->n = fts_get_int_arg( ac, at, 1, 0);
}

static void integer_bang( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *)o;
  fts_atom_t a;

  fts_set_int( &a, this->n);
  fts_outlet_send( o, 0, fts_s_int, 1, &a);
}

static void integer_int( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *)o;

  this->n = fts_get_int_arg( ac, at, 0, 0);

  fts_outlet_send( o, 0, fts_s_int, 1, at);
}

static void integer_float( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *)o;
  fts_atom_t a;

  this->n = (long)fts_get_float_arg( ac, at, 0, 0);

  fts_set_int( &a, this->n);
  fts_outlet_send( o, 0, fts_s_int, 1, &a);
}

static void integer_in1( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *)o;

  this->n = fts_get_long_arg( ac, at, 0, 0);
}

static void integer_float1( fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  integer_t *this = (integer_t *)o;

  this->n = (int)fts_get_float_arg( ac, at, 0, 0);
}


/* -------------------- the integer object instantiation function -------------------- */

static fts_status_t integer_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_type_t t[2];

  post( "Instantiating class `integer' of package `sources'\n");

  /* initialize the class */
  fts_class_init( cl, sizeof(integer_t), 2, 1, 0);

  /* define the methods */
  t[0] = fts_t_symbol;
  t[1] = fts_t_int;
  fts_method_define_optargs( cl, fts_SystemInlet, fts_s_init, integer_init, 2, t, 1);

  t[0] = fts_t_int;
  fts_method_define( cl, 0, fts_type_get_selector(fts_t_int), integer_int, 1, t);

  t[0] = fts_t_float;
  fts_method_define( cl, 0, fts_type_get_selector(fts_t_float), integer_float, 1, t);

  fts_method_define( cl, 0, fts_s_bang, integer_bang, 0, 0 );

  t[0] = fts_t_int;
  fts_method_define( cl, 1, fts_type_get_selector(fts_t_int), integer_in1, 1, t);

  t[0] = fts_t_float;
  fts_method_define( cl, 1, fts_type_get_selector(fts_t_float), integer_float1, 1, t);

  /* define outlet type */
  t[0] = fts_t_int;
  fts_outlet_type_define( cl, 0, fts_type_get_selector(fts_t_int), 1, t);

  return fts_Success;
}

/* -------------------- the class installation function -------------------- */

void integer_config(void)
{
  post( "Installing class `integer' of package `sources'\n");

  /* Uses a bizarre class name no to overwrite the "int" standard class */
  fts_class_install( fts_new_symbol("_int"), integer_instantiate);

  /* ... and register 2 aliases: "i" and "integer" */
  fts_class_alias( fts_new_symbol("_integer"), fts_new_symbol("_int"));
  fts_class_alias( fts_new_symbol("_i"), fts_new_symbol("_int"));
}

