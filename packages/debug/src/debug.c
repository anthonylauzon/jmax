#include "fts.h"

extern void cheese_config(void);

/* extern void whereiam_config(void); */

static void
debug_init(void)
{
  cheese_config();
}

fts_module_t debug_module = {"debug", "Debug oriented objects", debug_init};

