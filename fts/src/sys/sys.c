/*
  Sys level initialization 
  */

#include "sys.h"

extern void fts_platform_init(void);

void
fts_sys_init(void)
{
  fts_platform_init();

  mem_init();
}
