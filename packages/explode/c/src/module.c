#include "fts.h"

extern void explay_config(void);
extern void explode_config(void);

static void
fts_explode_init(void)
{
  explay_config();
  explode_config();
}

fts_module_t explode_module = {"explode", "explode and explay scorfollowing/sequencing classes", fts_explode_init};
