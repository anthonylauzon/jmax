
#include "fts.h"


typedef struct {
  fts_object_t o;
} dsp_t;

static void
dsp_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if (dsp_is_running())
    dsp_chain_delete();

  dsp_chain_create(fts_get_long_arg(ac, at, 0, DEFAULTVS));
}

static void
dsp_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_delete();
}

static void
dsp_on_off(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  if(fts_get_long_arg(ac, at, 0, 0))
    {
      if (dsp_is_running())
	dsp_chain_delete();

      dsp_chain_create(DEFAULTVS);
    }
  else
    dsp_chain_delete();
}

static void
dsp_print(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post();
}

static void
dsp_save(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  FILE *f;
  
  if (fts_is_symbol(at))
    {
      const char *filename;

      filename = fts_symbol_name(fts_get_symbol(at));

      f = fopen(filename, "w");

      if (f)
	dsp_chain_fprint(f);

      fclose(f);
    }
}

static void
dsp_print_signals(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  dsp_chain_post_signals();
}

static fts_status_t
dsp_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(dsp_t), 1, 0, 0);

  a[0] = fts_s_int;
  fts_method_define_optargs(cl, 0, fts_new_symbol("start"), dsp_start, 1, a, 0);

  fts_method_define(cl, 0, fts_new_symbol("stop"), dsp_stop, 0, a);
 
  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_s_int, dsp_on_off, 1, a);
  
  fts_method_define(cl, 0, fts_s_bang, dsp_print, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("print"), dsp_print, 0, 0);

  a[0] = fts_s_symbol;
  fts_method_define(cl, 0, fts_new_symbol("save"), dsp_save, 1, a);

  fts_method_define(cl, 0, fts_new_symbol("print-signals"), dsp_print_signals, 0, 0);
  
  return fts_Success;
}

void
dsp_config(void)
{
  fts_metaclass_create(fts_new_symbol("dsp"),dsp_instantiate, fts_always_equiv);
  fts_metaclass_alias(fts_new_symbol("dsp~"), fts_new_symbol("dsp"));
}

