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

/************************************************************
 *
 *  object
 *
 */

static void for_int_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);
static void for_float_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at);

typedef struct 
{
  fts_object_t o;
  int init;
  int limit;
  int incr;
} for_int_t;

typedef struct 
{
  fts_object_t o;
  float init;
  float limit;
  float incr;
} for_float_t;

static void
for_int_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  for_int_t *this = (for_int_t *)o;
  
  this->init = 0;
  this->limit = 0;
  this->incr = 1;

  for_int_set(o, 0, 0, ac, at);
}

static void
for_float_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  for_float_t *this = (for_float_t *)o;
  
  this->init = 0.0;
  this->limit = 0.0;
  this->incr = 1.0;

  for_float_set(o, 0, 0, ac, at);
}

/************************************************************
 *
 *  user methods
 *
 */

/* int */

static void
for_int_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_int_t *this = (for_int_t *)o;
  int init = this->init;
  int limit = this->limit;
  int incr = this->incr;
  int i;

  if(incr > 0 && init < limit)
    {
      for(i=init; i<limit; i+=incr)
	fts_outlet_int(o, 0, i);
    }
  else if(incr < 0 && init > limit)
    {
      for(i=init; i>limit; i+=incr)
	fts_outlet_int(o, 0, i);
    }
}

static void
for_int_set_init_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_int_t *this = (for_int_t *)o;

  this->init = fts_get_number_int(at);
  for_int_go(o, 0, 0, 0, 0);
}

static void
for_int_set_limit(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_int_t *this = (for_int_t *)o;

  this->limit = fts_get_number_int(at);
}

static void
for_int_set_incr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_int_t *this = (for_int_t *)o;

  this->incr = fts_get_number_int(at);
}

static void
for_int_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  for_int_t *this = (for_int_t *)o;
  
  switch(ac)
    {
    case 3:
      this->incr = fts_get_number_int(at + 2);
    case 2:
      this->limit = fts_get_number_int(at + 1);
    case 1:
      this->init = fts_get_number_int(at + 0);
    }
}

static void
for_int_set_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_int_t *this = (for_int_t *)o;

  for_int_set(o, 0, 0, ac, at);
  for_int_go(o, 0, 0, 0, 0);
}

/* float */

static void
for_float_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_float_t *this = (for_float_t *)o;
  float init = this->init;
  float limit = this->limit;
  float incr = this->incr;
  float f;

  if(incr > 0.0 && init < limit)
    {
      for(f=init; f<limit; f+=incr)
	fts_outlet_float(o, 0, f);
    }
  else if(incr < 0.0 && init > limit)
    {
      for(f=init; f>limit; f+=incr)
	fts_outlet_float(o, 0, f);
    }
}

static void
for_float_set_init_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_float_t *this = (for_float_t *)o;

  this->init = fts_get_number_float(at);
  for_float_go(o, 0, 0, 0, 0);
}

static void
for_float_set_limit(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_float_t *this = (for_float_t *)o;

  this->limit = fts_get_number_float(at);
}

static void
for_float_set_incr(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_float_t *this = (for_float_t *)o;

  this->incr = fts_get_number_float(at);
}

static void
for_float_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  for_float_t *this = (for_float_t *)o;
  
  switch(ac)
    {
    case 3:
      this->incr = fts_get_number_float(at + 2);
    case 2:
      this->limit = fts_get_number_float(at + 1);
    case 1:
      this->init = fts_get_number_float(at + 0);
    }
}

static void
for_float_set_and_go(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  for_float_t *this = (for_float_t *)o;

  for_float_set(o, 0, 0, ac, at);
  for_float_go(o, 0, 0, 0, 0);
}

/************************************************************
 *
 *  class
 *
 */

int
for_is_int(int ac, const fts_atom_t *at)
{
  int i;

  for(i=0; i<ac; i++)
    {
      if(!fts_is_int(at + i))
	return 0;
    }
  
  return 1;
}

static fts_status_t
for_float_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];
  int i;

  for(i=0; i<ac; i++)
    if(!fts_is_number(at + i))
      return &fts_CannotInstantiate;

  if(for_is_int(ac, at))
    {
      fts_class_init(cl, sizeof(for_int_t), 3, 1, 0); 
      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, for_int_init);

      fts_method_define_varargs(cl, 0, fts_s_set, for_int_set);
      fts_method_define_varargs(cl, 0, fts_s_bang, for_int_go);
      fts_method_define_varargs(cl, 0, fts_s_list, for_int_set_and_go);

      fts_method_define_varargs(cl, 0, fts_s_int, for_int_set_init_and_go);
      fts_method_define_varargs(cl, 0, fts_s_float, for_int_set_init_and_go);

      fts_method_define_varargs(cl, 1, fts_s_int, for_int_set_limit);
      fts_method_define_varargs(cl, 1, fts_s_float, for_int_set_limit);

      fts_method_define_varargs(cl, 2, fts_s_int, for_int_set_incr);
      fts_method_define_varargs(cl, 2, fts_s_float, for_int_set_incr);
    }
  else
    {
      fts_class_init(cl, sizeof(for_float_t), 3, 1, 0);

      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, for_float_init);

      fts_method_define_varargs(cl, 0, fts_s_set, for_float_set);
      fts_method_define_varargs(cl, 0, fts_s_bang, for_float_go);
      fts_method_define_varargs(cl, 0, fts_s_list, for_float_set_and_go);

      fts_method_define_varargs(cl, 0, fts_s_int, for_float_set_init_and_go);
      fts_method_define_varargs(cl, 0, fts_s_float, for_float_set_init_and_go);

      fts_method_define_varargs(cl, 1, fts_s_int, for_float_set_limit);
      fts_method_define_varargs(cl, 1, fts_s_float, for_float_set_limit);

      fts_method_define_varargs(cl, 2, fts_s_int, for_float_set_incr);
      fts_method_define_varargs(cl, 2, fts_s_float, for_float_set_incr);
    }

  return fts_Success;
}

int
for_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return for_is_int(ac0, at0) == for_is_int(ac1, at1);
}

void
for_config(void)
{
  fts_metaclass_install(fts_new_symbol("for"), for_float_instantiate, for_equiv);
}
