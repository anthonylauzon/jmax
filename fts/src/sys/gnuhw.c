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

static fts_welcome_t gnu_welcome = {"GNU generic version\n"};
static int running_real_time = 1;
struct timespec pause_time = { 0, 10000};


void fts_platform_init(void)
{
  fts_add_welcome(&gnu_welcome);

  if (running_real_time)
    {
      if (setpriority(PRIO_PROCESS, 0, -20) < 0)
	{
	  running_real_time = 0;
	}
    }

  /* Get rid of root privilege if we have them */
  if (setreuid(geteuid(), getuid()) == -1)
    fprintf( stderr, "setreuid failed (%s) continuing\n", strerror(errno));
}

void fts_set_no_real_time()
{
  running_real_time = 0;
}

void fts_pause(void)
{
  if ( running_real_time)
    nanosleep( &pause_time, 0);
}

int fts_memory_is_locked()
{
  return 0;
}

void fts_unlock_memory()
{
}

/* API to catch the exceptions */


void fts_set_fpe_handler(fts_fpe_handler fh)
{
}

void fts_reset_fpe_handler(void)
{
}

unsigned int fts_check_fpe(void)
{
  return 0;
}
