#include "fts.h"


fts_symbol_t fts_s_print;
static fts_symbol_t phasor_function = 0;

typedef struct 
{
  double phase;
  float fconv;
} phasor_state_t;


void
ftl_phasor(fts_word_t *argv)
{
  float *freq = (float *)  fts_word_get_ptr(argv + 0);
  float *out  = (float *)  fts_word_get_ptr(argv + 1);
  phasor_state_t *x = (phasor_state_t *) fts_word_get_ptr(argv + 2);
  long int n = fts_word_get_long(argv + 3);
  int phase;
  float fconv;
  int i;

  phase = 0x7fffffffL * x->phase;
  fconv = x->fconv * 0x7fffffffL;

  for (i = 0; i < n ; i += 8)
    {
      int p0, p1, p2, p3, p4, p5, p6, p7;

      p0 = (phase + (int)(fconv * freq[i + 0])) & 0x7fffffffL;
      p1 = (p0 + (int)(fconv * freq[i + 1])) & 0x7fffffffL;
      p2 = (p1 + (int)(fconv * freq[i + 2])) & 0x7fffffffL;
      p3 = (p2 + (int)(fconv * freq[i + 3])) & 0x7fffffffL;
      p4 = (p3 + (int)(fconv * freq[i + 4])) & 0x7fffffffL;
      p5 = (p4 + (int)(fconv * freq[i + 5])) & 0x7fffffffL;
      p6 = (p5 + (int)(fconv * freq[i + 6])) & 0x7fffffffL;
      p7 = (p6 + (int)(fconv * freq[i + 7])) & 0x7fffffffL;

      out[i + 0] = (1.0f/0x7fffffffL) * p0;
      out[i + 1] = (1.0f/0x7fffffffL) * p1;
      out[i + 2] = (1.0f/0x7fffffffL) * p2;
      out[i + 3] = (1.0f/0x7fffffffL) * p3;
      out[i + 4] = (1.0f/0x7fffffffL) * p4;
      out[i + 5] = (1.0f/0x7fffffffL) * p5;
      out[i + 6] = (1.0f/0x7fffffffL) * p6;
      out[i + 7] = (1.0f/0x7fffffffL) * p7;

      phase = p7;
    }

  x->phase = (1.0f/0x7fffffffL) * phase;
}


typedef struct 
{
  fts_object_t _o;

  ftl_data_t phasor_ftl_data;
} phasor_t;


static void
phasor_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  fts_atom_t argv[4];
  float fconv;
  const double zero = 0.0f;

  fconv = 1.0f / fts_dsp_get_input_srate(dsp, 0);

  ftl_data_set(phasor_state_t, this->phasor_ftl_data, fconv, &fconv);

  fts_set_symbol( argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_symbol( argv+1, fts_dsp_get_output_name(dsp, 0));
  fts_set_ftl_data( argv+2, this->phasor_ftl_data);
  fts_set_long( argv+3, fts_dsp_get_input_size(dsp, 0));

  dsp_add_funcall(phasor_function, 4, argv);
}


static void
phasor_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  const double zero = 0.0f;

  this->phasor_ftl_data = ftl_data_new(phasor_state_t);
  ftl_data_set(phasor_state_t, this->phasor_ftl_data, phase, &zero);

  dsp_list_insert(o);		/* just put object in list */
}


static void
phasor_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  post("phasor: phase print not implemented\n");
}


static void
phasor_number(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;
  double f = (double) fts_get_number(at);

  ftl_data_set(phasor_state_t, this->phasor_ftl_data, phase, &f);
}


static void
phasor_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  phasor_t *this = (phasor_t *)o;

  ftl_data_free(this->phasor_ftl_data);
  dsp_list_remove(o);
}


static fts_status_t
phasor_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(phasor_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, fts_SystemInlet, fts_s_init, phasor_init, 1, a);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, phasor_delete, 0, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, phasor_put, 1, a);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, phasor_number, 1, a);

  a[0] = fts_s_float;
  fts_method_define(cl, 0, fts_s_float, phasor_number, 1, a);

  fts_method_define(cl, 0, fts_s_print, phasor_print, 0, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  phasor_function = fts_new_symbol("phasor");
  dsp_declare_function(phasor_function, ftl_phasor);

  return fts_Success;
}

void
phasor_config(void)
{
  fts_s_print = fts_new_symbol("print");

  fts_metaclass_create(fts_new_symbol("phasor~"),phasor_instantiate, fts_always_equiv);
}
