
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.3 $ IRCAM $Date: 1998/05/05 13:24:47 $
 *
 */

#include "fts.h"


/*------------------------- vtoggle class -------------------------------------*/

typedef struct
{
  fts_object_t o;
  long n;
} vtoggle_t;


static void
vtoggle_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vtoggle_t *this = (vtoggle_t *) o;
  int v;

  v = fts_get_number(at);

  this->n = (v ? 1 : 0);

  fts_outlet_int(o, 0, this->n);

  fts_object_ui_property_changed(o, fts_s_value);
}

static void
vtoggle_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vtoggle_t *this = (vtoggle_t *) o;
  int v;

  if(fts_is_number(at))
    {
      v = fts_get_number(at);

      this->n = (v ? 1 : 0);

      fts_outlet_int(o, 0, this->n);

      fts_object_ui_property_changed(o, fts_s_value);
    }
}

static void
vtoggle_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vtoggle_t *this = (vtoggle_t *) o;
  this->n = !this->n;

  fts_outlet_int(o, 0, this->n);

  fts_object_ui_property_changed(o, fts_s_value);
}


static void
vtoggle_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  vtoggle_t *this = (vtoggle_t *) o;
  int v;

  v = fts_get_number_arg(ac, at, 0, 0);

  this->n = (v ? 1 : 0);

  fts_object_ui_property_changed(o, fts_s_value);
}


/* property daemon */

static void
vtoggle_get_value(fts_daemon_action_t action, fts_object_t *obj,
	       int idx, fts_symbol_t property, fts_atom_t *value)
{
  vtoggle_t *this = (vtoggle_t *)obj;

  fts_set_int(value, this->n);
}

static void
vtoggle_put_value(fts_daemon_action_t action, fts_object_t *obj,
	       int idx, fts_symbol_t property, fts_atom_t *value)
{
  vtoggle_t *this = (vtoggle_t *)obj;
  int v;

  v = fts_get_number(value);

  this->n = (v ? 1 : 0);

  fts_outlet_int(obj, 0, this->n);

  fts_object_ui_property_changed(obj, fts_s_value);
}


static fts_status_t
vtoggle_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(vtoggle_t), 1, 1, 0);

  fts_method_define(cl, 0, fts_s_bang, vtoggle_bang, 0, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, vtoggle_number, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, vtoggle_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, vtoggle_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_set, vtoggle_set, 1, a);

   /* Add  the value daemon */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, vtoggle_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, vtoggle_put_value);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
vtoggle_config(void)
{
  fts_metaclass_create(fts_new_symbol("toggle"),vtoggle_instantiate, fts_always_equiv);
}




