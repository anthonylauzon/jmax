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

static int
const_obj_arg_is_constant(int ac, const fts_atom_t *at)
{
  if(ac > 1)
    {
      if(fts_is_number(at + 1) || fts_is_symbol(at + 1))
	return 1;
      else if(fts_is_data(at + 1))
	return fts_data_is_const(fts_get_data(at + 1));
      else
	return 0;
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
  fts_atom_t a;		
} const_obj_t;

static void
const_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_obj_t *this = (const_obj_t *) o;
  
  this->a = at[1];

  if(fts_is_data(&this->a))
    fts_data_refer(fts_get_data(&this->a));
}

static void
const_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_obj_t *this = (const_obj_t *) o;

  /* this is NOT garbage collected! */
  if(fts_is_data(&this->a))
    fts_data_derefer(fts_get_data(&this->a));
}

/********************************************************************
 *
 *   user methods
 *
 */

static void
const_obj_output(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_obj_t *this = (const_obj_t *) o;
  const fts_atom_t *atom = &this->a;

  if(fts_is_data(&this->a))
    fts_outlet_send(o, 0, fts_data_get_class_name(fts_get_data(atom)), 1, atom);
  else
    fts_outlet_send(o, 0, fts_get_type(atom), 1, atom);
}

/********************************************************************
 *
 *   class
 *
 */

/* When defining a valiable: daemon for getting the property "state". */
static void
const_obj_get_state(fts_daemon_action_t action, fts_object_t *obj, fts_symbol_t property, fts_atom_t *value)
{
  const_obj_t *this = (const_obj_t *) obj;

  *value = this->a;
}

static fts_status_t
const_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  if(const_obj_arg_is_constant(ac, at))
    {
      fts_symbol_t a[2];
      
      /* initialize the class */
      fts_class_init(cl, sizeof(const_obj_t), 1, 1, 0);
      
      /* define message template entries */
      a[0] = fts_s_symbol;
      a[1] = fts_s_anything;
      fts_method_define(cl, fts_SystemInlet, fts_s_init, const_obj_init, 2, a);
      fts_method_define(cl, fts_SystemInlet, fts_s_delete, const_obj_delete, 0, 0);
      
      fts_method_define(cl, 0, fts_s_bang, const_obj_output, 0, 0);
      
      /* daemon for the state property */
      fts_class_add_daemon(cl, obj_property_get, fts_s_state, const_obj_get_state);
      
      return fts_Success;
    }
  else
    return &fts_CannotInstantiate;
}

static int
const_obj_equiv(int ac0, const fts_atom_t *at0, int ac1, const fts_atom_t *at1)
{
  return (const_obj_arg_is_constant(ac0, at0) == const_obj_arg_is_constant(ac1, at1));
}

void
const_obj_config(void)
{
  fts_metaclass_create(fts_new_symbol("const"), const_obj_instantiate, const_obj_equiv);
}
