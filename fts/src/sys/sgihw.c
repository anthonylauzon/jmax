/*
   Platform initialization for the SGI platform.
*/

#include <errno.h>

#include <sys/fcntl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/syssgi.h>
#include <sys/immu.h>
#include <signal.h>
#include <sys/lock.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/schedctl.h>
#include <sys/fpu.h>

#include "sys.h"

#ifdef IRIX6_4
#include <sched.h>
#endif


static fts_welcome_t  sgi_welcome = {"SGI platform\n"};
static int running_high_priority = 0;
struct timespec pause_time;

/*
  To avoid loosing (a lot of) time with underflows:

  set the special "flush zero" but (FS, bit 24) in the
  Control Status Register of the FPU of R4k and beyond
  so that the result of any underflowing operation will
  be clamped to zero, and no exception of any kind will
  be generated on the CPU.  
  */


static void
flush_all_underflows_to_zero(void)
{
  union fpc_csr f;
  f.fc_word = get_fpc_csr();
  f.fc_struct.flush = 1;
  
  /* f.fc_struct.en_invalid = 0; */
  /* f.fc_struct.en_divide0 = 0; */
  /* f.fc_struct.en_overflow = 0; */
  /* f.fc_struct.en_underflow = 0; */
  /* f.fc_struct.en_inexact = 0; */

  set_fpc_csr(f.fc_word);
}


void
fts_platform_init(void)
{
  fts_add_welcome(&sgi_welcome);

  flush_all_underflows_to_zero();


  /* Lock to physical memory, if we can */

  plock(PROCLOCK);

  /* raise priority to a high value */

#ifdef IRIX6_4
  {
    struct sched_param param;
  
    param.sched_priority  = sched_get_priority_max(SCHED_FIFO);

      if (sched_setscheduler(0, SCHED_FIFO, &param) < 0)
	running_high_priority = 0;
      else
	running_high_priority = 1;
  }

#else

  if (schedctl( NDPRI,  0,  NDPNORMMAX) < 0)
    running_high_priority = 0;
  else
    running_high_priority = 1;

#endif

  /* Get rid of root privilege if we have them  */

  setreuid(getuid(), getuid());

  pause_time.tv_sec = 0;
  pause_time.tv_nsec = 100000;
}



void fts_pause(void)
{
  if (running_high_priority)
    nanosleep(&pause_time, 0);
}





