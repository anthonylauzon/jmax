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
 * This file contains Linux (Intel compatible processors) platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dlfcn.h>
#include <sched.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include <fts/sys.h>


/* *************************************************************************** */
/*                                                                             */
/* Dynamic loader                                                              */
/*                                                                             */
/* *************************************************************************** */

/* The Linux implementation uses dlopen() and dlsym(). */

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
  char *dlerr;

  *address = dlsym( handle, symbol);

  dlerr = dlerror();
  if (dlerr != NULL)
    {
      strcpy( error, dlerr);
      return 0;
    }

  return 1;
}

/* *************************************************************************** */
/*                                                                             */
/* Floating-point unit                                                         */
/*                                                                             */
/* *************************************************************************** */

/*
 * #define this if you want denormalized f.p. traps to be reported 
 */
#undef ENABLE_DENORMALIZED_TRAPS


/* (fd) HACK !!! */
extern void fts_fpe_handler( int which);

/* (Francois, dechelle@ircam.fr                                                */
/* As Linux does not provide (yet?) a way to access the status of the          */
/* floating-point unit, it is done by hand using __asm__. Great ;-(            */

/* --------------------------------------------------------------------------- */
/* Pentium FPU Control Word                                                    */
/* --------------------------------------------------------------------------- */

/* Exception masks */
#define _FPU_CONTROL_IM   0x001  /* Invalid Operation */
#define _FPU_CONTROL_DM   0x002  /* Denormalized Operand */
#define _FPU_CONTROL_ZM   0x004  /* Zero Divide */
#define _FPU_CONTROL_OM   0x008  /* Overflow */
#define _FPU_CONTROL_UM   0x010  /* Underflow */
#define _FPU_CONTROL_PM   0x020  /* Precision */

/* Precision control */
#define _FPU_CONTROL_SP   0x000  /* Single Precision */
#define _FPU_CONTROL_DP   0x200  /* Double Precision */
#define _FPU_CONTROL_EP   0x300  /* Extended Precision */

/* Rounding control */
#define _FPU_CONTROL_NR   0x000  /* Nearest */
#define _FPU_CONTROL_DW   0x400  /* Down */
#define _FPU_CONTROL_UP   0x800  /* Up */
#define _FPU_CONTROL_ZR   0x800  /* Zero */

/* Macros for accessing the FPU control word.  */
/* get the FPU control word */
#define _FPU_GET_CW(st) __asm__ ("fnstcw %0" : "=m" (*&cw))
#define _FPU_SET_CW(st) __asm__ ("fldcw %0" : : "m" (*&cw))

/* --------------------------------------------------------------------------- */
/* Pentium FPU Status Word                                                     */
/* --------------------------------------------------------------------------- */

/* Exception flags */
#define _FPU_STATUS_IE    0x001  /* Invalid Operation */
#define _FPU_STATUS_DE    0x002  /* Denormalized Operand */
#define _FPU_STATUS_ZE    0x004  /* Zero Divide */
#define _FPU_STATUS_OE    0x008  /* Overflow */
#define _FPU_STATUS_UE    0x010  /* Underflow */
#define _FPU_STATUS_PE    0x020  /* Precision */
/* Stack Fault */
#define _FPU_STATUS_SF    0x040  /* Precision */
/* Exception summary */
#define _FPU_STATUS_ES    0x080  /* Error Summary Status */

/* Macros for accessing the FPU status word.  */
/* get the FPU status */
#define _FPU_GET_SW(sw) __asm__ ("fnstsw %0" : "=m" (*&sw))
/* clear the FPU status */
#define _FPU_CLR_SW __asm__ ("fnclex" : : )


static void linux_fpe_signal_handler( int sig)
{
  unsigned int s;

  _FPU_GET_SW( s);
  _FPU_CLR_SW;

  if (s & _FPU_STATUS_ZE)
    fts_fpe_handler( FTS_DIVIDE0_FPE);
  else if (s & _FPU_STATUS_IE)
    fts_fpe_handler( FTS_INVALID_FPE);
  else if (s & _FPU_STATUS_OE)
    fts_fpe_handler( FTS_OVERFLOW_FPE);
  else if (s & _FPU_STATUS_UE)
    fts_fpe_handler( FTS_UNDERFLOW_FPE);
  else if (s & _FPU_STATUS_PE)
    fts_fpe_handler( FTS_INEXACT_FPE);
  else if (s & _FPU_STATUS_DE)
    fts_fpe_handler( FTS_DENORMALIZED_FPE);
  else
    fts_fpe_handler(0);
}

void fts_enable_fpe_traps( void)
{
  unsigned  int cw;

  signal( SIGFPE, linux_fpe_signal_handler);

#ifdef ENABLE_DENORMALIZED_TRAPS
  cw = 0x1000 + _FPU_CONTROL_EP + _FPU_CONTROL_NR /* extended precision + round to nearest */
    + _FPU_CONTROL_UM /* disable underflow */
    + _FPU_CONTROL_PM; /* disable precision */
#else
  cw = 0x1000 + _FPU_CONTROL_EP + _FPU_CONTROL_NR /* extended precision + round to nearest */
    + _FPU_CONTROL_DM /* disable denormalized */
    + _FPU_CONTROL_UM /* disable underflow */
    + _FPU_CONTROL_PM; /* disable precision */
#endif

  _FPU_SET_CW( cw);
}

void fts_disable_fpe_traps( void)
{
  unsigned  int cw;

  signal( SIGFPE, SIG_IGN);

  cw = 0x1000 + _FPU_CONTROL_EP + _FPU_CONTROL_NR
    + _FPU_CONTROL_IM /* disable invalid operand */
    + _FPU_CONTROL_DM /* disable denormalized */
    + _FPU_CONTROL_ZM /* disable divide by zero */
    + _FPU_CONTROL_OM /* disable overflow */
    + _FPU_CONTROL_UM /* disable underflow */
    + _FPU_CONTROL_PM; /* disable precision */

  _FPU_SET_CW( cw);
}

unsigned int fts_check_fpe( void)
{
  unsigned int s, ret;

  _FPU_GET_SW( s);
  _FPU_CLR_SW;

  ret = 0;

  if (s & _FPU_STATUS_IE)
    ret |= FTS_INVALID_FPE;

  if (s & _FPU_STATUS_ZE)
    ret |= FTS_DIVIDE0_FPE;

  if (s & _FPU_STATUS_OE)
    ret |= FTS_OVERFLOW_FPE;

  return ret;
}

/* *************************************************************************** */
/*                                                                             */
/* Memory locking                                                              */
/*                                                                             */
/* *************************************************************************** */

int fts_unlock_memory( void)
{
  return 0;
}


/* *************************************************************************** */
/*                                                                             */
/* Platform specific initialization                                            */
/*                                                                             */
/* *************************************************************************** */

static int wdpipe[2];

void set_priority( int delta)
{
  struct sched_param sp;

  sp.sched_priority = sched_get_priority_max( SCHED_FIFO) + delta;

  if ( sched_setscheduler( 0, SCHED_FIFO, &sp) < 0)
    {
      /* ??? */
    }    
}

static void abandon_root( void)
{
  /* Get rid of root privilege if we have them */
  seteuid( getuid());
}

static void watchdog_signal_handler( int sig)
{
  struct timeval timeout;

  timeout.tv_sec = 0;
  timeout.tv_usec = 30000;
  
  select( 0, 0, 0, 0, &timeout);

  fprintf( stderr, "[FTS] watchdog activated\n");
}

static void sigchld_signal_handler( int sig)
{
  fprintf( stderr, "[%d] Child has exited. Exiting...\n", getpid());
  _exit( 1);
}

static void watchdog_low( void)
{
  set_priority( -3);
  abandon_root();

  while (1)
    {
      if (write( wdpipe[1], "\n", 1) < 1)
	exit( 1);

      sleep( 1);
    }
}

static void watchdog_high( void)
{
  int armed = 0;

  set_priority( -1);
  abandon_root();

  while (1)
    {
      struct timeval timeout;
      fd_set rfds;
      int r;

      timeout.tv_sec = (armed) ? 1 : 5;
      timeout.tv_usec = 0;

      FD_ZERO( &rfds);
      FD_SET( wdpipe[0], &rfds);
      
      r = select( wdpipe[0]+1, &rfds, 0, 0, &timeout);

      if (r > 0)
	{
	  char buffer[100];

	  if ( read( wdpipe[0], &buffer, 100) <= 0)
	    exit( 1);

	  armed = 0;
	}
      else if (r == 0)
	{
	  armed = 1;
	  kill( getppid(), SIGHUP);
	  fprintf(stderr, "watchdog waking up\n"); 
	}
      else
	exit( 1);
    }
}

static void do_fork( void (*f)(void))
{
  int pid;

  if ((pid = fork()) < 0)
    fprintf( stderr, "cannot fork (%s)\n", strerror( errno));
  else if (pid)
    (*f)();
}

static void start_watchdog( void)
{
  if (pipe( wdpipe) < 0)
    {
      fprintf( stderr, "cannot create pipe (%s)\n", strerror( errno));
      return;
    }

  signal( SIGCHLD, sigchld_signal_handler);

  do_fork( watchdog_low);
  do_fork( watchdog_high);

  signal( SIGHUP, watchdog_signal_handler);
}


void fts_platform_init( int argc, char **argv)
{
  int i, no_watchdog = 0;

  for ( i = 0; i < argc; i++)
    {
      if ( !strcmp( argv[i], "--no-watchdog"))
	no_watchdog = 1;
    }
    
  if ( !no_watchdog)
    start_watchdog();

  set_priority( -2);

  abandon_root();

  fts_disable_fpe_traps();
}
