/*
   Platform initialization the GNU platform.
*/

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "sys.h"

/*****************************************************************************/
/*                                                                           */
/*               Hardware configuration functions                            */
/*                                                                           */
/*****************************************************************************/

static fts_welcome_t  gnu_welcome = {"GNU generic version\n"};
static int running_high_priority = 0;
struct timespec pause_time = { 0, 10000};


void
fts_platform_init(void)
{
  fts_add_welcome(&gnu_welcome);

  if (setpriority(PRIO_PROCESS, 0, -20) < 0)
    {
      fprintf( stderr, "fts: cannot set priority (%s) using normal priority\n", strerror(errno));
      running_high_priority = 0;
    }
  else
    running_high_priority = 1;

  /* Get rid of root privilege if we have them */
  if (setreuid(geteuid(), getuid()) == -1)
    fprintf( stderr, "setreuid failed (%s) continuing\n", strerror(errno));
}

void fts_pause(void)
{
  if ( running_high_priority)
    nanosleep( &pause_time, 0);
}

unsigned int fts_check_fpe(void)
{
  return 0;
}
