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
#include <sigfpe.h>

#include "sys.h"

#ifdef IRIX6_4
#include <sched.h>
#endif

extern void post( const char *format, ...);

static fts_welcome_t  sgi_welcome = {"SGI platform\n"};
static int running_real_time = 1;
static int memory_is_locked = 0;
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
init_fpe(void)
{
  union fpc_csr f;
  f.fc_word = get_fpc_csr();
  f.fc_struct.flush = 1;
  
  /* Enable all the exceptions by default */

  f.fc_struct.en_invalid = 0;
  f.fc_struct.en_divide0 = 0;
  f.fc_struct.en_overflow = 0;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);
}

static void
enable_fpes(void)
{
  union fpc_csr f;
  f.fc_word = get_fpc_csr();
  f.fc_struct.flush = 1;
  
  /* Enable all the exceptions by default */

  f.fc_struct.en_invalid = 1;
  f.fc_struct.en_divide0 = 1;
  f.fc_struct.en_overflow = 1;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);
}



void
fts_platform_init(void)
{
  fts_add_welcome(&sgi_welcome);

  init_fpe();

#ifdef IRIX6_4
  if (running_real_time)
    {
      struct sched_param param;
  
      /* raise priority to a high value */

      param.sched_priority  = sched_get_priority_max(SCHED_FIFO);

      if (sched_setscheduler(0, SCHED_FIFO, &param) < 0)
	{
	  running_real_time = 0;
	  memory_is_locked = 0;
	}
      else
	{
	  /* Iff we are running real time, lock to physical memory, if we can */

	  if (mlockall(MCL_CURRENT | MCL_FUTURE) == 0)
	    {
	      memory_is_locked = 1;
	    }
	  else
	    {
	      /* ??? */

	      post("Cannot lock memory: %s\n", strerror (errno));
	    }
	}
    }
#else
  if (running_real_time)
    {
      /* Lock to physical memory, if we can */

      if (plock( PROCLOCK) == 0)
	memory_is_locked = 1;
      else
	{
	  if (errno != EPERM)
	    post("Cannot lock memory: %s\n", strerror (errno));
	}

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

int fts_memory_is_locked()
{
  return memory_is_locked;
}

void fts_unlock_memory()
{
  munlockall();
  memory_is_locked = 0;
}

/* API to catch the exceptions */

static fts_fpe_handler fpe_handler = 0;

/* Sgi man page tell to use this not ANSI prototype !! */

int fts_sgi_fpe_handler( sig, code, sc )
     int sig, code;
     struct sigcontext *sc;
{
  if (fpe_handler)
    {
      int exception = __fpe_trap_type();

      switch (exception)
	{
	case _OVERFL:
	  (* fpe_handler)(FTS_OVERFLOW_FPE);
	  break;
	case _UNDERFL:
	  (* fpe_handler)(FTS_UNDERFLOW_FPE);
	  break;
	case _DIVZERO:
	  (* fpe_handler)(FTS_DIVIDE0_FPE);
	  break;
	case _INVALID:
	  (* fpe_handler)(FTS_INVALID_FPE);
	  break;
	default:
	  (* fpe_handler)(0);
	  break;
	}
    }

  return 0;
}


void fts_set_fpe_handler(fts_fpe_handler fh)
{
  enable_fpes();
  fpe_handler = fh;

  handle_sigfpes (_ON, _EN_OVERFL | _EN_DIVZERO | _EN_INVALID, 0,
		  _USER_HANDLER,  fts_sgi_fpe_handler);
}

void fts_reset_fpe_handler()
{
  handle_sigfpes(_OFF, 0, 0, _USER_HANDLER, 0);
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


