/*
 * jMax
 * 
 * Copyright (C) 1999 by IRCAM
 * All rights reserved.
 *
 * Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
 * 
 * This program may be used and distributed under the terms of the 
 * accompanying LICENSE.
 *
 * This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
 * for DISCLAIMER OF WARRANTY.
 * 
 */
#include "fts.h"

static fts_symbol_t sym_const = 0;
static fts_symbol_t sym_var = 0;

fts_symbol_t
var_get_class(int ac, const fts_atom_t *at)
{
  if(ac > 0 && fts_is_symbol(at))
    return fts_get_symbol(at);
  else
    return 0;
}

fts_symbol_t
var_get_type(int ac, const fts_atom_t *at)
{
  if(ac == 2)
    return (fts_is_data(at + 1)? fts_data_get_class_name(fts_get_data(at + 1)): fts_get_type(at + 1));
  if(ac > 2 && fts_is_symbol(at + 1))
    return fts_get_symbol(at + 1);
  else
    return 0;
}

const fts_atom_t *
var_get_initializer(int ac, const fts_atom_t *at)
{
  if(ac == 2)
    return (at + 1);
  if(ac > 2)
    return (at + 2);
  else
    return 0;
}

static fts_symbol_t
var_get_initializer_type(int ac, const fts_atom_t *at)
{
  const fts_atom_t *init = var_get_initializer(ac, at);

  if(init)
    return (fts_is_data(init)? fts_data_get_class_name(fts_get_data(init)): fts_get_type(init));
  else
    return fts_s_void; /* might be better than 0 */
}

static int
var_initializer_is_constant(int ac, const fts_atom_t *at)
{
  const fts_atom_t *init = var_get_initializer(ac, at);

  if(init)
    {
      if(fts_is_data(init))
	return fts_data_is_const(fts_get_data(init));
      else
	return (fts_is_int(init) || fts_is_float(init) || fts_is_symbol(init));
    }
  else
    return 0;
}

/********************************************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_symbol_t type;
  fts_atom_t a;		
} var_t;

static void
var_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *)o;
  fts_symbol_t class = var_get_class(ac, at);
  fts_symbol_t type = var_get_type(ac, at);
  const fts_atom_t *init = var_get_initializer(ac, at);

  this->type = type;

  fts_set_void(&this->a);

  if(fts_is_data(init))
    {
      fts_data_t *data = fts_get_data(init);

      if(fts_data_has_no_reference(data))
	fts_data_set_var(data); /* reuse initializer (just remove const tag) */
      else
	{
	  fts_data_t *copy;

	  /* copy initializer */
	  if(type == fts_s_integer_vector)
	    {      
	      copy = (fts_data_t *)fts_integer_vector_new(0);
	      fts_integer_vector_copy((fts_integer_vector_t *)data, (fts_integer_vector_t *)copy);
	    }
	  else if(type == fts_s_float_vector)
	    {      
	      copy = (fts_data_t *)fts_float_vector_new(0);
	      fts_float_vector_copy((fts_float_vector_t *)data, (fts_float_vector_t *)copy);
	    }
	  else if(type == fts_s_atom_array)
	    {      
	      copy = (fts_data_t *)fts_atom_array_new(0);
	      fts_atom_array_copy((fts_atom_array_t *)data, (fts_atom_array_t *)copy);
	    }

	  data = copy;
	}
      
      if(class == sym_const)
	fts_data_set_const(data);

      fts_data_refer(data);
      fts_set_data(&this->a, data);
    }
  else
    this->a = *init;
}

static void
var_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;

  if(fts_is_data(&this->a))
    fts_data_derefer(fts_get_data(&this->a));
}

static void
var_get_data(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  var_t *this = (var_t *)obj;

  if(fts_is_data(&this->a))
    {
      fts_data_t *data = fts_get_data(&this->a);

      if(!fts_data_is_const(data))
	fts_set_data(value, data);
    }

  fts_set_void(value);
}

/********************************************************************
 *
 *   class
 *
 */

static int
var_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (var_get_type(ac0, at0) == var_get_type(ac1, at1) &&
	  var_initializer_is_constant(ac0, at0) == var_initializer_is_constant(ac1, at1) &&
	  var_get_initializer_type(ac0, at0) == var_get_initializer_type(ac1, at1));
}

/* when defining a variable: daemon for getting the property "state". */
static void
var_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  var_t *this = (var_t *) obj;

  *value = this->a;
}

static fts_status_t
var_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t s[4];
  fts_symbol_t class = var_get_class(ac, at);
  fts_symbol_t type = var_get_type(ac, at);
  const fts_atom_t *init = var_get_initializer(ac, at);
  fts_symbol_t init_type = var_get_initializer_type(ac, at);
  
  if(!type)
    {
      post("%s: missing type specifier\n", fts_symbol_name(class));
      return &fts_CannotInstantiate;
    }
    
  if(class == sym_const &&
     type != fts_s_int &&
     type != fts_s_float &&
     type != fts_s_symbol &&
     type != fts_s_integer_vector &&
     type != fts_s_float_vector &&
     type != fts_s_atom_array)
    {
      post("%s: %s is not a data type supported for constants\n", fts_symbol_name(class), fts_symbol_name(type));
      return &fts_CannotInstantiate;
    }
  else if(class == sym_var &&
     type != fts_s_integer_vector &&
     type != fts_s_float_vector &&
     type != fts_s_atom_array)
    {
      post("%s: %s is not a data type supported for variables\n", fts_symbol_name(class), fts_symbol_name(type));
      return &fts_CannotInstantiate;
    }

  if(!init)
    {
      post("%s: missing initializer\n", fts_symbol_name(class));
      return &fts_CannotInstantiate;
    }
    
  if(!var_initializer_is_constant(ac, at))
    {
      post("%s: initializer must be constant\n", fts_symbol_name(class));
      return &fts_CannotInstantiate;
    }

  if(type != init_type)
    {
      /* no conversion yet :-( */
      post("%s: can not initialize %s with %s\n", fts_symbol_name(class), fts_symbol_name(type), fts_symbol_name(init_type));
      return &fts_CannotInstantiate;
    }

  fts_class_init(cl, sizeof(var_t), 0, 0, 0);

  s[0] = fts_s_symbol;
  s[1] = fts_s_anything;
  s[2] = fts_s_anything;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, var_init, 3, s, 1);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, var_delete, 0, 0);

  fts_class_add_daemon(cl, obj_property_get, fts_s_data, var_get_data);
  fts_class_add_daemon(cl, obj_property_get, fts_s_state, var_get_state);
  
  return fts_Success;
}

void
var_config(void)
{
  sym_var = fts_new_symbol("var");
  sym_const = fts_new_symbol("const");

  fts_metaclass_install(sym_var, var_instantiate, var_equiv);
  fts_metaclass_install(sym_const, var_instantiate, var_equiv);
}

