/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
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

  fts_outlet_int( o, 0, this->n);
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

  this->n = (long)fts_get_float_arg( ac, at, 0, 0);
  fts_outlet_int( o, 0, this->n);
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
  fts_symbol_t a[2];

  post( "Instantiating class `integer' of package `sources'\n");

  /* initialize the class */
  fts_class_init( cl, sizeof(integer_t), 2, 1, 0);

  /* define the methods */
  a[0] = fts_s_symbol;
  a[1] = fts_s_int;
  fts_method_define_optargs( cl, fts_SystemInlet, fts_s_init, integer_init, 2, a, 1);

  a[0] = fts_s_int;
  fts_method_define( cl, 0, fts_s_int, integer_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define( cl, 0, fts_s_float, integer_float, 1, a);

  fts_method_define( cl, 0, fts_s_bang, integer_bang, 0, 0 );

  a[0] = fts_s_int;
  fts_method_define( cl, 1, fts_s_int, integer_in1, 1, a );

  a[0] = fts_s_float;
  fts_method_define( cl, 1, fts_s_float, integer_float1, 1, a );

  /* define outlet type */
  a[0] = fts_s_int;
  fts_outlet_type_define( cl, 0, fts_s_int, 1, a );

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

