#include "fts.h"

static void profiler_start(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftl_program_start_profiler( dsp_get_current_dsp_chain());
}

static void profiler_stop(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftl_program_stop_profiler( dsp_get_current_dsp_chain());
}

static void profiler_clear(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftl_program_stop_profiler( dsp_get_current_dsp_chain());
  ftl_program_clear_profile_data( dsp_get_current_dsp_chain());
}

static void profiler_show_by_class(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
  ftl_program_show_profile_by_class( dsp_get_current_dsp_chain());
}

static void profiler_set_period(fts_object_t *o, int winlet, fts_symbol_t s, int ac, const fts_atom_t *at)
{
}

static fts_status_t profiler_instantiate(fts_class_t *cl, int ac, const fts_atom_t *at)
{
  fts_symbol_t a[1];

  fts_class_init(cl, sizeof(fts_object_t), 1, 0, 0);

  fts_method_define(cl, 0, fts_new_symbol("start"), profiler_start, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("stop"), profiler_stop, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("clear"), profiler_clear, 0, 0);
  fts_method_define(cl, 0, fts_new_symbol("show"), profiler_show_by_class, 0, 0);

  a[0] = fts_s_int;
  fts_method_define(cl, 0, fts_new_symbol("period"), profiler_set_period, 1, a);

  return fts_Success;
}

void profiler_config( void)
{
  fts_metaclass_create( fts_new_symbol("profiler"),profiler_instantiate, fts_always_equiv);
}
