
/*
 *                      Copyright (c) 1993 by IRCAM
 *                          All rights reserved.
 *
 *  For any information regarding this and other IRCAM software, please
 *  send email to:
 *                              manager@ircam.fr
 *
 *      $Revision: 1.3 $ IRCAM $Date: 1998/03/18 19:05:47 $
 *
 */

#include "fts.h"

/*------------------------- gfloat class -------------------------------------*/

typedef struct {
  fts_object_t o;
  float f;
} gfloat_t;

static void
gfloat_open(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  fts_object_ui_property_changed(o, fts_s_value);
}

static void
gfloat_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  fts_outlet_float(o, 0, this->f);
}

static void
gfloat_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  this->f = (float) fts_get_long(at);

  fts_outlet_float(o, 0, this->f);

  fts_object_ui_property_changed(o, fts_s_value);
}

static void
gfloat_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  this->f = fts_get_float(at);

  fts_outlet_send(o, 0, fts_s_float, ac, at);

  fts_object_ui_property_changed(o, fts_s_value);
}

/* in case of list, only the first value is significative */

static void
gfloat_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (fts_is_long(at))
    gfloat_int(o, winlet, fts_s_int, 1, at);
  else if (fts_is_float(at))
    gfloat_float(o, winlet, fts_s_float, 1, at);
}


static void
gfloat_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  gfloat_t *this = (gfloat_t *)o;

  this->f = fts_get_number_arg(ac, at, 0, 0);

  fts_object_ui_property_changed(o, fts_s_value);
}

/* Daemons to put the "value" property*/

static void
gfloat_get_value(fts_daemon_action_t action, fts_object_t *obj,
		 int idx, fts_symbol_t property, fts_atom_t *value)
{
  gfloat_t *this = (gfloat_t *)obj;

  fts_set_float(value, this->f);
}


static void
gfloat_put_value(fts_daemon_action_t action, fts_object_t *obj,
		 int idx, fts_symbol_t property, fts_atom_t *value)
{
  gfloat_t *this = (gfloat_t *)obj;

  this->f = fts_get_float(value);

  fts_outlet_send(obj, 0, fts_s_float, 1, value);
}


static fts_status_t
gfloat_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(gfloat_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_new_symbol("open"), gfloat_open, 0, 0); 

  fts_method_define(cl, 0, fts_s_bang, gfloat_bang, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, gfloat_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, gfloat_float, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, gfloat_list);

  a[0] = fts_s_anything;	/* number */
  fts_method_define(cl, 0, fts_s_set, gfloat_set, 1, a);

  /* Add  the value daemons */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, gfloat_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, gfloat_put_value);

  a[0] = fts_s_float;
  fts_outlet_type_define(cl, 0, fts_s_float, 1, a);

  return fts_Success;
}

void
gfloat_config(void)
{
  fts_metaclass_create(fts_new_symbol("floatbox"),gfloat_instantiate, fts_always_equiv);
}

