#include "fts.h"




/* Don't call thjios tyep sig_t, it conflict with system
   types in a lot of systems
*/

#define CLASS_NAME "sig~"

/**********************************************************
 *
 *    object
 *
 */
 
typedef struct 
{
  fts_object_t _o;

  ftl_data_t sig_ftl_data;
} sigobj_t;


static void
sig_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  int down = (int) fts_get_number_arg(ac, at, 2, 0);
  float value = (float) fts_get_number_arg(ac, at, 1, 0.0f);

  this->sig_ftl_data = ftl_data_new(float);
  ftl_data_copy(float, this->sig_ftl_data, &value);

  if (down > 0)
    {
      fts_atom_t a;

      fts_set_long(&a, down);
      fts_object_put_prop(o, fts_s_dsp_downsampling, &a);
    }

  dsp_list_insert(o); /* just put object in list */
}


static void
sig_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;

  ftl_data_free(this->sig_ftl_data);
  dsp_list_remove(o);
}

/**********************************************************
 *
 *    dsp
 *
 */
 
static void
sig_put_dsp_function(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_ftl_data(argv, this->sig_ftl_data);
  fts_set_symbol(argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv+2, fts_dsp_get_output_size(dsp, 0));

  dsp_add_funcall(ftl_sym.fill.f, 3, argv);
}

/**********************************************************
 *
 *    user methods
 *
 */
 
static void
sig_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigobj_t *this = (sigobj_t *)o;
  float value = (float) fts_get_number(at);

  ftl_data_copy(float, this->sig_ftl_data, &value);
}

static void
sig_bang(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  /* 0.26 compatibility ... */
}

/**********************************************************
 *
 *    class
 *
 */
 
static fts_status_t
sig_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(sigobj_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_number;
  a[2] = fts_s_number;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sig_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sig_delete, 0, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sig_put_dsp_function, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, sig_number, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sig_number, 1, a);

  fts_method_define(cl, 0, fts_s_bang, sig_bang, 0, 0);


  /* definir les inlets et outlets sig */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  return fts_Success;
}

void
sig_config(void)
{
  fts_metaclass_create(fts_new_symbol(CLASS_NAME),sig_instantiate, fts_always_equiv);
}
