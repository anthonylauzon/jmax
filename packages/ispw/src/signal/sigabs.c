/******************** sigabs ***********************/
/* generates a the absolute value of a signal */ 
/* ported to the new FTS by MDC from the original Zack Settel code */

#include "fts.h"



typedef struct 
{
  fts_object_t obj;

} sigabs_t;

static fts_symbol_t sigabs_function = 0;

void
ftl_sigabs(fts_word_t *argv)
{
  float *in = (float *)fts_word_get_ptr(argv + 0);
  float *out = (float *)fts_word_get_ptr(argv + 1);
  long n = fts_word_get_long(argv + 2);
  int i;

#ifdef HAS_UNROLL_BY8
  for (i = 0; i < n; i += 8)
    {
      float f0, f1, f2, f3, f4, f5, f6, f7;

      f0 = (float) in[i + 0];
      f1 = (float) in[i + 1];
      f2 = (float) in[i + 2]; 
      f3 = (float) in[i + 3];
      f4 = (float) in[i + 4];
      f5 = (float) in[i + 5];
      f6 = (float) in[i + 6];
      f7 = (float) in[i + 7];

      out[i + 0] = (float) (f0 >= 0 ? f0 : (- f0));
      out[i + 1] = (float) (f1 >= 0 ? f1 : (- f1));
      out[i + 2] = (float) (f2 >= 0 ? f2 : (- f2));
      out[i + 3] = (float) (f3 >= 0 ? f3 : (- f3));
      out[i + 4] = (float) (f4 >= 0 ? f4 : (- f4));
      out[i + 5] = (float) (f5 >= 0 ? f5 : (- f5));
      out[i + 6] = (float) (f6 >= 0 ? f6 : (- f6));
      out[i + 7] = (float) (f7 >= 0 ? f7 : (- f7));
    }

#else
  for (i = 0; i < n; i ++)
    {
      float f;

      f = in[i];

      out[i] = (f >= 0 ? f : (- f));
    }
#endif
}


static void
sigabs_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_set_symbol(argv + 0, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol(argv + 1, fts_dsp_get_output_name(dsp, 0));
  fts_set_long  (argv + 2, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(sigabs_function, 3, argv);
}


static void
sigabs_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_insert(o); 
}


static void
sigabs_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_list_remove(o);
}


static fts_status_t
sigabs_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  /* initialize the class */

  fts_class_init(cl, sizeof(sigabs_t), 1, 1, 0); 

  /* define the system methods */

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, sigabs_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigabs_delete, 0, 0);

  fts_method_define_varargs(cl, fts_SystemInlet, fts_s_put, sigabs_put);

  /* Type the outlet */

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  sigabs_function = fts_new_symbol("sigabs");
  dsp_declare_function(sigabs_function, ftl_sigabs);

  return fts_Success;
}

void
sigabs_config(void)
{
  fts_metaclass_create(fts_new_symbol("abs~"),sigabs_instantiate, fts_always_equiv);
}






