/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/*
 * This file contains SGI platform dependent functions:
 *  - dynamic loader
 *  - real-time: scheduling mode and priority, memory locking
 *  - FPU settings
 */

#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/sysmp.h>
#include <sys/sysinfo.h>
#include <sys/pda.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/fpu.h>
#include <sigfpe.h>
#include <sys/lock.h>
#include <sys/mman.h>
#include <sched.h>

#include <fts/sys.h>


/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

/* The SGI implementation uses dlopen() and dlsym(). */

void *fts_dl_open( const char *filename, char *error)
{
  void *handle;

  handle = dlopen( filename, RTLD_NOW | RTLD_GLOBAL);

  if (!handle)
    strcpy( error, dlerror());

  return handle;
}

int fts_dl_lookup( void *handle, const char *symbol, void **address, char *error)
{
  void *p;
  char *dlerr;

  p = dlsym( handle, symbol);

  dlerr = dlerror();
  if (dlerr != NULL)
    {
      strcpy( error, dlerr);
      return 0;
    }

  *address = p;
  
  return 1;
}

/* *************************************************************************** */
/*                                                                             */
/* Floating-point unit                                                         */
/*                                                                             */
/* *************************************************************************** */

/*
  To avoid loosing (a lot of) time with underflows:

  set the special "flush zero" but (FS, bit 24) in the
  Control Status Register of the FPU of R4k and beyond
  so that the result of any underflowing operation will
  be clamped to zero, and no exception of any kind will
  be generated on the CPU.  
  */


static void sgi_init_fpu( void)
{
  union fpc_csr f;

  f.fc_word = get_fpc_csr();

  f.fc_struct.flush = 1;
  
  /* Disable all the exceptions by default */

  f.fc_struct.en_invalid = 0;
  f.fc_struct.en_divide0 = 0;
  f.fc_struct.en_overflow = 0;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);
}

/* Sgi man page tell to use this not ANSI prototype !! */
int sgi_fpe_signal_handler( sig, code, sc )
     int sig, code;
     struct sigcontext *sc;
{
  switch (__fpe_trap_type()) {
  case _OVERFL:
    fts_fpe_handler( FTS_OVERFLOW_FPE);
    break;
  case _UNDERFL:
    fts_fpe_handler( FTS_UNDERFLOW_FPE);
    break;
  case _DIVZERO:
    fts_fpe_handler( FTS_DIVIDE0_FPE);
    break;
  case _INVALID:
    fts_fpe_handler( FTS_INVALID_FPE);
    break;
  default:
    fts_fpe_handler( 0);
    break;
  }

  return 0;
}

void fts_enable_fpe_traps( void)
{
  union fpc_csr f;

  f.fc_word = get_fpc_csr();

  f.fc_struct.flush = 1;
  
  /* Enable all the exceptions except inexact and underflow */

  f.fc_struct.en_invalid = 1;
  f.fc_struct.en_divide0 = 1;
  f.fc_struct.en_overflow = 1;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);

  handle_sigfpes (_ON, _EN_OVERFL | _EN_DIVZERO | _EN_INVALID, 0,
		  _USER_HANDLER,  sgi_fpe_signal_handler);
}

void fts_disable_fpe_traps( void)
{
  union fpc_csr f;

  f.fc_word = get_fpc_csr();
  f.fc_struct.flush = 1;
  
  /* Disable all the exceptions */

  f.fc_struct.en_invalid = 0;
  f.fc_struct.en_divide0 = 0;
  f.fc_struct.en_overflow = 0;
  f.fc_struct.en_underflow = 0;
  f.fc_struct.en_inexact = 0;

  set_fpc_csr(f.fc_word);

  handle_sigfpes(_OFF, 0, 0, _USER_HANDLER, 0);
}

unsigned int fts_check_fpe( void)
{
  union fpc_csr f;
  unsigned int ret = 0;

  f.fc_word = get_fpc_csr();

  if (f.fc_struct.se_invalid)
    ret |= FTS_INVALID_FPE;

  if (f.fc_struct.se_divide0)
    ret |= FTS_DIVIDE0_FPE;

  if (f.fc_struct.se_overflow)
    ret |= FTS_OVERFLOW_FPE;

  /* put the flags to zero anyway */

  f.fc_struct.se_invalid = 0;
  f.fc_struct.se_divide0 = 0;
  f.fc_struct.se_overflow = 0;

  set_fpc_csr(f.fc_word);

  return ret;
}

/* *************************************************************************** */
/*                                                                             */
/* Memory locking                                                              */
/*                                                                             */
/* *************************************************************************** */

static int memory_is_locked = 0;

int fts_unlock_memory( void)
{
  if (memory_is_locked)
    {
      munlockall();
      memory_is_locked = 0;

      return 1;
    }

  return 0;
}


/* *************************************************************************** */
/*                                                                             */
/* Platform specific initialization                                            */
/*                                                                             */
/* *************************************************************************** */

static int run_on_cpu( int cpu)
{
  if (sysmp( MP_MUSTRUN, cpu) < 0)
    {
      post( "sysmp( MP_MUSTRUN, %d) failed [%d, \"%s\"]\n", cpu, strerror( errno));
      return -1;
    }

  return 0;
}

static int find_isolated_cpu( void)
{
  struct pda_stat *p;
  int n_processors, n;

  n_processors = sysmp( MP_NPROCS);
  if ( n_processors < 0)
    {
      post( "sysmp( MP_NPROCS) failed [%d, \"%s\"]\n", strerror( errno));
      return -1;
    }

  p = (struct pda_stat *)malloc( n_processors * sizeof( struct pda_stat));
  if ( sysmp( MP_STAT, p) < 0)
    {
      post( "sysmp( MP_STAT) failed [%d, \"%s\"]\n", strerror( errno));
      return -1;
    }

  for ( n = 1; n < n_processors; n++)
    {
      if (p[n].p_flags & PDAF_ISOLATED)
	return n;
    }

  return 0;
}

static int get_cpu_from_command_line( int argc, char **argv, int *pcpu)
{
  while ( argc)
    {
      if ( ! strncmp( *argv, "--cpu=", 6))
	{
	  char *p = strchr( *argv, '=') + 1;

	  if (sscanf( p, "%d", pcpu) == 1)
	    return 1;
	  else
	    {
	      post( "Invalid CPU number: %s\n", p);
	      return -1;
	    }
	}
      
      argc--;
      argv++;
    }

  return 0;
}

void fts_platform_init( int argc, char **argv)
{
  struct sched_param param;
  
  int cpu = -1;
  int r;

  r = get_cpu_from_command_line( argc, argv, &cpu);

  if ( r == 1)
    {
      if ( run_on_cpu( cpu) >= 0)
	post( "Running on CPU %d\n", cpu);
    }
  else if ( r == 0)
    {
      cpu = find_isolated_cpu();

      if (cpu > 0 && run_on_cpu( cpu) >= 0)
	post( "Running on isolated CPU %d\n", cpu);
    }

  /* raise priority to a high value */
  param.sched_priority  = 199;

  if (sched_setscheduler( 0, SCHED_FIFO, &param) < 0)
    {
      post( "Warning: cannot switch to real-time scheduling mode.\n");
      post( "         Real-time performance will be degraded resulting in audio clicks.\n");
      post( "         This warning result from installation problems (jMax server not owned by root).\n");
    }

  /* Iff we are running real time, lock to physical memory, if we can */
  if (mlockall(MCL_CURRENT | MCL_FUTURE) == 0)
    {
      memory_is_locked = 1;
    }
  else
    {
      post("Warning: cannot lock memory (error \"%s\")\n", strerror (errno));
    }

  /* Get rid of root privilege if we have them */
  setreuid(getuid(), getuid());

  sgi_init_fpu();
}
