/* sigthrow.c */

#include <string.h>

#include "fts.h"




/* --------------------------- sigcatch~ ---------------------------- */

static fts_hash_table_t catch_table;
static fts_symbol_t fts_s_print;

#define MAXVS 64

static fts_symbol_t sigcatch_function = 0;

#ifdef HI_OPT
static fts_symbol_t sigcatch_64_function = 0;
#endif

typedef struct sigcatch_t sigcatch_t;

struct sigcatch_t
{
  fts_object_t _o;
  fts_symbol_t sym;

  float samps[MAXVS];
};

#ifdef HI_OPT
static void
sigcatch_64_dsp_fun(fts_word_t *argv)
{
  float *out = (float *)fts_word_get_ptr(argv);
  float *buf = (float *)fts_word_get_ptr(argv+1);
  int i;

  /* #pragma ivdep   */

  for (i = 0; i < 64; i++)
    {
      out[i] = buf[i];
      buf[i] = 0;
    }
}
#endif

static void
sigcatch_dsp_fun(fts_word_t *argv)
{
  fts_vecx_fcpy(fts_word_get_ptr(argv+1),
		(float *)fts_word_get_ptr(argv), 
		fts_word_get_long(argv+2));

  fts_vecx_fzero((float *)fts_word_get_ptr(argv+1), fts_word_get_long(argv+2));
}

static void
sigcatch_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);

  fts_vecx_fzero(this->samps, MAXVS);
#ifdef HI_OPT

  if (fts_dsp_get_output_size(dsp, 0) == 64)
    {
      fts_set_symbol(argv,   fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr   (argv+1, this->samps);

      dsp_add_funcall(sigcatch_64_function, 2, argv);
    }
  else
    {
      fts_set_symbol(argv,   fts_dsp_get_output_name(dsp, 0));
      fts_set_ptr   (argv+1, this->samps);
      fts_set_long  (argv+2, fts_dsp_get_output_size(dsp, 0));

      dsp_add_funcall(sigcatch_function, 3, argv);
    }
#else
  fts_set_symbol(argv,   fts_dsp_get_output_name(dsp, 0));
  fts_set_ptr   (argv+1, this->samps);
  fts_set_long  (argv+2, fts_dsp_get_output_size(dsp, 0));

  dsp_add_funcall(sigcatch_function, 3, argv);
#endif
}

static void
sigcatch_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o, *sigc;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 1, 0);

  if ((s == 0) || (*(fts_symbol_name(s)) == '0'))
    s = fts_new_symbol("sigcatch~");

  this->sym = s;

  if (fts_hash_table_lookup(&catch_table, this->sym, (void **)&sigc))
    post("catch~: duplicated name: %s\n", fts_symbol_name(this->sym));
  else
    fts_hash_table_insert(&catch_table, this->sym, (void *)this);

  dsp_list_insert(o);
}

static void
sigcatch_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigcatch_t *this = (sigcatch_t *)o, *sigc;
  fts_symbol_t s = this->sym;

  if (fts_hash_table_lookup(&catch_table, s, (void **)&sigc))
    fts_hash_table_remove(&catch_table, s);

  dsp_list_remove(o);
}

static fts_status_t
sigcatch_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigcatch_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigcatch_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigcatch_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigcatch_put, 1, a);
  
  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);
  
  sigcatch_function = fts_new_symbol("sigcatch");
  dsp_declare_function(sigcatch_function, sigcatch_dsp_fun);

#ifdef HI_OPT
  sigcatch_64_function = fts_new_symbol("sigcatch_64");
  dsp_declare_function(sigcatch_64_function, sigcatch_64_dsp_fun);
#endif
  
  return fts_Success;
}

void
sigcatch_config(void)
{
  fts_hash_table_init(&catch_table);

  fts_metaclass_create(fts_new_symbol("catch~"),sigcatch_instantiate, fts_always_equiv);
}

/* --------------------------- sigthrow~ ---------------------------- */

static fts_symbol_t sigthrow_function = 0;

#ifdef HI_OPT
static fts_symbol_t sigthrow_64_function = 0;
#endif

typedef struct
{
  fts_object_t _o;
  fts_symbol_t sym;
  ftl_data_t ftl_data;
  int rec_prot;			/* housekeeping for dead code elimination */
} sigthrow_t;


static void
sigthrow_dsp_fun(fts_word_t *argv)
{
  float *p;

  p = *((float **)fts_word_get_ptr(argv+1));

  if(p)
    fts_vecx_fadd((float *)fts_word_get_ptr(argv), p, p, fts_word_get_long(argv+2));
}

#ifdef HI_OPT
static void
sigthrow_dsp_64_fun(fts_word_t *argv)
{
  float *out;

  out = *((float **)fts_word_get_ptr(argv+1));


  if (out)
    {
      float *in = (float *)fts_word_get_ptr(argv+0);
      int i;

      /* #pragma ivdep */

      for (i = 0; i < 64; i+=8)
	{
	  float f0, f1, f2, f3, f4, f5, f6, f7;

	  f0 = in[i + 0];
	  f1 = in[i + 1];
	  f2 = in[i + 2];
	  f3 = in[i + 3];
	  f4 = in[i + 4];
	  f5 = in[i + 5];
	  f6 = in[i + 6];
	  f7 = in[i + 7];

	  out[i + 0] = out[i + 0] + f0;
	  out[i + 1] = out[i + 1] + f1;
	  out[i + 2] = out[i + 2] + f2;
	  out[i + 3] = out[i + 3] + f3;
	  out[i + 4] = out[i + 4] + f4;
	  out[i + 5] = out[i + 5] + f5;
	  out[i + 6] = out[i + 6] + f6;
	  out[i + 7] = out[i + 7] + f7;
	}
    }
}
#endif

static void
sigthrow_put(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_atom_t argv[3];
  fts_dsp_descr_t *dsp = (fts_dsp_descr_t *)fts_get_ptr_arg(ac, at, 0, 0);
  sigcatch_t *sigc;

  /* look here for the corresponing catch buffer, to eliminate
     instantiation order dependency between catch~ and throw~.
   */

  if (fts_hash_table_lookup(&catch_table, this->sym, (void **)&sigc))
    {
      float *samps = sigc->samps;

      ftl_data_copy(float *, this->ftl_data, &samps);
    }
  else
    {
      const float *zerop = 0;

      ftl_data_copy(float *, this->ftl_data, &zerop);
    }

  fts_set_symbol(argv,   fts_dsp_get_input_name(dsp, 0));
  fts_set_ftl_data   (argv+1, this->ftl_data);

#ifdef HI_OPT
  if (fts_dsp_get_input_size(dsp, 0) == 64)
    dsp_add_funcall(sigthrow_64_function, 2, argv);
  else
    {
      fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0));
      dsp_add_funcall(sigthrow_function, 3, argv);
    }
#else
  fts_set_long  (argv+2, fts_dsp_get_input_size(dsp, 0));
  dsp_add_funcall(sigthrow_function, 3, argv);
#endif
}

static void
sigthrow_set(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);
  sigthrow_t *this = (sigthrow_t *)o;
  sigcatch_t *c;

  this->sym = s;

  if (fts_hash_table_lookup(&catch_table, s, (void **)&c))
    {
      float *samps = c->samps;

      ftl_data_copy(float *, this->ftl_data, &samps);
    }
  else
    {
      const float *zerop = 0;

      ftl_data_copy(float *, this->ftl_data, &zerop);
    }
}

static void
sigthrow_print(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 0, 0);

  if (!s)
    s = fts_new_symbol("throw~");

  post("%s: %s\n", fts_symbol_name(s), fts_symbol_name(this->sym));
}

static void
sigthrow_int(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  char buf[30];
  fts_atom_t a;

  gensampname(buf, "catch", fts_get_long(at));
  fts_set_symbol(&a, fts_new_symbol_copy(buf));
  sigthrow_set(o, winlet, s, 1, &a);
}

static void
sigthrow_init(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;
  fts_symbol_t s = fts_get_symbol_arg(ac, at, 1, 0);

  this->sym = s;
  this->rec_prot = 0;
  this->ftl_data = ftl_data_new(float *);

  dsp_list_insert(o);
}

static void
sigthrow_delete(fts_object_t *o, int winlet, fts_symbol_t is, int ac, const fts_atom_t *at)
{
  sigthrow_t *this = (sigthrow_t *)o;

  ftl_data_free(this->ftl_data);

  dsp_list_remove(o);
}

/* Daemon to know if throw is a dsp sink; it look for the throw, and
   ask him */

#ifdef _NOT_YET_IMPLEMENTED
static fts_atom_t *
throw_is_dsp_sink_get_daemon(fts_object_t *obj, fts_symbol_t property)
{
  sigthrow_t *this = (sigthrow_t *)obj;

  if (this->rec_prot)
    return 0;			/* recursive search protection */
  else
    {
      sigcatch_t *sigc;

      if (fts_hash_table_lookup(&catch_table, this->sym, (void **)&sigc))
	{
	  fts_atom_t *a;

	  this->rec_prot = 1;
	  a =  fts_object_get_prop((fts_object_t *) sigc, property);
	  this->rec_prot = 0;

	  return a;
	}
      else
	return 0;
    }
}
#endif

static fts_status_t
sigthrow_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[2];

  fts_class_init(cl, sizeof(sigthrow_t), 1, 1, 0);

  a[0] = fts_s_symbol;
  a[1] = fts_s_symbol;
  fts_method_define_optargs(cl, fts_SystemInlet, fts_s_init, sigthrow_init, 2, a, 1);

  fts_method_define(cl, fts_SystemInlet, fts_s_delete, sigthrow_delete, 0, a);

  a[0] = fts_s_ptr;  
  fts_method_define(cl, fts_SystemInlet, fts_s_put, sigthrow_put, 1, a);

  a[0] = fts_s_symbol;
  fts_method_define_optargs(cl, 0, fts_s_print, sigthrow_print, 1, a, 0);
  
  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_s_set, sigthrow_set, 1, a);
  
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, sigthrow_int, 1, a);

  dsp_sig_inlet(cl, 0);
  dsp_sig_outlet(cl, 0);

  /* Class DSP properties; to know if a throw is a dsp sink, we look for the
     catch, and ask him: do not work with dinamic dispatching of the throw;
     we can use it only if we know that the throw do not switch targets
     dynamically ... we can know it by testing the existance of a control input.
     */

  /* fts_class_add_get_daemon(cl, fts_s_dsp_is_sink, throw_is_dsp_sink_get_daemon); */

  sigthrow_function = fts_new_symbol("sigthrow");
  dsp_declare_function(sigthrow_function, sigthrow_dsp_fun);

#ifdef HI_OPT
  sigthrow_64_function = fts_new_symbol("sigthrow_64");
  dsp_declare_function(sigthrow_64_function, sigthrow_dsp_64_fun);
#endif

  return fts_Success;
}

void
sigthrow_config(void)
{
  fts_metaclass_create(fts_new_symbol("throw~"),sigthrow_instantiate, fts_always_equiv);

  fts_s_print = fts_new_symbol("print");
}





