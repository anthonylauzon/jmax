#include "fts.h"

extern void ucs_config(void);
extern void dsp_config(void);
extern void arch_config(void);

static void
system_module_init(void)
{
  ucs_config();
  dsp_config();
  arch_config();
}

fts_module_t system_module = {"system", "Basic System Objects", system_module_init};
