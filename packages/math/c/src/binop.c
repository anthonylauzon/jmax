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
#include "intvec.h"
#include "floatvec.h"

fts_symbol_t math_sym_add = 0;
fts_symbol_t math_sym_sub = 0;
fts_symbol_t math_sym_mul = 0;
fts_symbol_t math_sym_div = 0;
fts_symbol_t math_sym_bus = 0;
fts_symbol_t math_sym_vid = 0;

fts_symbol_t math_sym_ee = 0;
fts_symbol_t math_sym_ne = 0;
fts_symbol_t math_sym_gt = 0;
fts_symbol_t math_sym_ge = 0;
fts_symbol_t math_sym_lt = 0;
fts_symbol_t math_sym_le = 0;

fts_symbol_t math_sym_min = 0;
fts_symbol_t math_sym_max = 0;
  
extern fts_status_t binop_number_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);
extern fts_status_t binop_ivec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);
extern fts_status_t binop_fvec_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at);

static fts_status_t
binop_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(ac == 2)
    {
      if(fts_is_number(at + 1))
	return binop_number_instantiate(cl, ac, at);
      else if(int_vector_atom_is(at + 1))
	return binop_ivec_instantiate(cl, ac, at);
      else if(float_vector_atom_is(at + 1)) 
      return binop_fvec_instantiate(cl, ac, at);
    }

  return &fts_CannotInstantiate;
}

static fts_object_t *
binop_doctor_set_default_arg_zero(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];
  int i;

  if(ac == 1)
    {
      a[0] = at[0];
      fts_set_int(a + 1, 0);
      
      obj = fts_eval_object_description(patcher, 2, a);

      return obj;
    }
  else
    return 0;
}

static fts_object_t *
binop_doctor_set_default_arg_one(fts_patcher_t *patcher, int ac, const fts_atom_t *at)
{
  fts_object_t *obj;
  fts_atom_t a[2];
  int i;

  if(ac == 1)
    {
      a[0] = at[0];
      fts_set_int(a + 1, 1);
      
      obj = fts_eval_object_description(patcher, 2, a);

      return obj;
    }
  else
    return 0;
}

void
math_binop_config(void)
{
  math_sym_add = fts_new_symbol("v+");
  math_sym_sub = fts_new_symbol("v-");
  math_sym_mul = fts_new_symbol("v*");
  math_sym_div = fts_new_symbol("v/");
  math_sym_bus = fts_new_symbol("v-+");
  math_sym_vid = fts_new_symbol("v/*");
  
  math_sym_ee = fts_new_symbol("v==");
  math_sym_ne = fts_new_symbol("v!=");
  math_sym_gt = fts_new_symbol("v>");
  math_sym_ge = fts_new_symbol("v>=");
  math_sym_lt = fts_new_symbol("v<");
  math_sym_le = fts_new_symbol("v<=");
  
  math_sym_min = fts_new_symbol("vmin");
  math_sym_max = fts_new_symbol("vmax");
  
  fts_metaclass_install(math_sym_add, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_sub, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_mul, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_div, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_bus, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_vid, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_ee, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_ne, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_gt, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_ge, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_lt, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_le, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_min, binop_instantiate, fts_arg_type_equiv);
  fts_metaclass_install(math_sym_max, binop_instantiate, fts_arg_type_equiv);

  fts_register_object_doctor(math_sym_add, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_sub, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_mul, binop_doctor_set_default_arg_one);
  fts_register_object_doctor(math_sym_div, binop_doctor_set_default_arg_one);
  fts_register_object_doctor(math_sym_bus, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_vid, binop_doctor_set_default_arg_one);
  fts_register_object_doctor(math_sym_ee, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_ne, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_gt, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_ge, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_lt, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_le, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_min, binop_doctor_set_default_arg_zero);
  fts_register_object_doctor(math_sym_max, binop_doctor_set_default_arg_zero);
}

