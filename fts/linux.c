/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * 
 * See file COPYING.LIB for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/*
 * This file contains Linux (Intel compatible processors and PowerPC) platform dependent functions:
 *  - dynamic loader
 *  - FPU settings
 *  - real-time: scheduling mode and priority, memory locking
 * 
 */

#include <fts/fts.h>
#include <ftsconfig.h>

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dlfcn.h>
#include <sched.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <errno.h>

#include <ftsprivate/fpe.h>
#include <ftsprivate/platform.h>
#include <ftsprivate/package.h>
#include <fts/thread.h>

/***********************************************************************
 *
 * Root directory
 *
 */

#define PREF_DIR ".jmax"

fts_symbol_t fts_get_default_root_directory( void)
{
  return fts_new_symbol( DEFAULT_ROOT);
}

/***********************************************************************
 *
 * Project and configuration files
 *
 */

fts_symbol_t get_user_directory(void)
{
  char dir[MAXPATHLEN];

  strcpy( dir, getenv("HOME"));
  strcat( dir, "/");
  strcat( dir, PREF_DIR);
  
  /* create the directory if necessary */
  if (!fts_file_exists(dir) && mkdir( dir, 0755) < 0)
    fts_log( "[user] error: cannot create directory %s\n", dir);

  if (fts_is_file( dir))
    fts_log( "[user] error: %s is a file, but must be a directory\n", dir);

  return fts_new_symbol(dir);
}

fts_symbol_t
fts_get_user_project( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( get_user_directory(), fts_s_default_project, path, MAXPATHLEN);

  return fts_new_symbol( path);
}

fts_symbol_t 
fts_get_system_project( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path(DEFAULT_ROOT, fts_s_default_project, path, MAXPATHLEN);

  return fts_new_symbol(path);
}

fts_symbol_t 
fts_get_user_configuration( void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path( get_user_directory(), fts_s_default_config, path, MAXPATHLEN);

  return fts_new_symbol(path);
}

fts_symbol_t 
fts_get_system_configuration(void)
{
  char path[MAXPATHLEN];

  fts_make_absolute_path(DEFAULT_ROOT, fts_s_default_config, path, MAXPATHLEN);

  return fts_new_symbol(path);
}

/***********************************************************************
 *
 * Dynamic loader
 * The Linux implementation uses dlopen() and dlsym().
 *
 */

fts_status_t fts_load_library( const char *filename, const char *symbol)
{
  static char error_description[1024];
  static fts_status_description_t load_library_error = { error_description};
  void *handle;
  void (*fun)(void);

  handle = dlopen( filename, RTLD_NOW | RTLD_GLOBAL);

  if (!handle)
    {
      strcpy( error_description, dlerror());
      return &load_library_error;
    }

  fun = (void (*)(void))dlsym( handle, symbol);

  if ( fun == NULL)
    {
      strcpy( error_description, dlerror());
      return &load_library_error;
    }

  (*fun)();

  return fts_ok;
}


/***********************************************************************
 *
 * Floating-point unit handling for Intel and compatible
 *
 */

#ifdef i386

/*
 * #define this if you want denormalized f.p. traps to be reported 
 */
#undef ENABLE_DENORMALIZED_TRAPS


/* As Linux does not provide (yet?) a way to access the status of the          */
/* floating-point unit, it is done by hand using __asm__.                      */

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
  fts_fpe_handler( fts_check_fpe());
}

void fts_enable_fpe_traps( void) __attribute__ ((no_check_memory_usage));

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

void fts_disable_fpe_traps( void) __attribute__ ((no_check_memory_usage));

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

unsigned int fts_check_fpe( void) __attribute__ ((no_check_memory_usage));

unsigned int fts_check_fpe( void)
{
  unsigned int s;
  int which = 0;

  _FPU_GET_SW( s);
  _FPU_CLR_SW;

  if (s & _FPU_STATUS_ZE)
    which |= FTS_DIVIDE0_FPE;
  if (s & _FPU_STATUS_IE)
    which |= FTS_INVALID_FPE;
  if (s & _FPU_STATUS_OE)
    which |= FTS_OVERFLOW_FPE;
  if (s & _FPU_STATUS_UE)
    which |= FTS_UNDERFLOW_FPE;
  if (s & _FPU_STATUS_PE)
    which |= FTS_INEXACT_FPE;
#ifdef ENABLE_DENORMALIZED_TRAPS
  if (s & _FPU_STATUS_DE)
    which |= FTS_DENORMALIZED_FPE;
#endif

  return which;
}

#endif


/***********************************************************************
 *
 * Floating-point unit handling for PowerPC
 *
 */

#ifdef PPC

static void linux_fpe_signal_handler( int sig)
{
  fts_fpe_handler( fts_check_fpe());
}

void fts_enable_fpe_traps( void) __attribute__ ((no_check_memory_usage));

void fts_enable_fpe_traps( void)
{
  unsigned  int cw;

  signal( SIGFPE, linux_fpe_signal_handler);

  /* ... PPC specific code ... */
}

void fts_disable_fpe_traps( void) __attribute__ ((no_check_memory_usage));

void fts_disable_fpe_traps( void)
{
  signal( SIGFPE, SIG_IGN);

  /* ... PPC specific code ... */
}

unsigned int fts_check_fpe( void) __attribute__ ((no_check_memory_usage));

unsigned int fts_check_fpe( void)
{
  unsigned int s;
  int which = 0;

  /* ... PPC specific code ... */

  return which;
}

#endif

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

static void set_priority( int delta)
{
  struct sched_param sp;

  sp.sched_priority = sched_get_priority_max( SCHED_FIFO) + delta;

  if ( sched_setscheduler( 0, SCHED_FIFO, &sp) < 0)
    {
      fts_log("[sched] cannot set scheduling policy to SCHED_FIFO with priority %d\n",
	      sp.sched_priority);
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

  fts_log( "[FTS] watchdog activated\n");
}

static void sigchld_signal_handler( int sig)
{
  fprintf( stderr, "[%d] Child has exited. Exiting...\n", getpid());
  _exit( 1);
}

static void watchdog_low( void)
{
  abandon_root();

  signal( SIGCHLD, sigchld_signal_handler);

  while (1)
    {
      if (write( wdpipe[1], "\n", 1) < 1)
	exit( 1);

      sleep( 1);
    }
}

static void my_fd_zero( fd_set *fds) __attribute__ ((no_check_memory_usage));

static void my_fd_zero( fd_set *fds)
{
  FD_ZERO( fds);
}

static void my_fd_set( int fd, fd_set *fds) __attribute__ ((no_check_memory_usage));

static void my_fd_set( int fd, fd_set *fds)
{
  FD_SET( fd, fds);
}

static void watchdog_high( int fts_pid)
{
  int armed = 0;

  set_priority( -1);
  abandon_root();

  signal( SIGCHLD, sigchld_signal_handler);

  while (1)
    {
      struct timeval timeout;
      fd_set rfds;
      int r;

      timeout.tv_sec = (armed) ? 1 : 5;
      timeout.tv_usec = 0;

      my_fd_zero( &rfds);
      my_fd_set( wdpipe[0], &rfds);
      
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
	  kill( fts_pid, SIGHUP);
	  fts_log( "watchdog waking up\n"); 
	}
      else
	exit( 1);
    }
}

static int do_fork( void)
{
  int pid;

  if ((pid = fork()) < 0)
    {
      fts_log( "cannot fork (%s)\n", strerror( errno));
      exit( 2);
    }

  return pid;
}

static void start_watchdog( void)
{
  int child_pid;

  if (pipe( wdpipe) < 0)
    {
      fprintf( stderr, "cannot create pipe (%s)\n", strerror( errno));
      return;
    }

  if ((child_pid = do_fork()))
    watchdog_low();

  if ((child_pid = do_fork()))
    watchdog_high( child_pid);

  signal( SIGHUP, watchdog_signal_handler);
}


void fts_platform_init( void)
{
  if ( !fts_cmd_args_get( fts_new_symbol( "no-watchdog")))
    start_watchdog();

  set_priority( -2);

  abandon_root();

  fts_disable_fpe_traps();

  /* load aflib package */
  /* fts_package_load(fts_new_symbol("aflib")); */


  /*  if we want to have audio
   we load audio packages */
  if (NULL == fts_cmd_args_get(fts_s_noaudio))
  { 
#ifdef USE_ALSA   
    /* load alsa package */
    fts_package_load(fts_new_symbol("alsa"));
#endif
/* USE_JACK is defined in configure if jack support is enabled */
#ifdef USE_JACK
    /* load jack package */
    fts_package_load(fts_new_symbol("jack"));
#endif /* USE_JACK */
  }
}


/* *************************************************************************** */
/*                                                                             */
/* System time                                                                   */
/*                                                                             */
/* *************************************************************************** */

double fts_systime()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  return (double) now.tv_sec * 1000.0 + (double) now.tv_usec / 1000.0;
}

/* ************************************************** */
/*                                                    */
/* Thread manager platform dependent function         */
/*                                                    */
/* ************************************************** */
int thread_manager_start(thread_manager_t* self)
{
  int success;
  /* Time to create the thread */
  success = pthread_create(&self->thread_manager_ID,
			   NULL,
			   thread_manager_main,
			   (void*)self);

  return success;
}

void* thread_manager_run_thread(void* arg)
{
  fts_thread_function_t* thread_func = (fts_thread_function_t*)arg;
  fts_object_t* object = thread_func->object;
  fts_method_t method = thread_func->method;
  fts_symbol_t symbol = thread_func->symbol;
  int ac = thread_func->ac;
  fts_atom_t* at = thread_func->at;
  int delay_ms = thread_func->delay_ms;
  struct timespec time_req;
  struct timespec time_rem;
  time_req.tv_sec = 0;
  time_req.tv_nsec = delay_ms * 1000 * 1000;

  while(1)
    {
      if (0 == thread_func->is_dead)
	{
	  method(object, fts_system_inlet, symbol, ac, at);
	}
      nanosleep(&time_req, &time_rem);
    }

  return 0;
}


/** 
 * Main function of the thread manager
 * 
 * @param arg 
 * 
 * @return 
 */
void* thread_manager_main(void* arg)
{
  int work_done;
  int success;
  thread_manager_t* manager = (thread_manager_t*)arg;

  while(1)
    {
      work_done = 0;
      /* Is there some data in create FIFO */
      if (fts_fifo_read_level(&manager->create_fifo) >= sizeof(fts_atom_t))
	{
	  fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->create_fifo);	    
	  fts_thread_worker_t* worker;

	  fts_fifo_incr_read(&manager->create_fifo, sizeof(fts_atom_t));

	  worker = (fts_thread_worker_t*)fts_get_pointer(atom);
	    
	  success = pthread_create((pthread_t *)&worker->id,
				   NULL,
				   thread_manager_run_thread,
				   (void*)worker->thread_function);
	  if (0 != success)
	    {
	      fts_post("[thread_manager] cannot start a new thread \n");
	    }
	  else
	    {
	      /* try to detach thread */
	      success = pthread_detach(worker->id);
	      if (0 != success)
		{
		  fts_post("[thread manager] cannot detach thread %d \n", worker->id);
		}
	      work_done++;

	    }

	}
      /* Is there some data in cancel FIFO */
      if (fts_fifo_read_level(&manager->cancel_fifo) >= sizeof(fts_atom_t))
	{
	  fts_atom_t* atom = (fts_atom_t*)fts_fifo_read_pointer(&manager->cancel_fifo);	    
	  fts_thread_worker_t* worker;

	  fts_fifo_incr_read(&manager->cancel_fifo, sizeof(fts_atom_t));

	  worker = (fts_thread_worker_t*)fts_get_pointer(atom);
	    
	  success = pthread_cancel(worker->id);
	  if (0 != success)
	    {
	      if (ESRCH == success)
		{
		  fts_post("[thread manager] no such thread \n");
		}
	      fts_post("[thread manager] error while cancelling thread \n");
	    }
	  work_done++;
	}
      if (0 == work_done)
	{	
	  int delay_ms = manager->delay_ms;
	  struct timespec time_req;
	  struct timespec time_rem;
	  time_req.tv_sec = 0;
	  time_req.tv_nsec = delay_ms * 1000 * 1000;
	    
	  /* fts_post("[thread_manager] thread is running \n"); */
	  nanosleep(&time_req, &time_rem);
	}
    }
    
  return NULL;
}


/** EMACS **
 * Local variables:
 * mode: c
 * c-basic-offset:2
 * End:
 */
