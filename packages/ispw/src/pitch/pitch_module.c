#include "fts.h"

extern void pitch_config(void);
extern void pt_config(void);

static void
pitch_module_init(void)
{
  pitch_config();
  pt_config();
}

fts_module_t pitch_module = {"pitch", "ISPW pitch analysis classes", pitch_module_init};

