#include "fts.h"



/* the class */
enum
{
  INLET_sig = 0,
  N_INLETS
};

enum
{
  OUTLET_sig = 0,
  N_OUTLETS
};

#define DEFINE_PUT_FUN(name)\
  static void dsp_put_ ## name\
    (fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)\
    {dsp_put_all(o, (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0), ftl_sym.name.vec);}

  
/************************************************
 *
 *    object
 *
 */

typedef struct
{
  fts_object_t head;
  float scl;
} obj_t;

/* objects creation arguments */
enum
{
  OBJ_ARG_class,
  N_OBJ_ARGS
};

static void
obj_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o);
}

static void
obj_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}

/************************************************
 *
 *    dsp
 *
 */


static void
dsp_put_all(fts_object_t *o, fts_dsp_descr_t *dsp, fts_symbol_t dsp_symbol)
{
  fts_atom_t argv[3];

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(dsp_symbol, 3, argv);
}

/* a single put fun for each class */

  DEFINE_PUT_FUN(sin)
  DEFINE_PUT_FUN(cos)
  DEFINE_PUT_FUN(tan)
  DEFINE_PUT_FUN(asin)
  DEFINE_PUT_FUN(acos)
  DEFINE_PUT_FUN(atan)
  DEFINE_PUT_FUN(sinh)
  DEFINE_PUT_FUN(cosh)
  DEFINE_PUT_FUN(tanh)
  
/************************************************
 *
 *    user methods
 *
 */
 
/* no methods */
 
/************************************************
 *
 *    class
 *
 */

static fts_status_t
class_instantiate_realize(fts_class_t *cl, int ac, const fts_atom_t *at, fts_method_t mth)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(obj_t), N_INLETS, N_OUTLETS, 0);
  
  a[OBJ_ARG_class] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, obj_init, N_OBJ_ARGS, a);
  fts_method_define(cl, fts_SystemInlet, fts_s_delete, obj_delete, 0, 0);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, mth, 1, a);

  dsp_sig_inlet(cl, INLET_sig);
  dsp_sig_outlet(cl, OUTLET_sig);
  
  return fts_Success;
}


static fts_status_t
sin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_sin);
}

static fts_status_t
cos_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_cos);
}

static fts_status_t
tan_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_tan);
}

static fts_status_t
asin_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_asin);
}


static fts_status_t
acos_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_acos);
}


static fts_status_t
atan_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_atan);
}


static fts_status_t
sinh_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_sinh);
}


static fts_status_t
cosh_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_cosh);
}


static fts_status_t
tanh_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  return class_instantiate_realize(cl, ac, at, dsp_put_tanh);
}


void
vectrigon_config(void)
{
  fts_metaclass_create(fts_new_symbol("sin~"), sin_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("cos~"), cos_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("tan~"), tan_instantiate, fts_always_equiv);

  fts_metaclass_create(fts_new_symbol("asin~"), asin_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("acos~"), acos_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("atan~"), atan_instantiate, fts_always_equiv);

  fts_metaclass_create(fts_new_symbol("sinh~"), sinh_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("cosh~"), cosh_instantiate, fts_always_equiv);
  fts_metaclass_create(fts_new_symbol("tanh~"), tanh_instantiate, fts_always_equiv);
}







