#include "fts.h"

/* The Cheese object print a signal vector (like print~), 
   but print the vector automatically only when there are NaN (Cheese Nan) in the
   input vector.

   To avoid too many prints, cheese take an argument, that are the number
   of vectors that cheese can print, and add to this number any int that
   is sent to its inlet; default to 16.

   It have a first optional symbol argument, used to differentiate the cheese
   print.
   */


typedef struct cheese_t
{
  fts_object_t _o;
  int n_print;	
  fts_symbol_t sym;
} cheese_t;

static fts_symbol_t cheese_dsp_function = 0;

static void cheese_init(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cheese_t *this = ((cheese_t *)o);

  this->sym = fts_get_symbol_arg(ac, at, 1, fts_new_symbol(""));
  this->n_print = fts_get_long_arg(ac, at, 2, 16);

  dsp_list_insert(o);
}

static void cheese_delete(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cheese_t *this = ((cheese_t *)o);

  dsp_list_remove(o);
}

static void cheese_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cheese_t *this = ((cheese_t *)o);

  this->n_print = this->n_print + fts_get_long(at);
}

static void ftl_cheese(fts_word_t *argv)
{
  float *in      = (float *)fts_word_get_ptr(argv);
  cheese_t *this = (cheese_t *)fts_word_get_ptr(argv + 1);
  long vs        = fts_word_get_long(argv + 2);
  int i;

  for (i = 0; i < vs; i++)
    if (isnanf(in[i]))
      if (this->n_print > 0)
	{
	  this->n_print--;

	  if (this->sym)
	    post("%s:\n", fts_symbol_name(this->sym));

	  post_vector(vs, in);

	  return;
	}
}

static void cheese_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  cheese_t *this = ((cheese_t *)o);
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  int vs = fts_dsp_get_input_size(dsp, 0);
  
  fts_set_symbol(argv, fts_dsp_get_input_name(dsp, 0));
  fts_set_ptr(argv + 1, o);
  fts_set_long(argv + 2, vs);
  dsp_add_funcall(cheese_dsp_function, 3, argv);
}

static fts_status_t cheese_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[3];

  fts_class_init(cl, sizeof(cheese_t), 1, 0, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  a[2] = fts_s_int;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, cheese_init, 3, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, cheese_delete, 0, a);

  a[0] = fts_s_ptr;
  fts_method_define(cl, fts_SystemInlet, fts_s_put, cheese_put, 1, a);


  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, cheese_int, 1, a);

  cheese_dsp_function = fts_new_symbol("cheese");
  dsp_declare_function(cheese_dsp_function, ftl_cheese);

  dsp_sig_inlet(cl, 0);
  return fts_Success;
}

void
cheese_config(void)
{
  fts_metaclass_create(fts_new_symbol("cheese~"),cheese_instantiate, fts_always_equiv);
}
