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

static fts_symbol_t sym_const = 0;

static int
var_is_constant(int ac, const fts_atom_t *at)
{
  return (ac > 1 && fts_is_symbol(at + 1) && fts_get_symbol(at + 1) == sym_const);
}

static int
var_args_ok(int ac, const fts_atom_t *at)
{
  if(var_is_constant(ac, at) && ac > 3) /* var const <type> <args ...> */
    return (fts_is_symbol(at + 2));
  else if(ac > 1) /* var <type> [args ...] */
    return (fts_is_symbol(at + 1));
  else
    return 0;
}

static fts_symbol_t
var_get_type(int ac, const fts_atom_t *at)
{
  if(var_is_constant(ac, at))
    {
      ac--;
      at++;
    }

  /* get type name */
  if(ac > 1 && fts_is_symbol(at + 1))
    return fts_get_symbol(at + 1);
  else
    return 0;
}

static int
var_is_primitive(int ac, const fts_atom_t *at)
{
  fts_symbol_t type = var_get_type(ac, at);
  
  return (type == fts_s_int || type == fts_s_float || type == fts_s_symbol);
}

/********************************************************************
 *
 *   object
 *
 */

typedef struct
{
  fts_object_t o;
  fts_atom_t a;		
} var_t;

static void
var_init_primitive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;

  this->a = at[3];
}

static void
var_delete_primitive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;
}

static void
var_init_data(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;
  fts_symbol_t type = var_get_type(ac, at);
  fts_data_t *data;

  if(var_is_constant(ac, at))
    data = fts_data_new_const(type, ac - 3, at + 3);
  else
    data = fts_data_new(type, ac - 2, at + 2);

  fts_set_data(&this->a, data);
  fts_data_refer(data);
}

static void
var_delete_data(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;

  fts_data_release(fts_get_data(&this->a));
}

/*********************************************************************
 *
 *  client methods
 *
 */

static void
var_get_data(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  var_t *this = (var_t *)obj;

  if(fts_is_data(&this->a))
    fts_set_data(value, fts_get_data(&this->a));
  else
    fts_set_void(value);
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
var_output_primitive(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;

  fts_outlet_send(o, 0, fts_get_type(&this->a), 1, &this->a);
}

static void
var_output_data(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  var_t *this = (var_t *) o;

  fts_outlet_send(o, 0, fts_data_get_class_name(fts_get_data(&this->a)), 1, &this->a);
}

/********************************************************************
 *
 *   class
 *
 */

/* When defining a valiable: daemon for getting the property "state". */
static void
var_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  var_t *this = (var_t *) obj;

  *value = this->a;
}

static fts_status_t
var_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(var_args_ok(ac, at))
    {
      fts_symbol_t type = var_get_type(ac, at);
      
      /* initialize the class */
      fts_class_init(cl, sizeof(var_t), 1, 1, 0);
      
      if(var_is_constant(ac, at) && var_is_primitive(ac, at))
	{
	  if(fts_get_type(at + 3) == type)
	    {
	      fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, var_init_primitive);
	      fts_method_define(cl, fts_SystemInlet, fts_s_delete, var_delete_primitive, 0, 0);

	      fts_method_define_bang(cl, 0, var_output_primitive);
	    }
	  else
	    return &fts_CannotInstantiate;
	}
      else if(fts_data_is_class_name(type))
	{
	  if(!var_is_constant(ac, at))
	    {
	      /* variable definition */
	      fts_class_add_daemon(cl, obj_property_get, fts_s_data, var_get_data);
	    }
	  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_init, var_init_data);
	  fts_method_define(cl, fts_SystemInlet, fts_s_delete, var_delete_data, 0, 0);

	  fts_method_define_bang(cl, 0, var_output_data);
	}
      else
	return &fts_CannotInstantiate;

      /* daemon for the state property */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, var_get_state);

      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static int
var_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (var_args_ok(ac0, at0) == var_args_ok(ac1, at1) &&
	  var_is_constant(ac0, at0) == var_is_constant(ac1, at1) &&
	  var_is_primitive(ac0, at0) == var_is_primitive(ac1, at1)
	  );
}

void
var_config(void)
{
  sym_const = fts_new_symbol("const");
  fts_metaclass_create(fts_new_symbol("var"), var_instantiate, var_equiv);
}
