/*
   Platform initialization for the SGI platform.
*/

#include <stdio.h>
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
static int running_real_time = 1;
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
  
  f.fc_struct.en_invalid = 0;
  f.fc_struct.en_divide0 = 0;
  f.fc_struct.en_overflow = 0;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);
}



void
fts_platform_init(void)
{
  fts_add_welcome(&sgi_welcome);

  flush_all_underflows_to_zero();

#ifdef IRIX6_4
  if (running_real_time)
    {
      struct sched_param param;
  
      /* Lock to physical memory, if we can */

      plock( PROCLOCK);

      /* raise priority to a high value */

      param.sched_priority  = sched_get_priority_max(SCHED_FIFO);

      if (sched_setscheduler(0, SCHED_FIFO, &param) < 0)
	running_real_time = 0;
    }
#else
  if (running_real_time)
    {
      /* Lock to physical memory, if we can */

      plock( PROCLOCK);

      /* raise priority to a high value */

      if (schedctl( NDPRI,  0,  NDPNORMMAX) < 0)
	running_real_time = 0;
    }
#endif

  /* Get rid of root privilege if we have them  */

  setreuid( getuid(), getuid());

  pause_time.tv_sec = 0;
  pause_time.tv_nsec = 100000;
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


/* Return non zero if we got 
   fpe since the last call; first call,
   return zero.
 */

static int done_once = 0;

unsigned int fts_check_fpe(void)
{
  unsigned int ret = 0;

  union fpc_csr f;
  f.fc_word = get_fpc_csr();

  if (done_once)
    {
      if (f.fc_struct.se_invalid)
	ret |= FTS_INVALID_FPE;
      
      if (f.fc_struct.se_divide0)
	ret |= FTS_DIVIDE0_FPE;

      if (f.fc_struct.se_overflow)
	ret |= FTS_OVERFLOW_FPE;
    }

  /* put the flags to zero anyway */

  f.fc_struct.se_invalid = 0;
  f.fc_struct.se_divide0 = 0;
  f.fc_struct.se_overflow = 0;

  /* f.fc_struct.se_inexact = 0; */

  set_fpc_csr(f.fc_word);

  done_once = 1;

  return ret;
}


