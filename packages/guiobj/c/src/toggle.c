#include "fts.h"

typedef struct
{
  fts_object_t o;
  long n;
} toggle_t;


static void
toggle_send_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_client_send_property(o, fts_s_value);
}


static void
toggle_send_ui_properties(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_object_ui_property_changed(o, fts_s_value);
}

static void
toggle_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;
  int v;

  v = fts_get_number(at);

  this->n = (v ? 1 : 0);

  fts_outlet_int(o, 0, this->n);

  fts_object_ui_property_changed(o, fts_s_value);
}

static void
toggle_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;
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
toggle_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;
  this->n = !this->n;

  fts_outlet_int(o, 0, this->n);

  fts_object_ui_property_changed(o, fts_s_value);
}


static void
toggle_set(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  toggle_t *this = (toggle_t *) o;
  int v;

  v = fts_get_int_arg(ac, at, 0, 0);

  this->n = (v ? 1 : 0);

  fts_object_ui_property_changed(o, fts_s_value);
}


/* property daemon */

static void
toggle_get_value(fts_daemon_action_t action, fts_object_t *obj,
	       fts_symbol_t property, fts_atom_t *value)
{
  toggle_t *this = (toggle_t *)obj;

  fts_set_int(value, this->n);
}

static void
toggle_put_value(fts_daemon_action_t action, fts_object_t *obj,
	       fts_symbol_t property, fts_atom_t *value)
{
  toggle_t *this = (toggle_t *)obj;
  int v;

  v = fts_get_number(value);

  this->n = (v ? 1 : 0);

  fts_outlet_int(obj, 0, this->n);

  fts_object_ui_property_changed(obj, fts_s_value);
}


static fts_status_t
toggle_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(toggle_t), 1, 1, 0);

  fts_method_define(cl, fts_SystemInlet, fts_s_send_properties, toggle_send_properties, 0, 0); 
  fts_method_define(cl, fts_SystemInlet, fts_s_send_ui_properties, toggle_send_properties, 0, 0); 

  fts_method_define(cl, 0, fts_s_bang, toggle_bang, 0, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, toggle_number, 1, a);
  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, toggle_number, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, toggle_list);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_set, toggle_set, 1, a);

   /* Add  the value daemon */

  fts_class_add_daemon(cl, obj_property_get, fts_s_value, toggle_get_value);
  fts_class_add_daemon(cl, obj_property_put, fts_s_value, toggle_put_value);

  a[0] = fts_s_int;
  fts_outlet_type_define(cl, 0, fts_s_int, 1, a);

  return fts_Success;
}

void
toggle_config(void)
{
  fts_metaclass_create(fts_new_symbol("toggle"),toggle_instantiate, fts_always_equiv);
}




