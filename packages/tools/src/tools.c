#include "fts.h"

/* extern void profiler_config(void); */
static void tools_init(void)
{
  /* profiler_config(); */
}

fts_module_t tools_module = { "Tools", "Tools module", tools_init};
