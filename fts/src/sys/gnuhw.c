/*
   Platform initialization the GNU platform.
*/

#include "sys.h"

/*****************************************************************************/
/*                                                                           */
/*               Hardware configuration functions                            */
/*                                                                           */
/*****************************************************************************/

static fts_welcome_t  gnu_welcome = {"GNU generic version\n"};



void
fts_platform_init(void)
{
  fts_add_welcome(&gnu_welcome);
}

void fts_pause(void)
{
  // ???
}
