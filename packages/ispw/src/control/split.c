#include "fts.h"

typedef struct {
  fts_object_t o;
  long int_bound[2];
  float float_bound[2];
} split_t;

static void
split_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  split_t *this = (split_t *)o;
  long l = fts_get_long(at);

  if (l <= this->int_bound[1] && l >= this->int_bound[0])
    fts_outlet_int(o, 0, l);
  else
    fts_outlet_int(o, 1, l);
}

static void
split_float(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  split_t *this = (split_t *)o;
  float f = fts_get_float(at);

  if (f <= this->float_bound[1] && f >= this->float_bound[0])
    fts_outlet_send(o, 0, fts_s_float, 1, at);
  else
    fts_outlet_send(o, 1, fts_s_float, 1, at);
}

static void
split_bound(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  split_t *this = (split_t *)o;
  float float_bound = (float)fts_get_number_arg(ac, at, 0, 0);
  long int_bound = float_bound;

  this->int_bound[winlet-1] = ((float)int_bound == float_bound)? int_bound: int_bound + 1;
  this->float_bound[winlet-1] = float_bound;
}

static void
split_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  split_bound(o, 1, 0, 1, at + 1);
  split_bound(o, 2, 0, 1, at + 2);
}

static void
split_list(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{ 
  if(ac >= 1)
    {
      if(fts_is_float(at))
	split_float(o, 0, 0, 1, at);
      else if(fts_is_float(at))
	split_int(o, 0, 0, 1, at);	
    }

  if(ac >= 2)
    split_bound(o, 1, 0, 1, at + 1);

  if(ac >= 3)
    split_bound(o, 2, 0, 1, at + 2);
}

static fts_status_t
split_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(split_t), 3, 2, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, split_init, 3, a, 1);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, split_int, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, split_float, 1, a);

  a[0] = fts_s_number;
  fts_method_define(cl, 1, fts_s_int, split_bound, 1, a);
  fts_method_define(cl, 1, fts_s_float, split_bound, 1, a);

  a[0] = fts_s_number;
  fts_method_define(cl, 2, fts_s_int, split_bound, 1, a);
  fts_method_define(cl, 2, fts_s_float, split_bound, 1, a);

  fts_method_define_varargs(cl, 0, fts_s_list, split_list);

  return fts_Success;
}

void
split_config(void)
{
  fts_metaclass_create(fts_new_symbol("split"),split_instantiate, fts_always_equiv);
}

