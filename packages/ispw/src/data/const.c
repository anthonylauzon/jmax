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

/* --------------------- metaclass symbol: const.c ---------------------- */


#include "fts.h"


typedef struct
{
  fts_object_t o;
  fts_atom_t a;		
} const_obj_t;


static void const_obj_init(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static void const_obj_delete(fts_object_t *, int, fts_symbol_t , int, const fts_atom_t *);
static fts_status_t const_obj_instantiate(fts_class_t *, int, const fts_atom_t *);
static void const_obj_get_state(fts_daemon_action_t action, fts_object_t *obj,
				fts_symbol_t property, fts_atom_t *value);

/* ---------------- the metaclass configuration function ------------------ */

/* takes no arguments, returns a status */

void
const_obj_config(void)
{
  fts_metaclass_create(fts_new_symbol("const"),const_obj_instantiate, fts_always_equiv);
}

/* ---------------- the metaclass instantiation function ------------------ */

static fts_status_t
const_obj_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  /* initialize the class */

  fts_class_init(cl, sizeof(const_obj_t), 0, 0, 0);

  /* define message template entries */

  a[0] = fts_s_symbol;
  a[1] = fts_s_anything;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, const_obj_init, 2, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, const_obj_delete, 0, 0);

  /* daemon for the state property */

  fts_class_add_daemon(cl, obj_property_get, fts_s_state, const_obj_get_state);

  return fts_Success;
}

/* ------------------------- the $init method --------------------------- */

static void
const_obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_obj_t *this = (const_obj_t *) o;

  this->a = *(at + 1);
}

/* ------------------------- the delete method --------------------------- */

static void
const_obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  const_obj_t *this = (const_obj_t *) o;

  if (fts_is_atom_array(&(this->a)))
    fts_atom_array_free(fts_get_atom_array(&(this->a)));
}

/* Daemon for getting the property "state".
 */

static void
const_obj_get_state(fts_daemon_action_t action, fts_object_t *obj,
		    fts_symbol_t property, fts_atom_t *value)
{
  const_obj_t *this = (const_obj_t *) obj;

  *value = this->a;
}

