/* vecmod.c */

#include "vecmod.h"

extern void ftl_functions_init(void);
extern void ftl_cheap_init(void);

static void fts_veclib_init(void)
{
  ftl_functions_init(); /* first this!! */
}

fts_module_t fts_veclib_module =
  {"VecLib", "The vector function library", fts_veclib_init, 0};
